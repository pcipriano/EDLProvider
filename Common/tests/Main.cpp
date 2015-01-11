#include <QCoreApplication>

#include "AutoTest.h"

#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP       //Initialize logging

int main(int argc, char** argv)
{
    QCoreApplication application(argc, argv);
    Q_UNUSED(application);

    return AutoTest::run(argc, argv);
}
