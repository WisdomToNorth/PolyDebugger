#ifndef FLATCOLORGEOMETRYNODE_H
#define FLATCOLORGEOMETRYNODE_H

#include <QSGFlatColorMaterial>
#include <QSGGeometryNode>
namespace debugger
{
class FlatColorGeometryNode : public QSGGeometryNode
{
public:
    FlatColorGeometryNode(bool useUInt32Index = false);
    QColor const &color() const;
    void setColor(QColor const &color);
    bool isVisible() const;
    void setIsVisible(bool isVisible);
    bool isSubtreeBlocked() const override;

protected:
    bool visible_;
    QSGGeometry qsg_geometry_;
    QSGFlatColorMaterial m_material;
};
} // namespace debugger
#endif // FLATCOLORGEOMETRYNODE_H
