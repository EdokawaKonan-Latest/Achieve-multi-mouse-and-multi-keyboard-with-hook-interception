// dllmain.cpp : Defines the entry point for the DLL application.
#include <Header/dllmain.h>

struct MouseMessage {
    LONG x;
    LONG y;
    ULONG button;
    INT id;
    USHORT dist;
};
struct KeyboardMessage {
    QString letter;
    INT id;
};

int g_mouse_idx = 0;
int g_key_idx = 0;
int g_directionx[4] = {1, 1, 1, 1};
int g_directiony[4] = {1, 1, 1, 1};
HWND g_m_hwnd = NULL; //���ڵľ��
HHOOK g_h_hook = NULL; //�̹߳��ӵľ��
DWORD g_thread_id = 0; //��¼�̵߳�id,��main���������ʼ��
unordered_map<HANDLE, MouseMessage> g_points; //��������豸�ľ���͸�����λ��
unordered_map<HANDLE, KeyboardMessage> g_keyboard; //��������豸�ľ���ͼ��̵��ַ�
unordered_map<USHORT, QString> g_key_value;//key value

//�豸id��ͬ��겻һ���� ע��\\ת���ַ�

string g_mouse_dev[100] = {"\\\\?\\HID#VID_046D&PID_C53F&MI_01&Col01#7&3aefd595&0&0000#{378de44c-56ef-11d1-bc8c-00a0c91405dd}",
                           "\\\\?\\HID#VID_17EF&PID_602E#6&2a8933e&2&0000#{378de44c-56ef-11d1-bc8c-00a0c91405dd}",
                           "\\\\?\\HID#VID_18F8&PID_0F97&MI_00#7&162b7b0f&0&0000#{378de44c-56ef-11d1-bc8c-00a0c91405dd}",
                           "\\\\?\\HID#VID_413C&PID_301A#6&221bbfb9&0&0000#{378de44c-56ef-11d1-bc8c-00a0c91405dd}"
                          };

string g_keyboard_dev[100] = {"\\\\?\\HID#VID_413C&PID_2113&MI_00#7&2d9b9e3e&0&0000#{884b96c3-56ef-11d1-bc8c-00a0c91405dd}",
                              "\\\\?\\HID#VID_413C&PID_2107#6&38bcfe3c&0&0000#{884b96c3-56ef-11d1-bc8c-00a0c91405dd}",
                              "\\\\?\\HID#{00001124-0000-1000-8000-00805f9b34fb}_VID&000205ac_PID&024f&Col01#8&2b8baa02&1&0000#{884b96c3-56ef-11d1-bc8c-00a0c91405dd}",
                              "\\\\?\\HID#VID_046D&PID_C53F&MI_01&Col01#7&3aefd595&0&0000#{378de44c-56ef-11d1-bc8c-00a0c91405dd}"
                             };
//���̰�����ʼ��
void KeyValueInit() {
    std::ifstream accfile;
    string s;
    accfile.open("KeyValue.txt");
    if(!accfile.is_open()) {
        cerr << "open failed" << endl;
        return;
    }
    while(getline(accfile, s)) {
        string temp="";
        int a = 0;
        for(auto c : s) {
            if(c == ' ') {
                a = std::stoi(temp);
                temp.clear();
            }
            else temp += c;
        }
        QString c = QString::fromStdString(temp);
        g_key_value.insert({a, c});
        //qDebug() <<a << " "<< c;
    }
    if(accfile.is_open()) {
        accfile.close();
    }
}

void PositiveDirectionX(int id) {
    g_directionx[id] = 1;
}
void OppositeDirectionX(int id){
    g_directionx[id] = -1;
}
void PositiveDirectionY(int id) {
    g_directiony[id] = 1;
}
void OppositeDirectionY(int id) {
    g_directiony[id] = -1;
}

///  ע���豸����Ϊֻ��һ���豸���ͣ����Բ��ܶ��ٸ���꣬���һ���͹���
void RawRegister(HWND hWnd) {
    g_m_hwnd = hWnd; //���ھ����ֵ
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
    g_h_hook = SetWindowsHookEx(WH_GETMESSAGE, GetMessageProc, NULL, g_thread_id);  //ע�ṳ��
}

/// ��ȡ��ǰ��⵽����豸����
int GetMouseCount() {
    return g_points.size();
}

///��ȡ��ǰ��⵽�����豸������
int GetKeyboardCount() {
    return g_keyboard.size();
}

