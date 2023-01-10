// dllmain.cpp : Defines the entry point for the DLL application.

#include <Header/dllmain.h>
#include "Header/Helper.hpp"
#include <qDebug>
#include <unordered_map>
unordered_map<HANDLE, POINT> Points; //��������豸�ľ���͸�����λ��

HWND m_hWnd = NULL; //���ڵľ��
HHOOK hHook = NULL; //���ӵľ��

DWORD ThreadID = 0;
//

void Test()
{
    //MessageBoxA(0, "Test", "Info", 0);
}

/// <summary>
///  ע���豸����Ϊֻ��һ���豸���ͣ����Բ��ܶ��ٸ���꣬���һ���͹���
/// </summary>
/// <param name="hWnd">�����ڵľ��</param>
/// <returns></returns>
void RawRegister(HWND hWnd)
{
    m_hWnd = hWnd;
    RAWINPUTDEVICE Rid[1];

    Rid[0].usUsagePage = 0x01;  //�豸����Ϊ���
    Rid[0].usUsage = 0x02;
    Rid[0].dwFlags = RIDEV_INPUTSINK; //RIDEV_INPUTSINK�����ó�����Ժ�̨����
    Rid[0].hwndTarget = hWnd;

    //  Rid[1].usUsagePage = 0x01;
    //  Rid[1].usUsage = 0x02;
    //  Rid[1].dwFlags = RIDEV_INPUTSINK;
    // Rid[1].hwndTarget = hWnd;
    RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));
    //    if (RegisterRawInputDevices(Rid, 1, sizeof(Rid[0])) == FALSE)
    //    {
    //        int Sk = GetLastError();
    //        MsgBox("Failed");
    //    }
    /*
     *���Ӻ��������֣���ǰ�̺߳�ȫ��
     * �߳�ֻ�����ص�ǰ�̵߳Ĺ��ӣ�ȫ�ֹ��ӻ����������ĳ���
     * ���������ص����̵߳Ĺ���
     * Ϊʲô��ȫ�ֹ��ӵ�ʱ�򣬱���Ҫ��dll�أ���Ϊ��������Ӻ�����(ע���ǹ��Ӻ���)��ȡ�������������Ϣ��
       �ص����������ڡ����ӵĳ�����ִ�еġ���������"����ȡ�ĳ�����"ִ�еģ�Ҳ�ʹ��������ûдdll������£�
       ����̣߳��������̵��̣߳�����û�취ȥ��������ص���������δ������д��һ��dllģ�����棬�����𵽴���ע�롣
       Ȼ�󵱹�����Ч��ʱ��ϵͳ��ǿ�ȱ���ȡ�ĳ���Ҳ�������dll��������̾��Ǵ���ע�롣����������򣬱���ȡ�ĳ���
       ���ڳ���ע�ṳ�ӵĺ��������ǵĳ���
      *�������windows��Ϣ
        WM MOUSEMOVE WM LBUTTONDOWN WM RBUTTONDOWN WM LBUTTONUP
        ���ӻ�ػ����д������Ϣ
       * �������ӵĺ��� ��һ����������һֱ���ӷ�������Ϣ���е���Ϣ��
       *�ڶ��������������ǵĻص�������
       * ������������NULL,
       * ���ĸ������Ǵ������ڵ��̵߳�id
    */
    //������Ŀ�� ����֪ͨϵͳ������Ϣ����Ŀ��֮ǰ����ִ�лص�����
    //ȫ�־�������ϵͳ�������̵߳���Ϣ �߳̾���ֻ�����Լ���
    //����ֵ�ǹҹ��ľ��
    hHook = SetWindowsHookEx(WH_GETMESSAGE, GetMessageProc, NULL, ThreadID);
}

/// <summary>
/// ��ȡ��ǰ��⵽���豸����
/// </summary>
/// <returns>�豸����</returns>
int GetDeviceCount()
{
    return Points.size();
}

/// <summary>
/// ��ȡĳ������豸��X���Y��
/// </summary>
/// <param name="Index">����豸���±�</param>
/// <param name="XY">trueΪX�ᣬfalseΪY��</param>
/// <returns></returns>
int GetData(int Index, int XY)
{
    int t = 0;
    for (auto i : Points)
    {
        if (Index != t) { t++; continue; }
        if (XY)
        {
            return i.second.x;
        }
        else {
            return i.second.y;
        }
        t++;
    }

    return 0;
}

LRESULT CALLBACK GetMessageProc(int nCode, WPARAM wParam, LPARAM lParam)//nCode  ��Ϣ ��Ϣ����
{
    LPMSG pMsg = (LPMSG)lParam;
    //���ҹ���Ϣ���ݵ���ǰ�ҹ����е���һ���ҹ�
    //�ж����ص�����Ϣ�ǲ��Ǹ��Լ����ڵ�����Ǿʹ����ǵĻ��ͼ���������ȥ������ֱ�Ӷ������㶪�˱���զ�� ����Ҳ���ܲ��ñ�Ĵ��ڴ���
    if (pMsg->hwnd != m_hWnd) return CallNextHookEx(hHook, nCode, wParam, lParam);

    switch (pMsg->message)
    {
    case WM_INPUT: { //ֻ����WM_INPUT��Ϣ
        UINT dataSize;
        GetRawInputData(reinterpret_cast<HRAWINPUT>(pMsg->lParam),
                        RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));
        //Need to populate data size first
        //��ȡ���ݵĴ�С
        if (dataSize > 0) //������ݲ�Ϊ����������
        {
            std::vector<BYTE> rawdata(dataSize);
            //vector���������ڴ�й©�������ͷ�ָ��

            //��ȡ����
            if (GetRawInputData(reinterpret_cast<HRAWINPUT>(pMsg->lParam), RID_INPUT, rawdata.data(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize)
            {
                RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawdata.data());

                if (raw->header.dwType == RIM_TYPEMOUSE) //������յ����������Ϣ
                {

                    if (Points.count(raw->header.hDevice) == 0)  //���map�в����ڸ����������˵����һ���µ���꣬�������map��
                        Points.insert({ raw->header.hDevice, { 0,0 } });
                    POINT new_point = { Points[raw->header.hDevice].x,Points[raw->header.hDevice].y };
                    new_point.x += raw->data.mouse.lLastX;
                    new_point.y += raw->data.mouse.lLastY;
                    if(new_point.x <= 0)
                        new_point.x = 0;
                    else if(new_point.x >= 1280)
                        new_point.x = 1280;
                    if(new_point.y <= 0)
                        new_point.y = 0;
                    else if(new_point.y >= 720)
                        new_point.x = 720;
                    //��map�б�������λ�ã��ͱ�����Ϣ�л�ȡ�������λ����ӣ�Ȼ�����map�б����ֵ
                    Points[raw->header.hDevice] = new_point;

                }
            }
        }
        return 0;
    }
    }

    return CallNextHookEx(hHook, nCode, wParam, lParam);
}
