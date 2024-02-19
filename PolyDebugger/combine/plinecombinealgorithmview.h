#ifndef PLINECOMBINEALGORITHMVIEW_H
#define PLINECOMBINEALGORITHMVIEW_H

#include "cavc/polyline.hpp"

#include "adaptor/geometrycanvasitem.h"

class QSGGeometryNode;
class QSGOpacityNode;
namespace debugger
{
class NgViewModel;
class PointSetNode;
class PlineCombineAlgorithmView : public GeometryCanvasItem
{
    Q_OBJECT
    Q_PROPERTY(bool interacting READ interacting NOTIFY interactingChanged)
    Q_PROPERTY(int windingNumber READ windingNumber NOTIFY windingNumberChanged)
    Q_PROPERTY(bool showIntersects READ showIntersects WRITE setShowIntersects NOTIFY
                   showIntersectsChanged)
    Q_PROPERTY(PlineCombineMode plineCombineMode READ plineCombineMode WRITE setPlineCombineMode
                   NOTIFY plineCombineModeChanged)
    Q_PROPERTY(bool showVertexes READ showVertexes WRITE setShowVertexes NOTIFY showVertexesChanged)
    Q_PROPERTY(bool flipArgOrder READ flipArgOrder WRITE setFlipArgOrder NOTIFY flipArgOrderChanged)
    Q_PROPERTY(bool showWindingNumberPoint READ showWindingNumberPoint WRITE
                   setShowWindingNumberPoint NOTIFY showWindingNumberPointChanged)

public:
    enum PlineCombineMode
    {
        NoCombine,
        Union,
        Exclude,
        Intersect,
        XOR,
        CoincidentSlices
    };
    Q_ENUM(PlineCombineMode)

    PlineCombineAlgorithmView(QQuickItem *parent = nullptr);

    int windingNumber() const;
    bool interacting() const;

    bool showIntersects() const;
    void setShowIntersects(bool showIntersects);

    PlineCombineMode plineCombineMode() const;
    void setPlineCombineMode(PlineCombineMode plineCombineMode);

    bool showVertexes() const;
    void setShowVertexes(bool showVertexes);

    bool flipArgOrder() const;
    void setFlipArgOrder(bool flipArgOrder);

    bool showWindingNumberPoint() const;
    void setShowWindingNumberPoint(bool showWindingNumberPoint);

signals:
    void windingNumberChanged(int windingNumber);
    void interactingChanged(bool interacting);
    void showIntersectsChanged(bool showIntersects);
    void plineCombineModeChanged(PlineCombineMode plineCombineMode);
    void showVertexesChanged(bool showVertexes);
    void flipArgOrderChanged(bool flipArgOrder);

    void showWindingNumberPointChanged(bool showWindingNumberPoint);

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    cavc::Polyline<double> m_plineA;
    cavc::Polyline<double> m_plineB;
    cavc::Polyline<double> m_testPoint;
    cavc::Polyline<double> *polyline_grabbed_;

private:
    NgViewModel *m_polylineANode;
    NgViewModel *m_polylineBNode;
    NgViewModel *m_testPointNode;
    PointSetNode *m_intersectsNode;

    void setWindingNumber(int windingNumber);
    void setInteracting(bool interacting);
    QSGOpacityNode *m_dynamicPlinesParentNode;
    int m_windingNumber;

    bool isVertexGrabbed();
    QPointF mouse_pick_pt_;
    std::size_t m_vertexGrabbed;
    QPointF m_origVertexGlobalPos;
    bool m_interacting;
    bool m_showIntersects;
    PlineCombineMode m_plineCombineMode;
    bool m_showVertexes;
    bool m_flipArgOrder;
    bool m_showWindingNumberPoint;
};
} // namespace debugger
#endif // PLINECOMBINEALGORITHMVIEW_H
