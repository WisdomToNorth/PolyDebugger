#include "pointsetnode.h"

#include <iostream>

#include "graphicshelpers.h"
#include "simplecirclenode.h"

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
        // std::cout << "x: " << v.x() << " y: " << v.y() << std::endl;
        addPoint(v.x(), v.y());
    }
}
//    static bool getNextSegment(double &x0, double &y0, double &x1, double &y1, double &center_x,
//   double &center_y, double &radius, int &type)

void PointSetNode::addPolygonVertexes(NGPolygonSet const &polygonSet)
{
    double x0, y0, x1, y1, center_x, center_y, radius;
    int type;

    while (polygonSet.getNextSegment(x0, y0, x1, y1, center_x, center_y, radius, type))
    {
        // if (type == 0)
        // {
        addPoint(x0, y0);
        // addPoint(x1, y1);// Confirmed that this point is not needed

        // }
        // else if (type == 1)
        // {
        //     addPoint(x0, y0);
        //     addPoint(x1, y1);
        //     auto newNode = new SimpleCircleNode();
        //     newNode->setColor(m_color);
        //     newNode->setGeometry(center_x, center_y, radius);
        //     newNode->setFlag(QSGNode::OwnedByParent);
        //     appendChildNode(newNode);
        // }
        // else if (type == 2)
        // {
        //     addPoint(x0, y0);
        //     addPoint(x1, y1);
        //     auto newNode = new SimpleCircleNode();
        //     newNode->setColor(m_color);
        //     newNode->setGeometry(center_x, center_y, radius);
        //     newNode->setFlag(QSGNode::OwnedByParent);
        //     appendChildNode(newNode);
        // }
    }
}