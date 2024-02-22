//#include "gisTest2.h"
//
//gisTest2::gisTest2(QWidget *parent)
//    : QMainWindow(parent)
//{
//    ui.setupUi(this);
//}

#include "gisTest2.h"
#include "gisTest2layertreeviewmenuprovider.h"

#include <qmenubar.h>                      //菜单栏
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qgsvectorlayer.h>                //矢量图层
#include <qgsrasterlayer.h>                //栅格图层
#include <qgsmaptoolpan.h>                 //漫游工具
#include <qgsmaptoolidentify.h>            //识别工具
#include <qgsmaptoolzoom.h>                //缩放工具
#include <qgsmaptool.h>                    //漫游、缩放、全图等工具
#include <qgsproject.h>                    //添加栅格矢量图时注册
#include <qgridlayout.h>                   //布局
#include <qdesktopservices.h>              //帮助打开url
#include <qgslayertreeregistrybridge.h>    //图层管理器右键connect发送者所需
#include <qgspallabeling.h>
#include <qgsvectorlayerlabeling.h>
#include "qgstaskmanager.h"
#include "qgsapplication.h"
#include "gdal_priv.h"                    //apps路径下下载的gdal库（通过osgeow下载，并复制到此路径下）
#include <qtextcodec.h>
#include <qprogressdialog.h>


gisTest2* gisTest2::sm_instance = nullptr;

//自定义窗体类gisTest2构造函数
gisTest2::gisTest2(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    //this->resize(600, 400);
    this->resize(1200, 800);

    //创建文件菜单栏 并添加行为 
    fileMenu = this->menuBar()->addMenu("File");

    openVectorFileAction = new QAction("Open Vector", this);
    this->connect(openVectorFileAction, SIGNAL(triggered(bool)), this, SLOT(on_openVectorFileAction_triggered()));//连接槽函数和信号
    fileMenu->addAction(openVectorFileAction);

    openRasterFileAction = new QAction("Open Raster", this);
    this->connect(openRasterFileAction, SIGNAL(triggered(bool)), this, SLOT(on_openRasterFileAction_triggered()));
    fileMenu->addAction(openRasterFileAction);

    //预处理菜单栏
    preprocessingMenu = this->menuBar()->addMenu("Preprocessing");

    //功能菜单栏
    functionMenu = this->menuBar()->addMenu("function");

    ndviAction = new QAction("NDVI", this);
    this->connect(ndviAction, SIGNAL(triggered(bool)), this, SLOT(computerNDVI()));//连接槽函数和信号
    functionMenu->addAction(ndviAction);


    //帮助菜单栏
    helpMenu = this->menuBar()->addMenu("help");
    helpAction = new QAction("help", this);
    this->connect(helpAction, SIGNAL(triggered(bool)), this, SLOT(on_help_triggered()));//连接槽函数和信号
    helpMenu->addAction(helpAction);
  
    //---------------------------------------------

    //创建工具栏并添加按钮
    mMapToolBar = addToolBar(tr("Tools"));
    //mMapToolBar->addSeparator();
    mMapToolBar->addAction(ui.mActionPan);
    mMapToolBar->addAction(ui.mActionZoomIn);
    mMapToolBar->addAction(ui.mActionZoomOut);
    mMapToolBar->addAction(ui.mActionZoomFull);

    //给工具栏按钮添加行为
    mPan = new QgsMapToolPan(mapCanvas);
    mPan->setAction(ui.mActionPan);
    connect(ui.mActionPan, SIGNAL(triggered()), this, SLOT(on_Pan_triggered()));

    mZoomIn = new QgsMapToolZoom(mapCanvas, false);
    mZoomIn->setAction(ui.mActionZoomIn);
    connect(ui.mActionZoomIn, SIGNAL(triggered()), this, SLOT(on_ZoonIn_triggered()));

    mZoomOut = new QgsMapToolZoom(mapCanvas, true);
    mZoomOut->setAction(ui.mActionZoomOut);
    connect(ui.mActionZoomOut, SIGNAL(triggered()), this, SLOT(on_ZoonOut_triggered()));

    connect(ui.mActionZoomFull, SIGNAL(triggered()), this, SLOT(on_ZoonFull_triggered()));

    //---------------------------------------------
    
    //初始化地图画布
    mapCanvas = new QgsMapCanvas();
    //this->setCentralWidget(mapCanvas);

    mapCanvas->setCanvasColor(QColor(255, 255, 255));
    mapCanvas->setVisible(true);
    mapCanvas->enableAntiAliasing(true);

    //---------------------------------------------
    
    //初始化图层管理器
    layerTreeView = new QgsLayerTreeView(this);
    

    //文本框
    m_testBrowser = new QTextBrowser(this);

    //栅格布局
    QWidget* centralWidget = this->centralWidget();
    QGridLayout* centralLayout = new QGridLayout(centralWidget);
    centralLayout->addWidget(mapCanvas, 1, 1, 2, 2);                     //画布位置 初始行列 占位行类
    //centralLayout->addWidget(layerTreeView, 0, 0, 4, 1);                 //图层管理器位置
    centralLayout->addWidget(m_testBrowser, 3, 1, 1, 1);                 //交互文本信息位置


    initLayerTreeView();
  
    QStatusBar* pStatusBar = new QStatusBar(); 
    setStatusBar(pStatusBar); 
    createStatusBar();
}

