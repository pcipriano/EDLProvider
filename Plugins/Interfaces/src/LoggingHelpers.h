#ifndef PLUGINS_INTERFACES_SRC_LOGGINGHELPERS_H
#define PLUGINS_INTERFACES_SRC_LOGGINGHELPERS_H

#include "easylogging++.h"

#include "EDLProviderStub.h"

#include <QString>

inline MAKE_LOGGABLE(fims__TimeType, timeType, os)
{
    switch (timeType.__union_TimeType)
    {
        case SOAP_UNION__fims__union_TimeType_timecode:
            os << "TC:" << QString::fromStdWString(*timeType.union_TimeType.timecode).toStdString();
            break;
        case SOAP_UNION__fims__union_TimeType_normalPlayTime:
            os << "T:" << QString::fromStdWString(*timeType.union_TimeType.normalPlayTime).toStdString();
            break;
        case SOAP_UNION__fims__union_TimeType_editUnitNumber:
            os << "units:" << timeType.union_TimeType.editUnitNumber->__item
               << " rate:" << QString::fromStdWString(timeType.union_TimeType.editUnitNumber->editRate).toStdString()
               << " " << QString::fromStdWString(timeType.union_TimeType.editUnitNumber->factorNumerator).toStdString()
               << "/" << QString::fromStdWString(timeType.union_TimeType.editUnitNumber->factorDenominator).toStdString();
            break;
        default:
            break;
    }

    return os;
}

#endif // PLUGINS_INTERFACES_SRC_LOGGINGHELPERS_H
