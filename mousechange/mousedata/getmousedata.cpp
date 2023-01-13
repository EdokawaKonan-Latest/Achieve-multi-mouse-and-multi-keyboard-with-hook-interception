#pragma once
#include <Header/getmousedata.h>
#include <Header/dllmain.h>

//����Dll���
HMODULE hDll = NULL;
HWND hWnd = NULL;
const char * title ="RawInputDemo2022";
bool add[6];
//�������Լ������ʼ���ź�
void GetMouseData::countAdd(int i) {
    if(add[i]) return;
    else {
        if(i == 1) {
            emit countOne();
            add[i] = true;
        }
        else if(i == 2) {
            emit countTwo();
            add[i] = true;
        } else if(i == 3) {
            emit countThree();
            add[i] = true;
        } else if(i == 4) {
            emit countFour();
            add[i] = true;
        }
    }
}
//���߳�run
void GetMouseData::run() {
    do {
        hWnd = ::FindWindowA(NULL, title);//�ҵ�qml���� ���ҹ�ס
        Sleep(1);
    } while(hWnd==GetDesktopWindow());//ֻҪ��windows����,�Ҿ�һֱ�Ҿ�����
    //�����Ϣ��ע��
    RawRegister(hWnd);
    while(1) {
        //���������Ϣ
        countAdd(mouseCount());
//        qDebug() << getKeyString(0);
        emit mouse();
        Sleep(10);
    }
}
GetMouseData::GetMouseData() {}
//��ȡ��갴����Ϣ
unsigned long long GetMouseData::getButton(int p) {
    return  GetMouseButton(p);
}
unsigned short GetMouseData::getDistance(int p) {
    return GetDistance(p);
}
//��ȡ���x����
int GetMouseData::getX(int p) {
    return GetData(p, 1);
}
//��ȡ���y����
int GetMouseData::getY(int p) {
    return GetData(p, 0);
}
//��ȡ����豸����
int GetMouseData::mouseCount() {
    return GetMouseCount();
}
//�������1״̬
int GetMouseData::setMouse(int id, int i) {
    if(i == 1) { //���id   ��i��״̬
        PositiveDirectionX(id);
        PositiveDirectionY(id);
        return 1;
    }
    else if(i == 2) { //
        PositiveDirectionX(id);
        OppositeDirectionY(id);
        return 2;
    }
    else if(i == 3) {
        OppositeDirectionX(id);
        OppositeDirectionY(id);
        return 3;
    }
    else if(i == 4) {
        OppositeDirectionX(id);
        PositiveDirectionY(id);
        return 4;
    }
}
int arr[10];
int GetMouseData::getKind(int id) { //��ȡ�������
    return arr[id];
}

QString GetMouseData::getKeyString(int p) {
    return GetKeyString(p);
}
//��������ƶ�����
void GetMouseData::init() {
    KeyValueInit();
    arr[0] = setMouse(0, 1);
    arr[1] = setMouse(1, 1);
    arr[2] = setMouse(2, 1);
    arr[3] = setMouse(3, 1);
    this->start();
}