//构造函数结束----------------------------------------------------------------------------------------------------------------------------------------

//析构函数
gisTest2::~gisTest2()
{

}

//-------------------------------------------------------------------------------------------

//打开矢量数据功能
void gisTest2::on_openVectorFileAction_triggered() {
    
    addVectorLayer();
}

void gisTest2::addVectorLayer()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open shape file"), "", "*.shp");
    if (fileName.isNull()) //如果文件未选择则返回
    {
        return;
    }
    else {
        QString& temptext = QString::fromLocal8Bit(">> 打开了一个矢量数据");
        m_testBrowser->append(temptext);
    }

    QStringList temp = fileName.split('/');
    QString basename = temp.at(temp.size() - 1);
    QgsVectorLayer* vecLayer = new QgsVectorLayer(fileName, basename, "ogr");

    if (!vecLayer->isValid())
    {
        QMessageBox::critical(this, "error", QString("layer is invalid: \n") + fileName);
        return;
    } 

    //显示标签
    QgsPalLayerSettings layerSettings;
    layerSettings.drawLabels = true;
    layerSettings.fieldName = vecLayer->fields()[0].name();
    layerSettings.isExpression = false;
    layerSettings.placement = QgsPalLayerSettings::OverPoint;
    layerSettings.yOffset = 2.50;

    QgsTextBufferSettings buffersettings;
    buffersettings.setEnabled(false);
    buffersettings.setSize(1);
    buffersettings.setColor(QColor());

    QgsTextFormat format;
    QFont font("SimSun", 5, 5, false);
    font.setUnderline(false);
    format.setFont(font);
    format.setBuffer(buffersettings);
    layerSettings.setFormat(format);


    QgsVectorLayerSimpleLabeling* labeling = new QgsVectorLayerSimpleLabeling(layerSettings);
    vecLayer->setLabeling(labeling);
    vecLayer->setLabelsEnabled(true);

    //QGIS 3注册方式
    QgsProject::instance()->addMapLayer(vecLayer);

    mapCanvas->setExtent(vecLayer->extent());
    layers.append(vecLayer);
    mapCanvas->setLayers(layers);
    mapCanvas->setVisible(true);
    mapCanvas->freeze(false);
    mapCanvas->refresh();
}

//-------------------------------------------------------------------------------------------

//打开栅格数据功能
void gisTest2::on_openRasterFileAction_triggered() {
    addRasterLayer();
}

void gisTest2::addRasterLayer()
{
    //步骤1：打开文件选择对话框
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open raster file"), "", "remote sensing image(*.tif *.tiff);;image(*.jpg *.jpeg *.png *.bmp)");
    if (fileName.isNull()) //如果文件未选择则返回
    {
        return;
    }
    else {
        QString& temptext = QString::fromLocal8Bit(">> 打开了一个栅格数据");
        m_testBrowser->append(temptext);
    }

    QStringList temp = fileName.split('/');
    QString basename = temp.at(temp.size() - 1);//获取栅格数据名称

    //步骤2：创建QgsRasterLayer类
    QgsRasterLayer* rasterLayer = new QgsRasterLayer(fileName, basename, "gdal");
    if (!rasterLayer->isValid())
    {
        QMessageBox::critical(this, "error", QString("layer is invalid: \n") + fileName);
        return;
    }

    m_testBrowser->append(rasterLayer->bandName(1));
    m_testBrowser->append(fileName);


    //步骤3：添加栅格数据
    QgsProject::instance()->addMapLayer(rasterLayer); //注册

    mapCanvas->setExtent(rasterLayer->extent());
    layers.append(rasterLayer);
    mapCanvas->setLayers(layers);
    mapCanvas->setVisible(true);
    mapCanvas->freeze(false);
    mapCanvas->refresh();
}

