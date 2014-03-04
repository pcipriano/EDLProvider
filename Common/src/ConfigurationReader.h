#ifndef COMMON_SRC_CONFIGURATIONREADER_H
#define COMMON_SRC_CONFIGURATIONREADER_H

#include <QUrl>
#include <QString>
#include <QDomDocument>

#include "Pimpl.h"
#include "Uncopyable.h"

namespace common
{
namespace util
{

/*!
 * \brief The ConfigurationReader class manages configurations, reading and writing modifications.
 *
 *  The configuration must be in XML format.
 *
 *  To use getter/setter functions with element paths
 *  Example XML:
 * \verbatim<root>
       <child1>
           <child2>My value</child2>
       </child1>
   </root>\endverbatim
 *
 *
 *  \code
 *  //Accessing child2 value
 *  ConfigurationReader confReader;
 *  confReader.loadXml(myConfigText);
 *
 *  auto value = confReader.getValueOrDefault<QString>(QStringList() << "child1" << "child2", "Default");
 *  //value contains "My value"
 *  \endcode
 */
class ConfigurationReader : Uncopyable
{
public:
    /*!
     * \brief ConfigurationReader constructor.
     * \param autoUpdate If \c true configuration updates automatically when changes occur in the filesystem.
     */
    explicit ConfigurationReader(bool autoUpdate = false);

    /*!
     * \overload
     * \param schemaPath The schema file path to use for validating the xml of the configuration.
     */
    explicit ConfigurationReader(const QString& schemaPath, bool autoUpdate = false);

    /*!
     * \overload
     * \param schemaPath The \c QUrl pointing to a resource containing the schema to use on validation.
     */
    explicit ConfigurationReader(const QUrl& schemaPath, bool autoUpdate = false);

    /*!
     * \brief ConfigurationReader destructor.
     */
    ~ConfigurationReader();

    /*!
     * \brief Check if the xml loaded is valid and if a schema was specified that it complies to the schema
     * \return \c true if configuration is valid.
     */
    bool getIsValid();

    /*!
     * \brief Loads a file containing XML data.
     * \param filePath The file path where the is located.
     * \return \c True if loading the file was successful.
     */
    bool loadFile(const QString& filePath);

    /*!
     * \brief Loads XML data in a \c QString.
     * \param xmlData The XML data to load.
     * \return \c True if loading was successful.
     */
    bool loadXml(const QString& xmlData);

    /*!
     * \brief Sets the file path where the configuration will be saved or filesystem events are checked.
     * \param filePath The new file path to set.
     */
    void setFilePath(const QString& filePath);

    /*!
     * \brief Saves the current configuration in the file path set from setFilePath() or loadFile().
     * \param overwrite Set to \c true if file should be ovewritten.
     * \return If successful returns \c true.
     * \warning If setFilePath() or loadFile() was not used the call will fail because no path was set.
     */
    bool save(bool overwrite = true) const;

    /*!
     * \overload
     * \param newPath The path to where the configuration should be saved.
     * \warning The newPath will not change the file path set previously using setFilePath() or loadFile().
     */
    bool save(const QString& newPath, bool overwrite = true) const;

    /*!
     * \brief Gets the current DOM representation of the configuration being managed.
     * \param detached If \c true the \c QDomDocument returned will be cloned.
     * \return A \c QDomDocument of the configuration being managed.
     */
    QDomDocument getConfiguration(bool detached = false) const;

    /*!
     * \brief Gets a value from an element in the configuration.
     * \param elementName The element name to get the value from.
     * \param ok If ok is non null ok will be set to \c true if call is successful.
     * \return The value converted to the type specified.
     */
    template<typename T>
    T getValue(const QString& elementName, bool* ok = nullptr) const;

    /*!
     * \overload
     * \param elementPath The element path to get the value from.
     */
    template<typename T>
    T getValue(const QStringList& elementPath, bool* ok = nullptr) const;

    /*!
     * \brief Gets a value from an element and if not found returns a default value.
     * \param elementName The element name to get the value from.
     * \param defaultValue The default value to return in case the element is not found.
     * \return The value converted to the type specified or the default provided.
     */
    template<typename T>
    T getValueOrDefault(const QString& elementName, const T& defaultValue = T()) const;

    /*!
     * \overload
     * \param elementPath The element path to get the value from.
     */
    template<typename T>
    T getValueOrDefault(const QStringList& elementPath, const T& defaultValue = T()) const;

    /*!
     * \brief Gets an attribute value from the configuration.
     * \param attributeName The attribute name to retrieve.
     * \param elementName The element to search for the attribute. If empty the XML root element is used.
     * \param ok If ok is non null ok will be set to \c true if call is successful.
     * \return The value converted to the type specified.
     */
    template<typename T>
    T getAttribute(const QString& attributeName, const QString& elementName = QString(), bool* ok = nullptr) const;

    /*!
     * \overload
     * \param elementPath The element path to get the value from.
     */
    template<typename T>
    T getAttribute(const QString& attributeName, const QStringList& elementPath, bool* ok = nullptr) const;

    /*!
     * \brief Gets an attribute value from the configuration, if not found returns a default value.
     * \param attributeName The attribute name to retrieve.
     * \param elementName The element to search for the attribute. If empty the XML root element is used.
     * \param defaultValue The default value to return in case the attribute is not found.
     * \return The value converted to the type specified or the default provided.
     */
    template<typename T>
    T getAttributeOrDefault(const QString& attributeName, const QString& elementName = QString(), const T& defaultValue = T()) const;

    /*!
     * \overload
     * \param elementPath The element path to get the attribute from.
     */
    template<typename T>
    T getAttributeOrDefault(const QString& attributeName, const QStringList& elementPath, const T& defaultValue = T()) const;

    /*!
     * \brief Gets a section from a XML file.
     *  All the elements will be returned and converted to the specified type. If an element fails to convert the
     *  function returns all the elements found until the error.
     * \param parentElementName The parent element that contains the child elements to get.
     * \param ok If ok is non null ok will be set to \c true if call is successful.
     * \return Returns a list of pairs containing the name of the element and the converted value.
     */
    template<typename T>
    QList<std::pair<QString, T>> getSection(const QString& parentElementName, bool* ok = nullptr);

    /*!
     * \overload
     * \param parentElementPath The parent element path to get the child elements from.
     */
    template<typename T>
    QList<std::pair<QString, T>> getSection(const QStringList& parentElementPath, bool* ok = nullptr);

    /*!
     * \brief Sets a XML element value.
     * \param elementName The element name to set the value.
     * \param value The value to set.
     * \param create If \c true the element is created if not found. \c false by default.
     * \return \c true if element value was successfully set.
     */
    bool setValue(const QString& elementName, const char* value, bool create = false);

    /*!
     * \overload
     * \param elementPath The element path where to set the value. If create is \c true the full hierarchy of elements is created.
     */
    bool setValue(const QStringList& elementPath, const char* value, bool create = false);

    /*!
     * \overload
     */
    bool setValue(const QString& elementName, const QString& value, bool create = false);

    /*!
     * \overload
     */
    bool setValue(const QStringList& elementPath, const QString& value, bool create = false);

    /*!
     * \overload
     */
    template<typename T>
    bool setValue(const QString& elementName, const T& value, bool create = false);

    /*!
     * \overload
     */
    template<typename T>
    bool setValue(const QStringList& elementPath, const T& value, bool create = false);

private:
    class Impl;             //!< Private implementation class.
    Pimpl<Impl> impl_;      //!< Private implementation instance.
};

}
}

#include "ConfigurationReaderImpl.h"

#endif // COMMON_SRC_CONFIGURATIONREADER_H
