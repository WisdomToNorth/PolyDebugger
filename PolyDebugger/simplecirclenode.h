#ifndef SIMPLECIRCLENODE_H
#define SIMPLECIRCLENODE_H

#include "flatcolorgeometrynode.h"
namespace debugger
{
class SimpleCircleNode : public FlatColorGeometryNode
{
public:
    SimpleCircleNode();
    void setGeometry(qreal x, qreal y, qreal radius);

private:
    qreal m_radius;
    qreal m_xPos;
    qreal m_yPos;
    void updateGeometry();
};
} // namespace debugger

#endif // SIMPLECIRCLENODE_H
