#include "flatcolorgeometrynode.h"
namespace debugger
{
FlatColorGeometryNode::FlatColorGeometryNode(bool useUInt32Index) :
    m_isVisible(true),
    qsg_geometry_(QSGGeometry::defaultAttributes_Point2D(), 0, 0,
                  useUInt32Index ? QSGGeometry::UnsignedIntType : QSGGeometry::UnsignedShortType)
{
    qsg_geometry_.setLineWidth(1);
    qsg_geometry_.setDrawingMode(QSGGeometry::DrawLineStrip);
    setGeometry(&qsg_geometry_);
    m_material.setColor(Qt::darkGreen);
    setMaterial(&m_material);
}

const QColor &FlatColorGeometryNode::color() const
{
    return m_material.color();
}

void FlatColorGeometryNode::setColor(const QColor &color)
{
    m_material.setColor(color);
    markDirty(QSGNode::DirtyMaterial);
}

bool FlatColorGeometryNode::isVisible() const
{
    return m_isVisible;
}

void FlatColorGeometryNode::setIsVisible(bool isVisible)
{
    if (m_isVisible != isVisible)
    {
        m_isVisible = isVisible;
        markDirty(QSGNode::DirtySubtreeBlocked);
    }
}

bool FlatColorGeometryNode::isSubtreeBlocked() const
{
    return !m_isVisible;
}
} // namespace debugger
