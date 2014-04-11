import os, platform, subprocess
print("Generate soap server side code")
gsoapRootPath = input("Path to gsoap root folder: ")
scriptDir = os.path.dirname(os.path.realpath(__file__))
destinationDir = os.path.join(scriptDir, "src")

if not os.path.exists(destinationDir):
    os.makedirs(destinationDir)

if os.path.isdir(gsoapRootPath) and os.path.exists(gsoapRootPath):
    if platform.system() == "Windows":
        wsdl2hPath = os.path.join(gsoapRootPath, "bin", "win32", "wsdl2h.exe")
    elif platform.system() == "Linux":
        wsdl2hPath = os.path.join(gsoapRootPath, "bin", "linux386", "wsdl2h")

    edlInterfaceHeader = os.path.join(destinationDir, "EDLProviderInterface.h")
    result = subprocess.call([wsdl2hPath, '-o', edlInterfaceHeader, os.path.join(scriptDir, "..", "Wsdl", "EDLProvider.wsdl"), '-t', os.path.join(scriptDir, "typemapcustom.dat")])

    if not result:
        if platform.system() == "Windows":
            soapcpp2Path = os.path.join(gsoapRootPath, "bin", "win32", "soapcpp2.exe")
        elif platform.system() == "Linux":
            soapcpp2Path = os.path.join(gsoapRootPath, "bin", "linux386", "soapcpp2")

        result = subprocess.call([soapcpp2Path, '-j', '-i', '-S', '-I', os.path.join(gsoapRootPath, "import") + ";" + gsoapRootPath, edlInterfaceHeader, '-p', 'EDLProvider', '-d', destinationDir])
        if result:
            print("Call to soapcpp2 in [{}] failed.".format(soapcpp2Path))
    else:
        print("Call to wsdl2h in [{}] failed.".format(wsdl2hPath))
else:
    print("Path provided not found")
