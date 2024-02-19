#ifndef GEOMETRYCANVASITEM_H
#define GEOMETRYCANVASITEM_H

#include <QMatrix4x4>
#include <QQuickItem>

#include "cavc/polyline.hpp"
#include "ngpoly/ngpolygonset.h"

/// Base class for setting up interactive geometry views.
namespace debugger
{
class GeometryCanvasItem : public QQuickItem
{
    Q_OBJECT
public:
    explicit GeometryCanvasItem(QQuickItem *parent = nullptr);
    QPointF convertToGlobalUICoord(const QPointF &pt);
    // QPointF convertToLocalCoord(const QPointF &pt);

protected:
    // members for mapping "real" coordinates to UI coordinates
    double m_uiScaleFactor;
    QMatrix4x4 m_realToUICoord;
    QMatrix4x4 m_uiToRealCoord;
    void updateCoordMatrices(qreal width, qreal height);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0) && QT_VERSION < QT_VERSION_CHECK(6, 1, 0))
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;
#elif (QT_VERSION >= QT_VERSION_CHECK(6, 1, 0))
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;
#endif

    QPointF convertToGlobalUICoord(const cavc::Vector2<double> &pt);

private:
    std::size_t vertexUnderPosition(QPointF uiGlobalPos, const cavc::Polyline<double> &pline);
    std::size_t vertexUnderPosition(QPointF uiGlobalPos, const NGPolygonSet &polygonSet);
};
} // namespace debugger
#endif // GEOMETRYCANVASITEM_H
