#include "viewmodel.h"

#include <QDebug>

#include "flatcolorgeometrynode.h"
#include "graphicshelpers.h"
#include "pointsetnode.h"
#include "settings/settings.h"

namespace debugger
{
using namespace cavc;

namespace
{
void setOpacityNodeVisibility(QSGOpacityNode *node, bool isVisible)
{
    node->setOpacity(isVisible ? 1.0 : 0.0);
}
} // namespace

NgViewModel::NgViewModel() :
    path_visible_(true), vertexes_visible_(false), path_node_(nullptr), vertexes_(nullptr),
    path_color_(Qt::blue), vertexes_color_(Qt::red), visible_(true)
{
}
void NgViewModel::clearVmData()
{
    if (polyline_data_.first)
    {
        delete polyline_data_.first;
        polyline_data_ = {nullptr, false};
    }

    if (polygonSet_data_)
    {
        delete polygonSet_data_;
        polygonSet_data_ = nullptr;
    }
}

void NgViewModel::updateVM(const cavc::Polyline<double> &pline, bool is_hole)
{
    clearVmData();
    polyline_data_ = {new cavc::Polyline<double>(pline), is_hole};

    updateVmData();
    updateGeometry();
    updateVertexesData();
    markDirty(QSGNode::DirtyGeometry);
}

void NgViewModel::updateVM(NGPolygonSet *polygonSet)
{
    clearVmData();
    polygonSet_data_ = polygonSet;

    updateVmData();
    updateGeometry();
    updateVertexesData();
    markDirty(QSGNode::DirtyGeometry);
}

void NgViewModel::updateGeometry()
{
    std::vector<cavc::Vector2<float>> l_vertexesBuffer;

    for (const auto &[parawire, style] : vm_params_)
    {
        double end_x, end_y;
        for (const auto &para : parawire)
        {
            if (para.type == 0) // Line
            {
                if (style == Style::Normal)
                {
                    l_vertexesBuffer.emplace_back(para.x0, para.y0);
                }
                else
                {
                    // generate multiple points along the segment for dashes
                    double length = cavc::length(cavc::Vector2<double>(para.x1, para.y1)
                                                 - cavc::Vector2<double>(para.x0, para.y0));
                    double dashSpacing = 0.5;
                    int dashCount = static_cast<int>(std::ceil(length / dashSpacing));
                    for (int i = 0; i < dashCount; ++i)
                    {
                        double t = static_cast<double>(i) / dashCount;
                        double x = para.x0 + t * (para.x1 - para.x0);
                        double y = para.y0 + t * (para.y1 - para.y0);
                        l_vertexesBuffer.emplace_back(x, y);
                    }
                }
            }
            else // if (type == 1 || type == 2)
            {
                if (para.radius < arcApproxError_ + utils::realThreshold<double>())
                {
                    l_vertexesBuffer.emplace_back(para.x0, para.y0);
                    end_x = para.x1;
                    end_y = para.y1;
                    continue;
                }

                auto startAngle = angle(cavc::Vector2<double>(para.center_x, para.center_y),
                                        cavc::Vector2<double>(para.x0, para.y0));
                auto endAngle = angle(cavc::Vector2<double>(para.center_x, para.center_y),
                                      cavc::Vector2<double>(para.x1, para.y1));
                double deltaAngle = std::abs(cavc::utils::deltaAngle(startAngle, endAngle));

                double segmentSubAngle =
                    std::abs(2.0 * std::acos(1.0 - arcApproxError_ / para.radius));
                std::size_t segmentCount =
                    static_cast<std::size_t>(std::ceil(deltaAngle / segmentSubAngle));
                // update segment subangle for equal length segments
                segmentSubAngle = deltaAngle / segmentCount;

                if (para.type == 2)
                {
                    segmentSubAngle = -segmentSubAngle;
                }
                // add the start point
                l_vertexesBuffer.emplace_back(para.x0, para.y0);

                // add remaining points
                for (std::size_t i = 1; i < segmentCount; ++i)
                {
                    double angle = i * segmentSubAngle + startAngle;
                    l_vertexesBuffer.emplace_back(para.radius * std::cos(angle) + para.center_x,
                                                  para.radius * std::sin(angle) + para.center_y);
                }
            }
            end_x = para.x1;
            end_y = para.y1;
        }
        l_vertexesBuffer.emplace_back(end_x, end_y);

        // Duplicate code, should be refactored
        /* update node geometry from vertexes buffer */
        const int plineSegVertexCount = static_cast<int>(l_vertexesBuffer.size());

        if (!path_node_)
        {
            path_node_ = new FlatColorGeometryNode(true);
            path_node_->geometry()->setLineWidth(1);
            path_node_->setColor(path_color_);
            appendChildNode(path_node_);
            path_node_->setFlag(QSGNode::OwnedByParent);
            path_node_->setIsVisible(path_visible_);
        }

        QSGGeometry *pathGeom = path_node_->geometry();
        // TODO: bind line type to vector of parameter, in polygonset, this can not work well

        pathGeom->setDrawingMode(style == Style::Normal ? QSGGeometry::DrawLineStrip :
                                                          QSGGeometry::DrawLines);

        pathGeom->allocate(plineSegVertexCount, plineSegVertexCount);

        std::uint32_t *segVertexIndices = pathGeom->indexDataAsUInt();
        for (int i = 0; i < plineSegVertexCount; ++i)
        {
            segVertexIndices[i] = static_cast<std::uint32_t>(i);
        }

        QSGGeometry::Point2D *vertexData = pathGeom->vertexDataAsPoint2D();
        for (std::size_t i = 0; i < l_vertexesBuffer.size(); ++i)
        {
            vertexData[i].set(l_vertexesBuffer[i].x(), l_vertexesBuffer[i].y());
        }
    }
    path_node_->markDirty(QSGNode::DirtyGeometry);
}

const QColor &NgViewModel::color() const
{
    return path_color_;
}

void NgViewModel::setColor(const QColor &color)
{
    if (path_color_ != color)
    {
        path_color_ = color;
        if (path_node_)
        {
            path_node_->setColor(path_color_);
        }
    }
}

bool NgViewModel::pathVisible() const
{
    return path_visible_;
}

void NgViewModel::setPathVisible(bool pathVisible)
{
    if (pathVisible != path_visible_)
    {
        path_visible_ = pathVisible;
        if (path_node_)
        {
            path_node_->setIsVisible(pathVisible);
        }
    }
}

const QColor &NgViewModel::vertexesColor() const
{
    return vertexes_color_;
}

void NgViewModel::setVertexesColor(const QColor &vertexesColor)
{
    if (vertexesColor != vertexes_color_)
    {
        vertexes_color_ = vertexesColor;
        if (vertexes_)
        {
            vertexes_->setColor(vertexesColor);
        }
    }
}

bool NgViewModel::vertexesVisible() const
{
    return vertexes_visible_;
}

void NgViewModel::setVertexesVisible(bool vertexesVisible)
{
    if (vertexesVisible != vertexes_visible_)
    {
        vertexes_visible_ = vertexesVisible;
        if (vertexes_)
        {
            setOpacityNodeVisibility(vertexes_, vertexesVisible);
        }
    }
}

void NgViewModel::setIsVisible(bool isVisible)
{
    path_visible_ = isVisible;
    vertexes_visible_ = isVisible;
    if (path_node_)
    {
        path_node_->setIsVisible(isVisible);
    }
    if (vertexes_)
    {
        setOpacityNodeVisibility(vertexes_, isVisible);
    }
}

void NgViewModel::setArcApproxError(double arcApproxError)
{
    this->arcApproxError_ = arcApproxError;
}

void NgViewModel::updateVmData()
{
    auto type = NgSettings::getCurAlgorithmCore();
    switch (type)
    {
    case NgSettings::AppAlgorithmCore::kCavc:
    {
        buildPlineParameter();
        break;
    }
    case NgSettings::AppAlgorithmCore::kNGPoly:
    {
        buildPolyParameter();
        break;
    }
    default: break;
    }
}

void NgViewModel::buildPlineParameter()
{
    assert(polyline_data_.first);
    const cavc::Polyline<double> &pline = *(polyline_data_.first);
    ParaWire parawire;
    vm_params_.clear();

    // update vertexes buffer
    auto visitor = [&](std::size_t i, std::size_t j) -> bool
    {
        const cavc::PlineVertex<double> &v1 = pline[i];
        const cavc::PlineVertex<double> &v2 = pline[j];

        if (v1.bulgeIsZero() || fuzzyEqual(v1.pos(), v2.pos()))
        {
            parawire.emplace_back(VMParameter{v1.x(), v1.y(), v2.x(), v2.y()});
        }
        else // Arc
        {
            auto arc = arcRadiusAndCenter(v1, v2);
            int type = v1.bulgeIsNeg() ? 2 : 1;
            parawire.emplace_back(VMParameter{v1.x(), v1.y(), v2.x(), v2.y(), arc.center.x(),
                                              arc.center.y(), arc.radius, type});
        }

        return true;
    };

    pline.visitSegIndices(visitor);
    Style style = polyline_data_.second ? Dashed : Normal;
    vm_params_.emplace_back(std::make_pair(parawire, style));
}

void NgViewModel::buildPolyParameter()
{
    assert(polygonSet_data_);
    const NGPolygonSet &polygonSet = *polygonSet_data_;
    [[maybe_unused]] ParaWire parawire;
    vm_params_.clear();
    double x0, y0, x1, y1, center_x, center_y, radius;
    // TODO: dummy funnction, replace with real one
    auto visitor = [&]() -> bool
    {
        int type;
        bool get_para = polygonSet.getNextSegment(x0, y0, x1, y1, center_x, center_y, radius, type);
        if (!get_para)
        {
            return false;
        }
        // vm_params_.emplace_back(
        //     std::make_pair(VMParameter{x0, y0, x1, y1, center_x, center_y, radius, type},
        //     false));
    };
}

void NgViewModel::updateVertexesData()
{
    if (!vertexes_)
    {
        vertexes_ = new PointSetNode();
        appendChildNode(vertexes_);
        vertexes_->setFlag(QSGNode::OwnedByParent);
        vertexes_->setColor(vertexes_color_);
        setOpacityNodeVisibility(vertexes_, vertexes_visible_);
    }
    vertexes_->clear();

    auto type = NgSettings::getCurAlgorithmCore();
    switch (type)
    {
    case NgSettings::AppAlgorithmCore::kCavc:
    {
        assert(polyline_data_.first);
        vertexes_->addPolylineVertexes(*(polyline_data_.first));
        break;
    }
    case NgSettings::AppAlgorithmCore::kNGPoly:
    {
        assert(polygonSet_data_);
        vertexes_->addPolygonVertexes(polygonSet_data_);
        break;
    }
    default: break;
    }
}

} // namespace debugger
