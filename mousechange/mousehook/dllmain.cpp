// dllmain.cpp : Defines the entry point for the DLL application.
#include <iostream>
#include <Header/dllmain.h>
#include "Header/Helper.hpp"
#include <qDebug>
#include <atlbase.h>
#include <atlconv.h>
#include <unordered_map>
#include <fstream>
struct mouseMessage {
    LONG x;
    LONG y;
    ULONG button;
    int id;
    USHORT dist;
};
struct keyboardMessage {
    USHORT letter;
};
int i = 0;
int directionx[4] = {1, 1, 1, 1};
int directiony[4] = {1, 1, 1, 1};
//�豸id��ͬ��겻һ���� ע��\\ת���ַ�
string str[100] = {"\\\\?\\HID#VID_046D&PID_C53F&MI_01&Col01#7&26a318b7&0&0000#{378de44c-56ef-11d1-bc8c-00a0c91405dd}",
                   "\\\\?\\HID#VID_17EF&PID_602E#6&2a8933e&2&0000#{378de44c-56ef-11d1-bc8c-00a0c91405dd}",
                   "\\\\?\\HID#VID_18F8&PID_0F97&MI_00#7&2fd1be7a&0&0000#{378de44c-56ef-11d1-bc8c-00a0c91405dd}",
                   "\\\\?\\HID#VID_413C&PID_301A#6&1bc7e9a5&0&0000#{378de44c-56ef-11d1-bc8c-00a0c91405dd}"
                  };
unordered_map<HANDLE, mouseMessage> Points; //��������豸�ľ���͸�����λ��
unordered_map<HANDLE, keyboardMessage> KeyBoard; //��������豸�ľ���ͼ��̵��ַ�

int button;
HWND m_hWnd = NULL; //���ڵľ��
HHOOK hHook = NULL; //�̹߳��ӵľ��

DWORD ThreadID = 0; //��¼�̵߳�id,��main���������ʼ��

void PositiveDirectionX(int id) {
    directionx[id] = 1;
}
void OppositeDirectionX(int id){
    directionx[id] = -1;
}
void PositiveDirectionY(int id) {
    directiony[id] = 1;
}
void OppositeDirectionY(int id) {
    directiony[id] = -1;
}

///  ע���豸����Ϊֻ��һ���豸���ͣ����Բ��ܶ��ٸ���꣬���һ���͹���
void RawRegister(HWND hWnd) {
    m_hWnd = hWnd; //���ھ����ֵ
    RAWINPUTDEVICE Rid[2];
    //ע�����
    Rid[0].usUsagePage = 0x01;  //�豸����Ϊ���
    Rid[0].usUsage = 0x02;
    Rid[0].dwFlags = RIDEV_INPUTSINK; //RIDEV_INPUTSINK�����ó�����Ժ�̨����
    Rid[0].hwndTarget = hWnd;
    //ע�����
    Rid[1].usUsagePage = 0x01;
    Rid[1].usUsage = 0x06;
    Rid[1].dwFlags = RIDEV_INPUTSINK;
    Rid[1].hwndTarget = hWnd;

    //��Ҫ���� WM_INPUT ��Ϣ��Ӧ�ó��������ʹ�� RegisterRawInputDevices ����ע��ԭʼ�����豸��
    //��ΪĬ������£�Ӧ�ó��򲻻����ԭʼ���롣
    //ע������豸 �����豸
    RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])); //ע������豸
    hHook = SetWindowsHookEx(WH_GETMESSAGE, GetMessageProc, NULL, ThreadID);  //ע�ṳ��
}

/// ��ȡ��ǰ��⵽����豸����
int GetMouseCount() {
    return Points.size();
}

///��ȡ��ǰ��⵽�����豸������
int GetKeyboardCount() {
    return KeyBoard.size();
}

/// ��ȡĳ������豸��X���Y��
/// <param name="Index">����豸���±�</param>
/// <param name="XY">trueΪX�ᣬfalseΪY��</param>
int GetData(int Index, int XY) {
    for (auto i : Points) {
        if(i.second.id != Index) continue;
        if (XY == 1) { //x
            return i.second.x;
        }
        else if(XY == 0) { //y
            return i.second.y;
        }
    }
    return 0;
}

ULONG GetMouseButton(int Index) {
    for (auto i : Points) {
        if(i.second.id != Index) continue;
        return i.second.button;
    }
}

USHORT GetDistance(int Index) {
    for (auto i : Points) {
        if(i.second.id != Index) continue;
        return i.second.dist;
    }
}
void Limit(long &x, long &y) { //�������ݳ���
    if(x <= 0)
        x = 0;
    else if(x >= 960)
        x = 960;
    if(y <= 0)
        y = 0;
    else if(y >= 960)
        y = 960;
}

