#include "geometrycanvasitem.h"

#include "cavc/mathutils.hpp"

namespace debugger
{
using namespace cavc;

GeometryCanvasItem::GeometryCanvasItem(QQuickItem *parent) : QQuickItem(parent), m_uiScaleFactor(20)
{
    setFlag(ItemHasContents, true);
    setAcceptedMouseButtons(Qt::LeftButton);
    updateCoordMatrices(width(), height());
}

void GeometryCanvasItem::updateCoordMatrices(qreal width, qreal height)
{
    m_realToUICoord.setToIdentity();
    m_realToUICoord.translate(static_cast<float>(width / 2), static_cast<float>(height / 2));
    m_realToUICoord.scale(static_cast<float>(m_uiScaleFactor),
                          static_cast<float>(-m_uiScaleFactor));
    m_uiToRealCoord = m_realToUICoord.inverted();
}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0) && QT_VERSION < QT_VERSION_CHECK(6, 1, 0))
void GeometryCanvasItem::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
#elif (QT_VERSION >= QT_VERSION_CHECK(6, 1, 0))
void GeometryCanvasItem::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChange(newGeometry, oldGeometry);
#endif
    Q_UNUSED(oldGeometry)

    updateCoordMatrices(newGeometry.width(), newGeometry.height());
    update();
}

QPointF GeometryCanvasItem::convertToGlobalUICoord(const cavc::Vector2<double> &pt)
{
    return mapToGlobal(m_realToUICoord * QPointF(pt.x(), pt.y()));
}

QPointF GeometryCanvasItem::convertToGlobalUICoord(const QPointF &pt)
{
    return mapToGlobal(m_realToUICoord * pt);
}

// QPointF GeometryCanvasItem::convertToLocalCoord(const QPointF &pt)
// {
//     return m_realToUICoord.inverted() * mapFromGlobal(pt);
// }

std::size_t GeometryCanvasItem::vertexUnderPosition(QPointF uiGlobalPos,
                                                    const Polyline<double> &pline)
{
    auto containsVertex = [&](const QPointF &vPosInGlobal)
    {
        return utils::fuzzyEqual(vPosInGlobal.x(), uiGlobalPos.x(), 5.0)
               && utils::fuzzyEqual(vPosInGlobal.y(), uiGlobalPos.y(), 5.0);
    };

    std::size_t vertexGrabbed = std::numeric_limits<std::size_t>::max();
    double minDist = std::numeric_limits<double>::infinity();
    for (std::size_t i = 0; i < pline.size(); ++i)
    {
        QPointF vPosInGlobal = convertToGlobalUICoord(pline[i].pos());
        if (containsVertex(vPosInGlobal))
        {
            // grab the vertex closest to the mouse in the case of vertexes being very close to each
            // other
            QPointF vDiff = vPosInGlobal - uiGlobalPos;
            double dist = QPointF::dotProduct(vDiff, vDiff);
            if (dist < minDist)
            {
                vertexGrabbed = i;
                minDist = dist;
            }
        }
    }

    return vertexGrabbed;
}

std::size_t GeometryCanvasItem::vertexUnderPosition(QPointF uiGlobalPos,
                                                    const NGPolygonSet &polygonSet)
{
    auto containsVertex = [&](const QPointF &vPosInGlobal)
    {
        return utils::fuzzyEqual(vPosInGlobal.x(), uiGlobalPos.x(), 5.0)
               && utils::fuzzyEqual(vPosInGlobal.y(), uiGlobalPos.y(), 5.0);
    };

    std::size_t vertexGrabbed = std::numeric_limits<std::size_t>::max();
    // TODO
    return vertexGrabbed;
}
} // namespace debugger
