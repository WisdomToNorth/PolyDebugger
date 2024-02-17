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
    m_pathVisible(true), m_vertexesVisible(false), m_pathNode(nullptr), m_vertexesNode_(nullptr),
    m_pathColor(Qt::blue), m_vertexesColor(Qt::red), m_isVisible(true)
{
}
void NgViewModel::clearVmData()
{
    if (polyline_data_)
    {
        delete polyline_data_;
    }
    if (polygonSet_data_)
    {
        delete polygonSet_data_;
    }
}

void NgViewModel::updateVM(const cavc::Polyline<double> &pline)
{
    clearVmData();
    polyline_data_ = new cavc::Polyline<double>(pline);
    polygonSet_data_ = nullptr;

    updateVertexesData();
    updateVmData();
    markDirty(QSGNode::DirtyGeometry);
}

void NgViewModel::updateVM(NGPolygonSet *polygonSet)
{
    clearVmData();
    polygonSet_data_ = polygonSet;
    polyline_data_ = nullptr;

    updateVertexesData();
    updateVmData();
    markDirty(QSGNode::DirtyGeometry);
}

void NgViewModel::updateGeometry()
{
    std::vector<cavc::Vector2<float>> m_vertexesBuffer;
    double x0, y0, x1, y1, center_x, center_y, radius;

    for (auto &para : m_vmParameter_)
    {
        if (para.type == 0) // Line
        {
            if (!para.is_hole) // == PathDrawMode::NormalPath)
            {
                m_vertexesBuffer.emplace_back(para.x0, para.y0);
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
                    m_vertexesBuffer.emplace_back(x, y);
                }
            }
        }
        else // if (type == 1 || type == 2)
        {
            if (para.radius < arcApproxError_ + utils::realThreshold<double>())
            {
                m_vertexesBuffer.emplace_back(para.x0, para.y0);
                continue;
            }

            auto startAngle = angle(cavc::Vector2<double>(para.center_x, para.center_y),
                                    cavc::Vector2<double>(para.x0, para.y0));
            auto endAngle = angle(cavc::Vector2<double>(para.center_x, para.center_y),
                                  cavc::Vector2<double>(para.x1, para.y1));
            double deltaAngle = std::abs(cavc::utils::deltaAngle(startAngle, endAngle));

            double segmentSubAngle = std::abs(2.0 * std::acos(1.0 - arcApproxError_ / para.radius));
            std::size_t segmentCount =
                static_cast<std::size_t>(std::ceil(deltaAngle / segmentSubAngle));
            // update segment subangle for equal length segments
            segmentSubAngle = deltaAngle / segmentCount;

            if (para.type == 2 || para.type == 4)
            {
                segmentSubAngle = -segmentSubAngle;
            }
            // add the start point
            m_vertexesBuffer.emplace_back(para.x0, para.y0);
        }
    }
    m_vertexesBuffer.emplace_back(x1, y1);

    // Duplicate code, should be refactored
    /* update node geometry from vertexes buffer */
    const int plineSegVertexCount = static_cast<int>(m_vertexesBuffer.size());

    if (!m_pathNode)
    {
        m_pathNode = new FlatColorGeometryNode(true);
        m_pathNode->geometry()->setLineWidth(1);
        m_pathNode->setColor(m_pathColor);
        appendChildNode(m_pathNode);
        m_pathNode->setFlag(QSGNode::OwnedByParent);
        m_pathNode->setIsVisible(m_pathVisible);
    }

    QSGGeometry *pathGeom = m_pathNode->geometry();
    // TODO: bind line type to vector of parameter
    pathGeom->setDrawingMode(drawMode_ == NormalPath ? QSGGeometry::DrawLineStrip :
                                                       QSGGeometry::DrawLines);

    pathGeom->allocate(plineSegVertexCount, plineSegVertexCount);

    std::uint32_t *segVertexIndices = pathGeom->indexDataAsUInt();
    for (int i = 0; i < plineSegVertexCount; ++i)
    {
        segVertexIndices[i] = static_cast<std::uint32_t>(i);
    }

    QSGGeometry::Point2D *vertexData = pathGeom->vertexDataAsPoint2D();
    for (std::size_t i = 0; i < m_vertexesBuffer.size(); ++i)
    {
        vertexData[i].set(m_vertexesBuffer[i].x(), m_vertexesBuffer[i].y());
    }

    m_pathNode->markDirty(QSGNode::DirtyGeometry);
}

