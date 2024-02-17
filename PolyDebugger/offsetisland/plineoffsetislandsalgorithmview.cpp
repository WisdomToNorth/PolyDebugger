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
    m_vertexGrabbed(std::numeric_limits<std::size_t>::max()), m_polylineGrabbed(nullptr)
{
    auto machine_type = NgSettings::AppAlgorithmCore();
    std::cout << "type in offset class: " << static_cast<int>(machine_type) << std::endl;
    switch (machine_type)
    {
    case NgSettings::AppAlgorithmCore::kCavc: buildCavcData(); break;
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

    // outer loops
    for (auto const &loop : m_ccwLoops)
    {
        addPline(loop, false, QColor("blue"), m_showVertexes, QColor("blue"));
    }

    // islands
    for (auto const &island : m_cwLoops)
    {
        addPline(island, true, QColor("red"), m_showVertexes);
    }

    if (!utils::fuzzyEqual(m_offsetDelta, 0.0) && m_offsetCount > 0)
    {
        ParallelOffsetIslands<double> alg;
        OffsetLoopSet<double> loopSet;
        for (auto const &loop : m_ccwLoops)
        {
            loopSet.ccwLoops.push_back({0, loop, createApproxSpatialIndex(loop)});
        }
        for (auto const &loop : m_cwLoops)
        {
            loopSet.cwLoops.push_back({0, loop, createApproxSpatialIndex(loop)});
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
    m_globalMouseDownPoint = QPointF(event->globalX(), event->globalY());

    for (auto &loop : m_ccwLoops)
    {
        m_vertexGrabbed = vertexUnderPosition(m_globalMouseDownPoint, loop);
        if (isVertexGrabbed())
        {
            m_polylineGrabbed = &loop;
            m_origVertexGlobalPos = convertToGlobalUICoord(loop[m_vertexGrabbed].pos());
            break;
        }
    }

    if (!isVertexGrabbed())
    {
        for (auto &island : m_cwLoops)
        {
            m_vertexGrabbed = vertexUnderPosition(m_globalMouseDownPoint, island);
            if (isVertexGrabbed())
            {
                m_polylineGrabbed = &island;
                m_origVertexGlobalPos = convertToGlobalUICoord(island[m_vertexGrabbed].pos());
                break;
            }
        }
    }

    if (!isVertexGrabbed())
    {
        event->ignore();
        m_polylineGrabbed = nullptr;
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
    QPointF mouseDelta = QPointF(event->globalX(), event->globalY()) - m_globalMouseDownPoint;
    QPointF newGlobalVertexPos = mouseDelta + m_origVertexGlobalPos;
    QPointF newLocalVertexPos = mapFromGlobal(newGlobalVertexPos);
    QPointF newRealVertexPos = m_uiToRealCoord * newLocalVertexPos;

    (*m_polylineGrabbed)[m_vertexGrabbed].x() = newRealVertexPos.x();
    (*m_polylineGrabbed)[m_vertexGrabbed].y() = newRealVertexPos.y();

    update();
}

void PlineOffsetIslandsAlgorithmView::mouseReleaseEvent(QMouseEvent *event)
{
    if (!isVertexGrabbed())
    {
        return;
    }
    m_vertexGrabbed = std::numeric_limits<std::size_t>::max();
    event->accept();
}

bool PlineOffsetIslandsAlgorithmView::isVertexGrabbed()
{
    return m_vertexGrabbed != std::numeric_limits<std::size_t>::max();
}

void PlineOffsetIslandsAlgorithmView::buildCavcData()
{
    auto radius = 40;
    auto centerX = 0;
    auto centerY = 0;
    std::size_t segmentCount = 16;
    cavc::Polyline<double> mainOuterPline;
    for (std::size_t i = 0; i < segmentCount; ++i)
    {
        double angle = static_cast<double>(i) * utils::tau<double>() / segmentCount;
        mainOuterPline.addVertex(radius * std::cos(angle) + centerX,
                                 radius * std::sin(angle) + centerY, i % 2 == 0 ? 1 : -1);
    }
    mainOuterPline.isClosed() = true;
    m_ccwLoops.push_back(std::move(mainOuterPline));

    cavc::Polyline<double> island1;
    island1.addVertex(-7, -25, 0);
    island1.addVertex(-4, -25, 0);
    island1.addVertex(-4, -15, 0);
    island1.addVertex(-7, -15, 0);
    island1.isClosed() = true;
    invertDirection(island1);
    m_cwLoops.push_back(std::move(island1));

    cavc::Polyline<double> island2;
    island2.addVertex(22, -20, 1);
    island2.addVertex(27, -20, 1);
    island2.isClosed() = true;
    invertDirection(island2);
    m_cwLoops.push_back(std::move(island2));

    cavc::Polyline<double> island3;
    island3.addVertex(0, 25, 1);
    island3.addVertex(-4, 0, 0);
    island3.addVertex(2, 0, 1);
    island3.addVertex(10, 0, -0.5);
    island3.addVertex(8, 9, 0.374794619217547);
    island3.addVertex(21, 0, 0);
    island3.addVertex(23, 0, 1);
    island3.addVertex(32, 0, -0.5);
    island3.addVertex(28, 0, 0.5);
    island3.addVertex(28, 12, 0.5);
    island3.isClosed() = true;
    invertDirection(island3);
    m_cwLoops.push_back(std::move(island3));
}
} // namespace debugger
