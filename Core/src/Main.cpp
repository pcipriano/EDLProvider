#include "easylogging++.h"

#include "EDLProviderService.h"

_INITIALIZE_EASYLOGGINGPP       //Initialize logging

int main(int argc, char** argv)
{
    edlprovider::core::EDLProviderService service(argc, argv);

    //Start the service
    return service.exec();
}
