#ifndef GRAPHICSHELPERS_H
#define GRAPHICSHELPERS_H
#include <QColor>
#include <QSGGeometry>

class QSGGeometryNode;

namespace gh
{
QColor indexToColor(std::size_t index);

QSGGeometryNode *createSimpleGeomNode(int vertexCount, QColor const &color, float width,
                                      QSGGeometry::DrawingMode mode);
} // namespace gh

#endif // GRAPHICSHELPERS_H
