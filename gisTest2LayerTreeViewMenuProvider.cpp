
#include "gisTest2.h"
#include "gisTest2layertreeviewmenuprovider.h"

#include <QMenu>
#include <QModelIndex>
#include <QIcon>

// QGis include
#include <qgslayertreeviewdefaultactions.h>
#include <qgslayertreenode.h>
#include <qgslayertreemodel.h>
#include <qgslayertree.h>
#include <qgsrasterlayer.h>
#include <qgsvectorlayer.h>
#include "qmainwindow.h"



// 构造函数
//gisTest2LayerTreeViewMenuProvider::gisTest2LayerTreeViewMenuProvider(QgsLayerTreeView* view, QgsMapCanvas* canvas)
//{
//    m_layerTreeView = view;
//    m_mapCanvas = canvas;
//}
gisTest2LayerTreeViewMenuProvider::gisTest2LayerTreeViewMenuProvider(QgsLayerTreeView* view, QgsMapCanvas* canvas)
    : m_layerTreeView(view)
    , m_mapCanvas(canvas)
{
}

//析构
//gisTest2LayerTreeViewMenuProvider::~gisTest2LayerTreeViewMenuProvider()
//{
//
//}

//重写方法获取右键菜单
QMenu* gisTest2LayerTreeViewMenuProvider::createContextMenu()
{
    // 设置这个路径是为了获取图标文件
    QString iconDir = "E:\\GIS development\\QGIS\\gisTest2\\image\\";

    QMenu* menu = new QMenu;
    QgsLayerTreeViewDefaultActions* actions = m_layerTreeView->defaultActions();
    QModelIndex idx = m_layerTreeView->currentIndex();

    // global menu
    if (!idx.isValid())
    {
        menu->addAction(actions->actionAddGroup(menu));
        menu->addAction(QIcon(iconDir + "mActionExpandTree.png"), tr("&Expand All"), m_layerTreeView, SLOT(expandAll()));
        menu->addAction(QIcon(iconDir + "mActionCollapseTree.png"), tr("&Collapse All"), m_layerTreeView, SLOT(collapseAll()));
        menu->addSeparator();
        /*if (gisTest2::instance()->clipboard()->hasFormat(QGSCLIPBOARD_MAPLAYER_MIME))
        {
            QAction* actionPasteLayerOrGroup = new QAction(QgsApplication::getThemeIcon(QStringLiteral("/mActionEditPaste.svg")), tr("Paste Layer/Group"), menu);
            connect(actionPasteLayerOrGroup, &QAction::triggered, QgisApp::instance(), &QgisApp::pasteLayer);
            menu->addAction(actionPasteLayerOrGroup);
        }*/
    }

    else if (QgsLayerTreeNode* node = m_layerTreeView->layerTreeModel()->index2node(idx))
    {
        // layer or group selected
        if (QgsLayerTree::isGroup(node))
        {
            menu->addAction(actions->actionZoomToGroup(m_mapCanvas, menu));
            menu->addAction(QIcon(iconDir + "layer-remove.png"), tr("&Remove"), gisTest2::instance(), SLOT(removeLayer()));
            menu->addAction(actions->actionRenameGroupOrLayer(menu));
            menu->addAction(actions->actionRenameGroupOrLayer());
            if (m_layerTreeView->selectedNodes(true).count() >= 2)
            {
                menu->addAction(actions->actionGroupSelected(menu));
            }
            menu->addAction(actions->actionAddGroup(menu));
        }
        else if (QgsLayerTree::isLayer(node))
        {
            QgsMapLayer* layer = QgsLayerTree::toLayer(node)->layer();
            menu->addAction(actions->actionZoomToLayer(m_mapCanvas, menu));
            //menu->addAction(actions->actionShowInOverview(menu));
            menu->addAction(actions->actionRemoveGroupOrLayer(menu));
            menu->addAction(tr("&Label"), gisTest2::instance(), SLOT(slot_labelShowAction()));
        
            // 如果选择的是矢量图层
            if (layer->type() == QgsMapLayerType::VectorLayer)
            {
                //点矢量图层增加点样式设置菜单
                QgsVectorLayer* veclayer = qobject_cast<QgsVectorLayer*>(layer);
                if (veclayer->geometryType() == QgsWkbTypes::PointGeometry)
                {
                    menu->addAction(tr("PointStyle"), gisTest2::instance(), SLOT(slot_pointstyle()));
                }
                else if (veclayer->geometryType() == QgsWkbTypes::PolygonGeometry)
                {
                    menu->addAction(tr("PolygonStyle"), gisTest2::instance(), SLOT(slot_polygonstyle()));
                }
            }
            
            // 如果选择的是栅格图层
            if (layer && layer->type() == QgsMapLayerType::RasterLayer)
            {
                // TO DO:
            }
        }
    }

    return menu;
}



void gisTest2LayerTreeViewMenuProvider::addLegendLayerAction(QAction* action, const QString& menu, QString id, QgsMapLayerType type, bool allLayers)
{

}

void gisTest2LayerTreeViewMenuProvider::addLegendLayerActionForLayer(QAction* action, QgsMapLayer* layer)
{

}

void gisTest2LayerTreeViewMenuProvider::removeLegendLayerActionsForLayer(QgsMapLayer* layer)
{

}

QList< LegendLayerAction > gisTest2LayerTreeViewMenuProvider::legendLayerActions(QgsMapLayerType type) const
{
    return mLegendLayerActionMap.contains(type) ? mLegendLayerActionMap.value(type) : QList<LegendLayerAction>();
}

bool gisTest2LayerTreeViewMenuProvider::removeLegendLayerAction(QAction* action)
{
    QMap< QgsMapLayerType, QList< LegendLayerAction > >::iterator it;
    for (it = mLegendLayerActionMap.begin();
        it != mLegendLayerActionMap.end(); ++it)
    {
        for (int i = 0; i < it->count(); i++)
        {
            if ((*it)[i].action == action)
            {
                (*it).removeAt(i);
                return true;
            }
        }
    }
    return false;
}

void gisTest2LayerTreeViewMenuProvider::addCustomLayerActions(QMenu* menu, QgsMapLayer* layer)
{

}



