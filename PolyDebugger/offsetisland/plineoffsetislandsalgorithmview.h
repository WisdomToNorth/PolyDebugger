#ifndef PLINEOFFSETISLANDSALGORITHMVIEW_H
#define PLINEOFFSETISLANDSALGORITHMVIEW_H

#include <vector>

#include "cavc/polyline.hpp"

#include "adaptor/geometrycanvasitem.h"

class QSGOpacityNode;
namespace debugger
{
class PlineOffsetIslandsAlgorithmView : public GeometryCanvasItem
{
    Q_OBJECT
    Q_PROPERTY(bool showVertexes READ showVertexes WRITE setShowVertexes NOTIFY showVertexesChanged)
    Q_PROPERTY(double offsetDelta READ offsetDelta WRITE setOffsetDelta NOTIFY offsetDeltaChanged)
    Q_PROPERTY(int offsetCount READ offsetCount WRITE setOffsetCount NOTIFY offsetCountChanged)
public:
    explicit PlineOffsetIslandsAlgorithmView(QQuickItem *parent = nullptr);

    bool showVertexes() const;
    void setShowVertexes(bool showVertexes);

    double offsetDelta() const;
    void setOffsetDelta(double offsetDelta);

    int offsetCount() const;
    void setOffsetCount(int offsetCount);

signals:
    void showVertexesChanged(bool showVertexes);
    void offsetDeltaChanged(double offsetDelta);
    void offsetCountChanged(int offsetCount);

protected:
    /*Cavc*/
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    typedef std::vector<std::tuple<double, double, double>> PolygonLoop;
    std::vector<std::pair<PolygonLoop, bool>> cases_data_;
    void createCaseData();

    std::vector<std::pair<cavc::Polyline<double>, bool>> calc_loops_;
    // std::vector<cavc::Polyline<double>> m_cwLoops;
    cavc::Polyline<double> *polyline_grabbed_;

    void buildCavcCase();
    static cavc::Polyline<double>
    buildCavcData(const std::vector<std::tuple<double, double, double>> &data, bool is_hole);

private:
    QSGOpacityNode *m_dynamicPlinesParentNode;
    bool m_showVertexes;
    double m_offsetDelta;
    int m_offsetCount;

    bool isVertexGrabbed();
    void resetVertexGrabbed();

    QPointF mouse_pick_pt_;
    std::pair<int, int> vertex_pick_index_;

    std::size_t m_vertexGrabbed;
    QPointF m_origVertexGlobalPos;
};
} // namespace debugger
#endif // PLINEOFFSETISLANDSALGORITHMVIEW_H