/// ��ȡĳ������豸��X���Y��
/// <param name="Index">����豸���±�</param>
/// <param name="XY">trueΪX�ᣬfalseΪY��</param>
int GetData(int Index, int XY) {
    for (auto i : g_points) {
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
QString GetKeyString(int Index) {
    for(auto i : g_keyboard) {
        if(i.second.id != Index) continue;
        return i.second.letter;
    }
    return "";
}

ULONG GetMouseButton(int Index) {
    for (auto i : g_points) {
        if(i.second.id != Index) continue;
        return i.second.button;
    }
}

USHORT GetDistance(int Index) {
    for (auto i : g_points) {
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

WCHAR* GetDeviceName(RAWINPUT* raw) {
    int nResult;
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
    return wcDeviceName;
}

//����豸���� �Լ��豸id��ȡ
void GetMouseDevice(RAWINPUT* raw) {
    //�Ƚ��豸����
    if (g_points.count(raw->header.hDevice) == 0) {
        WCHAR* wcDeviceName = GetDeviceName(raw);
        USES_CONVERSION;
        const char * s = W2A(wcDeviceName);
        qDebug() << s;
        if(g_mouse_dev[0].compare(s) == 0) g_mouse_idx = 0;
        else if(g_mouse_dev[1].compare(s) == 0) g_mouse_idx = 1;
        else if(g_mouse_dev[2].compare(s) == 0) g_mouse_idx = 2;
        else if(g_mouse_dev[3].compare(s) == 0) g_mouse_idx = 3;
        else{
            g_mouse_idx = 100;
        }
        g_points.insert({ raw->header.hDevice, { 0,0,0, g_mouse_idx, 0}});
        s = NULL;
    }
}

void UpdateMouseData(RAWINPUT *raw) { //�����������
    MouseMessage new_point = {
        g_points[raw->header.hDevice].x,
        g_points[raw->header.hDevice].y,
        g_points[raw->header.hDevice].button,
        g_points[raw->header.hDevice].id,
        g_points[raw->header.hDevice].dist
    };
    int k1 = g_directionx[new_point.id], k2 = g_directiony[new_point.id];
    new_point.button = raw->data.mouse.ulButtons;
    new_point.x += k1 * raw->data.mouse.lLastX;
    new_point.y += k2 * raw->data.mouse.lLastY;
    new_point.dist += raw->data.mouse.usButtonData;//��������
    Limit(new_point.x, new_point.y);//�����ж�
    //��map�б�������λ�ã��ͱ�����Ϣ�л�ȡ�������λ����ӣ�Ȼ�����map�б����ֵ
    g_points[raw->header.hDevice] = new_point;
}

void GetKeyBoardDevice(RAWINPUT* raw) {
    if (g_keyboard.count(raw->header.hDevice) == 0) {
        WCHAR* wcDeviceName = GetDeviceName(raw);
        const char * s;
        USES_CONVERSION;
        s = W2A(wcDeviceName);
        qDebug() << s;
        //�Ƚ��豸����
        if(g_keyboard_dev[0].compare(s) == 0) g_key_idx = 0;
        else if(g_keyboard_dev[1].compare(s) == 0) g_key_idx = 1;
        else if(g_keyboard_dev[2].compare(s) == 0) g_key_idx = 2;
        else if(g_keyboard_dev[3].compare(s) == 0) g_key_idx = 3;
        else g_key_idx = 100;
        g_keyboard.insert({raw->header.hDevice, {"",g_key_idx}});
        s = NULL;
    }
}


void UpdateKeyboardData(RAWINPUT *raw) { //���¼�������  
    KeyboardMessage new_keyboard = {
        g_keyboard[raw->header.hDevice].letter,
        g_keyboard[raw->header.hDevice].id
    };
    int makecode = raw->data.keyboard.MakeCode;
    //�����ѹ��
    if(raw->data.keyboard.Flags == 0) {
        qDebug() << "the key is pressed";
        new_keyboard.letter += g_key_value[makecode];
    } else {
        qDebug() << "the key is realsed";
    }
    g_keyboard[raw->header.hDevice] = new_keyboard;
    qDebug() << g_keyboard[raw->header.hDevice].letter;
    qDebug() << g_keyboard[raw->header.hDevice].id;
}


LRESULT CALLBACK GetMessageProc(int nCode, WPARAM wParam, LPARAM lParam) {//nCode  ��Ϣ ��Ϣ����
    LPMSG pMsg = (LPMSG)lParam;
    //���ҹ���Ϣ���ݵ���ǰ�ҹ����е���һ���ҹ�
    //�ж����ص�����Ϣ�ǲ��Ǹ��Լ����ڵ�����Ǿʹ����ǵĻ��ͼ���������ȥ������ֱ�Ӷ������㶪�˱���զ�� ����Ҳ���ܲ��ñ�Ĵ��ڴ���
    if (pMsg->hwnd != g_m_hwnd) return CallNextHookEx(g_h_hook, nCode, wParam, lParam);
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
    return CallNextHookEx(g_h_hook, nCode, wParam, lParam);
}
