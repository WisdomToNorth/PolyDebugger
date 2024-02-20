#ifndef DATAMANAGER_H
#define DATAMANAGER_H
#include <QString>

#include "casedata.h"

namespace debugger
{

class DocumetData
{
public:
    CaseData case_data_;
    CavcPolygonSet cavc_polygonset_;
    QString case_index_;

    static DocumetData &getInstance()
    {
        static DocumetData instance;
        return instance;
    }

    void changeData(QString caseindex)
    {
        DataUtils::createData(case_data_, caseindex);
    }

private:
    DocumetData() = default;
    ~DocumetData() = default;
};

} // namespace debugger
#endif // CASEDATA_H
