#include "casedata.h"

#include <QMap>
#include <QString>

#include "cavc/mathutils.hpp"

namespace debugger
{
void DataUtils::createData(CaseData &data, QString caseindex)
{
    QMap<QString, int> case_data{{"default1", 1}, {"default2", 2}};

    switch (case_data[caseindex])
    {
    case 1: buildCase1(data); break;
    case 2: buildCase2(data); break;
    default: break;
    }
}

void DataUtils::buildCavcCase(const CaseData &casedata, CavcPolygonSet &cavc_polygonset)
{
    CavcPolygonSet calc_loops;
    for (auto const &data : casedata)
    {
        cavc::Polyline<double> pline = buildCavcPline(data.first, data.second);
        calc_loops.push_back({std::move(pline), data.second});
    }
    cavc_polygonset.swap(calc_loops);
}

cavc::Polyline<double> DataUtils::buildCavcPline(const PlineData &data, IsHole is_hole)
{
    cavc::Polyline<double> pline;

    for (auto const &pt : data)
    {
        pline.addVertex(std::get<0>(pt), std::get<1>(pt), std::get<2>(pt));
    }
    pline.isClosed() = true;

    if (is_hole)
    {
        invertDirection(pline);
    }
    return pline;
}

void DataUtils::buildCase1(CaseData &data)
{
    std::vector<std::pair<PlineData, IsHole>> case_data;

    /*outboundry*/
    auto radius = 40;
    auto centerX = 0;
    auto centerY = 0;
    std::size_t segmentCount = 16;
    std::vector<std::tuple<double, double, double>> data1;
    for (std::size_t i = 0; i < segmentCount; ++i)
    {
        double angle = static_cast<double>(i) * cavc::utils::tau<double>() / segmentCount;
        data1.push_back(std::make_tuple(radius * std::cos(angle) + centerX,
                                        radius * std::sin(angle) + centerY, i % 2 == 0 ? 1 : -1));
    }
    /*hole*/
    std::vector<std::tuple<double, double, double>> data2{
        std::make_tuple(-7, -25, 0), std::make_tuple(-4, -25, 0), std::make_tuple(-4, -15, 0),
        std::make_tuple(-7, -15, 0)};

    /*hole*/
    std::vector<std::tuple<double, double, double>> data3{std::make_tuple(22, -20, 1),
                                                          std::make_tuple(27, -20, 1)};
    /*hole*/
    std::vector<std::tuple<double, double, double>> data4{std::make_tuple(0, 25, 1),
                                                          std::make_tuple(-4, 0, 0),
                                                          std::make_tuple(2, 0, 1),
                                                          std::make_tuple(10, 0, -0.5),
                                                          std::make_tuple(8, 9, 0.374794619217547),
                                                          std::make_tuple(21, 0, 0),
                                                          std::make_tuple(23, 0, 1),
                                                          std::make_tuple(32, 0, -0.5),
                                                          std::make_tuple(28, 0, 0.5),
                                                          std::make_tuple(28, 12, 0.5)};

    case_data.push_back(std::make_pair(data1, false));
    case_data.push_back(std::make_pair(data2, true));
    case_data.push_back(std::make_pair(data3, true));
    case_data.push_back(std::make_pair(data4, true));
    data.swap(case_data);
}

void DataUtils::buildCase2(CaseData &data)
{
    std::vector<std::pair<PlineData, IsHole>> case_data;

    /*outboundry*/
    std::vector<std::tuple<double, double, double>> data1{
        std::make_tuple(0, 0, 0), std::make_tuple(0, 10, 0), std::make_tuple(10, 10, 0),
        std::make_tuple(10, 0, 0)};
    /*hole*/
    std::vector<std::tuple<double, double, double>> data2{
        std::make_tuple(2, 2, -1), std::make_tuple(2, 8, -1), std::make_tuple(8, 8, -1),
        std::make_tuple(8, 2, -1)};

    case_data.push_back(std::make_pair(data1, true));
    case_data.push_back(std::make_pair(data2, false));
    data.swap(case_data);
}
} // namespace debugger
