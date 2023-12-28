#ifndef GEOMETRYCANVASITEM_H
#define GEOMETRYCANVASITEM_H

#include <QMatrix4x4>
#include <QQuickItem>

#include "cavc/polyline.hpp"

/// Base class for setting up interactive geometry views.
class GeometryCanvasItem : public QQuickItem
{
    Q_OBJECT
public:
    explicit GeometryCanvasItem(QQuickItem *parent = nullptr);

protected:
    // members for mapping "real" coordinates to UI coordinates
    double m_uiScaleFactor;
    QMatrix4x4 m_realToUICoord;
    QMatrix4x4 m_uiToRealCoord;
    void updateCoordMatrices(qreal width, qreal height);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0) && QT_VERSION < QT_VERSION_CHECK(6, 1, 0))
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;
#elif (QT_VERSION >= QT_VERSION_CHECK(6, 1, 0))
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)override;
#endif

    QPointF convertToGlobalUICoord(const cavc::Vector2<double> &pt);
    std::size_t vertexUnderPosition(QPointF uiGlobalPos, const cavc::Polyline<double> &pline);
};

#endif // GEOMETRYCANVASITEM_H
