#include "offsetisland/plineoffsetislandsalgorithmview.h"

#include <QSGTransformNode>

#include "cavc/polylineoffsetislands.hpp"

// #include "adaptor/polylinenode.h"
#include "adaptor/viewmodel.h"
#include "datamanager.h"
#include "settings/settings.h"

using namespace cavc;
namespace debugger
{
OffsetIslandsView::OffsetIslandsView(QQuickItem *parent) :
    GeometryCanvasItem(parent), m_showVertexes(true), m_offsetDelta(1), m_offsetCount(20),
    m_vertexGrabbed(std::numeric_limits<std::size_t>::max())
{
    auto &docData = DocumetData::getInstance();
    docData.changeData("default1");

    auto machine_type = NgSettings::AppAlgorithmCore();
    std::cout << "type in offset class: " << static_cast<int>(machine_type) << std::endl;
    switch (machine_type)
    {
    case NgSettings::AppAlgorithmCore::kCavc:
    {
        DataUtils::buildCavcCase(docData.case_data_, cavc_polygonset_);
        break;
    }
    case NgSettings::AppAlgorithmCore::kNGPoly: break;
    case NgSettings::AppAlgorithmCore::kClipper: break;

    default: break;
    }
}

QString OffsetIslandsView::caseIndex() const
{
    return DocumetData::getInstance().case_index_;
}

void OffsetIslandsView::setCaseIndex(QString caseindex)
{
    std::cout << "caseindex: " << caseindex.toStdString() << std::endl;
    if (caseindex == DocumetData::getInstance().case_index_)
    {
        return;
    }
    std::cout << "change case data" << std::endl;

    DocumetData::getInstance().changeData(caseindex);
    DataUtils::buildCavcCase(DocumetData::getInstance().case_data_, cavc_polygonset_);

    emit changeCaseDataSignal(caseindex);
    update();
}

bool OffsetIslandsView::showVertexes() const
{
    return m_showVertexes;
}

void OffsetIslandsView::setShowVertexes(bool showVertexes)
{
    if (m_showVertexes == showVertexes)
        return;

    m_showVertexes = showVertexes;
    update();
    emit showVertexesChanged(m_showVertexes);
}

double OffsetIslandsView::offsetDelta() const
{
    return m_offsetDelta;
}

void OffsetIslandsView::setOffsetDelta(double offsetDelta)
{
    if (qFuzzyCompare(m_offsetDelta, offsetDelta))
    {
        return;
    }

    m_offsetDelta = offsetDelta;
    update();
    emit offsetDeltaChanged(m_offsetDelta);
}

int OffsetIslandsView::offsetCount() const
{
    return m_offsetCount;
}

void OffsetIslandsView::setOffsetCount(int offsetCount)
{
    if (m_offsetCount == offsetCount)
        return;

    m_offsetCount = offsetCount;
    update();
    emit offsetCountChanged(m_offsetCount);
}

QSGNode *OffsetIslandsView::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *)
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

    for (const auto &[data, is_hole] : cavc_polygonset_)
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
        for (const auto &[data, is_hole] : cavc_polygonset_)
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

void OffsetIslandsView::mousePressEvent(QMouseEvent *event)
{
    mouse_pick_pt_ = QPointF(event->globalX(), event->globalY());

    auto &case_data = DocumetData::getInstance().case_data_;

    // find if pick to point in case_data_, record to vertex_pick_index_. if not find, put {-1, -1}
    for (int i = 0; i < case_data.size(); i++)
    {
        auto const &data = case_data[i].first;
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

void OffsetIslandsView::mouseMoveEvent(QMouseEvent *event)
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
    auto &case_changed = DocumetData::getInstance().case_data_[pline_index].first[vertex_index];

    std::get<0>(case_changed) = newRealVertexPos.x();
    std::get<1>(case_changed) = newRealVertexPos.y();

    switch (NgSettings::AppAlgorithmCore())
    {
    case NgSettings::AppAlgorithmCore::kCavc:
    {
        ////way 1:  regenerate cavc data
        IsHole is_hole = DocumetData::getInstance().case_data_[pline_index].second;
        cavc_polygonset_[pline_index] = {
            DataUtils::buildCavcPline(DocumetData::getInstance().case_data_[pline_index].first,
                                      is_hole),
            is_hole};

        ////way 2:  update polyline, more efficient, not able to work well if index is 0 or n-1
        // auto &vertex = cavc_polygonset_[pline_index].first[vertex_index];
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

void OffsetIslandsView::mouseReleaseEvent(QMouseEvent *event)
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

bool OffsetIslandsView::isVertexGrabbed()
{
    return vertex_pick_index_.first != -1 && vertex_pick_index_.second != -1;
}

void OffsetIslandsView::resetVertexGrabbed()
{
    vertex_pick_index_ = std::make_pair(-1, -1);
}

} // namespace debugger
