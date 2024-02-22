//#pragma once
//
//#include <QtWidgets/QMainWindow>
//#include "ui_gisTest2.h"
//
//class gisTest2 : public QMainWindow
//{
//    Q_OBJECT
//
//public:
//    gisTest2(QWidget *parent = Q_NULLPTR);
//
//private:
//    Ui::gisTest2Class ui;
//};

#include <QtWidgets/QMainWindow>
#include "ui_gisTest2.h"
#include <qmenu.h>
#include <qaction.h>
#include <qgsmapcanvas.h>                  //��ͼ����
#include <qgslayertreeview.h>              //ͼ�������
#include <qgslayertreemapcanvasbridge.h>   //���ӻ����͵�ͼ������
#include <qgslayertreemodel.h>             //��ʼ��ͼ�������
#include <qlabel.h>                        //״̬��
#include <qlineedit.h>
#include <qgsscalecombobox.h>
#include <qtextbrowser.h>                  //��ʾ�ı�
#include <qdockwidget.h>




class gisTest2 : public QMainWindow
{
    Q_OBJECT

public:
    gisTest2(QWidget* parent = Q_NULLPTR);
    ~gisTest2();

    
    //! ���ֵ�һʵ��
    //static inline gisTest2* instance() { return sm_instance; }
    static gisTest2* instance() { return sm_instance; }
    

private:
    static gisTest2* sm_instance;

    // create the menus and then add the actions to them.
    QMenu* fileMenu;
    QMenu* preprocessingMenu;
    QMenu* functionMenu;
    QMenu* helpMenu;
    QAction* openVectorFileAction;
    QAction* openRasterFileAction;
    QAction* helpAction;
    QAction* ndviAction;

    //map canvas
    QgsMapCanvas* mapCanvas;
    QList<QgsMapLayer*> layers;

    //layer TreeView
    QgsLayerTreeView* layerTreeView;
    QgsLayerTreeMapCanvasBridge* layerTreeCanvasBridge;

    //QGIS����С����
    Ui::gisTest2Class ui;
    QToolBar* mMapToolBar;
    QgsMapTool* mPan;
    QgsMapTool* mIdentify;
    QgsMapTool* mZoomIn;
    QgsMapTool* mZoomOut;

    //״̬��
    QLabel* m_coordsLabel;              //��״̬����ʾ"Coordinate / Extent"
    QLineEdit* m_coordsEdit;            //��״̬����ʾ��������
    QLabel* m_scaleLabel;               //��״̬������ʾ��scale 1:��
    QgsScaleComboBox* m_scaleEdit;      //��״̬������ʾ������ֵ

    //��ʾ�ı�
    QTextBrowser* m_testBrowser;

    QgsLayerTreeMapCanvasBridge* m_bridge;    //ͼ����ƴ����뻭�������Ž���

    QList< QgsMapCanvas* > mapCanvases();

public slots:                                   //�ۺ���
    void on_openVectorFileAction_triggered();
    void on_openRasterFileAction_triggered();
    void on_Pan_triggered();
    void on_ZoomIn_triggered();
    void on_ZoomOut_triggered();
    void on_ZoonFull_triggered();
    void on_help_triggered();
    void slot_autoSelectAddedLayer(const QList<QgsMapLayer*> layers);
    void autoSelectAddedLayer(QList<QgsMapLayer*> layers);
    
    void slot_labelShowAction();    //����ͼ����ʾ��ǩ
    void slot_pointstyle();  //����ʽ�����Կ�
    void slot_polygonstyle();//����ʽ�����Կ�

    //void removeLayer();

    //void refreshMapCanvas(bool redrawAllLayers = false);

    void computerNDVI();

public:
    void addVectorLayer();
    void addRasterLayer();

    void initLayerTreeView();
    
    void createStatusBar();

    void addDockWidget(Qt::DockWidgetArea area, QDockWidget* dockwidget);

    QList<QgsMapCanvas> mapCanvasLayerSet; // ��ͼ�������õ�ͼ�㼯��

};