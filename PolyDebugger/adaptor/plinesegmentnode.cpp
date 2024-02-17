#include "plinesegmentnode.h"

#include "graphicshelpers.h"
namespace debugger
{
void PlineSegmentNode::updateGeometry(const cavc::PlineVertex<double> &v1,
                                      const cavc::PlineVertex<double> &v2, double arcApproxError)
{
    if (v1.bulgeIsZero() || fuzzyEqual(v1.pos(), v2.pos()))
    {
        qsg_geometry_.allocate(2);
        qsg_geometry_.vertexDataAsPoint2D()[0].set(static_cast<float>(v1.x()),
                                                   static_cast<float>(v1.y()));
        qsg_geometry_.vertexDataAsPoint2D()[1].set(static_cast<float>(v2.x()),
                                                   static_cast<float>(v2.y()));
    }
    else
    {
        auto arc = arcRadiusAndCenter(v1, v2);
        if (arc.radius < arcApproxError + cavc::utils::realThreshold<double>())
        {
            qsg_geometry_.allocate(static_cast<int>(1));
            qsg_geometry_.vertexDataAsPoint2D()[0].set(static_cast<float>(v1.x()),
                                                       static_cast<float>(v1.y()));
        }
        else
        {
            auto startAngle = angle(arc.center, v1.pos());
            auto endAngle = angle(arc.center, v2.pos());
            double deltaAngle = std::abs(cavc::utils::deltaAngle(startAngle, endAngle));

            double segmentSubAngle = std::abs(2.0 * std::acos(1.0 - arcApproxError / arc.radius));
            std::size_t segmentCount =
                static_cast<std::size_t>(std::ceil(deltaAngle / segmentSubAngle));
            // update segment subangle for equal length segments
            segmentSubAngle = deltaAngle / segmentCount;

            if (v1.bulge() < 0.0)
            {
                segmentSubAngle = -segmentSubAngle;
            }

            qsg_geometry_.allocate(static_cast<int>(segmentCount + 1));

            for (std::size_t i = 0; i <= segmentCount; ++i)
            {
                double angle = i * segmentSubAngle + startAngle;
                qsg_geometry_.vertexDataAsPoint2D()[i].set(
                    static_cast<float>(arc.radius * std::cos(angle) + arc.center.x()),
                    static_cast<float>(arc.radius * std::sin(angle) + arc.center.y()));
            }
        }
    }

    markDirty(QSGNode::DirtyGeometry);
}
} // namespace debugger
