#ifndef CORE_SRC_CONFIGURATION_H
#define CORE_SRC_CONFIGURATION_H

#include "ConfigurationReader.h"

namespace edlprovider
{
namespace core
{

class Configuration
{
public:
    Configuration();

    bool load(const QString& configPath);

    quint16 getServicePort() const;

    QString getServiceHostName() const;

private:
    common::util::ConfigurationReader configReader_;
};

}
}

#endif // CORE_SRC_CONFIGURATION_H
