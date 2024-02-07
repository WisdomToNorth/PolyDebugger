#ifndef NGPOLYGONSET_H
#define NGPOLYGONSET_H
#include <cmath>
#include <iostream>

struct SegmentOri
{
    double x0;
    double y0;
    double x1;
    double y1;
    double center_x;
    double center_y;
    double radius;

    bool is_line;
    bool is_ccw;
    SegmentOri()
    {
    }
    SegmentOri(double x0, double y0, double x1, double y1, double center_x, double center_y,
               double radius, bool is_line, bool is_ccw)
    {
        this->x0 = x0;
        this->y0 = y0;
        this->x1 = x1;
        this->y1 = y1;
        this->center_x = center_x;
        this->center_y = center_y;
        this->radius = radius;
        this->is_line = is_line;
        this->is_ccw = is_ccw;
    }
};
struct SegmentWithBulge
{
    double x0;
    double y0;
    double x1;
    double y1;

    double bulge;
    bool is_line;
    bool is_ccw;
    SegmentWithBulge()
    {
    }
    SegmentWithBulge(double x0, double y0, double x1, double y1, double bulge, bool is_line,
                     bool is_ccw)
    {
        this->x0 = x0;
        this->y0 = y0;
        this->x1 = x1;
        this->y1 = y1;
        this->bulge = bulge;
        this->is_line = is_line;
        this->is_ccw = is_ccw;
    }
};

static SegmentWithBulge convertToSegmentWithBulge(const SegmentOri &)
{
    return {};
}

static SegmentOri convertToSegmentOri(const SegmentWithBulge &)
{
    return {};
}

/*This is a dummy class, all the function is not realized*/
class NGPolygonSet
{
public:
    NGPolygonSet()
    {
    }

    bool isClosed() const
    {
        return true;
    }
    /**
     * @param type 0: line, 1: arc in ccw, 2: arc in cw
     */
    static bool getNextSegment(double &x0, double &y0, double &x1, double &y1, double &center_x,
                               double &center_y, double &radius, int &type)
    {
        x0 = 0;
        y0 = 0;
        x1 = 0;
        y1 = 0;
        center_x = 0;
        center_y = 0;
        radius = 0;
        type = 0;
        return true;
    };
};

#endif // NGPOLYGONSET_H