//-------------------------------------------------------------------------------------------

//帮助菜单栏功能
void gisTest2::on_help_triggered()
{
    QDesktopServices::openUrl(QUrl(QLatin1String("https://blog.csdn.net")));
}

//-------------------------------------------------------------------------------------------

//工具栏 漫游
void gisTest2::on_Pan_triggered()
{

    mapCanvas->setMapTool(mPan);

    /*if (mapCanvas->layer(0)->type() == nullptr)
    {
        return;
    }
    QgsVectorLayer* player = (QgsVectorLayer*)mapCanvas->layer(0);
    player->removeSelection();*/
}

//工具栏 放大
void gisTest2::on_ZoomIn_triggered()
{
    //释放上一次使用的工具
    QgsMapTool* lastMapTool = mapCanvas->mapTool();
    mapCanvas->unsetMapTool(lastMapTool);

    //实现功能
    mapCanvas->setMapTool(mZoomIn);
}

//工具栏 缩小
void gisTest2::on_ZoomOut_triggered()
{
    QgsMapTool* lastMapTool = mapCanvas->mapTool();
    mapCanvas->unsetMapTool(lastMapTool);

    mapCanvas->setMapTool(mZoomOut);

}

//工具栏 全图显示
void gisTest2::on_ZoonFull_triggered()
{
    QgsMapTool* lastMapTool = mapCanvas->mapTool();
    mapCanvas->unsetMapTool(lastMapTool);

    mapCanvas->zoomToFullExtent();
}

//-------------------------------------------------------------------------------------------

