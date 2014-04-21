#ifndef PLUGINS_FINALCUT_SRC_FINALCUT_H
#define PLUGINS_FINALCUT_SRC_FINALCUT_H

#include <QObject>

#include "EdlInterface.h"

namespace plugins
{
namespace finalcut
{

class FinalCut : public QObject, public interfaces::EdlInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID EDL_INTERFACE_IDENTIFIER)
    Q_INTERFACES(plugins::interfaces::EdlInterface)

public:
    FinalCut();

    std::wstring getEdlName() const override final { return L"FinalCut"; }
};

}
}

#endif // PLUGINS_FINALCUT_SRC_FINALCUT_H