const QColor &NgViewModel::color() const
{
    return m_pathColor;
}

void NgViewModel::setColor(const QColor &color)
{
    if (m_pathColor != color)
    {
        m_pathColor = color;
        if (m_pathNode)
        {
            m_pathNode->setColor(m_pathColor);
        }
    }
}

bool NgViewModel::pathVisible() const
{
    return m_pathVisible;
}

void NgViewModel::setPathVisible(bool pathVisible)
{
    if (pathVisible != m_pathVisible)
    {
        m_pathVisible = pathVisible;
        if (m_pathNode)
        {
            m_pathNode->setIsVisible(pathVisible);
        }
    }
}

const QColor &NgViewModel::vertexesColor() const
{
    return m_vertexesColor;
}

void NgViewModel::setVertexesColor(const QColor &vertexesColor)
{
    if (vertexesColor != m_vertexesColor)
    {
        m_vertexesColor = vertexesColor;
        if (m_vertexesNode_)
        {
            m_vertexesNode_->setColor(vertexesColor);
        }
    }
}

bool NgViewModel::vertexesVisible() const
{
    return m_vertexesVisible;
}

void NgViewModel::setVertexesVisible(bool vertexesVisible)
{
    if (vertexesVisible != m_vertexesVisible)
    {
        m_vertexesVisible = vertexesVisible;
        if (m_vertexesNode_)
        {
            setOpacityNodeVisibility(m_vertexesNode_, vertexesVisible);
        }
    }
}

void NgViewModel::setIsVisible(bool isVisible)
{
    m_pathVisible = isVisible;
    m_vertexesVisible = isVisible;
    if (m_pathNode)
    {
        m_pathNode->setIsVisible(isVisible);
    }
    if (m_vertexesNode_)
    {
        setOpacityNodeVisibility(m_vertexesNode_, isVisible);
    }
}

void NgViewModel::setArcApproxError(double arcApproxError)
{
    this->arcApproxError_ = arcApproxError;
}

void NgViewModel::setDrawMode(PathDrawMode drawMode)
{
    this->drawMode_ = drawMode;
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
    assert(polyline_data_);
    const cavc::Polyline<double> &pline = *polyline_data_;
    m_vmParameter_;

    // update vertexes buffer
    auto visitor = [&](std::size_t i, std::size_t j) -> bool
    {
        const cavc::PlineVertex<double> &v1 = pline[i];
        const cavc::PlineVertex<double> &v2 = pline[j];

        if (v1.bulgeIsZero() || fuzzyEqual(v1.pos(), v2.pos()))
        {
            m_vmParameter_.emplace_back(VMParameter{v1.x(), v1.y(), v2.x(), v2.y()});
        }
        else // Arc
        {
            auto arc = arcRadiusAndCenter(v1, v2);
            m_vmParameter_.emplace_back(VMParameter{v1.x(), v1.y(), v2.x(), v2.y(), arc.center.x(),
                                                    arc.center.y(), arc.radius, 1, false});
        }

        return true;
    };

    pline.visitSegIndices(visitor);
}

void NgViewModel::buildPolyParameter()
{
    assert(polygonSet_data_);
    const NGPolygonSet &polygonSet = *polygonSet_data_;

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
        m_vmParameter_.emplace_back(
            VMParameter{x0, y0, x1, y1, center_x, center_y, radius, type, false});
    };
}

void NgViewModel::updateVertexesData()
{
    if (!m_vertexesNode_)
    {
        m_vertexesNode_ = new PointSetNode();
        appendChildNode(m_vertexesNode_);
        m_vertexesNode_->setFlag(QSGNode::OwnedByParent);
        m_vertexesNode_->setColor(m_vertexesColor);
        setOpacityNodeVisibility(m_vertexesNode_, m_vertexesVisible);
    }
    m_vertexesNode_->clear();

    auto type = NgSettings::getCurAlgorithmCore();
    switch (type)
    {
    case NgSettings::AppAlgorithmCore::kCavc:
    {
        assert(polyline_data_);
        m_vertexesNode_->addPolylineVertexes(*polyline_data_);
        break;
    }
    case NgSettings::AppAlgorithmCore::kNGPoly:
    {
        assert(polygonSet_data_);
        m_vertexesNode_->addPolygonVertexes(polygonSet_data_);
        break;
    }
    default: break;
    }
}

} // namespace debugger
