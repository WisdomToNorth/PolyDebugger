#ifndef POINTSETNODE_H
#define POINTSETNODE_H
#include <QColor>
#include <QSGOpacityNode>

#include "cavc/polyline.hpp"
#include "ngpoly/ngpolygonset.h"
namespace debugger
{
class PointSetNode : public QSGOpacityNode
{
public:
    void addPolylineVertexes(cavc::Polyline<double> const &polyline);
    void addPolygonVertexes(const NGPolygonSet *polygonSet);

public:
    PointSetNode();
    QColor const &color() const;
    void setColor(QColor const &color);
    void addPoint(qreal x, qreal y);

    void clear();

private:
    QColor m_color;
};
} // namespace debugger
#endif // POINTSETNODE_H
