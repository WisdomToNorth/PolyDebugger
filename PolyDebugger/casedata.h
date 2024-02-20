#ifndef CASEDATA_H
#define CASEDATA_H

#include <tuple>
#include <vector>

#include <QString>

#include "cavc/polyline.hpp"

namespace debugger
{
typedef std::vector<std::tuple<double, double, double>> PlineData;
typedef bool IsHole; // cw
typedef std::vector<std::pair<PlineData, IsHole>> CaseData;

/*cavc*/
typedef std::vector<std::pair<cavc::Polyline<double>, IsHole>> CavcPolygonSet;

struct DataUtils
{
public:
    static void createData(CaseData &data, QString caseindex);

    static void buildCavcCase(const CaseData &casedata, CavcPolygonSet &cavc_polygonset);

    static cavc::Polyline<double> buildCavcPline(const PlineData &data, IsHole is_hole);

private:
    static void buildCase1(CaseData &data);
    static void buildCase2(CaseData &data);
};
} // namespace debugger
#endif // CASEDATA_H