//图层管理器
void gisTest2::initLayerTreeView()
{
    QgsLayerTreeModel* model = new QgsLayerTreeModel(QgsProject::instance()->layerTreeRoot(), this);
    model->setFlag(QgsLayerTreeModel::AllowNodeRename);
    model->setFlag(QgsLayerTreeModel::AllowNodeReorder);
    model->setFlag(QgsLayerTreeModel::AllowNodeChangeVisibility);
    model->setFlag(QgsLayerTreeModel::ShowLegendAsTree);
    model->setAutoCollapseLegendNodes(10);
    layerTreeView->setModel(model);
    layerTreeView->setFixedWidth(200);

    //图层管理器添加右键菜单
    //layerTreeView->setMenuProvider(new gisTest2LayerTreeViewMenuProvider(layerTreeView, mapCanvas));
    //connect(QgsProject::instance()->layerTreeRegistryBridge(), SIGNAL(addedLayersToLayerTree(QList<QgsMapLayer*>)), this, SLOT(autoSelectAddedLayer(QList<QgsMapLayer*>)));
    
    //m_bridge = new QgsLayerTreeMapCanvasBridge(QgsProject::instance()->layerTreeRoot(), mapCanvas, this);
    //connect(m_bridge, &QgsLayerTreeMapCanvasBridge::canvasLayersChanged, mapCanvas, &QgsMapCanvas::setLayers);
    
    //右键菜单
    layerTreeView->setMenuProvider(new gisTest2LayerTreeViewMenuProvider(layerTreeView, mapCanvas));
    //connect(QgsProject::instance()->layerTreeRegistryBridge(), SIGNAL(addedLayersToLayerTree(const QList<QgsMapLayer*>)), this, 
    //    SLOT(slot_autoSelectAddedLayer(const QList<QgsMapLayer*>)));
    connect(QgsProject::instance()->layerTreeRegistryBridge(), &QgsLayerTreeRegistryBridge::addedLayersToLayerTree, this,
        &gisTest2::slot_autoSelectAddedLayer);

    

    // 设置这个路径是为了获取图标文件
    QString iconDir = "E:\\GIS development\\QGIS\\gisTest2\\image\\"; //中文路径图标会不显示

    // add group tool button
    QToolButton* btnAddGroup = new QToolButton();
    btnAddGroup->setAutoRaise(true);
    btnAddGroup->setIcon(QIcon(iconDir + "layer-group-add.png"));
    btnAddGroup->setFixedSize(24, 24);
    btnAddGroup->setToolTip(tr("Add Group"));
    connect(btnAddGroup, SIGNAL(clicked()), layerTreeView->defaultActions(), SLOT(addGroup()));

    // expand / collapse tool buttons
    QToolButton* btnExpandAll = new QToolButton();
    btnExpandAll->setAutoRaise(true);
    btnExpandAll->setIcon(QIcon(iconDir + "mActionExpandTree.png"));
    btnAddGroup->setFixedSize(24, 24);
    btnExpandAll->setToolTip(tr("Expand All"));
    connect(btnExpandAll, SIGNAL(clicked()), layerTreeView, SLOT(expandAll()));

    QToolButton* btnCollapseAll = new QToolButton();
    btnCollapseAll->setAutoRaise(true);
    btnCollapseAll->setIcon(QIcon(iconDir + "mActionCollapseTree.png"));
    btnAddGroup->setFixedSize(24, 24);
    btnCollapseAll->setToolTip(tr("Collapse All"));
    connect(btnCollapseAll, SIGNAL(clicked()), layerTreeView, SLOT(collapseAll()));

    // remove item button
    QToolButton* btnRemoveItem = new QToolButton();
    // btnRemoveItem->setDefaultAction( this->m_actionRemoveLayer );
    btnRemoveItem->setAutoRaise(true);
    btnRemoveItem->setIcon(QIcon(iconDir + "layer-remove.png"));
    btnAddGroup->setFixedSize(24, 24);
    btnRemoveItem->setToolTip(tr("Remove"));
    connect(btnRemoveItem, SIGNAL(clicked()), layerTreeView, SLOT(remove()));

    // 按钮布局
    QHBoxLayout* toolbarLayout = new QHBoxLayout();
    toolbarLayout->setContentsMargins(QMargins(5, 0, 5, 0));
    toolbarLayout->addWidget(btnAddGroup);
    toolbarLayout->addWidget(btnCollapseAll);
    toolbarLayout->addWidget(btnExpandAll);
    toolbarLayout->addWidget(btnRemoveItem);
    toolbarLayout->addStretch();

    QVBoxLayout* vboxLayout = new QVBoxLayout();
    vboxLayout->setMargin(0);
    vboxLayout->addLayout(toolbarLayout);
    vboxLayout->addWidget(layerTreeView);

    // 装进dock widget中
    QDockWidget* m_layerTreeDock = new QDockWidget(tr("Layers"),this);
    m_layerTreeDock->setObjectName("Layers");
    m_layerTreeDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    QWidget* w = new QWidget();
    w->setLayout(vboxLayout);
    m_layerTreeDock->setWidget(w);
    addDockWidget(Qt::LeftDockWidgetArea, m_layerTreeDock);        //初始位置


    // 连接地图画布和图层管理器
    layerTreeCanvasBridge = new QgsLayerTreeMapCanvasBridge(QgsProject::instance()->layerTreeRoot(), mapCanvas, this);
    connect(QgsProject::instance(), SIGNAL(writeProject(QDomDocument&)),
        layerTreeCanvasBridge, SLOT(writeProject(QDomDocument&)));
    connect(QgsProject::instance(), SIGNAL(readProject(QDomDocument)),
        layerTreeCanvasBridge, SLOT(readProject(QDomDocument)));



}

//图层控制器树结构变化
void gisTest2::slot_autoSelectAddedLayer(const QList<QgsMapLayer*> layers)
{
    if (!layers.isEmpty())
    {
        QgsLayerTreeLayer* nodeLayer = QgsProject::instance()->layerTreeRoot()->findLayer(layers[0]->id());

        if (!nodeLayer)
            return;

        QModelIndex index = layerTreeView->layerTreeModel()->node2index(nodeLayer);
        layerTreeView->setCurrentIndex(index);
    }
}

void gisTest2::autoSelectAddedLayer(QList<QgsMapLayer*> layers)
{
    //if (layers.count())
    if (!layers.isEmpty())
    {
        QgsLayerTreeLayer* nodeLayer = QgsProject::instance()->layerTreeRoot()->findLayer(layers[0]->id());

        if (!nodeLayer)
        {
            return;
        }

        QModelIndex index = layerTreeView->layerTreeModel()->node2index(nodeLayer);
        layerTreeView->setCurrentIndex(index);
    }
}

void gisTest2::addDockWidget(Qt::DockWidgetArea area, QDockWidget* dockwidget)
{
    QMainWindow::addDockWidget(area, dockwidget);
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    dockwidget->show();
    mapCanvas->refresh();
}

