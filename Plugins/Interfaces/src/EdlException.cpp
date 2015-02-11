#include "EdlException.h"

using namespace plugins::interfaces;

EdlException::EdlException(const EdlException::EdlError errorType)
    : errorType_(errorType)
{
}

EdlException::EdlException(const EdlException::EdlError errorType, std::string detailInfo)
    : errorType_(errorType),
      detailedInfo_(detailInfo)
{
}

const char* EdlException::what() const
{
    switch (errorType_)
    {
        case EdlError::REQUIRED_VALUE:
            return "Missing required value in the request.";
        case EdlError::MARKIN_BIGGER_THAN_MARKOUT:
            return "MarkIn is bigger than MarkOut.";
        case EdlError::MARK_INOUT_OUTSIDE_DURATION:
            return "MarkIn or MarkOut is outside of the clip total duration.";
        case EdlError::UNSUPPORTED_FRAME_RATE:
            return "Specified frame rate not supported by the system.";
        default:
            return "";  //Should never occur
    }
}

