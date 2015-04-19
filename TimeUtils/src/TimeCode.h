#include <string>
#include <cstdint>

/*!
 * \brief The Timecode class is a helper class for managing timecodes.
 */
class Timecode
{
public:
    /*!
     * \brief Extract timecode information from a timecode string (ex: 01:00:10:05 or 01:00:10;05)
     * \param timecode The timecode string to read.
     * \param rateNum The numerator representing the edit unit rate.
     * \param rateDen The denominator representing the edit unit rate.
     * \return \c Timecode object generated from the input timecode.
     */
    static Timecode fromTimecodeString(const std::wstring& timecode, int32_t rateNum, int32_t rateDen);

    /*!
     * \brief Builds a \c Timecode from milliseconds.
     * \param milliseconds The milliseconds value to read.
     * \param rateNum The numerator representing the edit unit rate.
     * \param rateDen The denominator representing the edit unit rate.
     * \param dropFrame \c True if timecode is drop frame.
     * \return \c Timecode object generated from the input timecode.
     */
    static Timecode fromMilliseconds(double milliseconds, int32_t rateNum, int32_t rateDen, bool dropFrame);

    /*!
     * \brief Timecode constructor.
     */
    Timecode();

    /*!
     * \overload
     * \param roundedRate The rounded rate of the timecode (e.g. 25, 30).
     * \param dropFrame \c True if timecode is drop frame.
     */
    Timecode(uint16_t roundedRate, bool dropFrame);

    /*!
     * \overload
     * \param offset Offset of the timecode from 0 in frames.
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
     * \brief Check if timecode is a valid timecode.
     * \return \c True if timecode is invalid.
     */
    bool isInvalid() const { return roundedTCBase_ == 0; }

    /*!
     * \brief Gets the rounded timecode base of the \c Timecode object.
     * \return The timecode base (eg: 25, 30 etc...)
     */
    uint16_t getRoundedTCBase() const { return roundedTCBase_; }

    /*!
     * \brief If timecode is a drop frame timecode or not.
     * \return True if current timecode is drop frame.
     */
    bool isDropFrame() const { return dropFrame_; }

    /*!
     * \brief Gets the hours component of \c Timecode object.
     * \return The hour of the timecode.
     */
    int16_t getHour() const { return hour_; }

    /*!
     * \brief Gets the minutes component of \c Timecode object.
     * \return The minutes of the timecode.
     */
    int16_t getMin() const { return min_; }

    /*!
     * \brief Gets the seconds component of \c Timecode object.
     * \return The seconds of the timecode.
     */
    int16_t getSec() const { return sec_; }

    /*!
     * \brief Gets the frames component of \c Timecode object.
     * \return The frames of the timecode.
     */
    int16_t getFrame() const { return frame_; }

    /*!
     * \brief Gets the total offset of the timecode from a \c Timecode object 00:00:00:00
     * \return The total offset in frames.
     */
    int64_t getOffset() const { return offset_; }

    /*!
     * \brief Gets the total number of frames the current timecode represents.
     * \return Number of total frames.
     */
    uint64_t getTotalFrames();

    /*!
     * \brief Invalidates the current timecode.
     */
    void setInvalid();

    /*!
     * \brief Initializes the timecode object.
     * \param roundedRate The rounded rate of the timecode (e.g. 25, 30).
     * \param dropFrame \c True if timecode is drop frame.
     */
    void init(uint16_t roundedRate, bool dropFrame);

    /*!
     * \overload
     * \param offset Offset of the timecode from 0 in frames.
     */
    void init(uint16_t roundedRate, bool dropFrame, int64_t offset);

    /*!
     * \overload
     * \param rateNum Timecode rate numerator.
     * \param rateDen Timecode rate denominator.
     */
    void init(int32_t rateNum, int32_t rateDen, bool dropFrame);

    /*!
     * \overload
     */
    void init(int64_t offset);

    /*!
     * \overload
     */
    void init(int32_t rateNum, int32_t rateDen, bool dropFrame, int64_t offset);