//设置图层显示标签
void gisTest2::slot_labelShowAction()
{//具体实现在下面的小节中说明
}
//点样式-右键菜单
void gisTest2::slot_pointstyle()
{//具体实现在下面的小节中说明
}
//面样式窗口显控-右键菜单
void gisTest2::slot_polygonstyle()
{//具体实现在下面的小节中说明
}

//void gisTest2::removeLayer()
//{
//    if (!layerTreeView)
//    {
//        return;
//    }
//
//    // look for layers recursively so we catch also those that are within selected groups
//    const QList<QgsMapLayer*> selectedLayers = layerTreeView->selectedLayersRecursive();
//
//    QStringList nonRemovableLayerNames;
//    for (QgsMapLayer* layer : selectedLayers)
//    {
//        if (!layer->flags().testFlag(QgsMapLayer::Removable))
//            nonRemovableLayerNames << layer->name();
//    }
//    if (!nonRemovableLayerNames.isEmpty())
//    {
//        QMessageBox::warning(this, tr("Required Layers"),
//            tr("The following layers are marked as required by the project:\n\n%1\n\nPlease deselect them (or unmark as required) and retry.").arg(nonRemovableLayerNames.join(QLatin1Char('\n'))));
//        return;
//    }
//
//    /*for (QgsMapLayer* layer : selectedLayers)
//    {
//        QgsVectorLayer* vlayer = qobject_cast<QgsVectorLayer*>(layer);
//        if (vlayer && vlayer->isEditable() && !toggleEditing(vlayer, true))
//            return;
//    }*/
//
//    QStringList activeTaskDescriptions;
//    for (QgsMapLayer* layer : selectedLayers)
//    {
//        QList< QgsTask* > tasks = QgsApplication::taskManager()->tasksDependentOnLayer(layer);
//        if (!tasks.isEmpty())
//        {
//            const auto constTasks = tasks;
//            for (QgsTask* task : constTasks)
//            {
//                activeTaskDescriptions << tr(" • %1").arg(task->description());
//            }
//        }
//    }
//
//    if (!activeTaskDescriptions.isEmpty())
//    {
//        QMessageBox::warning(this, tr("Active Tasks"),
//            tr("The following tasks are currently running which depend on this layer:\n\n%1\n\nPlease cancel these tasks and retry.").arg(activeTaskDescriptions.join(QLatin1Char('\n'))));
//        return;
//    }
//
//    QList<QgsLayerTreeNode*> selectedNodes = layerTreeView->selectedNodes(true);
//
//    ////validate selection
//    //if (selectedNodes.isEmpty())
//    //{
//    //    visibleMessageBar()->pushMessage(tr("No legend entries selected"),
//    //        tr("Select the layers and groups you want to remove in the legend."),
//    //        Qgis::Info, messageTimeout());
//    //    return;
//    //}
//
//    bool promptConfirmation = QgsSettings().value(QStringLiteral("qgis/askToDeleteLayers"), true).toBool();
//
//    // Don't show prompt to remove a empty group.
//    if (selectedNodes.count() == 1
//        && selectedNodes.at(0)->nodeType() == QgsLayerTreeNode::NodeGroup
//        && selectedNodes.at(0)->children().count() == 0)
//    {
//        promptConfirmation = false;
//    }
//
//    bool shiftHeld = QApplication::queryKeyboardModifiers().testFlag(Qt::ShiftModifier);
//    //display a warning
//    if (!shiftHeld && promptConfirmation && QMessageBox::warning(this, tr("Remove layers and groups"), tr("Remove %n legend entries?", "number of legend items to remove", selectedNodes.count()), QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel)
//    {
//        return;
//    }
//
//    const auto constSelectedNodes = selectedNodes;
//    for (QgsLayerTreeNode* node : constSelectedNodes)
//    {
//        QgsLayerTreeGroup* parentGroup = qobject_cast<QgsLayerTreeGroup*>(node->parent());
//        if (parentGroup)
//            parentGroup->removeChildNode(node);
//    }
//
//    //showStatusMessage(tr("%n legend entries removed.", "number of removed legend entries", selectedNodes.count()));
//
//    refreshMapCanvas();
//}

