#ifndef SPATIALINDEXBOUNDINGBOXESNODE_H
#define SPATIALINDEXBOUNDINGBOXESNODE_H

#include <QSGOpacityNode>

#include "cavc/staticspatialindex.hpp"

#include "flatcolorgeometrynode.h"
#include "graphicshelpers.h"

namespace debugger
{
class SpatialIndexBoundingBoxesNode : public QSGOpacityNode
{
public:
    SpatialIndexBoundingBoxesNode(){};

    template <typename Real, std::size_t N>
    void updateGeometry(cavc::StaticSpatialIndex<Real, N> const &spatialIndex)
    {
        auto bbNode = static_cast<FlatColorGeometryNode *>(this->firstChild());
        auto bbVisitor = [&](std::size_t level, Real minX, Real minY, Real maxX, Real maxY)
        {
            if (!bbNode)
            {
                bbNode = new FlatColorGeometryNode();
                bbNode->setFlag(QSGNode::OwnedByParent);
                bbNode->geometry()->allocate(4);
                bbNode->geometry()->setDrawingMode(QSGGeometry::DrawingMode::DrawLineLoop);
                this->appendChildNode(bbNode);
            }

            bbNode->setIsVisible(true);
            bbNode->setColor(gh::indexToColor(level + 3));

            QSGGeometry::Point2D *vertexes = bbNode->geometry()->vertexDataAsPoint2D();
            vertexes[0].set(minX, minY);
            vertexes[1].set(maxX, minY);
            vertexes[2].set(maxX, maxY);
            vertexes[3].set(minX, maxY);
            bbNode->markDirty(DirtyGeometry);

            bbNode = static_cast<FlatColorGeometryNode *>(bbNode->nextSibling());

            return true;
        };

        spatialIndex.visitBoundingBoxes(bbVisitor);

        // hide remaining nodes (keep around as cache to avoid repeated alloc/dealloc)
        while (bbNode)
        {
            bbNode->setIsVisible(false);
            bbNode = static_cast<FlatColorGeometryNode *>(bbNode->nextSibling());
        }
    }
};
} // namespace debugger
#endif // SPATIALINDEXBOUNDINGBOXESNODE_H
