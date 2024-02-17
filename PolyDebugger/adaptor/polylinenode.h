#ifndef POLYLINENODE_H
#define POLYLINENODE_H

#include <vector>

#include <QSGFlatColorMaterial>
#include <QSGOpacityNode>

#include "cavc/polyline.hpp"
#include "cavc/vector2.hpp"
#include "ngpoly/ngpolygonset.h"

namespace debugger
{

class FlatColorGeometryNode;
class PointSetNode;

class PolylineNode : public QSGOpacityNode
{
public:
    enum PathDrawMode
    {
        NormalPath,
        DashedPath
    };
    PolylineNode();

    QColor const &color() const;
    void setColor(QColor const &color);

    bool pathVisible() const;
    void setPathVisible(bool pathVisible);

    QColor const &vertexesColor() const;
    void setVertexesColor(QColor const &vertexesColor);

    bool vertexesVisible() const;
    void setVertexesVisible(bool vertexesVisible);
    void setIsVisible(bool isVisible);

public:
    /* cava */
    void updateGeometry(cavc::Polyline<double> const &pline, PathDrawMode pathDrawMode = NormalPath,
                        double arcApproxError = 0.005);
    /*ngpoly*/
    void updateGeometry(NGPolygonSet const &polygonSet, PathDrawMode pathDrawMode = NormalPath,
                        double arcApproxError = 0.005);

private:
    /* cava */
    void updatePathNode(cavc::Polyline<double> const &pline, double arcApproxError,
                        PathDrawMode drawMode);
    void updateVertexesNode(cavc::Polyline<double> const &pline);

    /*ngpoly*/
    void updatePathNode(NGPolygonSet const &polygonSet, double arcApproxError,
                        PathDrawMode drawMode);
    void updateVertexesNode(NGPolygonSet const &polygonSet);

private:
    PointSetNode *m_vertexesNode;

    bool m_pathVisible;
    bool m_vertexesVisible;
    FlatColorGeometryNode *m_pathNode;
    QColor m_pathColor;
    QColor m_vertexesColor;
    bool m_isVisible;
};
} // namespace debugger
#endif // POLYLINENODE_H