//void gisTest2::refreshMapCanvas(bool redrawAllLayers)
//{
//    const auto canvases = mapCanvases();
//    for (QgsMapCanvas* canvas : canvases)
//    {
//        //stop any current rendering
//        canvas->stopRendering();
//        if (redrawAllLayers)
//            canvas->refreshAllLayers();
//        else
//            canvas->refresh();
//    }
//}
//-------------------------------------------------------------------------------------------

//添加状态栏
void gisTest2::createStatusBar()
{
    statusBar()->setStyleSheet("QStatusBar::item {border: none;}");

    ////! 添加进度条  没有QProcessBar这个类
    //m_progressBar = new QProcessBar(statusBar());
    //m_progressBar->setObjectName("m_progressBar");
    //m_progressBar->setMaximum(100);
    //m_progressBar->hide();
    //statusBar()->addPermanentWidget(m_progressBar, 1);
    //connect(mapCanvas, SIGNAL(renderStarting()), this, SLOT(canvasRefreshStarted()));
    //connect(mapCanvas, SIGNAL(mapCanvasRefreshed()), this, SLOT(canvasRefreshFinished()));

    QFont myFont("Arial", 9);
    statusBar()->setFont(myFont);

    //! 添加坐标显示标签
    m_coordsLabel = new QLabel(QString(), statusBar());
    m_coordsLabel->setObjectName("m_coordsLabel");
    m_coordsLabel->setFont(myFont);
    m_coordsLabel->setMinimumWidth(10);
    m_coordsLabel->setMargin(3);
    m_coordsLabel->setAlignment(Qt::AlignCenter);
    m_coordsLabel->setFrameStyle(QFrame::NoFrame);
    m_coordsLabel->setText(tr("Coordinate:"));
    m_coordsLabel->setToolTip(tr("Current map coordinate"));
    statusBar()->addPermanentWidget(m_coordsLabel, 0);

    //! 坐标编辑标签
    m_coordsEdit = new QLineEdit(QString(), statusBar());
    m_coordsEdit->setObjectName("m_coordsEdit");
    m_coordsEdit->setFont(myFont);
    m_coordsEdit->setMinimumWidth(10);
    m_coordsEdit->setMaximumWidth(300);
    m_coordsEdit->setContentsMargins(0, 0, 0, 0);
    m_coordsEdit->setAlignment(Qt::AlignCenter);
    statusBar()->addPermanentWidget(m_coordsEdit, 0);
    connect(m_coordsEdit, SIGNAL(returnPressed()), this, SLOT(userCenter()));

    //! 比例尺标签
    m_scaleLabel = new QLabel(QString(), statusBar());
    m_scaleLabel->setObjectName("m_scaleLabel");
    m_scaleLabel->setFont(myFont);
    m_scaleLabel->setMinimumWidth(10);
    m_scaleLabel->setMargin(3);
    m_scaleLabel->setAlignment(Qt::AlignCenter);
    m_scaleLabel->setFrameStyle(QFrame::NoFrame);
    m_scaleLabel->setText(tr("Scale"));
    m_scaleLabel->setToolTip(tr("Current map scale"));
    statusBar()->addPermanentWidget(m_scaleLabel, 0);

    //! 比例尺编辑
    m_scaleEdit = new QgsScaleComboBox(statusBar());
    m_scaleEdit->setObjectName("m_scaleEdit");
    m_scaleEdit->setFont(myFont);
    m_scaleEdit->setMinimumWidth(10);
    m_scaleEdit->setMaximumWidth(100);
    m_scaleEdit->setContentsMargins(0, 0, 0, 0);
    m_scaleEdit->setToolTip(tr("Current map scale (formatted as x:y)"));
    statusBar()->addPermanentWidget(m_scaleEdit, 0);
    connect(m_scaleEdit, SIGNAL(scaleChanged()), this, SLOT(userScale()));

}

//-------------------------------------------------------------------------------------------

//计算NDVI功能
//输入的影像波段3为红光波段、4波段为近红外

