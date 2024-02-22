
#pragma once
#ifndef GISTSET2LAYERTREEVIEWMENUPROVIDER_H
#define GISTSET2LAYERTREEVIEWMENUPROVIDER_H

#include <QObject>

#include <qgslayertreeview.h>
#include <qgsmaplayer.h>

#include <qgsmapcanvas.h>
#include <qdockwidget.h>                   //
#include <qgslayertree.h>                  //ͼ��������Ҽ�
#include <qgslayertreeviewdefaultactions.h>//ͼ��������Ҽ�
#include <qtoolbutton.h>                   //��ʼ��ͼ�������

class QAction;
class QgsMapCanvas;

struct LegendLayerAction
{
    LegendLayerAction(QAction* a, QString m, QString i, bool all)
        : action(a), menu(m), id(i), allLayers(all) {}
    QAction* action = nullptr;
    QString menu;
    QString id;
    bool allLayers;
    QList<QgsMapLayer*> layers;
};

//ͼ��������Ҽ��˵���
class gisTest2LayerTreeViewMenuProvider : public QObject, public QgsLayerTreeViewMenuProvider
{
    Q_OBJECT
public:
    gisTest2LayerTreeViewMenuProvider(QgsLayerTreeView* view, QgsMapCanvas* canvas); // ���캯���������壬ֱ�ӻ��ͼ��������͵�ͼ�����ؼ����Ա�����ǽ�����ϵ
    //~gisTest2LayerTreeViewMenuProvider();

    //! ��д�����������ȡ�Ҽ��˵�
    QMenu* createContextMenu() override;

    //
    void addLegendLayerAction(QAction* action, const QString& menu, QString id,
        QgsMapLayerType type, bool allLayers);
    bool removeLegendLayerAction(QAction* action);
    void addLegendLayerActionForLayer(QAction* action, QgsMapLayer* layer);
    void removeLegendLayerActionsForLayer(QgsMapLayer* layer);
    QList< LegendLayerAction > legendLayerActions(QgsMapLayerType type) const;

protected:

    void addCustomLayerActions(QMenu* menu, QgsMapLayer* layer);

    QgsLayerTreeView* m_layerTreeView = nullptr;;
    QgsMapCanvas* m_mapCanvas = nullptr;

    QMap<QgsMapLayerType, QList< LegendLayerAction>> mLegendLayerActionMap;

//private slots:

    //void editVectorSymbol(const QString& layerId);
   /* void copyVectorSymbol(const QString& layerId);
    void pasteVectorSymbol(const QString& layerId);
    void setVectorSymbolColor(const QColor& color);
    void editSymbolLegendNodeSymbol(const QString& layerId, const QString& ruleKey);
    void copySymbolLegendNodeSymbol(const QString& layerId, const QString& ruleKey);
    void pasteSymbolLegendNodeSymbol(const QString& layerId, const QString& ruleKey);
    void setSymbolLegendNodeColor(const QColor& color);
    void setLayerCrs(const QgsCoordinateReferenceSystem& crs);
private:
    bool removeActionEnabled();*/
};

#endif // GISTSET2LAYERTREEVIEWMENUPROVIDER_H
