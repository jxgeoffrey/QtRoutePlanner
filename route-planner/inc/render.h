#pragma once
#include <QtGui>
#include <QMatrix>
#include <unordered_map>
#include "rasterwindow.h"
#include "route_model.h"


class RenderMap: public RasterWindow
{
public:
    explicit RenderMap(RouteModel &model);
    
protected:
    virtual void render(QPainter *p) override;

private:
    void BuildRoadReps();
    void BuildLanduseBrushes();
    
    void DrawBuildings(QPainter *p) const;
    void DrawHighways(QPainter *p) const;
    void DrawRailways(QPainter *p) const;
    void DrawLeisure(QPainter *p) const;
    void DrawWater(QPainter *p) const;
    void DrawLanduses(QPainter *p) const;
    void DrawStartPosition(QPainter *p) const;
    void DrawEndPosition(QPainter *p) const;
    void DrawPath(QPainter *p) const;
    QPainterPath PathFromWay(const Model::Way &way) const;
    QPainterPath PathFromMP(const Model::Multipolygon &mp) const;
    QPainterPath PathLine() const;

    int m_windowHeight;
    int m_windowWidth;
    
    RouteModel &m_Model;
    float m_Scale = 1.f;
    float m_PixelsInMeter = 1.f;
    QMatrix m_Matrix;
    
    QColor m_BackgroundFillColor;
    QColor m_BuildingFillColor;
    QColor m_BuildingOutlineColor;
    float m_BuildingOutlineStrokeProps = 0.001f;
    
    QColor m_LeisureFillColor;
    QColor m_LeisureOutlineColor;
    float m_LeisureOutlineStrokeProps = 0.003f;

    QColor m_WaterFillColor;
        
    QColor m_RailwayStrokeColor;
    QBrush m_RailwayDashBrush;
    Qt::PenStyle m_RailwayDashes = Qt::DashDotLine;
    float m_RailwayOuterWidth = 0.003f;
    float m_RailwayInnerWidth = 0.002f;
    
    struct RoadRep {
        QColor color;
        Qt::PenStyle dashes;
        float metric_width = 1.f;
    };
    std::unordered_map<Model::Road::Type, RoadRep> m_RoadReps;
    
    std::unordered_map<Model::Landuse::Type, QColor> m_LanduseBrushes;
};
