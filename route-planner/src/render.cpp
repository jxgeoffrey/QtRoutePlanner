#include <QPainter>
#include <iostream>
#include "../inc/render.h"


static float RoadMetricWidth(Model::Road::Type type);
static QColor RoadColor(Model::Road::Type type);
static Qt::PenStyle RoadDashes(Model::Road::Type type);
static QPointF ToPoint2D( const Model::Node &node ) noexcept;

RenderMap::RenderMap( RouteModel &model)
    : m_windowHeight(0), m_windowWidth(0), m_Model(model)
{

    m_BackgroundFillColor.setRgb(238, 235, 227);
    m_BuildingFillColor.setRgb(208, 197, 190);
    m_BuildingOutlineColor.setRgb(181, 167, 154);

    m_LeisureFillColor.setRgb(189, 252, 193);
    m_LeisureOutlineColor.setRgb(160, 248, 162);

    m_WaterFillColor.setRgb(155, 201, 215);

    m_RailwayStrokeColor.setRgb(Qt::black);
    //m_RailwayStrokeColor.setStyle(Qt::VerPattern);
    m_RailwayDashBrush.setColor(Qt::white);

    BuildRoadReps();
    BuildLanduseBrushes();
}

void RenderMap::render(QPainter *p)
{
    m_Scale = static_cast<float>(std::min(height() , width()));
    m_PixelsInMeter = static_cast<float>(static_cast<float>(m_Scale) / m_Model.MetricScale());
    p->fillRect(0,0,width(),height(),m_BackgroundFillColor);

    p->scale(static_cast<double>(m_Scale),static_cast<double>(m_Scale));
//    p->translate(0.f, -1.0f);

    DrawLanduses(p);
    DrawLeisure(p);
    DrawWater(p);
    DrawRailways(p);
    DrawHighways(p);
    DrawBuildings(p);
    DrawPath(p);
    DrawStartPosition(p);
    DrawEndPosition(p);
}


void RenderMap::DrawPath(QPainter *p) const{
    p->setRenderHint(QPainter::Antialiasing);
    float width = 0.005f;
    QPen forePen(QColor(255,165,0), width * m_PixelsInMeter, Qt::SolidLine); //orange
    p->strokePath(PathLine(), forePen);

}

void RenderMap::DrawEndPosition(QPainter *p) const{
    p->setRenderHint(QPainter::Antialiasing);
    QColor foreColor(Qt::red);

    QPainterPath pb;
    pb.addEllipse(QRectF(static_cast<float>(m_Model.path.front().x - 0.005f), static_cast<float>(m_Model.path.front().y - 0.005f), 0.01f, 0.01f));
    p->fillPath(pb, foreColor);


}
void RenderMap::DrawStartPosition(QPainter *p) const{
    p->setRenderHint(QPainter::Antialiasing);
    QColor foreColor(Qt::green);

    QPainterPath pb;
    pb.addEllipse(QRectF(static_cast<float>(m_Model.path.back().x - 0.005f), static_cast<float>(m_Model.path.back().y - 0.005f), 0.01f, 0.01f));
    p->fillPath(pb, foreColor);
}


void RenderMap::DrawHighways(QPainter *p) const
{
    auto ways = m_Model.Ways().data();
    for( auto road: m_Model.Roads() )
        if( auto rep_it = m_RoadReps.find(road.type); rep_it != m_RoadReps.end() ) {
            auto &rep = rep_it->second;   
            auto &way = ways[road.way];
            auto width = rep.metric_width > 0.f ? (rep.metric_width * m_PixelsInMeter) : 1.f;
            p->strokePath(PathFromWay(way), QPen(rep.color, width, rep.dashes));
        }
}


void RenderMap::DrawWater(QPainter *p) const
{
    for( auto &water: m_Model.Waters())
        p->fillPath(PathFromMP(water), m_WaterFillColor);
}

void RenderMap::DrawLanduses(QPainter *p) const
{
    for( auto &landuse: m_Model.Landuses() )
        if( auto br = m_LanduseBrushes.find(landuse.type); br != m_LanduseBrushes.end() )
            p->fillPath(PathFromMP(landuse), br->second);
}

void RenderMap::DrawLeisure(QPainter *p) const
{
    for( auto &leisure: m_Model.Leisures()) {
        auto path = PathFromMP(leisure);
        p->fillPath(path, m_LeisureFillColor);
        p->strokePath(path, QPen(m_LeisureOutlineColor, m_LeisureOutlineStrokeProps * m_PixelsInMeter, Qt::DashDotLine));

    }
}

void RenderMap::DrawBuildings(QPainter *p) const
{
    for( auto &building: m_Model.Buildings() ) {
        auto path = PathFromMP(building);
        p->fillPath(path,m_BuildingFillColor);
        p->strokePath(path, QPen(m_BuildingOutlineColor, m_BuildingOutlineStrokeProps * m_PixelsInMeter, Qt::DashLine));
    }
}
void RenderMap::DrawRailways(QPainter *p) const
{     
    auto ways = m_Model.Ways().data();
    for( auto &railway: m_Model.Railways() ) {
        auto &way = ways[railway.way];
        auto path = PathFromWay(way);
        QPen pen(QBrush(m_RailwayStrokeColor, Qt::VerPattern), (m_RailwayOuterWidth * m_PixelsInMeter));
        p->strokePath(path,pen);
        pen.setBrush(m_RailwayDashBrush);
        pen.setWidthF(m_RailwayInnerWidth * m_PixelsInMeter);
        p->strokePath(path, pen);
    }
}

