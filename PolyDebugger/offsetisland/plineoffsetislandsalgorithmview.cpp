#include "offsetisland/plineoffsetislandsalgorithmview.h"

#include <QSGTransformNode>

#include "cavc/polylineoffsetislands.hpp"

// #include "adaptor/polylinenode.h"
#include "adaptor/viewmodel.h"
#include "settings/settings.h"

using namespace cavc;
namespace debugger
{
PlineOffsetIslandsAlgorithmView::PlineOffsetIslandsAlgorithmView(QQuickItem *parent) :
    GeometryCanvasItem(parent), m_showVertexes(true), m_offsetDelta(1), m_offsetCount(20),
    m_vertexGrabbed(std::numeric_limits<std::size_t>::max())
{
    createCaseData();

    auto machine_type = NgSettings::AppAlgorithmCore();
    std::cout << "type in offset class: " << static_cast<int>(machine_type) << std::endl;
    switch (machine_type)
    {
    case NgSettings::AppAlgorithmCore::kCavc: buildCavcCase(); break;
    case NgSettings::AppAlgorithmCore::kNGPoly: break;
    case NgSettings::AppAlgorithmCore::kClipper: break;

    default: break;
    }
}

bool PlineOffsetIslandsAlgorithmView::showVertexes() const
{
    return m_showVertexes;
}

void PlineOffsetIslandsAlgorithmView::setShowVertexes(bool showVertexes)
{
    if (m_showVertexes == showVertexes)
        return;

    m_showVertexes = showVertexes;
    update();
    emit showVertexesChanged(m_showVertexes);
}

double PlineOffsetIslandsAlgorithmView::offsetDelta() const
{
    return m_offsetDelta;
}

void PlineOffsetIslandsAlgorithmView::setOffsetDelta(double offsetDelta)
{
    if (qFuzzyCompare(m_offsetDelta, offsetDelta))
    {
        return;
    }

    m_offsetDelta = offsetDelta;
    update();
    emit offsetDeltaChanged(m_offsetDelta);
}

int PlineOffsetIslandsAlgorithmView::offsetCount() const
{
    return m_offsetCount;
}

void PlineOffsetIslandsAlgorithmView::setOffsetCount(int offsetCount)
{
    if (m_offsetCount == offsetCount)
        return;

    m_offsetCount = offsetCount;
    update();
    emit offsetCountChanged(m_offsetCount);
}

QSGNode *PlineOffsetIslandsAlgorithmView::updatePaintNode(QSGNode *oldNode,
                                                          QQuickItem::UpdatePaintNodeData *)
{
    QSGTransformNode *rootNode = nullptr;
    if (!oldNode)
    {
        rootNode = new QSGTransformNode();
        m_dynamicPlinesParentNode = new QSGOpacityNode();
        rootNode->appendChildNode(m_dynamicPlinesParentNode);
        m_dynamicPlinesParentNode->setOpacity(1);
    }
    else
    {
        rootNode = static_cast<QSGTransformNode *>(oldNode);
    }
    rootNode->setMatrix(m_realToUICoord);

    NgViewModel *plineNode = static_cast<NgViewModel *>(m_dynamicPlinesParentNode->firstChild());

    auto addPline = [&](cavc::Polyline<double> const &pline, bool is_hole,
                        QColor color = QColor("blue"), bool vertexesVisible = false,
                        QColor vertexesColor = QColor("red"))
    {
        if (!plineNode)
        {
            plineNode = new NgViewModel();
            m_dynamicPlinesParentNode->appendChildNode(plineNode);
        }
        plineNode->setColor(color);
        plineNode->setIsVisible(true);
        plineNode->setVertexesVisible(vertexesVisible);
        plineNode->setVertexesColor(vertexesColor);
        plineNode->updateVM(pline, is_hole);
        plineNode = static_cast<NgViewModel *>(plineNode->nextSibling());
    };

    for (const auto &[data, is_hole] : calc_loops_)
    {
        if (is_hole)
        {
            addPline(data, true, QColor("red"), m_showVertexes, QColor("red"));
        }
        else
        {
            addPline(data, false, QColor("blue"), m_showVertexes, QColor("blue"));
        }
    }

    if (!utils::fuzzyEqual(m_offsetDelta, 0.0) && m_offsetCount > 0)
    {
        ParallelOffsetIslands<double> alg;
        OffsetLoopSet<double> loopSet;
        for (const auto &[data, is_hole] : calc_loops_)
        {
            if (is_hole)
            {
                loopSet.cwLoops.push_back({0, data, createApproxSpatialIndex(data)});
            }
            else
            {
                loopSet.ccwLoops.push_back({0, data, createApproxSpatialIndex(data)});
            }
        }

        int i = 0;
        while (i < m_offsetCount)
        {
            loopSet = alg.compute(loopSet, m_offsetDelta);
            if (loopSet.cwLoops.size() == 0 && loopSet.ccwLoops.size() == 0)
            {
                break;
            }
            for (auto const &loop : loopSet.cwLoops)
            {
                addPline(loop.polyline, false);
            }
            for (auto const &loop : loopSet.ccwLoops)
            {
                addPline(loop.polyline, false);
            }
            i += 1;
        }
    }

    while (plineNode)
    {
        plineNode->setIsVisible(false);
        plineNode = static_cast<NgViewModel *>(plineNode->nextSibling());
    }

    return rootNode;
}

void PlineOffsetIslandsAlgorithmView::mousePressEvent(QMouseEvent *event)
{
    mouse_pick_pt_ = QPointF(event->globalX(), event->globalY());

    // find if pick to point in cases_data_, record to vertex_pick_index_. if not find, put {-1, -1}
    for (int i = 0; i < cases_data_.size(); i++)
    {
        auto const &data = cases_data_[i].first;
        for (int j = 0; j < data.size(); j++)
        {
            QPointF vPosInGlobal =
                convertToGlobalUICoord(QPointF(std::get<0>(data[j]), std::get<1>(data[j])));
            if (utils::fuzzyEqual(vPosInGlobal.x(), mouse_pick_pt_.x(), 5.0)
                && utils::fuzzyEqual(vPosInGlobal.y(), mouse_pick_pt_.y(), 5.0))
            {
                vertex_pick_index_ = std::make_pair(i, j);
                break;
            }
        }
    }

    if (!isVertexGrabbed())
    {
        event->ignore();

        return;
    }

    event->accept();
}

void PlineOffsetIslandsAlgorithmView::mouseMoveEvent(QMouseEvent *event)
{
    if (!isVertexGrabbed())
    {
        return;
    }

    // convert back from global coordinates to get real delta
    QPointF newGlobalVertexPos = QPointF(event->globalX(), event->globalY());
    QPointF newLocalVertexPos = mapFromGlobal(newGlobalVertexPos);
    QPointF newRealVertexPos = m_uiToRealCoord * newLocalVertexPos;

    int pline_index = vertex_pick_index_.first;
    int vertex_index = vertex_pick_index_.second;
    auto &case_changed = cases_data_[pline_index].first[vertex_index];

    std::get<0>(case_changed) = newRealVertexPos.x();
    std::get<1>(case_changed) = newRealVertexPos.y();

    switch (NgSettings::AppAlgorithmCore())
    {
    case NgSettings::AppAlgorithmCore::kCavc:
    {
        ////way 1:  regenerate cavc data
        calc_loops_[pline_index] = {
            buildCavcData(cases_data_[pline_index].first, cases_data_[pline_index].second),
            cases_data_[pline_index].second};

        ////way 2:  update polyline, more efficient, not able to work well if index is 0 or n-1
        // auto &vertex = calc_loops_[pline_index].first[vertex_index];
        // vertex.x() = newRealVertexPos.x();
        // vertex.y() = newRealVertexPos.y();
        break;
    }
    case NgSettings::AppAlgorithmCore::kNGPoly: break;
    case NgSettings::AppAlgorithmCore::kClipper: break;

    default: break;
    }

    update();
}

void PlineOffsetIslandsAlgorithmView::mouseReleaseEvent(QMouseEvent *event)
{
    if (!isVertexGrabbed())
    {
        return;
    }
    else
    {
        m_vertexGrabbed = std::numeric_limits<std::size_t>::max();
        event->accept();
        resetVertexGrabbed();
    }
}

bool PlineOffsetIslandsAlgorithmView::isVertexGrabbed()
{
    return vertex_pick_index_.first != -1 && vertex_pick_index_.second != -1;
}

void PlineOffsetIslandsAlgorithmView::resetVertexGrabbed()
{
    vertex_pick_index_ = std::make_pair(-1, -1);
}

cavc::Polyline<double> PlineOffsetIslandsAlgorithmView::buildCavcData(
    const std::vector<std::tuple<double, double, double>> &data, bool is_hole)
{
    cavc::Polyline<double> pline;
    for (auto const &pt : data)
    {
        pline.addVertex(std::get<0>(pt), std::get<1>(pt), std::get<2>(pt));
    }
    pline.isClosed() = true;
    if (is_hole)
    {
        invertDirection(pline);
    }
    return pline;
}

void PlineOffsetIslandsAlgorithmView::createCaseData()
{
    std::vector<std::pair<PolygonLoop, bool>> case_data;

    /*outboundry*/
    auto radius = 40;
    auto centerX = 0;
    auto centerY = 0;
    std::size_t segmentCount = 16;
    std::vector<std::tuple<double, double, double>> data1;
    for (std::size_t i = 0; i < segmentCount; ++i)
    {
        double angle = static_cast<double>(i) * utils::tau<double>() / segmentCount;
        data1.push_back(std::make_tuple(radius * std::cos(angle) + centerX,
                                        radius * std::sin(angle) + centerY, i % 2 == 0 ? 1 : -1));
    }
    /*hole*/
    std::vector<std::tuple<double, double, double>> data2{
        std::make_tuple(-7, -25, 0), std::make_tuple(-4, -25, 0), std::make_tuple(-4, -15, 0),
        std::make_tuple(-7, -15, 0)};

    /*hole*/
    std::vector<std::tuple<double, double, double>> data3{std::make_tuple(22, -20, 1),
                                                          std::make_tuple(27, -20, 1)};
    /*hole*/
    std::vector<std::tuple<double, double, double>> data4{std::make_tuple(0, 25, 1),
                                                          std::make_tuple(-4, 0, 0),
                                                          std::make_tuple(2, 0, 1),
                                                          std::make_tuple(10, 0, -0.5),
                                                          std::make_tuple(8, 9, 0.374794619217547),
                                                          std::make_tuple(21, 0, 0),
                                                          std::make_tuple(23, 0, 1),
                                                          std::make_tuple(32, 0, -0.5),
                                                          std::make_tuple(28, 0, 0.5),
                                                          std::make_tuple(28, 12, 0.5)};

    case_data.push_back(std::make_pair(data1, false));
    case_data.push_back(std::make_pair(data2, true));
    case_data.push_back(std::make_pair(data3, true));
    case_data.push_back(std::make_pair(data4, true));
    cases_data_.swap(case_data);
}

void PlineOffsetIslandsAlgorithmView::buildCavcCase()
{
    calc_loops_.clear();
    for (auto const &data : cases_data_)
    {
        cavc::Polyline<double> pline = buildCavcData(data.first, data.second);
        calc_loops_.push_back({std::move(pline), data.second});
    }
}
} // namespace debugger
