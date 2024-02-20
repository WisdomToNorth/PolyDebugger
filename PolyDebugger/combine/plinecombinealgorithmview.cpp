#include "combine/plinecombinealgorithmview.h"

#include <QSGGeometryNode>
#include <QSGOpacityNode>
#include <QSGTransformNode>

#include "cavc/polylinecombine.hpp"
#include "cavc/polylineintersects.hpp"

#include "adaptor/pointsetnode.h"
#include "adaptor/viewmodel.h"
#include "graphicshelpers.h"

namespace debugger
{
using namespace cavc;

PlineCombineAlgorithmView::PlineCombineAlgorithmView(QQuickItem *parent) :
    GeometryCanvasItem(parent), m_polylineANode(nullptr), m_polylineBNode(nullptr),
    m_testPointNode(nullptr), m_intersectsNode(nullptr), m_dynamicPlinesParentNode(nullptr),
    m_windingNumber(0), m_vertexGrabbed(std::numeric_limits<std::size_t>::max()),
    polyline_grabbed_(nullptr), m_interacting(false), m_showIntersects(true),
    m_plineCombineMode(NoCombine), m_showVertexes(true), m_flipArgOrder(false),
    m_showWindingNumberPoint(false)
{
    m_plineA.addVertex(0, 1, 1);
    m_plineA.addVertex(10, 1, 1);
    m_plineA.isClosed() = true;

    m_plineB.addVertex(3, -10, 0);
    m_plineB.addVertex(6, -10, 0);
    m_plineB.addVertex(6, 10, 0);
    m_plineB.addVertex(3, 10, 0);
    m_plineB.isClosed() = true;

    m_testPoint.addVertex(1, 1, 0);
}

int PlineCombineAlgorithmView::windingNumber() const
{
    return m_windingNumber;
}

bool PlineCombineAlgorithmView::interacting() const
{
    return m_interacting;
}

bool PlineCombineAlgorithmView::showIntersects() const
{
    return m_showIntersects;
}

void PlineCombineAlgorithmView::setShowIntersects(bool showIntersects)
{
    if (m_showIntersects == showIntersects)
        return;

    m_showIntersects = showIntersects;
    update();
    emit showIntersectsChanged(m_showIntersects);
}

PlineCombineAlgorithmView::PlineCombineMode PlineCombineAlgorithmView::plineCombineMode() const
{
    return m_plineCombineMode;
}

void PlineCombineAlgorithmView::setPlineCombineMode(
    PlineCombineAlgorithmView::PlineCombineMode plineCombineMode)
{
    if (m_plineCombineMode == plineCombineMode)
        return;

    m_plineCombineMode = plineCombineMode;
    update();
    emit plineCombineModeChanged(m_plineCombineMode);
}

bool PlineCombineAlgorithmView::showVertexes() const
{
    return m_showVertexes;
}

void PlineCombineAlgorithmView::setShowVertexes(bool showVertexes)
{
    if (m_showVertexes == showVertexes)
        return;

    m_showVertexes = showVertexes;
    update();
    emit showVertexesChanged(m_showVertexes);
}

bool PlineCombineAlgorithmView::flipArgOrder() const
{
    return m_flipArgOrder;
}

void PlineCombineAlgorithmView::setFlipArgOrder(bool flipArgOrder)
{
    if (m_flipArgOrder == flipArgOrder)
        return;

    m_flipArgOrder = flipArgOrder;
    if (m_plineCombineMode != NoCombine)
    {
        update();
    }
    emit flipArgOrderChanged(m_flipArgOrder);
}

bool PlineCombineAlgorithmView::showWindingNumberPoint() const
{
    return m_showWindingNumberPoint;
}

void PlineCombineAlgorithmView::setShowWindingNumberPoint(bool showWindingNumberPoint)
{
    if (m_showWindingNumberPoint == showWindingNumberPoint)
        return;

    m_showWindingNumberPoint = showWindingNumberPoint;
    update();
    emit showWindingNumberPointChanged(m_showWindingNumberPoint);
}

void PlineCombineAlgorithmView::setInteracting(bool interacting)
{
    if (m_interacting == interacting)
        return;

    m_interacting = interacting;
    emit interactingChanged(m_interacting);
}

bool PlineCombineAlgorithmView::isVertexGrabbed()
{
    return m_vertexGrabbed != std::numeric_limits<std::size_t>::max();
}

void PlineCombineAlgorithmView::setWindingNumber(int windingNumber)
{
    if (m_windingNumber == windingNumber)
        return;

    m_windingNumber = windingNumber;
    emit windingNumberChanged(m_windingNumber);
}

QSGNode *PlineCombineAlgorithmView::updatePaintNode(QSGNode *oldNode,
                                                    QQuickItem::UpdatePaintNodeData *)
{
    QSGTransformNode *rootNode = nullptr;
    if (!oldNode)
    {
        rootNode = new QSGTransformNode();
        m_polylineANode = new NgViewModel();
        m_polylineANode->setFlag(QSGNode::OwnedByParent);
        m_polylineANode->setColor(Qt::blue);
        m_polylineANode->setVertexesColor(Qt::blue);
        rootNode->appendChildNode(m_polylineANode);

        m_polylineBNode = new NgViewModel();
        m_polylineBNode->setFlag(QSGNode::OwnedByParent);
        m_polylineBNode->setColor(Qt::red);
        m_polylineBNode->setVertexesColor(Qt::red);
        rootNode->appendChildNode(m_polylineBNode);

        m_testPointNode = new NgViewModel();
        m_testPointNode->setFlag(QSGNode::OwnedByParent);
        m_testPointNode->setVertexesColor(Qt::darkMagenta);
        rootNode->appendChildNode(m_testPointNode);
    }
    else
    {
        rootNode = static_cast<QSGTransformNode *>(oldNode);
    }
    rootNode->setMatrix(m_realToUICoord);

    setWindingNumber(cavc::getWindingNumber(m_plineA, m_testPoint[0].pos()));

    m_polylineANode->setVertexesVisible(m_showVertexes);
    m_polylineBNode->setVertexesVisible(m_showVertexes);

    m_polylineANode->updateVM(m_plineA);
    m_polylineBNode->updateVM(m_plineB);

    m_testPointNode->setVertexesVisible(m_showWindingNumberPoint);
    if (m_showWindingNumberPoint)
    {
        m_testPointNode->updateVM(m_testPoint);
    }

    auto plineASpatialIndex = createApproxSpatialIndex(m_plineA);
    PlineIntersectsResult<double> inters;
    findIntersects(m_plineA, m_plineB, plineASpatialIndex, inters);
    std::vector<PlineIntersect<double>> intersectsAB;
    for (auto const &intr : inters.intersects)
    {
        intersectsAB.push_back(intr);
    }

    for (auto const &intr : inters.coincidentIntersects)
    {
        intersectsAB.push_back(PlineIntersect<double>(intr.sIndex1, intr.sIndex2, intr.point1));
        intersectsAB.push_back(PlineIntersect<double>(intr.sIndex1, intr.sIndex2, intr.point1));
    }

    if (m_showIntersects)
    {
        if (!m_intersectsNode)
        {
            m_intersectsNode = new PointSetNode();
            m_intersectsNode->setColor(Qt::darkCyan);
            rootNode->appendChildNode(m_intersectsNode);
        }
        else
        {
            m_intersectsNode->clear();
        }

        for (auto const &intr : intersectsAB)
        {
            m_intersectsNode->addPoint(intr.pos.x(), intr.pos.y());
        }
    }
    else if (m_intersectsNode)
    {
        rootNode->removeChildNode(m_intersectsNode);
        delete m_intersectsNode;
        m_intersectsNode = nullptr;
    }

    if (m_plineCombineMode != NoCombine)
    {
        m_polylineANode->setPathVisible(false);
        m_polylineBNode->setPathVisible(false);
        if (!m_dynamicPlinesParentNode)
        {
            m_dynamicPlinesParentNode = new QSGOpacityNode();
            rootNode->appendChildNode(m_dynamicPlinesParentNode);
        }
        m_dynamicPlinesParentNode->setOpacity(1);

        std::size_t sliceIndex = 0;
        NgViewModel *plineNode =
            static_cast<NgViewModel *>(m_dynamicPlinesParentNode->firstChild());
        auto addPline = [&](cavc::Polyline<double> const &pline, bool is_hole)
        {
            if (!plineNode)
            {
                plineNode = new NgViewModel();
                m_dynamicPlinesParentNode->appendChildNode(plineNode);
            }
            plineNode->setColor(gh::indexToColor(sliceIndex));
            plineNode->setIsVisible(true);
            plineNode->setVertexesVisible(false);
            plineNode->updateVM(pline, is_hole);
            plineNode = static_cast<NgViewModel *>(plineNode->nextSibling());
            sliceIndex++;
        };

        if (m_plineCombineMode == CoincidentSlices)
        {
            auto result = internal::processForCombine(m_plineA, m_plineB, plineASpatialIndex);
            for (const auto &slice : result.coincidentSlices)
            {
                addPline(slice, false);
            }
        }
        else
        {
            cavc::PlineCombineMode mode;
            switch (m_plineCombineMode)
            {
            case PlineCombineAlgorithmView::Union: mode = cavc::PlineCombineMode::Union; break;
            case PlineCombineAlgorithmView::Exclude: mode = cavc::PlineCombineMode::Exclude; break;
            case PlineCombineAlgorithmView::Intersect:
                mode = cavc::PlineCombineMode::Intersect;
                break;
            case PlineCombineAlgorithmView::XOR: mode = cavc::PlineCombineMode::XOR; break;
            default: mode = cavc::PlineCombineMode::Union; break;
            }

            auto combineResult = m_flipArgOrder ? combinePolylines(m_plineB, m_plineA, mode) :
                                                  combinePolylines(m_plineA, m_plineB, mode);

            for (const auto &slice : combineResult.remaining)
            {
                addPline(slice, false);
            }

            for (const auto &slice : combineResult.subtracted)
            {
                addPline(slice, true);
            }
        }

        while (plineNode)
        {
            plineNode->setIsVisible(false);
            plineNode = static_cast<NgViewModel *>(plineNode->nextSibling());
        }
    }
    else
    {
        m_polylineANode->setPathVisible(true);
        m_polylineBNode->setPathVisible(true);
        if (m_dynamicPlinesParentNode)
        {
            m_dynamicPlinesParentNode->setOpacity(0);
        }
    }

    return rootNode;
}

void PlineCombineAlgorithmView::mousePressEvent(QMouseEvent *event)
{
    mouse_pick_pt_ = QPointF(event->globalX(), event->globalY());
    // m_vertexGrabbed = vertexUnderPosition(mouse_pick_pt_, m_plineA);

    // if (isVertexGrabbed())
    // {
    //     polyline_grabbed_ = &m_plineA;
    //     m_origVertexGlobalPos = convertToGlobalUICoord(m_plineA[m_vertexGrabbed].pos());
    // }
    // else
    // {
    //     m_vertexGrabbed = vertexUnderPosition(mouse_pick_pt_, m_plineB);
    //     if (isVertexGrabbed())
    //     {
    //         polyline_grabbed_ = &m_plineB;
    //         m_origVertexGlobalPos = convertToGlobalUICoord(m_plineB[m_vertexGrabbed].pos());
    //     }
    //     else
    //     {
    //         m_vertexGrabbed = vertexUnderPosition(mouse_pick_pt_, m_testPoint);
    //         if (!isVertexGrabbed())
    //         {
    //             event->ignore();
    //             polyline_grabbed_ = nullptr;
    //             return;
    //         }

    //         polyline_grabbed_ = &m_testPoint;
    //         m_origVertexGlobalPos = convertToGlobalUICoord(m_testPoint[m_vertexGrabbed].pos());
    //     }
    // }

    // setInteracting(true);

    if (!isVertexGrabbed())
    {
        event->ignore();
        return;
    }

    event->accept();
}

void PlineCombineAlgorithmView::mouseMoveEvent(QMouseEvent *event)
{
    if (!isVertexGrabbed())
    {
        return;
    }

    // convert back from global coordinates to get real delta
    QPointF mouseDelta = QPointF(event->globalX(), event->globalY()) - mouse_pick_pt_;
    QPointF newGlobalVertexPos = mouseDelta + m_origVertexGlobalPos;
    QPointF newLocalVertexPos = mapFromGlobal(newGlobalVertexPos);
    QPointF newRealVertexPos = m_uiToRealCoord * newLocalVertexPos;

    if (polyline_grabbed_ == &m_plineA)
    {
        m_plineA[m_vertexGrabbed].x() = newRealVertexPos.x();
        m_plineA[m_vertexGrabbed].y() = newRealVertexPos.y();
    }
    else if (polyline_grabbed_ == &m_plineB)
    {
        m_plineB[m_vertexGrabbed].x() = newRealVertexPos.x();
        m_plineB[m_vertexGrabbed].y() = newRealVertexPos.y();
    }
    else if (polyline_grabbed_ == &m_testPoint)
    {
        m_testPoint[m_vertexGrabbed].x() = newRealVertexPos.x();
        m_testPoint[m_vertexGrabbed].y() = newRealVertexPos.y();
    }

    update();
}

void PlineCombineAlgorithmView::mouseReleaseEvent(QMouseEvent *event)
{
    if (!isVertexGrabbed())
    {
        return;
    }
    setInteracting(false);
    m_vertexGrabbed = std::numeric_limits<std::size_t>::max();
    event->accept();
}
} // namespace debugger