//����豸���� �Լ��豸id��ȡ
void GetMouseDevice(RAWINPUT* raw) {
    int nResult;
    if (Points.count(raw->header.hDevice) == 0) { //���map�в����ڸ����������˵����һ���µ���꣬�������map��
        UINT nBufferSize = 0;
        nResult = GetRawInputDeviceInfo(raw->header.hDevice,
                                        RIDI_DEVICENAME,
                                        NULL,
                                        &nBufferSize
                                        );
        WCHAR* wcDeviceName = new WCHAR[nBufferSize + 1];
        nResult = GetRawInputDeviceInfo(raw->header.hDevice, // Device
                                        RIDI_DEVICENAME,                // Get Device Name
                                        wcDeviceName,                   // Get Name!
                                        &nBufferSize);                 // Char Count
        USES_CONVERSION;
        const char *s = W2A(wcDeviceName);
        qDebug() << s;
        //�Ƚ��豸����
        if(str[0].compare(s) == 0) i = 0;
        else if(str[1].compare(s) == 0) i = 1;
        else if(str[2].compare(s) == 0) i = 2;
        else if(str[3].compare(s) == 0) i = 3;
        s = NULL;
        Points.insert({ raw->header.hDevice, { 0,0,0, i, 0}});
    }
}

void GetKeyBoardDevice(RAWINPUT* raw) {
    int nResult;

    if (KeyBoard.count(raw->header.hDevice) == 0) { //���map�в����ڸ����������˵����һ���µ���꣬�������map��
        UINT nBufferSize = 0;
        nResult = GetRawInputDeviceInfo(raw->header.hDevice,
                                        RIDI_DEVICENAME,
                                        NULL,
                                        &nBufferSize
                                        );
        WCHAR* wcDeviceName = new WCHAR[nBufferSize + 1];
        nResult = GetRawInputDeviceInfo(raw->header.hDevice, // Device
                                        RIDI_DEVICENAME,                // Get Device Name
                                        wcDeviceName,                   // Get Name!
                                        &nBufferSize);                 // Char Count
        USES_CONVERSION;
        const char *s = W2A(wcDeviceName);//sָ���� ���Ƕ�̬������ڴ�
        qDebug() << s;
        //�Ƚ��豸����
        //        if(str[0].compare(s) == 0) i = 0;
        //        else if(str[1].compare(s) == 0) i = 1;
        //        else if(str[2].compare(s) == 0) i = 2;
        //        else if(str[3].compare(s) == 0) i = 3;
        s = NULL;
        KeyBoard.insert({raw->header.hDevice, {0}});
    }
}


void UpdateMouseData(RAWINPUT *raw) { //�����������
    mouseMessage new_point = {
        Points[raw->header.hDevice].x,
        Points[raw->header.hDevice].y,
        Points[raw->header.hDevice].button,
        Points[raw->header.hDevice].id,
        Points[raw->header.hDevice].dist
    };
    int k1 = directionx[new_point.id], k2 = directiony[new_point.id];
    new_point.button = raw->data.mouse.ulButtons;
    new_point.x += k1 * raw->data.mouse.lLastX;
    new_point.y += k2 * raw->data.mouse.lLastY;
    //��������
    new_point.dist += raw->data.mouse.usButtonData;
    //�����ж�
    Limit(new_point.x, new_point.y);
    //��map�б�������λ�ã��ͱ�����Ϣ�л�ȡ�������λ����ӣ�Ȼ�����map�б����ֵ
    Points[raw->header.hDevice] = new_point;
}

void UpdateKeyboardData(RAWINPUT *raw) { //���¼�������
    keyboardMessage new_keyboard = {
        KeyBoard[raw->header.hDevice].letter
    };
    new_keyboard.letter = raw->data.keyboard.VKey;
    qDebug() << new_keyboard.letter;
}


LRESULT CALLBACK GetMessageProc(int nCode, WPARAM wParam, LPARAM lParam) {//nCode  ��Ϣ ��Ϣ����
    LPMSG pMsg = (LPMSG)lParam;
    //���ҹ���Ϣ���ݵ���ǰ�ҹ����е���һ���ҹ�
    //�ж����ص�����Ϣ�ǲ��Ǹ��Լ����ڵ�����Ǿʹ����ǵĻ��ͼ���������ȥ������ֱ�Ӷ������㶪�˱���զ�� ����Ҳ���ܲ��ñ�Ĵ��ڴ���
    if (pMsg->hwnd != m_hWnd) return CallNextHookEx(hHook, nCode, wParam, lParam);
    switch (pMsg->message) {
    case WM_INPUT: { //ֻ����WM_INPUT��Ϣ
        UINT dataSize;
        GetRawInputData(reinterpret_cast<HRAWINPUT>(pMsg->lParam),
                        RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));
        if (dataSize > 0) {
            //������ݲ�Ϊ����������
            std::vector<BYTE> rawdata(dataSize);
            //��ȡ����
            if (GetRawInputData(reinterpret_cast<HRAWINPUT>
                                (pMsg->lParam), RID_INPUT,
                                rawdata.data(), &dataSize,
                                sizeof(RAWINPUTHEADER)) == dataSize) {
                RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawdata.data());
                if (raw->header.dwType == RIM_TYPEMOUSE) {
                    //������յ����������Ϣ
                    GetMouseDevice(raw);
                    UpdateMouseData(raw);
                } else if(raw->header.dwType == RIM_TYPEKEYBOARD) {
                    //������ܵ��Ǽ�����Ϣ
                    GetKeyBoardDevice(raw);
                    UpdateKeyboardData(raw);
                }
            }
        }
        return 0;
    }
    }
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}