    /*!
     * \overload
     * \param hour The hour component of the timecode.
     * \param min The minutes component of the timecode.
     * \param sec The seconds component of the timecode.
     * \param frame The frames component of the timecode.
     */
    void init(int16_t hour, int16_t min, int16_t sec, int16_t frame);

    /*!
     * \overload
     */
    void init(int32_t rateNum, int32_t rateDen, bool dropFrame, int16_t hour, int16_t min, int16_t sec, int16_t frame);

    /*!
     * \brief Adds the specified offset frames to the timecode and updates it.
     * \param offset The offset value in frames to add to the existing instance offset.
     */
    void addOffset(int64_t offset);

    /*!
     * \overload
     * \param rateNum Timecode rate numerator.
     * \param rateDen Timecode rate denominator.
     */
    void addOffset(int64_t offset, int32_t rateNum, int32_t rateDen);

    /*!
     * \brief Gets the maximum offset allowed for the timecode considering how many frames 24h can contain.
     * \return The maximum offset allowed.
     */
    int64_t getMaxOffset() const;

    /*!
     * \brief Equal operator to compare two timecode instances.
     * \param right The other instance to compare to.
     * \return \c true if both \c this and the other instance are equal.
     */
    bool operator==(const Timecode& right) const;

    /*!
     * \brief Not equal operator to compare two timecode instances.
     * \param right The other instance to compare to.
     * \return \c true if \c this timecode is different thand the supplied one.
     */
    bool operator!=(const Timecode& right) const;

    /*!
     * \brief Smaller than operator.
     * \param right The other instance to check if is smaller or bigger than \c this.
     * \return \c true if the supplied timecode is bigger than \c this.
     */
    bool operator<(const Timecode& right) const;

    /*!
     * \brief Converts the current \c Timecode object to its string representation.
     * \param useDropDelimiter If \c true use the ';' to delimit the frame number part when the time code is drop frame.
     * \return String representation of the \c Timecode object.
     */
    std::wstring toTimecodeString(bool useDropDelimiter = true) const;

private:
    /*!
     * \brief The rounding possibilities.
     */
    enum class Rounding
    {
        ROUND_AUTO,     //!< Automatic rounding.
        ROUND_DOWN,     //!< Rounds to the nearest value downwards.
        ROUND_UP,       //!< Rounds to the nearest value upwards.
        ROUND_NEAREST   //!< Rounds to the nearest automatically.
    };

    /*!
     * \brief Helper function to get the offset into a valid value.
     */
    void cleanOffset();

    /*!
     * \brief Helper function to get the timecode values within valid ranges.
     */
    void cleanTimecode();

    /*!
     * \brief Updates the instance offset after changing the timecode values.
     */
    void updateOffset();

    /*!
     * \brief Initializes all the timecode components to correct values.
     */
    void updateTimecode();

    /*!
     * \brief Converts a position into a proper offset.
     * \param inPosition The input offset position.
     * \param factorTop The rounded rate of the destination timecode (e.g. 25, 30).
     * \param factorBottom The rounded rate of the offset (e.g. 25, 30).
     * \param rounding The rounding approach to use.
     * \return The computed offset.
     */
    int64_t convertPosition(int64_t inPosition, int64_t factorTop, int64_t factorBottom, Rounding rounding) const;

    uint16_t roundedTCBase_;    //!< The rounded timecode base (e.g. 25, 30).
    bool dropFrame_;            //!< Timecode drop frame flag.
    int16_t hour_;              //!< The hour component of the timecode.
    int16_t min_;               //!< The minutes component of the timecode.
    int16_t sec_;               //!< The seconds component of the timecode.
    int16_t frame_;             //!< The frames component of the timecode.
    int64_t offset_;            //!< The timecode offset in frames from hour 0.

    uint32_t framesPerMin_;     //!< Number of frames per minute for the current timecode.
    uint32_t nDFramesPerMin_;   //!< Number of non drop frames per minute for the current timecode.
    uint32_t framesPer10Min_;   //!< Number of frames per 10 minutes for the current timecode.
    uint32_t framesPerHour_;    //!< Number of frames per hour for the current timecode.
    int16_t dropCount_;         //!< Number of frames droped.
};

