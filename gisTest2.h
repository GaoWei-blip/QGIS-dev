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
#include <qgsmapcanvas.h>                  //地图画布
#include <qgslayertreeview.h>              //图层管理器
#include <qgslayertreemapcanvasbridge.h>   //连接画布和地图管理器
#include <qgslayertreemodel.h>             //初始化图层管理器
#include <qlabel.h>                        //状态栏
#include <qlineedit.h>
#include <qgsscalecombobox.h>
#include <qtextbrowser.h>                  //显示文本
#include <qdockwidget.h>




class gisTest2 : public QMainWindow
{
    Q_OBJECT

public:
    gisTest2(QWidget* parent = Q_NULLPTR);
    ~gisTest2();

    
    //! 保持单一实例
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

    //QGIS交互小工具
    Ui::gisTest2Class ui;
    QToolBar* mMapToolBar;
    QgsMapTool* mPan;
    QgsMapTool* mIdentify;
    QgsMapTool* mZoomIn;
    QgsMapTool* mZoomOut;

    //状态栏
    QLabel* m_coordsLabel;              //在状态栏显示"Coordinate / Extent"
    QLineEdit* m_coordsEdit;            //在状态栏显示地理坐标
    QLabel* m_scaleLabel;               //在状态栏中显示“scale 1:”
    QgsScaleComboBox* m_scaleEdit;      //在状态栏中显示比例尺值

    //显示文本
    QTextBrowser* m_testBrowser;

    QgsLayerTreeMapCanvasBridge* m_bridge;    //图层控制窗口与画布关联桥接器

    QList< QgsMapCanvas* > mapCanvases();

public slots:                                   //槽函数
    void on_openVectorFileAction_triggered();
    void on_openRasterFileAction_triggered();
    void on_Pan_triggered();
    void on_ZoomIn_triggered();
    void on_ZoomOut_triggered();
    void on_ZoonFull_triggered();
    void on_help_triggered();
    void slot_autoSelectAddedLayer(const QList<QgsMapLayer*> layers);
    void autoSelectAddedLayer(QList<QgsMapLayer*> layers);
    
    void slot_labelShowAction();    //设置图层显示标签
    void slot_pointstyle();  //点样式窗口显控
    void slot_polygonstyle();//面样式窗口显控

    //void removeLayer();

    //void refreshMapCanvas(bool redrawAllLayers = false);

    void computerNDVI();

public:
    void addVectorLayer();
    void addRasterLayer();

    void initLayerTreeView();
    
    void createStatusBar();

    void addDockWidget(Qt::DockWidgetArea area, QDockWidget* dockwidget);

    QList<QgsMapCanvas> mapCanvasLayerSet; // 地图画布所用的图层集合

};