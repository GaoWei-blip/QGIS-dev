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
    //设置应用名称
    QString myAppName = QString::fromLocal8Bit("我的第一个QGIS应用");
    QgsApplication::setApplicationName(myAppName);

    //建立QGIS应用
    QgsApplication a(argc, argv, true);
    QgsApplication::setPrefixPath("D:\QGIS-3.18.3\apps\qgis", true);//注意切换你的路径
    QgsApplication::initQgis();  //初始化QGIS应用

    //创建自己定义的窗体
    gisTest2 w;  //创建一个窗体，类似于Qt
    w.setWindowIcon(QIcon("E:\\GIS development\\QGIS\\gisTest2\\image\\qgis-icon-16x16.png"));//设置窗口图标
    w.show();

    return a.exec();
}
