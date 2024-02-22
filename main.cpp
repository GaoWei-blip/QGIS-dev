//#include "gisTest2.h"
//#include <QtWidgets/QApplication>
//
//int main(int argc, char *argv[])
//{
//    QApplication a(argc, argv);
//    gisTest2 w;
//    w.show();
//    return a.exec();
//}

#include "gisTest2.h"
#include <qgsapplication.h>


int main(int argc, char* argv[])
{
    //����Ӧ������
    QString myAppName = QString::fromLocal8Bit("�ҵĵ�һ��QGISӦ��");
    QgsApplication::setApplicationName(myAppName);

    //����QGISӦ��
    QgsApplication a(argc, argv, true);
    QgsApplication::setPrefixPath("D:\QGIS-3.18.3\apps\qgis", true);//ע���л����·��
    QgsApplication::initQgis();  //��ʼ��QGISӦ��

    //�����Լ�����Ĵ���
    gisTest2 w;  //����һ�����壬������Qt
    w.setWindowIcon(QIcon("E:\\GIS development\\QGIS\\gisTest2\\image\\qgis-icon-16x16.png"));//���ô���ͼ��
    w.show();

    return a.exec();
}
