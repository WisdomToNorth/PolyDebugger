#ifndef VIEWMODEL_H
#define VIEWMODEL_H

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

struct VMParameter
{
    double x0 = 0.0;
    double y0 = 0.0;
    double x1 = 0.0;
    double y1 = 0.0;
    double center_x = 0.0;
    double center_y = 0.0;
    double radius = 0.0;

    /**
     * @param type 0: line, 1: arc in ccw, 2: arc in cw
     */
    int type = 0;
    bool is_hole = false;
    VMParameter(double x0, double y0, double x1, double y1, double center_x = 0.0,
                double center_y = 0.0, double radius = 0.0, int type = 0, bool is_hole = false) :
        x0(x0),
        y0(y0), x1(x1), y1(y1), center_x(center_x), center_y(center_y), radius(radius), type(type),
        is_hole(is_hole){};
};

class NgViewModel : public QSGOpacityNode
{
public:
    enum PathDrawMode
    {
        NormalPath,
        DashedPath
    };
    NgViewModel();

public:
    /* cava */
    void updateVM(const cavc::Polyline<double> &pline);
    /*ngpoly*/
    void updateVM(NGPolygonSet *polygonSet);
    void updateGeometry();

private:
    void updateVertexesData();
    void updateVmData();

    // we confirm user to use member parameter to ensure culpture of the data
    /* cava */
    void buildPlineParameter();
    /*ngpoly*/
    void buildPolyParameter();
    void clearVmData();

public:
    // Attributes
    QColor const &color() const;
    void setColor(QColor const &color);

    bool pathVisible() const;
    void setPathVisible(bool pathVisible);

    QColor const &vertexesColor() const;
    void setVertexesColor(QColor const &vertexesColor);

    bool vertexesVisible() const;
    void setVertexesVisible(bool vertexesVisible);
    void setIsVisible(bool isVisible);

    void setArcApproxError(double arcApproxError);
    void setDrawMode(PathDrawMode drawMode);

public:
    cavc::Polyline<double> *polyline_data_;
    NGPolygonSet *polygonSet_data_;

private:
    std::vector<VMParameter> m_vmParameter_;
    PointSetNode *m_vertexesNode_;

    bool m_pathVisible;
    bool m_vertexesVisible;
    FlatColorGeometryNode *m_pathNode;
    QColor m_pathColor;
    QColor m_vertexesColor;
    bool m_isVisible;

    double arcApproxError_ = 0.005;
    PathDrawMode drawMode_ = NormalPath;
};

// struct VMUtils
// {
// public:
//     // IO
//     static NgViewModel *createVM(cavc::Polyline<double> const &polyline);
//     static NgViewModel *createVM(const NGPolygonSet *polygonSet);
//     static void updateVM(NgViewModel *vm, cavc::Polyline<double> const &polyline);
//     static void updateVM(NgViewModel *vm, const NGPolygonSet *polygonSet);
// };

} // namespace debugger
#endif // NgViewModel_H
