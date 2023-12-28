#ifndef RAWOFFSETSEGMENTSNODE_H
#define RAWOFFSETSEGMENTSNODE_H

#include <vector>

#include <QSGOpacityNode>

#include "cavc/polyline.hpp"
#include "cavc/polylineoffset.hpp"
#include "plinesegmentnode.h"

class RawOffsetSegmentsNode : public QSGOpacityNode
{
public:
    RawOffsetSegmentsNode();
    void
    updateGeometry(std::vector<cavc::internal::PlineOffsetSegment<double>> const &rawOffsetSegments,
                   double arcApproxError);
};

#endif // RAWOFFSETSEGMENTSNODE_H
