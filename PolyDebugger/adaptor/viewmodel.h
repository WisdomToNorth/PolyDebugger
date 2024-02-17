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
    VMParameter(double x0, double y0, double x1, double y1, double center_x = 0.0,
                double center_y = 0.0, double radius = 0.0, int type = 0) :
        x0(x0),
        y0(y0), x1(x1), y1(y1), center_x(center_x), center_y(center_y), radius(radius),
        type(type){};
};

class NgViewModel : public QSGOpacityNode
{
public:
    enum Style
    {
        Normal,
        Dashed
    };

    NgViewModel();

public:
    /* cava */
    void updateVM(const cavc::Polyline<double> &pline, bool is_hole = false);
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

public:
    std::pair<cavc::Polyline<double> *, bool> polyline_data_{nullptr, false}; // data,is_hole
    NGPolygonSet *polygonSet_data_ = nullptr;

private:
    typedef std::vector<VMParameter> ParaWire;

    std::vector<std::pair<ParaWire, Style>> vm_params_;
    PointSetNode *vertexes_;

    bool path_visible_;
    bool vertexes_visible_;
    FlatColorGeometryNode *path_node_;
    QColor path_color_;
    QColor vertexes_color_;
    bool visible_;

    double arcApproxError_ = 0.005;
};

} // namespace debugger
#endif // NgViewModel_H
