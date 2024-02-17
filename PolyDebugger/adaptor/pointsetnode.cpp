#include "pointsetnode.h"

#include <iostream>

#include "graphicshelpers.h"
#include "simplecirclenode.h"

namespace debugger
{
PointSetNode::PointSetNode()
{
}

const QColor &PointSetNode::color() const
{
    return m_color;
}

void PointSetNode::setColor(const QColor &color)
{
    if (m_color != color)
    {
        m_color = color;
        QSGNode *node = firstChild();
        while (node)
        {
            auto circle = static_cast<SimpleCircleNode *>(node);
            circle->setColor(color);
            node = node->nextSibling();
        }
    }
}

void PointSetNode::addPoint(qreal x, qreal y)
{
    const qreal pointRadius = 0.2;
    auto newNode = new SimpleCircleNode();
    newNode->setColor(m_color);
    newNode->setGeometry(x, y, pointRadius);
    newNode->setFlag(QSGNode::OwnedByParent);
    appendChildNode(newNode);
}

void PointSetNode::clear()
{
    removeAllChildNodes();
}

void PointSetNode::addPolylineVertexes(const cavc::Polyline<double> &polyline)
{
    for (auto const &v : polyline.vertexes())
    {
        addPoint(v.x(), v.y());
    }
}

void PointSetNode::addPolygonVertexes(const NGPolygonSet *polygonSet)
{
    double x0, y0, x1, y1, center_x, center_y, radius;
    int type;

    while (polygonSet->getNextSegment(x0, y0, x1, y1, center_x, center_y, radius, type))
    {
        addPoint(x0, y0);
    }
}
} // namespace debugger
