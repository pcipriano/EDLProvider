#include "Timecode.h"

#include <regex>
#include <cmath>
#include <cfenv>
#include <sstream>
#include <iostream>
#include <cassert>

template<typename T>
std::vector<T> split(const T& str, const T& delimiters)
{
    std::vector<T> v;
    T::size_type start = 0;
    auto pos = str.find_first_of(delimiters, start);
    while(pos != T::npos)
    {
        if(pos != start) // ignore empty tokens
            v.emplace_back(str, start, pos - start);
        start = pos + 1;
        pos = str.find_first_of(delimiters, start);
    }
    if(start < str.length()) // ignore trailing delimiter
        v.emplace_back(str, start, str.length() - start); // add what's left of the string
    return v;
}

template<typename T>
inline T convertToNumber(std::wstring const& s)
{
    std::wistringstream i(s);
    T x;
    if (!(i >> x))
    {
        std::string message("Conversion failed ");
        message += __FUNCTION__;
        throw std::runtime_error(message);
    }
    return x;
}

Timecode Timecode::fromTimecodeString(std::wstring timecode, int32_t rateNum, int32_t rateDen)
{
    //First validate
    std::wregex regExp(L"([0-1][0-9]|2[0-3])([:][0-5][0-9]){2}([:;][0-5][0-9])");
    std::wsmatch wsMatch;
    std::regex_match(timecode, wsMatch, regExp);

    if (wsMatch.empty())
        return Timecode();

    std::wstring matchValue = *wsMatch.cbegin();

    std::vector<std::wstring> components = split<std::wstring>(matchValue, L":;");

    assert(components.size() == 4);

    return Timecode(rateNum,
                    rateDen,
                    matchValue.find(';') != std::wstring::npos,
                    convertToNumber<int16_t>(components[0]),
                    convertToNumber<int16_t>(components[1]),
                    convertToNumber<int16_t>(components[2]),
                    convertToNumber<int16_t>(components[3]));
}

Timecode Timecode::fromMilliseconds(double milliseconds, int32_t rateNum, int32_t rateDen, bool dropFrame)
{
    std::fesetround(FE_DOWNWARD);
    int16_t hours = (int16_t) std::lrint(milliseconds / 3600000.0);
    milliseconds -= hours * 3600000.0;
    int16_t minutes = (int16_t) std::lrint(milliseconds / 60000.0);
    milliseconds -= minutes * 60000.0;
    int16_t seconds = (int16_t) std::lrint(milliseconds / 1000);
    milliseconds -= seconds * 1000;
    int16_t frames = (int16_t) std::lrint(((milliseconds * (double) rateNum) / (double) rateDen) / 1000.0);

    return Timecode(rateNum, rateDen, dropFrame, hours, minutes, seconds, frames);
}

Timecode::Timecode()
{
    setInvalid();
}

Timecode::Timecode(uint16_t roundedRate, bool dropFrame)
{
    init(roundedRate, dropFrame);
}

Timecode::Timecode(uint16_t roundedRate, bool dropFrame, int64_t offset)
{
    init(roundedRate, dropFrame, offset);
}

Timecode::Timecode(int32_t rateNum, int32_t rateDen, bool dropFrame)
{
    init(rateNum, rateDen, dropFrame);
}

Timecode::Timecode(int32_t rateNum, int32_t rateDen, bool dropFrame, int16_t hour, int16_t min, int16_t sec, int16_t frame)
{
    init(rateNum, rateDen, dropFrame);
    init(hour, min, sec, frame);
}

uint64_t Timecode::getTotalFrames()
{
    uint64_t totalFrames = 0;

    totalFrames = hour_ * framesPerHour_;
    if (dropFrame_)
    {
        totalFrames += (min_ / 10) * framesPer10Min_;
        totalFrames += (min_ % 10) * framesPerMin_;
    }
    else
    {
        totalFrames += min_ * framesPerMin_;
    }
    totalFrames += sec_ * roundedTCBase_;
    totalFrames += frame_;

    return totalFrames;
}

