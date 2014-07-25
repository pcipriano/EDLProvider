#ifndef PLUGINS_INTERFACES_SRC_LOGGINGHELPERS_H
#define PLUGINS_INTERFACES_SRC_LOGGINGHELPERS_H

#include "easylogging++.h"

#include "EDLProviderStub.h"

#include <QString>

inline MAKE_LOGGABLE(fimstime__TimeType, timeType, os)
{
    switch (timeType.__union_TimeType)
    {
        case SOAP_UNION__fimstime__union_TimeType_timecode:
            os << "TC:" << QString::fromStdWString(*timeType.union_TimeType.timecode).toStdString();
            break;
        case SOAP_UNION__fimstime__union_TimeType_normalPlayTime:
            os << "T:" << QString::fromStdWString(*timeType.union_TimeType.normalPlayTime).toStdString();
            break;
        case SOAP_UNION__fimstime__union_TimeType_editUnitNumber:
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

inline MAKE_LOGGABLE(fims__RationalType, rationalType, os)
{
    os << "rate:" << rationalType.__item
       << " " << QString::fromStdWString(rationalType.numerator).toStdString()
       << "/" << QString::fromStdWString(rationalType.denominator).toStdString();

    return os;
}

inline MAKE_LOGGABLE(fimstime__DurationType, durationType, os)
{
    switch (durationType.__union_DurationType)
    {
        case SOAP_UNION__fimstime__union_DurationType_timecode:
            os << "TC:" << QString::fromStdWString(*durationType.union_DurationType.timecode).toStdString();
            break;
        case SOAP_UNION__fimstime__union_DurationType_normalPlayTime:
            os << "T:" << durationType.union_DurationType.normalPlayTime;
            break;
        case SOAP_UNION__fimstime__union_DurationType_editUnitNumber:
            os << "units:" << durationType.union_DurationType.editUnitNumber->__item
               << " rate:" << QString::fromStdWString(durationType.union_DurationType.editUnitNumber->editRate).toStdString()
               << " " << QString::fromStdWString(durationType.union_DurationType.editUnitNumber->factorNumerator).toStdString()
               << "/" << QString::fromStdWString(durationType.union_DurationType.editUnitNumber->factorDenominator).toStdString();
            break;
        default:
            break;
    }

    return os;
}

#endif // PLUGINS_INTERFACES_SRC_LOGGINGHELPERS_H
