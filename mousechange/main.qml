import QtQuick 2.6
import QtQuick.Window 2.2
import GetMouseData 1.0

Window {
    Text {
        y:0
        id: text1
        font.pointSize: 26
        text: qsTr("text")
    }
    Text {
        y:240
        id: text2
        font.pointSize: 26
        text: qsTr("text")
    }
    Text {
        y:480
        id: text3
        font.pointSize: 26
        text: qsTr("text")
    }
    Text {
        y:720
        font.pointSize: 26
        id: text4
        text: qsTr("text")
    }
    visible: true
    width: 1000
    height: 1000
    title: qsTr("RawInputDemo2022")
    function buttonDetect(i) {
        if(i === 0)
            return 0;
        else if(i === 1) {
            //�������
            return 1;
        } else if(i === 2) {
            //�������
            return 0;
        } else if(i === 4) {
            //�Ҽ�����
            return 2;
        } else if(i === 8) {
            //�Ҽ�����
            return 0;
        } else return 0;
    }

    function mousePositon(p) {
        var array = ["blue", "yellow", "red"];
        var a = mouseData.getButton(p);
        var x = mouseData.getX(p);
        var y = mouseData.getY(p);
        var k = mouseData.getKind(p);
        if(k === 2) {
            var t = x;
            x = y;
            y = t;
        } else if(k === 4) {
            t = x;
            x = y;
            y = t;
        }

        if(p === 0) {
            //console.log("one", mouseData.getDistance(p));
            mouseOne.x = x;
            mouseOne.y = y;
            mouseOne.color = array[buttonDetect(a)];
        }
        else if (p === 1) {
            //console.log("two", mouseData.getDistance(p));
            mouseTwo.x = x;
            mouseTwo.y = y;
            mouseTwo.color = array[buttonDetect(a)];
        }
        else if (p === 2) {
            //console.log("three", mouseData.getDistance(p));
            mouseThree.x = x;
            mouseThree.y = y;
            mouseThree.color = array[buttonDetect(a)];
        }
        else if (p === 3) {
            //console.log("four", mouseData.getDistance(p));
            mouseFour.x = x;
            mouseFour.y = y;
            mouseFour.color = array[buttonDetect(a)];
        }
    }
    function txtchange(p) {
        var s =  mouseData.getKeyString(p);
        //console.log(s, p);
        if(p === 0) text1.text = s;
        else if(p === 1) text2.text = s;
        else if(p === 2) text3.text = s;
        else if(p === 3) text4.text = s;
    }

    function fuck() {
        for(var p = 0; p < 4; p++) {
            mousePositon(p);
            txtchange(p);

        }
    }
    GetMouseData {

        id: mouseData
        Component.onCompleted: {
            init();
        }
        onMouse: {
            fuck();
        }
        onCountOne: {
            mouseOne.x = 0;
            mouseOne.y = 0;
        }
        onCountTwo: {
            mouseTwo.x = 0;
            mouseTwo.y = 0;
        }
        onCountThree: {
            mouseThree.x = 0;
            mouseThree.y = 0;
        }
        onCountFour: {
            mouseFour.x = 0;
            mouseFour.y = 0;
        }
    }
    //��һ�����
    Rectangle {
        x: 1280;
        y: 720;
        id:mouseOne
        width: 20;            // ��ȣ���ʱRectangle�ؼ���Window�ؼ��ڣ�����Window�ĺ��ӣ�����ʹ��parent�̳�
        height: 20;           // �߶ȣ���ʱRectangle�ؼ���Window�ؼ��ڣ�����Window�ĺ��ӣ�����ʹ��parent
        color: "blue";         // ��ɫ��ȱʡΪ
        opacity: 0.5;          // ͸���ȣ�ȱʡΪ1
        radius: 100;           // Բ�ǣ�ͨ��Բ��������һ��Բ�δ������
        Text {
            id:textOne;
            anchors.centerIn: parent;
            text: qsTr("1")
        }
    }
    //�ڶ������
    Rectangle {
        x: 1280;
        y: 720;
        id:mouseTwo
        width: 20;            // ��ȣ���ʱRectangle�ؼ���Window�ؼ��ڣ�����Window�ĺ��ӣ�����ʹ��parent�̳�
        height: 20;           // �߶ȣ���ʱRectangle�ؼ���Window�ؼ��ڣ�����Window�ĺ��ӣ�����ʹ��parent
        color: "green";         // ��ɫ��ȱʡΪ��ɫ
        opacity: 0.5;          // ͸���ȣ�ȱʡΪ1
        radius: 100;           // Բ�ǣ�ͨ��Բ��������һ��Բ�δ������
        Text {
            id:textTwo
            anchors.centerIn: parent;
            text: qsTr("2")
        }
    }
    //���������
    Rectangle {
        x: 1280;
        y: 720;
        id:mouseThree
        width: 20;            // ��ȣ���ʱRectangle�ؼ���Window�ؼ��ڣ�����Window�ĺ��ӣ�����ʹ��parent�̳�
        height: 20;           // �߶ȣ���ʱRectangle�ؼ���Window�ؼ��ڣ�����Window�ĺ��ӣ�����ʹ��parent
        color: "red";         // ��ɫ��ȱʡΪ��ɫ
        opacity: 0.5;          // ͸���ȣ�ȱʡΪ1
        radius: 100;           // Բ�ǣ�ͨ��Բ��������һ��Բ�δ������
        Text {
            id:textThree
            anchors.centerIn: parent;
            text: qsTr("3")
        }
    }
    //���ĸ����
    Rectangle {
        x: 1280;
        y: 720;
        id:mouseFour
        width: 20;            // ��ȣ���ʱRectangle�ؼ���Window�ؼ��ڣ�����Window�ĺ��ӣ�����ʹ��parent�̳�
        height: 20;           // �߶ȣ���ʱRectangle�ؼ���Window�ؼ��ڣ�����Window�ĺ��ӣ�����ʹ��parent
        color: "yellow";         // ��ɫ��ȱʡΪ��ɫ
        opacity: 0.5;          // ͸���ȣ�ȱʡΪ1
        radius: 100;           // Բ�ǣ�ͨ��Բ��������һ��Բ�δ������
        Text {
            id:textFour
            anchors.centerIn: parent;
            text: qsTr("4")
        }
    }
}

