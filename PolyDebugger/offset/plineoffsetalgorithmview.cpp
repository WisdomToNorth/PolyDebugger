#include "plineoffsetalgorithmview.h"

#include <QRandomGenerator>
#include <QSGNode>
#include <QSGTransformNode>

#include "cavc/internal/diagnostics.hpp"
#include "cavc/polylineoffset.hpp"

#include "adaptor/plinesegmentnode.h"
#include "adaptor/pointsetnode.h"
#include "adaptor/viewmodel.h"
#include "graphicshelpers.h"
#include "rawoffsetsegmentsnode.h"
#include "spatialindexboundingboxesnode.h"

namespace debugger
{
using namespace cavc;

PlineOffsetAlgorithmView::PlineOffsetAlgorithmView(QQuickItem *parent) :
    GeometryCanvasItem(parent), m_origPolylineNode(nullptr), m_rawOffsetPolylineNode(nullptr),
    m_dualRawOffsetPolylineNode(nullptr), m_untrimmedSegmentsParentNode(nullptr),
    m_selfIntersectsNode(nullptr), m_boundingBoxesNode(nullptr), m_slicesParentNode(nullptr),
    m_repeatOffsetsParentNode(nullptr), m_endPointIntersectCirclesNode(nullptr),
    m_arcApproxError(0.005), mouse_pick_pt_(),
    m_vertexGrabbed(std::numeric_limits<std::size_t>::max()), m_interacting(false),
    m_showOrigPlineVertexes(true), m_showRawOffsetSegments(false), m_showRawOffsetPolyline(false),
    m_showRawOffsetPlineVertexes(false), m_plineOffset(0.5), m_offsetCount(10),
    m_spatialIndexTarget(None), m_selfIntersectsTarget(None), m_finishedPolyline(NoFinishedPline),
    m_showEndPointIntersectCircles(false), m_showDualRawOffsetPolyline(false),
    m_showLastPrunedRawOffsets(false)
{
    input_polyline_.addVertex(0, 25, 1);
    input_polyline_.addVertex(0, 0, 0);
    input_polyline_.addVertex(2, 0, 1);
    input_polyline_.addVertex(10, 0, -0.5);
    input_polyline_.addVertex(8, 9, 0.374794619217547);
    input_polyline_.addVertex(21, 0, 0);
    input_polyline_.addVertex(23, 0, 1);
    input_polyline_.addVertex(32, 0, -0.5);
    input_polyline_.addVertex(28, 0, 0.5);
    input_polyline_.addVertex(39, 21, 0);
    input_polyline_.addVertex(28, 12, 0.5);
    input_polyline_.isClosed() = true;
}

bool PlineOffsetAlgorithmView::interacting() const
{
    return m_interacting;
}

bool PlineOffsetAlgorithmView::showRawOffsetSegments() const
{
    return m_showRawOffsetSegments;
}

bool PlineOffsetAlgorithmView::showRawOffsetPolyline() const
{
    return m_showRawOffsetPolyline;
}

void PlineOffsetAlgorithmView::setShowRawOffsetSegments(bool showRawOffsetSegments)
{
    if (m_showRawOffsetSegments == showRawOffsetSegments)
        return;

    m_showRawOffsetSegments = showRawOffsetSegments;
    update();
    emit showRawOffsetSegmentsChanged(m_showRawOffsetSegments);
}

void PlineOffsetAlgorithmView::setShowRawOffsetPolyline(bool showRawOffsetPolyline)
{
    if (m_showRawOffsetPolyline == showRawOffsetPolyline)
        return;

    m_showRawOffsetPolyline = showRawOffsetPolyline;
    update();
    emit showRawOffsetPolylineChanged(m_showRawOffsetPolyline);
}

double PlineOffsetAlgorithmView::plineOffset() const
{
    return m_plineOffset;
}

void PlineOffsetAlgorithmView::setPlineOffset(double plineOffset)
{
    if (qFuzzyCompare(m_plineOffset, plineOffset))
        return;

    m_plineOffset = plineOffset;
    update();
    emit plineOffsetChanged(m_plineOffset);
}

bool PlineOffsetAlgorithmView::showOrigPlineVertexes() const
{
    return m_showOrigPlineVertexes;
}

void PlineOffsetAlgorithmView::setShowOrigPlineVertexes(bool showOrigPlineVertexes)
{
    if (m_showOrigPlineVertexes == showOrigPlineVertexes)
        return;

    m_showOrigPlineVertexes = showOrigPlineVertexes;
    update();
    emit showOrigPlineVertexesChanged(m_showOrigPlineVertexes);
}

bool PlineOffsetAlgorithmView::showRawOffsetPlineVertexes() const
{
    return m_showRawOffsetPlineVertexes;
}

void PlineOffsetAlgorithmView::setShowRawOffsetPlineVertexes(bool showRawOffsetPlineVertexes)
{
    if (m_showRawOffsetPlineVertexes == showRawOffsetPlineVertexes)
        return;

    m_showRawOffsetPlineVertexes = showRawOffsetPlineVertexes;
    update();
    emit showRawOffsetPlineVertexesChanged(m_showRawOffsetPlineVertexes);
}

PlineOffsetAlgorithmView::TargetPolyline PlineOffsetAlgorithmView::spatialIndexTarget() const
{
    return m_spatialIndexTarget;
}

void PlineOffsetAlgorithmView::setSpatialIndexTarget(
    PlineOffsetAlgorithmView::TargetPolyline spatialIndexTarget)
{
    if (m_spatialIndexTarget == spatialIndexTarget)
        return;

    m_spatialIndexTarget = spatialIndexTarget;
    update();
    emit spatialIndexTargetChanged(m_spatialIndexTarget);
}

PlineOffsetAlgorithmView::TargetPolyline PlineOffsetAlgorithmView::selfIntersectsTarget() const
{
    return m_selfIntersectsTarget;
}

void PlineOffsetAlgorithmView::setSelfIntersectsTarget(
    PlineOffsetAlgorithmView::TargetPolyline selfIntersectsTarget)
{
    if (m_selfIntersectsTarget == selfIntersectsTarget)
        return;

    m_selfIntersectsTarget = selfIntersectsTarget;
    update();
    emit selfIntersectsTargetChanged(m_selfIntersectsTarget);
}

PlineOffsetAlgorithmView::FinishedPolyline PlineOffsetAlgorithmView::finishedPolyline() const
{
    return m_finishedPolyline;
}

void PlineOffsetAlgorithmView::setFinishedPolyline(
    PlineOffsetAlgorithmView::FinishedPolyline finishedPolyline)
{
    if (m_finishedPolyline == finishedPolyline)
        return;

    m_finishedPolyline = finishedPolyline;
    update();
    emit finishedPolylineChanged(m_finishedPolyline);
}

int PlineOffsetAlgorithmView::offsetCount() const
{
    return m_offsetCount;
}

void PlineOffsetAlgorithmView::setOffsetCount(int offsetCount)
{
    if (m_offsetCount == offsetCount)
        return;

    m_offsetCount = offsetCount;
    update();
    emit offsetCountChanged(m_offsetCount);
}

bool PlineOffsetAlgorithmView::showEndPointIntersectCircles() const
{
    return m_showEndPointIntersectCircles;
}

void PlineOffsetAlgorithmView::setShowEndPointIntersectCircles(bool showEndPointIntersectCircles)
{
    if (m_showEndPointIntersectCircles == showEndPointIntersectCircles)
        return;

    m_showEndPointIntersectCircles = showEndPointIntersectCircles;
    update();
    emit showEndPointIntersectCirclesChanged(m_showEndPointIntersectCircles);
}

bool PlineOffsetAlgorithmView::showDualRawOffsetPolyline() const
{
    return m_showDualRawOffsetPolyline;
}

void PlineOffsetAlgorithmView::setShowDualRawOffsetPolyline(bool showDualRawOffsetPolyline)
{
    if (m_showDualRawOffsetPolyline == showDualRawOffsetPolyline)
        return;

    m_showDualRawOffsetPolyline = showDualRawOffsetPolyline;
    update();
    emit showDualRawOffsetPolylineChanged(m_showDualRawOffsetPolyline);
}

bool PlineOffsetAlgorithmView::showLastPrunedRawOffsets() const
{
    return m_showLastPrunedRawOffsets;
}

void PlineOffsetAlgorithmView::setShowLastPrunedRawOffsets(bool showLastPrunedRawOffsets)
{
    if (m_showLastPrunedRawOffsets == showLastPrunedRawOffsets)
        return;

    m_showLastPrunedRawOffsets = showLastPrunedRawOffsets;
    update();
    emit showLastPrunedRawOffsetsChanged(m_showLastPrunedRawOffsets);
}

void PlineOffsetAlgorithmView::setInteracting(bool interacting)
{
    if (m_interacting == interacting)
        return;

    m_interacting = interacting;
    emit interactingChanged(m_interacting);
}

QSGNode *PlineOffsetAlgorithmView::updatePaintNode(QSGNode *oldNode,
                                                   QQuickItem::UpdatePaintNodeData *)
{
    QSGTransformNode *rootNode = nullptr;
    if (!oldNode)
    {
        rootNode = new QSGTransformNode();
        m_origPolylineNode = new NgViewModel();
        m_origPolylineNode->setVertexesVisible(true);
        m_origPolylineNode->setFlag(QSGNode::OwnedByParent);
        rootNode->appendChildNode(m_origPolylineNode);

        m_rawOffsetPolylineNode = new NgViewModel();
        m_rawOffsetPolylineNode->setFlag(QSGNode::OwnedByParent);
        m_rawOffsetPolylineNode->setColor(Qt::darkGreen);
        rootNode->appendChildNode(m_rawOffsetPolylineNode);

        m_dualRawOffsetPolylineNode = new NgViewModel();
        m_dualRawOffsetPolylineNode->setFlag(QSGNode::OwnedByParent);
        m_dualRawOffsetPolylineNode->setColor(Qt::darkMagenta);
        rootNode->appendChildNode(m_dualRawOffsetPolylineNode);

        m_untrimmedSegmentsParentNode = new RawOffsetSegmentsNode();
        m_untrimmedSegmentsParentNode->setOpacity(0);
        m_untrimmedSegmentsParentNode->setFlag(QSGNode::OwnedByParent);
        rootNode->appendChildNode(m_untrimmedSegmentsParentNode);

        m_boundingBoxesNode = new SpatialIndexBoundingBoxesNode();
        m_boundingBoxesNode->setOpacity(0);
        m_boundingBoxesNode->setFlag(QSGNode::OwnedByParent);
        rootNode->appendChildNode(m_boundingBoxesNode);

        m_endPointIntersectCirclesNode = new QSGOpacityNode();
        m_endPointIntersectCirclesNode->setFlag(QSGNode::OwnedByParent);
        m_endPointIntersectCirclesNode->setOpacity(0);
        rootNode->appendChildNode(m_endPointIntersectCirclesNode);
    }
    else
    {
        rootNode = static_cast<QSGTransformNode *>(oldNode);
    }

    rootNode->setMatrix(m_realToUICoord);
    const cavc::Polyline<double> &prunedPline =
        pruneSingularities(input_polyline_, utils::realPrecision<double>());
    m_origPolylineNode->setVertexesVisible(m_showOrigPlineVertexes);
    m_origPolylineNode->setArcApproxError(m_arcApproxError);
    m_origPolylineNode->updateVM(prunedPline, false);

    // raw offset polyline
    cavc::Polyline<double> rawOffsetPline;
    if (m_showRawOffsetPolyline)
    {
        rawOffsetPline = internal::createRawOffsetPline(prunedPline, m_plineOffset);
        m_rawOffsetPolylineNode->setIsVisible(true);
        m_rawOffsetPolylineNode->setVertexesVisible(m_showRawOffsetPlineVertexes);
        m_rawOffsetPolylineNode->setArcApproxError(m_arcApproxError);
        m_rawOffsetPolylineNode->updateVM(rawOffsetPline, false);
    }
    else
    {
        m_rawOffsetPolylineNode->setIsVisible(false);
    }

    // dual raw offset polyline
    cavc::Polyline<double> dualRawOffsetPline;
    if (m_showRawOffsetPolyline && m_showDualRawOffsetPolyline)
    {
        dualRawOffsetPline = internal::createRawOffsetPline(prunedPline, -m_plineOffset);
        m_dualRawOffsetPolylineNode->setIsVisible(true);
        m_dualRawOffsetPolylineNode->setVertexesVisible(m_showRawOffsetPlineVertexes);
        m_dualRawOffsetPolylineNode->setArcApproxError(m_arcApproxError);
        m_dualRawOffsetPolylineNode->updateVM(dualRawOffsetPline, false);
    }
    else
    {
        m_dualRawOffsetPolylineNode->setIsVisible(false);
    }

    // raw offset segments
    if (m_showRawOffsetSegments)
    {
        m_untrimmedSegmentsParentNode->setOpacity(1);
        auto rawOffsetSegments =
            internal::createUntrimmedOffsetSegments(prunedPline, m_plineOffset);
        m_untrimmedSegmentsParentNode->updateGeometry(rawOffsetSegments, m_arcApproxError);
    }
    else
    {
        m_untrimmedSegmentsParentNode->setOpacity(0);
    }

    // self intersects
    std::vector<cavc::PlineIntersect<double>> selfIntersects;
    switch (m_selfIntersectsTarget)
    {
    case PlineOffsetAlgorithmView::None: break;
    case PlineOffsetAlgorithmView::OriginalPolyline:
    {
        auto spatialindex = createApproxSpatialIndex(prunedPline);
        allSelfIntersects(prunedPline, selfIntersects, spatialindex);
    }
    break;
    case PlineOffsetAlgorithmView::RawOffsetPolyline:
    {
        if (rawOffsetPline.size() == 0)
        {
            rawOffsetPline = internal::createRawOffsetPline(prunedPline, m_plineOffset);
        }
        auto spatialIndex = createApproxSpatialIndex(rawOffsetPline);
        allSelfIntersects(rawOffsetPline, selfIntersects, spatialIndex);

        if (dualRawOffsetPline.size() != 0)
        {
            PlineIntersectsResult<double> intrResults;
            findIntersects(rawOffsetPline, dualRawOffsetPline, spatialIndex, intrResults);
            std::copy(intrResults.intersects.begin(), intrResults.intersects.end(),
                      std::back_inserter(selfIntersects));
        }
    }
    break;
    }

    if (selfIntersects.size() != 0)
    {
        if (!m_selfIntersectsNode)
        {
            m_selfIntersectsNode = new PointSetNode();
            m_selfIntersectsNode->setColor(Qt::darkCyan);
            rootNode->appendChildNode(m_selfIntersectsNode);
        }
        else
        {
            m_selfIntersectsNode->clear();
        }

        for (auto const &intr : selfIntersects)
        {
            m_selfIntersectsNode->addPoint(intr.pos.x(), intr.pos.y());
        }
    }
    else if (m_selfIntersectsNode)
    {
        rootNode->removeChildNode(m_selfIntersectsNode);
        delete m_selfIntersectsNode;
        m_selfIntersectsNode = nullptr;
    }

    // end point intersect circle for open polylines
    if (m_showEndPointIntersectCircles && !prunedPline.isClosed())
    {
        m_endPointIntersectCirclesNode->setOpacity(1);
        QSGGeometryNode *startNode =
            static_cast<QSGGeometryNode *>(m_endPointIntersectCirclesNode->firstChild());
        QSGGeometryNode *endNode;
        if (!startNode)
        {
            startNode =
                gh::createSimpleGeomNode(50, QColor("orange"), 1, QSGGeometry::DrawLineLoop);
            m_endPointIntersectCirclesNode->appendChildNode(startNode);
            endNode = gh::createSimpleGeomNode(50, QColor("orange"), 1, QSGGeometry::DrawLineLoop);
            m_endPointIntersectCirclesNode->appendChildNode(endNode);
        }
        else
        {
            endNode = static_cast<QSGGeometryNode *>(startNode->nextSibling());
        }

        auto fillCircle = [this](QSGGeometryNode *n, Vector2<double> const &center)
        {
            auto *vData = n->geometry()->vertexDataAsPoint2D();
            auto rad = std::abs(m_plineOffset);
            for (std::size_t i = 0; i < 50; ++i)
            {
                double angle = static_cast<double>(i) / 50 * cavc::utils::tau<double>();
                auto pt = cavc::pointOnCircle(rad, center, angle);
                vData[i].set(static_cast<float>(pt.x()), static_cast<float>(pt.y()));
            }
        };

        fillCircle(startNode, prunedPline[0].pos());
        fillCircle(endNode, prunedPline.lastVertex().pos());

        startNode->markDirty(QSGNode::DirtyGeometry);
        endNode->markDirty(QSGNode::DirtyGeometry);
    }
    else
    {
        m_endPointIntersectCirclesNode->setOpacity(0);
    }

    // spatial index bounding box
    switch (m_spatialIndexTarget)
    {
    case PlineOffsetAlgorithmView::None: m_boundingBoxesNode->setOpacity(0); break;
    case PlineOffsetAlgorithmView::OriginalPolyline:
    {
        m_boundingBoxesNode->setOpacity(1);
        auto spatialIndex = createApproxSpatialIndex(prunedPline);
        m_boundingBoxesNode->updateGeometry(spatialIndex);
        break;
    }
    case PlineOffsetAlgorithmView::RawOffsetPolyline:
    {
        if (rawOffsetPline.size() > 1)
        {
            m_boundingBoxesNode->setOpacity(1);
            auto spatialIndex = createApproxSpatialIndex(rawOffsetPline);
            m_boundingBoxesNode->updateGeometry(spatialIndex);
        }
        else
        {
            m_boundingBoxesNode->setOpacity(0);
        }
        break;
    }
    }

    // slices
    if (m_finishedPolyline != PlineOffsetAlgorithmView::NoFinishedPline)
    {
        if (rawOffsetPline.size() == 0)
        {
            rawOffsetPline = internal::createRawOffsetPline(prunedPline, m_plineOffset);
        }
        if (!m_slicesParentNode)
        {
            m_slicesParentNode = new QSGOpacityNode();
            rootNode->appendChildNode(m_slicesParentNode);
        }

        m_slicesParentNode->setOpacity(1);
        if (dualRawOffsetPline.size() == 0)
        {
            dualRawOffsetPline = internal::createRawOffsetPline(prunedPline, -m_plineOffset);
        }
        auto slices = internal::dualSliceAtIntersectsForOffset(prunedPline, rawOffsetPline,
                                                               dualRawOffsetPline, m_plineOffset);

        std::size_t sliceIndex = 0;
        NgViewModel *sliceNode = static_cast<NgViewModel *>(m_slicesParentNode->firstChild());
        auto addPline = [&](cavc::Polyline<double> const &pline)
        {
            if (!sliceNode)
            {
                sliceNode = new NgViewModel();
                m_slicesParentNode->appendChildNode(sliceNode);
            }
            sliceNode->setColor(gh::indexToColor(sliceIndex));
            sliceNode->setIsVisible(true);
            sliceNode->setVertexesVisible(false);
            sliceNode->setArcApproxError(m_arcApproxError);
            sliceNode->updateVM(pline, false);

            sliceNode = static_cast<NgViewModel *>(sliceNode->nextSibling());
            sliceIndex++;
        };

        if (m_finishedPolyline == PlineOffsetAlgorithmView::Slices)
        {
            for (const auto &slice : slices)
            {
                addPline(slice.pline);
            }
        }
        else if (m_finishedPolyline == PlineOffsetAlgorithmView::DualSlices)
        {
            auto dualSlices = internal::dualSliceAtIntersectsForOffset(
                prunedPline, dualRawOffsetPline, rawOffsetPline, -m_plineOffset);
            slices.insert(slices.end(), dualSlices.begin(), dualSlices.end());
            for (const auto &slice : slices)
            {
                addPline(slice.pline);
            }
        }
        else
        {
            auto stichedPlines = stitchOffsetSlicesTogether(slices, rawOffsetPline.isClosed(),
                                                            rawOffsetPline.size() - 1);
            for (const auto &pline : stichedPlines)
            {
                addPline(pline);
            }
        }

        while (sliceNode)
        {
            sliceNode->setIsVisible(false);
            sliceNode = static_cast<NgViewModel *>(sliceNode->nextSibling());
        }
    }
    else if (m_slicesParentNode)
    {
        m_slicesParentNode->setOpacity(0);
    }

    // repeat offsets
    if (m_offsetCount > 0 && !qFuzzyCompare(m_plineOffset, 0.0))
    {
        if (!m_repeatOffsetsParentNode)
        {
            m_repeatOffsetsParentNode = new QSGOpacityNode();
            rootNode->appendChildNode(m_repeatOffsetsParentNode);
        }

        m_repeatOffsetsParentNode->setOpacity(1);
        NgViewModel *offsetNode =
            static_cast<NgViewModel *>(m_repeatOffsetsParentNode->firstChild());

        auto addPline = [&](cavc::Polyline<double> const &pline, QColor color)
        {
            if (!offsetNode)
            {
                offsetNode = new NgViewModel();
                m_repeatOffsetsParentNode->appendChildNode(offsetNode);
            }
            offsetNode->setColor(color);
            offsetNode->setIsVisible(true);
            offsetNode->setVertexesVisible(false);
            offsetNode->setArcApproxError(m_arcApproxError);
            offsetNode->updateVM(pline, false);
            offsetNode = static_cast<NgViewModel *>(offsetNode->nextSibling());
        };

        if (prunedPline.isClosed())
        {
            // folded repeat offsets
            std::vector<cavc::Polyline<double>> prevOffsets;
            double origPlineA = getArea(prunedPline);
            if (!prunedPline.isClosed() || std::abs(origPlineA) > 1e-4)
            {
                prevOffsets.push_back(prunedPline);
            }

            for (int i = 0; i < m_offsetCount; ++i)
            {
                if (prevOffsets.size() == 0)
                {
                    break;
                }
                std::vector<cavc::Polyline<double>> newOffsets =
                    std::vector<cavc::Polyline<double>>();

                for (const auto &pline : prevOffsets)
                {
                    auto offsetPlines = parallelOffset(pline, m_plineOffset);
                    newOffsets.insert(newOffsets.end(),
                                      std::make_move_iterator(offsetPlines.begin()),
                                      std::make_move_iterator(offsetPlines.end()));
                }

                auto copy = prevOffsets;

                newOffsets.erase(std::remove_if(newOffsets.begin(), newOffsets.end(),
                                                [&](const auto &pline)
                                                {
                                                    if (!prunedPline.isClosed())
                                                    {
                                                        return false;
                                                    }
                                                    double a = getArea(pline);
                                                    return ((a > 0) != (origPlineA > 0))
                                                           || std::abs(a) < 1e-4;
                                                }),
                                 newOffsets.end());

                if (newOffsets.size() == 0 && m_showLastPrunedRawOffsets)
                {
                    for (const auto &pline : copy)
                    {
                        auto rawOffsetPline = internal::createRawOffsetPline(pline, m_plineOffset);
                        addPline(rawOffsetPline, QColor("red"));
                        auto retry = parallelOffset(pline, m_plineOffset);
                    }
                }

                for (const auto &pline : newOffsets)
                {
                    addPline(pline, QColor("blue"));
                }

                prevOffsets = std::move(newOffsets);
            }
        }
        else
        {
            // direct (not folded) repeat offsets
            for (int i = 1; i < m_offsetCount + 1; ++i)
            {
                double offsVal = i * m_plineOffset;
                auto offsPlines = parallelOffset(prunedPline, offsVal);
                if (offsPlines.size() == 0)
                {
                    auto rawOffsetPline = internal::createRawOffsetPline(prunedPline, offsVal);
                    addPline(rawOffsetPline, QColor("red"));
                    break;
                }
                else
                {
                    for (auto const &pline : offsPlines)
                    {
                        addPline(pline, QColor("blue"));
                        //            std::size_t vertexCount = pline.size();
                        //            double area = getArea(pline);
                        //            double length = getPathLength(pline);
                        //            auto extents = getExtents(pline);
                        //            qDebug().nospace() << qSetRealNumberPrecision(14) << "(" <<
                        //            vertexCount <<
                        //            ", " << area
                        //                               << ", " << length << ", " << extents.xMin
                        //                               << ", " << extents.yMin
                        //                               << ", " << extents.xMax << ", " <<
                        //                               extents.yMax << ")";
                    }
                }
            }
        }

        while (offsetNode)
        {
            offsetNode->setIsVisible(false);
            offsetNode = static_cast<NgViewModel *>(offsetNode->nextSibling());
        }
    }
    else if (m_repeatOffsetsParentNode)
    {
        m_repeatOffsetsParentNode->setOpacity(0);
    }

    return rootNode;
}

void PlineOffsetAlgorithmView::mousePressEvent(QMouseEvent *event)
{
    if (!m_showOrigPlineVertexes)
    {
        event->ignore();
        return;
    }
    // // converting to global coordinates to get screen resolution delta even if current scale != 1
    // mouse_pick_pt_ = QPointF(event->globalX(), event->globalY());

    // m_vertexGrabbed = vertexUnderPosition(mouse_pick_pt_, input_polyline_);

    // m_origVertexGlobalPos = convertToGlobalUICoord(input_polyline_[m_vertexGrabbed].pos());

    // setInteracting(true);

    if (!isVertexGrabbed())
    {
        event->ignore();
        return;
    }

    event->accept();
}

void PlineOffsetAlgorithmView::mouseMoveEvent(QMouseEvent *event)
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

    input_polyline_[m_vertexGrabbed].x() = newRealVertexPos.x();
    input_polyline_[m_vertexGrabbed].y() = newRealVertexPos.y();
    update();
}

void PlineOffsetAlgorithmView::mouseReleaseEvent(QMouseEvent *event)
{
    if (!isVertexGrabbed())
    {
        return;
    }
    setInteracting(false);
    m_vertexGrabbed = std::numeric_limits<std::size_t>::max();
    event->accept();
}

bool PlineOffsetAlgorithmView::isVertexGrabbed()
{
    return m_vertexGrabbed != std::numeric_limits<std::size_t>::max();
}
} // namespace debugger
