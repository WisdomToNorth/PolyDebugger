#include "simplecirclenode.h"

#include <cmath>
namespace debugger
{
SimpleCircleNode::SimpleCircleNode() : FlatColorGeometryNode(true)
{
    m_xPos = 0;
    m_yPos = 0;
    m_radius = 1;
    qsg_geometry_.setDrawingMode(QSGGeometry::DrawTriangleFan);
}

void SimpleCircleNode::setGeometry(qreal x, qreal y, qreal radius)
{
    m_xPos = x;
    m_yPos = y;
    m_radius = radius;
    updateGeometry();
}

namespace internal
{
template <typename Real>
constexpr Real pi()
{
    return Real(3.14159265358979323846264338327950288);
}

template <typename Real>
constexpr Real tau()
{
    return Real(2) * pi<Real>();
}
} // namespace internal

void SimpleCircleNode::updateGeometry()
{
    int surroundingVertexes = 12;
    int vertexCount = surroundingVertexes + 2;

    qsg_geometry_.allocate(vertexCount, vertexCount);
    std::uint32_t *segVertexIndices = qsg_geometry_.indexDataAsUInt();
    for (int i = 0; i < vertexCount; ++i)
    {
        segVertexIndices[i] = static_cast<std::uint32_t>(i);
    }

    QSGGeometry::Point2D *vertexData = qsg_geometry_.vertexDataAsPoint2D();
    vertexData[0].set(m_xPos, m_yPos);
    for (int i = 1; i < vertexCount; ++i)
    {
        double angle = internal::tau<double>() * static_cast<double>(i - 1)
                       / static_cast<double>(surroundingVertexes);
        vertexData[i].set(m_xPos + m_radius * std::cos(angle), m_yPos + m_radius * std::sin(angle));
    }

    markDirty(QSGNode::DirtyGeometry);
}
} // namespace debugger
