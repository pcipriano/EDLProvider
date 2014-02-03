#include "EDLProviderService.h"

int main(int argc, char** argv)
{
    edlprovider::core::EDLProviderService service(argc, argv);

    //Start the service
    return service.exec();
}