QPainterPath RenderMap::PathLine() const
{    
    if( m_Model.path.empty() )
        return {};

    const auto nodes = m_Model.path;    
    
    QPainterPath pb;
    pb.moveTo( ToPoint2D( m_Model.path[0]));

    for( auto i=1; i < m_Model.path.size();i++ )
        pb.lineTo(ToPoint2D(m_Model.path[i]));

      
    return QPainterPath{pb};
}

QPainterPath RenderMap::PathFromWay(const Model::Way &way) const
{    
    if( way.nodes.empty() )
        return {};

    const auto nodes = m_Model.Nodes().data();    
    
    QPainterPath pb;
    pb.moveTo( ToPoint2D(nodes[way.nodes.front()]) );
    for( auto it = ++way.nodes.begin(); it != std::end(way.nodes); ++it )
    {
        //qDebug("path point:%f,%f",ToPoint2D(nodes[*it]).rx(),ToPoint2D(nodes[*it]).ry());
        pb.lineTo(ToPoint2D(nodes[*it]) );
    }
    return QPainterPath{pb};
}

QPainterPath RenderMap::PathFromMP(const Model::Multipolygon &mp) const
{
    const auto nodes = m_Model.Nodes().data();
    const auto ways = m_Model.Ways().data();

    QPainterPath pb;
    auto commit = [&](const Model::Way &way) {
        if( way.nodes.empty() )
            return;
        pb.moveTo(ToPoint2D(nodes[way.nodes.front()]) );
        //qDebug("path point:%d,%d",ToPoint2D(nodes[way.nodes.front()]).x(),ToPoint2D(nodes[way.nodes.front()]).y());
        for( auto it = ++way.nodes.begin(); it != std::end(way.nodes); ++it )
            pb.lineTo(ToPoint2D(nodes[*it]) );
        pb.closeSubpath();
    };
    
    for( auto way_num: mp.outer )
        commit( ways[way_num] );
    for( auto way_num: mp.inner )
        commit( ways[way_num] );
    
    return pb;
}

void RenderMap::BuildRoadReps()
{
    using R = Model::Road;
    auto types = {R::Motorway, R::Trunk, R::Primary,  R::Secondary, R::Tertiary,
        R::Residential, R::Service, R::Unclassified, R::Footway};

    for( auto type: types ) {
        auto &rep = m_RoadReps[type];
        rep.color = RoadColor(type);
        rep.metric_width = RoadMetricWidth(type);  
        rep.dashes = RoadDashes(type);
    }
}

void RenderMap::BuildLanduseBrushes()
{
    m_LanduseBrushes.insert_or_assign(Model::Landuse::Commercial, QColor(233, 195, 196));
    m_LanduseBrushes.insert_or_assign(Model::Landuse::Construction, QColor(187, 188, 165));
    m_LanduseBrushes.insert_or_assign(Model::Landuse::Grass, QColor(197, 236, 148));
    m_LanduseBrushes.insert_or_assign(Model::Landuse::Forest, QColor(158, 201, 141));
    m_LanduseBrushes.insert_or_assign(Model::Landuse::Industrial, QColor(223, 197, 220));
    m_LanduseBrushes.insert_or_assign(Model::Landuse::Railway, QColor(223, 197, 220));
    m_LanduseBrushes.insert_or_assign(Model::Landuse::Residential, QColor(209, 209, 209));
}

static float RoadMetricWidth(Model::Road::Type type)
{
    switch( type ) {
        case Model::Road::Motorway:     return 0.006f;
        case Model::Road::Trunk:        return 0.006f;
        case Model::Road::Primary:      return 0.005f;
        case Model::Road::Secondary:    return 0.005f;
        case Model::Road::Tertiary:     return 0.004f;
        case Model::Road::Residential:  return 0.003f;
        case Model::Road::Unclassified: return 0.003f;
        case Model::Road::Service:      return 0.001f;
        case Model::Road::Footway:      return 0.001f;
        default:                        return 0.001f;
    }
}

static QColor RoadColor(Model::Road::Type type)
{
    switch( type) {
        case Model::Road::Motorway:     return QColor(226, 122, 143);
        case Model::Road::Trunk:        return QColor(245, 161, 136);
        case Model::Road::Primary:      return QColor(249, 207, 144);
        case Model::Road::Secondary:    return QColor(244, 251, 173);
        case Model::Road::Tertiary:     return QColor(244, 251, 173);
        case Model::Road::Residential:  return QColor(254, 254, 254);
        case Model::Road::Service:      return QColor(254, 254, 254);
        case Model::Road::Footway:      return QColor(241, 106, 96);
        case Model::Road::Unclassified: return QColor(254, 254, 254);
    default:                        return Qt::gray;
    }
}

static Qt::PenStyle RoadDashes(Model::Road::Type type)
{
    return type == Model::Road::Footway ? Qt::DotLine : Qt::SolidLine;
}

static QPointF ToPoint2D( const Model::Node &node ) noexcept
{
    return QPointF(static_cast<float>(node.x), static_cast<float>(node.y));
}