void gisTest2::computerNDVI()
{
    QTextCodec* coder = QTextCodec::codecForName("GBK");//QT对中文的支持

    //步骤1：打开文件选择对话框
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open raster file"), "", "remote sensing image(*.tif *.tiff);;image(*.jpg *.jpeg *.png *.bmp)");
    if (fileName.isNull()) //如果文件未选择则返回
    {
        return;
    }

    QStringList temp = fileName.split('/');
    QString basename = temp.at(temp.size() - 1);//获取栅格数据名称

    //步骤2：创建QgsRasterLayer类
    QgsRasterLayer* rasterLayer = new QgsRasterLayer(fileName, basename, "gdal");
    if (!rasterLayer->isValid())
    {
        QMessageBox::critical(this, "error", QString("layer is invalid: \n") + fileName);
        return;
    }

    else if (rasterLayer != nullptr)
    {
        //生成临时内存文件
        GDALDriver* driver = GetGDALDriverManager()->GetDriverByName("GTiff");
        GDALDataset* ndviGdalDs = driver->Create("ndvi.tif",
            rasterLayer->width(),
            rasterLayer->height(),
            1,
            GDT_Int16, 0);//创建输出数据，该数据在内存中创建。
        if (ndviGdalDs == 0)
        {
            QMessageBox::information(0, coder->toUnicode("出错了"), coder->toUnicode("创建内存栅格文件失败."));
            return;
        }


        //坐标系统设置
        double geoTrans[6];
        geoTrans[0] = rasterLayer->dataProvider()->extent().xMinimum();
        geoTrans[1] = rasterLayer->rasterUnitsPerPixelX();
        geoTrans[2] = 0;
        geoTrans[3] = rasterLayer->dataProvider()->extent().yMaximum();
        geoTrans[4] = 0;
        geoTrans[5] = -rasterLayer->rasterUnitsPerPixelY();
        ndviGdalDs->SetGeoTransform(geoTrans);//设置geoTrans
        QString crsString = rasterLayer->crs().toWkt();//获取输入数据坐标系字符串
        ndviGdalDs->SetProjection(crsString.toStdString().c_str());//设置坐标系字符串
        short* outBuffer = new short[rasterLayer->width() * rasterLayer->height()];

        //GDALDataset* testRasterLayer = (GDALDataset*)GDALOpen("./xiayou_2014.tif", GA_ReadOnly);
        QgsRasterDataProvider* inputProvider = rasterLayer->dataProvider();
        QgsRectangle rect = inputProvider->extent(); //空间坐标范围
        int width = inputProvider->xSize(); //影像完整宽度
        int height = inputProvider->ySize();//影像完整高度
        QgsRasterBlock* blockRed = inputProvider->block(3, rect, width, height);//获取红波段数据 
        QgsRasterBlock* blockNir = inputProvider->block(4, rect, width, height);//获取近红外波段数据


        QProgressDialog progress(coder->toUnicode("计算NDVI..."), coder->toUnicode("放弃"), 0, 100, nullptr);
        progress.setWindowModality(Qt::WindowModal);
        progress.show();
        for (int iy = 0; iy < height; ++iy)
        {
            for (int ix = 0; ix < width; ++ix)
            {
                int redval = blockRed->value(iy, ix);
                int nirval = blockNir->value(iy, ix);
                if (redval > 0 && nirval > 0)
                {
                    float ndvi1 = (nirval - redval) * 1.0 / (nirval + redval);
                    int ndvi2 = ndvi1 * 10000;
                    outBuffer[iy * width + ix] = ndvi2;
                }
                else
                {
                    outBuffer[iy * width + ix] = -3000;//填充无效值-3000
                }
            }
            int per = iy * 1.0 / height * 100;
            progress.setValue(per);
        }
        ndviGdalDs->GetRasterBand(1)->RasterIO(GF_Write, 0, 0, width, height, outBuffer, width, height, GDT_Int16, 0, 0, 0);//写入数据集
        GDALClose(ndviGdalDs);//关闭文件
        delete[] outBuffer;
        outBuffer = 0;

        //加载新生成的ndvi图层
        QgsRasterLayer* ndviLayer = new QgsRasterLayer("ndvi.tif", "ndvi", "gdal");
        QgsRasterBandStats bandStats = ndviLayer->dataProvider()->bandStatistics(1);
        
        QgsProject::instance()->addMapLayer(ndviLayer);

        ndviLayer->triggerRepaint();
        layers.push_front(ndviLayer);        
        mapCanvas->setLayers(layers);
        mapCanvas->zoomToFullExtent();
        mapCanvas->setVisible(true);
        mapCanvas->freeze(false);
        mapCanvas->refresh();
        progress.setValue(100);
        progress.close();
        QMessageBox::information(nullptr, coder->toUnicode("信息"), coder->toUnicode("数据编辑完成."));
        QString& temptext = QString::fromLocal8Bit(">> NDVI计算结果已显示");                             //结果乘以了一万
        m_testBrowser->append(temptext);
    }
}