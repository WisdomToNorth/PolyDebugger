#include "polylinenode.h"

#include <QDebug>

#include "flatcolorgeometrynode.h"
#include "graphicshelpers.h"
#include "pointsetnode.h"

namespace debugger
{
using namespace cavc;

namespace
{
void setOpacityNodeVisibility(QSGOpacityNode *node, bool isVisible)
{
    node->setOpacity(isVisible ? 1.0 : 0.0);
}
} // namespace

PolylineNode::PolylineNode() :
    m_pathVisible(true), m_vertexesVisible(false), m_pathNode(nullptr), m_vertexesNode(nullptr),
    m_pathColor(Qt::blue), m_vertexesColor(Qt::red), m_isVisible(true)
{
}

void PolylineNode::updateGeometry(const cavc::Polyline<double> &pline, PathDrawMode pathDrawMode,
                                  double arcApproxError)
{
    updatePathNode(pline, arcApproxError, pathDrawMode);
    updateVertexesNode(pline);

    markDirty(QSGNode::DirtyGeometry);
}

void PolylineNode::updateGeometry(const NGPolygonSet *polygonSet, PathDrawMode pathDrawMode,
                                  double arcApproxError)
{
    updatePathNode(polygonSet, arcApproxError, pathDrawMode);
    updateVertexesNode(polygonSet);

    markDirty(QSGNode::DirtyGeometry);
}

const QColor &PolylineNode::color() const
{
    return m_pathColor;
}

void PolylineNode::setColor(const QColor &color)
{
    if (m_pathColor != color)
    {
        m_pathColor = color;
        if (m_pathNode)
        {
            m_pathNode->setColor(m_pathColor);
        }
    }
}

bool PolylineNode::pathVisible() const
{
    return m_pathVisible;
}

void PolylineNode::setPathVisible(bool pathVisible)
{
    if (pathVisible != m_pathVisible)
    {
        m_pathVisible = pathVisible;
        if (m_pathNode)
        {
            m_pathNode->setIsVisible(pathVisible);
        }
    }
}

const QColor &PolylineNode::vertexesColor() const
{
    return m_vertexesColor;
}

void PolylineNode::setVertexesColor(const QColor &vertexesColor)
{
    if (vertexesColor != m_vertexesColor)
    {
        m_vertexesColor = vertexesColor;
        if (m_vertexesNode)
        {
            m_vertexesNode->setColor(vertexesColor);
        }
    }
}

bool PolylineNode::vertexesVisible() const
{
    return m_vertexesVisible;
}

void PolylineNode::setVertexesVisible(bool vertexesVisible)
{
    if (vertexesVisible != m_vertexesVisible)
    {
        m_vertexesVisible = vertexesVisible;
        if (m_vertexesNode)
        {
            setOpacityNodeVisibility(m_vertexesNode, vertexesVisible);
        }
    }
}

void PolylineNode::setIsVisible(bool isVisible)
{
    m_pathVisible = isVisible;
    m_vertexesVisible = isVisible;
    if (m_pathNode)
    {
        m_pathNode->setIsVisible(isVisible);
    }
    if (m_vertexesNode)
    {
        setOpacityNodeVisibility(m_vertexesNode, isVisible);
    }
}

void PolylineNode::updatePathNode(const cavc::Polyline<double> &pline, double arcApproxError,
                                  PathDrawMode drawMode)
{
    std::vector<cavc::Vector2<float>> m_vertexesBuffer;

    // update vertexes buffer
    auto visitor = [&](std::size_t i, std::size_t j) -> bool
    {
        const cavc::PlineVertex<double> &v1 = pline[i];
        const cavc::PlineVertex<double> &v2 = pline[j];

        if (v1.bulgeIsZero() || fuzzyEqual(v1.pos(), v2.pos()))
        {
            if (drawMode == PathDrawMode::NormalPath)
            {
                m_vertexesBuffer.emplace_back(v1.x(), v1.y());
            }
            else
            {
                // generate multiple points along the segment for dashes
                double length = cavc::length(v2.pos() - v1.pos());
                double dashSpacing = 0.5;
                int dashCount = static_cast<int>(std::ceil(length / dashSpacing));
                for (int i = 0; i < dashCount; ++i)
                {
                    double t = static_cast<double>(i) / dashCount;
                    double x = v1.x() + t * (v2.x() - v1.x());
                    double y = v1.y() + t * (v2.y() - v1.y());
                    m_vertexesBuffer.emplace_back(x, y);
                }
            }
        }
        else // Arc
        {
            auto arc = arcRadiusAndCenter(v1, v2);

            if (arc.radius < arcApproxError + utils::realThreshold<double>())
            {
                m_vertexesBuffer.emplace_back(v1.x(), v1.y());
                return true;
            }

            auto startAngle = angle(arc.center, v1.pos());
            auto endAngle = angle(arc.center, v2.pos());
            double deltaAngle = std::abs(cavc::utils::deltaAngle(startAngle, endAngle));

            double segmentSubAngle = std::abs(2.0 * std::acos(1.0 - arcApproxError / arc.radius));
            std::size_t segmentCount =
                static_cast<std::size_t>(std::ceil(deltaAngle / segmentSubAngle));
            // update segment subangle for equal length segments
            segmentSubAngle = deltaAngle / segmentCount;

            if (v1.bulge() < 0.0)
            {
                segmentSubAngle = -segmentSubAngle;
            }
            // add the start point
            m_vertexesBuffer.emplace_back(v1.x(), v1.y());

            // add remaining points
            for (std::size_t i = 1; i < segmentCount; ++i)
            {
                double angle = i * segmentSubAngle + startAngle;
                m_vertexesBuffer.emplace_back(arc.radius * std::cos(angle) + arc.center.x(),
                                              arc.radius * std::sin(angle) + arc.center.y());
            }
        }

        return true;
    };

    pline.visitSegIndices(visitor);

    if (pline.size() != 0)
    {
        if (pline.isClosed())
        {
            m_vertexesBuffer.push_back(m_vertexesBuffer[0]);
        }
        else
        {
            m_vertexesBuffer.emplace_back(pline.lastVertex().x(), pline.lastVertex().y());
        }
    }

    // update node geometry from vertexes buffer
    const int plineSegVertexCount = static_cast<int>(m_vertexesBuffer.size());

    if (!m_pathNode)
    {
        m_pathNode = new FlatColorGeometryNode(true);
        m_pathNode->geometry()->setLineWidth(1);
        m_pathNode->setColor(m_pathColor);
        appendChildNode(m_pathNode);
        m_pathNode->setFlag(QSGNode::OwnedByParent);
        m_pathNode->setIsVisible(m_pathVisible);
    }

    QSGGeometry *pathGeom = m_pathNode->geometry();
    pathGeom->setDrawingMode(drawMode == NormalPath ? QSGGeometry::DrawLineStrip :
                                                      QSGGeometry::DrawLines);

    pathGeom->allocate(plineSegVertexCount, plineSegVertexCount);

    std::uint32_t *segVertexIndices = pathGeom->indexDataAsUInt();
    for (int i = 0; i < plineSegVertexCount; ++i)
    {
        segVertexIndices[i] = static_cast<std::uint32_t>(i);
    }

    QSGGeometry::Point2D *vertexData = pathGeom->vertexDataAsPoint2D();
    for (std::size_t i = 0; i < m_vertexesBuffer.size(); ++i)
    {
        vertexData[i].set(m_vertexesBuffer[i].x(), m_vertexesBuffer[i].y());
    }

    m_pathNode->markDirty(QSGNode::DirtyGeometry);
}

void PolylineNode::updatePathNode(const NGPolygonSet *_polygonSet, double arcApproxError,
                                  PathDrawMode drawMode)
{
    const NGPolygonSet &polygonSet = *_polygonSet;

    std::vector<cavc::Vector2<float>> m_vertexesBuffer;
    double x0, y0, x1, y1, center_x, center_y, radius;
    // update vertexes buffer
    auto visitor = [&]() -> bool
    {
        int type;
        bool get_para = polygonSet.getNextSegment(x0, y0, x1, y1, center_x, center_y, radius, type);
        if (!get_para)
        {
            return false;
        }

        if (type == 0) // Line
        {
            if (drawMode == PathDrawMode::NormalPath)
            {
                m_vertexesBuffer.emplace_back(x0, y0);
            }
            else
            {
                // generate multiple points along the segment for dashes
                double length =
                    cavc::length(cavc::Vector2<double>(x1, y1) - cavc::Vector2<double>(x0, y0));
                double dashSpacing = 0.5;
                int dashCount = static_cast<int>(std::ceil(length / dashSpacing));
                for (int i = 0; i < dashCount; ++i)
                {
                    double t = static_cast<double>(i) / dashCount;
                    double x = x0 + t * (x1 - x0);
                    double y = y0 + t * (y1 - y0);
                    m_vertexesBuffer.emplace_back(x, y);
                }
            }
        }
        else // if (type == 1 || type == 2)
        {
            if (radius < arcApproxError + utils::realThreshold<double>())
            {
                m_vertexesBuffer.emplace_back(x0, y0);
                return true;
            }

            auto startAngle =
                angle(cavc::Vector2<double>(center_x, center_y), cavc::Vector2<double>(x0, y0));
            auto endAngle =
                angle(cavc::Vector2<double>(center_x, center_y), cavc::Vector2<double>(x1, y1));
            double deltaAngle = std::abs(cavc::utils::deltaAngle(startAngle, endAngle));

            double segmentSubAngle = std::abs(2.0 * std::acos(1.0 - arcApproxError / radius));
            std::size_t segmentCount =
                static_cast<std::size_t>(std::ceil(deltaAngle / segmentSubAngle));
            // update segment subangle for equal length segments
            segmentSubAngle = deltaAngle / segmentCount;

            if (type == 2 || type == 4)
            {
                segmentSubAngle = -segmentSubAngle;
            }
            // add the start point
            m_vertexesBuffer.emplace_back(x0, y0);

            // add remaining points
            for (std::size_t i = 1; i < segmentCount; ++i)
            {
                double angle = i * segmentSubAngle + startAngle;
                m_vertexesBuffer.emplace_back(radius * std::cos(angle) + center_x,
                                              radius * std::sin(angle) + center_y);
            }
        }
        // else if (type == 3 || type == 4) // circle
        // {
        // }
        return true;
    };

    while (visitor()) {}; // TODO:

    if (polygonSet.isClosed())
    {
        m_vertexesBuffer.push_back(m_vertexesBuffer[0]);
    }
    else
    {
        m_vertexesBuffer.emplace_back(x1, y1);
    }

    // Duplicate code, should be refactored
    /* update node geometry from vertexes buffer */
    const int plineSegVertexCount = static_cast<int>(m_vertexesBuffer.size());

    if (!m_pathNode)
    {
        m_pathNode = new FlatColorGeometryNode(true);
        m_pathNode->geometry()->setLineWidth(1);
        m_pathNode->setColor(m_pathColor);
        appendChildNode(m_pathNode);
        m_pathNode->setFlag(QSGNode::OwnedByParent);
        m_pathNode->setIsVisible(m_pathVisible);
    }

    QSGGeometry *pathGeom = m_pathNode->geometry();
    pathGeom->setDrawingMode(drawMode == NormalPath ? QSGGeometry::DrawLineStrip :
                                                      QSGGeometry::DrawLines);

    pathGeom->allocate(plineSegVertexCount, plineSegVertexCount);

    std::uint32_t *segVertexIndices = pathGeom->indexDataAsUInt();
    for (int i = 0; i < plineSegVertexCount; ++i)
    {
        segVertexIndices[i] = static_cast<std::uint32_t>(i);
    }

    QSGGeometry::Point2D *vertexData = pathGeom->vertexDataAsPoint2D();
    for (std::size_t i = 0; i < m_vertexesBuffer.size(); ++i)
    {
        vertexData[i].set(m_vertexesBuffer[i].x(), m_vertexesBuffer[i].y());
    }

    m_pathNode->markDirty(QSGNode::DirtyGeometry);
}

void PolylineNode::updateVertexesNode(const cavc::Polyline<double> &pline)
{
    if (!m_vertexesNode)
    {
        m_vertexesNode = new PointSetNode();
        appendChildNode(m_vertexesNode);
        m_vertexesNode->setFlag(QSGNode::OwnedByParent);
        m_vertexesNode->setColor(m_vertexesColor);
        setOpacityNodeVisibility(m_vertexesNode, m_vertexesVisible);
    }

    m_vertexesNode->clear();
    m_vertexesNode->addPolylineVertexes(pline);
}

void PolylineNode::updateVertexesNode(NGPolygonSet const *polygonSet)
{
    if (!m_vertexesNode)
    {
        m_vertexesNode = new PointSetNode();
        appendChildNode(m_vertexesNode);
        m_vertexesNode->setFlag(QSGNode::OwnedByParent);
        m_vertexesNode->setColor(m_vertexesColor);
        setOpacityNodeVisibility(m_vertexesNode, m_vertexesVisible);
    }

    m_vertexesNode->clear();
    m_vertexesNode->addPolygonVertexes(polygonSet);
}
} // namespace debugger
