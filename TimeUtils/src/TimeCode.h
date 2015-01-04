#include <string>
#include <cstdint>

/*!
 * \brief The Timecode class is a helper class for managin timecodes.
 */
class Timecode
{
public:
    /*!
     * \brief Extract timecode information from a timecode string (ex: 01:00:10:05 or 01:00:10;05)
     * \param timecode The timecode string to read.
     * \param rateNum The numerator representing the edit unit rate.
     * \param rateDen The denominator representing the edit unit rate.
     * \return
     */
    static Timecode fromTimecodeString(std::wstring timecode, int32_t rateNum, int32_t rateDen);

    /*!
     * \brief Builds a \c Timecode from milliseconds.
     * \param milliseconds The milliseconds value to read.
     * \param rateNum The numerator representing the edit unit rate.
     * \param rateDen The denominator representing the edit unit rate.
     * \param dropFrame \c True if timecode is drop frame.
     * \return
     */
    static Timecode fromMilliseconds(double milliseconds, int32_t rateNum, int32_t rateDen, bool dropFrame);

    /*!
     * \brief Timecode constructor
     */
    Timecode();

    /*!
     * \brief Timecode constructor.
     * \param roundedRate The rounded rate of the timecode (e.g. 25, 30).
     * \param dropFrame \c True if timecode is drop frame.
     */
    Timecode(uint16_t roundedRate, bool dropFrame);

    /*!
     * \overload
     * \param offset offset of the timecode from 0.
     */
    Timecode(uint16_t roundedRate, bool dropFrame, int64_t offset);

    /*!
     * \overload
     * \param rateNum Timecode rate numerator.
     * \param rateDen Timecode rate denominator.
     */
    Timecode(int32_t rateNum, int32_t rateDen, bool dropFrame);

    /*!
     * \overload
     */
    Timecode(int32_t rateNum, int32_t rateDen, bool dropFrame, int64_t offset);

    /*!
     * \overload
     * \param hour The hour component of the timecode.
     * \param min The minutes component of the timecode.
     * \param sec The seconds component of the timecode.
     * \param frame The frames component of the timecode.
     */
    Timecode(int32_t rateNum, int32_t rateDen, bool dropFrame, int16_t hour, int16_t min, int16_t sec, int16_t frame);

    /*!
     * \brief If timecode is a valid timecode.
     * \return \c True if timecode is invalid.
     */
    bool isInvalid()            const { return roundedTCBase_ == 0; }
    uint16_t getRoundedTCBase() const { return roundedTCBase_; }
    bool isDropFrame()          const { return dropFrame_; }
    int16_t getHour()           const { return hour_; }
    int16_t getMin()            const { return min_; }
    int16_t getSec()            const { return sec_; }
    int16_t getFrame()          const { return frame_; }
    int64_t getOffset()         const { return offset_; }

    uint64_t getTotalFrames();

    void setInvalid();
    void init(uint16_t rounded_rate, bool drop_frame);
    void init(uint16_t rounded_rate, bool drop_frame, int64_t offset);
    void init(int32_t rateNum, int32_t rateDen, bool drop_frame);
    void init(int64_t offset);
    void init(int32_t rateNum, int32_t rateDen, bool drop_frame, int64_t offset);
    void init(int16_t hour, int16_t min, int16_t sec, int16_t frame);
    void init(int32_t rateNum, int32_t rateDen, bool drop_frame, int16_t hour, int16_t min, int16_t sec, int16_t frame);

    void addOffset(int64_t offset);
    void addOffset(int64_t offset, int32_t rateNum, int32_t rateDen);

    int64_t getMaxOffset() const;

    bool operator==(const Timecode& right) const;
    bool operator!=(const Timecode& right) const;
    bool operator<(const Timecode& right) const;

private:
    enum class Rounding
    {
        ROUND_AUTO,
        ROUND_DOWN,
        ROUND_UP,
        ROUND_NEAREST
    };

    void cleanOffset();
    void cleanTimecode();

    void updateOffset();
    void updateTimecode();

    int64_t convertPosition(int64_t in_position, int64_t factor_top, int64_t factor_bottom, Rounding rounding) const;

    uint16_t roundedTCBase_;
    bool dropFrame_;
    int16_t hour_;
    int16_t min_;
    int16_t sec_;
    int16_t frame_;
    int64_t offset_;

    uint32_t framesPerMin_;
    uint32_t nDFramesPerMin_;
    uint32_t framesPer10Min_;
    uint32_t framesPerHour_;
    int16_t dropCount_;
};