Timecode::Timecode(int32_t rateNum, int32_t rateDen, bool dropFrame, int64_t offset)
{
    init(rateNum, rateDen, dropFrame);
    init(offset);
}

void Timecode::setInvalid()
{
    roundedTCBase_ = 0;
    dropFrame_ = false;
    hour_ = 0;
    min_ = 0;
    sec_ = 0;
    frame_ = 0;
    offset_ = 0;
    framesPerMin_ = 0;
    nDFramesPerMin_ = 0;
    framesPer10Min_ = 0;
    framesPerHour_ = 0;
}

void Timecode::init(uint16_t rounded_rate, bool drop_frame)
{
    roundedTCBase_ = rounded_rate;
    if (rounded_rate == 30 || rounded_rate == 60)
        dropFrame_ = drop_frame;
    else
        dropFrame_ = false;

    if (dropFrame_)
    {
        // first 2 frame numbers shall be omitted at the start of each minute,
        // except minutes 0, 10, 20, 30, 40 and 50
        dropCount_ = 2;
        if (roundedTCBase_ == 60)
            dropCount_ *= 2;
        framesPerMin_ = roundedTCBase_ * 60 - dropCount_;
        framesPer10Min_ = framesPerMin_ * 10 + dropCount_;
    }
    else
    {
        framesPerMin_ = roundedTCBase_ * 60;
        framesPer10Min_ = framesPerMin_ * 10;
    }
    framesPerHour_ = framesPer10Min_ * 6;
    nDFramesPerMin_ = roundedTCBase_ * 60;

    hour_ = 0;
    min_ = 0;
    sec_ = 0;
    frame_ = 0;
    offset_ = 0;
}

void Timecode::init(uint16_t rounded_rate, bool drop_frame, int64_t offset)
{
    init(rounded_rate, drop_frame);
    offset_ = offset;
    cleanOffset();

    updateTimecode();
}

void Timecode::init(int32_t rateNum, int32_t rateDen, bool drop_frame)
{
    uint16_t roundedBase = (uint16_t)((rateNum + rateDen/2) / rateDen);
    init(roundedBase, drop_frame);
}

void Timecode::init(int64_t offset)
{
    offset_ = offset;
    cleanOffset();

    updateTimecode();
}

void Timecode::init(int32_t rateNum, int32_t rateDen, bool drop_frame, int64_t offset)
{
    init(rateNum, rateDen, drop_frame);
    init(offset);
}

void Timecode::init(int16_t hour, int16_t min, int16_t sec, int16_t frame)
{
    hour_ = hour;
    min_ = min;
    sec_ = sec;
    frame_ = frame;
    cleanTimecode();

    updateOffset();
}

void Timecode::init(int32_t rateNum, int32_t rateDen, bool drop_frame, int16_t hour, int16_t min, int16_t sec, int16_t frame)
{
    init(rateNum, rateDen, drop_frame);
    init(hour, min, sec, frame);
}

void Timecode::addOffset(int64_t offset)
{
    offset_ += offset;
    cleanOffset();

    updateTimecode();
}

void Timecode::addOffset(int64_t offset, int32_t rateNum, int32_t rateDen)
{
    uint16_t roundedBase = (uint16_t)((rateNum + rateDen/2) / rateDen);
    addOffset(convertPosition(offset, roundedTCBase_, roundedBase, Rounding::ROUND_AUTO));
}

int64_t Timecode::getMaxOffset() const
{
    return 24 * framesPerHour_;
}

bool Timecode::operator==(const Timecode& right) const
{
    return (roundedTCBase_ == 0 && right.roundedTCBase_ == 0) ||
           (roundedTCBase_ != 0 && right.roundedTCBase_ != 0 &&
               offset_ == convertPosition(right.offset_, roundedTCBase_, right.roundedTCBase_, Rounding::ROUND_AUTO));
}

bool Timecode::operator!=(const Timecode& right) const
{
    return !(*this == right);
}

