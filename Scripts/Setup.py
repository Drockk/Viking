import logging
import os
import shutil
from sys import platform
import wget

downloadPathPremake = "https://github.com/premake/premake-core/releases/download"
versionPremake = "5.0.0-beta1"

def download(source, destination):
    try:
        wget.download(source, destination)
    except:
        logging.error("Download error from {} to {}".format(source, destination))

def configurePremake():
    logging.info("Configuring Premake")

    if platform == "win32":
        premakeExectuable = "premake5.exe"
        operationSystem = "windows"
    else:
        logging.error("Not supported platform")

    premakePath = os.path.join("Vendor", "Premake", "Bin")
    premakeExectuablePath = os.path.join(premakePath, premakeExectuable)

    if not os.path.exists(premakeExectuablePath):
        logging.info("Download Premake")
        packageName = "premake-" + versionPremake + "-" + operationSystem + ".zip"
        downloadPath = downloadPathPremake + "/v" + versionPremake + "/" + packageName
        print(downloadPath)
        download(downloadPath, premakePath)

        logging.info("Unpacking Premake")
        shutil.unpack_archive(os.path.join(premakePath, packageName), premakePath)

        os.remove(os.path.join(premakePath, packageName))

    logging.info("Premake configured")
    os.system(premakeExectuablePath + " vs2022")



if __name__ == '__main__':
    logging.basicConfig(filename="setup.log", format='%(asctime)s - %(message)s', level=logging.INFO)
    configurePremake()