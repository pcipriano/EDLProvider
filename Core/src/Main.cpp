#define _ELPP_NO_DEFAULT_LOG_FILE   //No default log file, app must provide one or errors are issued
#include "easylogging++.h"

#include "EdlProviderService.h"

_INITIALIZE_EASYLOGGINGPP       //Initialize logging

int main(int argc, char** argv)
{
    edlprovider::core::EdlProviderService service(argc, argv);

    //Start the service
    return service.exec();
}