bool Timecode::operator<(const Timecode& right) const
{
    return (roundedTCBase_ != 0 && right.roundedTCBase_ == 0) ||
           (roundedTCBase_ != 0 && right.roundedTCBase_ != 0 &&
            offset_ < convertPosition(right.offset_, roundedTCBase_, right.roundedTCBase_, Rounding::ROUND_AUTO));
}

void Timecode::cleanOffset()
{
    if (roundedTCBase_ == 0)
        offset_ = 0;
    else
        offset_ %= getMaxOffset();

    if (offset_ < 0)
        offset_ += getMaxOffset();
}

void Timecode::cleanTimecode()
{
    if (roundedTCBase_ == 0)
    {
        hour_  = 0;
        min_   = 0;
        sec_   = 0;
        frame_ = 0;
    }
    else
    {
        hour_  %= 24;
        min_   %= 60;
        sec_   %= 60;
        frame_ %= roundedTCBase_;

        // replace invalid drop frame hhmmssff with a valid one after it
        if (dropFrame_ && sec_ == 0 && (min_ % 10) && frame_ < dropCount_)
            frame_ = dropCount_;
    }
}

void Timecode::updateOffset()
{
    if (roundedTCBase_ == 0)
    {
        offset_ = 0;
        return;
    }

    offset_ = getTotalFrames();
}

void Timecode::updateTimecode()
{
    if (roundedTCBase_ == 0)
    {
        hour_  = 0;
        min_   = 0;
        sec_   = 0;
        frame_ = 0;
        return;
    }

    int64_t offset = offset_;
    bool frames_dropped = false;

    hour_ = (int16_t)(offset / framesPerHour_);
    offset = offset % framesPerHour_;
    min_ = (int16_t)(offset / framesPer10Min_ * 10);
    offset = offset % framesPer10Min_;
    if (offset >= nDFramesPerMin_)
    {
        offset -= nDFramesPerMin_;
        min_ += (int16_t)((offset / framesPerMin_) + 1);
        offset = offset % framesPerMin_;
        frames_dropped = dropFrame_;
    }
    sec_ = (int16_t)(offset / roundedTCBase_);
    frame_ = (int16_t)(offset % roundedTCBase_);

    if (frames_dropped)
    {
        frame_ += dropCount_;
        if (frame_ >= roundedTCBase_)
        {
            frame_ -= roundedTCBase_;
            sec_++;
            if (sec_ >= 60)
            {
                sec_ = 0;
                min_++;
                if (min_ >= 60)
                {
                    min_ = 0;
                    hour_++;
                    if (hour_ >= 24)
                        hour_ = 0;
                }
            }
        }
    }
}

int64_t Timecode::convertPosition(int64_t in_position, int64_t factor_top, int64_t factor_bottom, Rounding rounding) const
{
    if (in_position == 0 || factor_top == factor_bottom)
        return in_position;

    if (in_position < 0) {
        if (rounding == Rounding::ROUND_UP || (rounding == Rounding::ROUND_AUTO && factor_top < factor_bottom))
            return -convertPosition(-in_position, factor_top, factor_bottom, Rounding::ROUND_DOWN);
        else
            return -convertPosition(-in_position, factor_top, factor_bottom, Rounding::ROUND_UP);
    }

    int64_t round_num = 0;
    if (rounding == Rounding::ROUND_UP || (rounding == Rounding::ROUND_AUTO && factor_top < factor_bottom))
        round_num = factor_bottom - 1;
    else if (rounding == Rounding::ROUND_NEAREST)
        round_num = factor_bottom / 2;

    if (in_position <= INT32_MAX)
    {
        // no chance of overflow (assuming there exists a result that fits into int64_t)
        return (in_position * factor_top + round_num) / factor_bottom;
    }
    else
    {
        // separate the calculation into 2 parts so there is no chance of an overflow (assuming there exists
        // a result that fits into int64_t)
        // a*b/c = ((a/c)*c + a%c) * b) / c = (a/c)*b + (a%c)*b/c
        return (in_position / factor_bottom) * factor_top +
               ((in_position % factor_bottom) * factor_top + round_num) / factor_bottom;
    }
}

