import sys
import os
from ctypes import *

from .datastruct import PyCfgData

class Parfis:

    lib = None
    libPath = None

    currPath = os.path.dirname(os.path.abspath(os.path.expanduser(__file__)))
    
    @staticmethod
    def load_lib(mode='Release'):
        """ Loads speciffic library version

        Args:
            mode (str): Available modes:
            - 'Release' - load release version
            - 'Debug' - load debug version
            - 'Copy' - load debug version if python is in debug mode, otherwise load release version
        """

        # If lib is loaded do nothing
        if Parfis.lib is not None:
            return

        linuxReleaseLib = os.path.join(Parfis.currPath, "libparfis.so")
        linuxDebugLib = os.path.join(Parfis.currPath, "libparfisd.so")
        winReleaseLib = os.path.join(Parfis.currPath, "parfis.dll")
        winDebugLib = os.path.join(Parfis.currPath, "parfisd.dll")

        pathBackup = os.environ['PATH'].split(os.pathsep)

        releaseLib = ""
        debugLib = ""
        # Checking platform.system() gets it wrong under docker, so just look for files
        if os.path.isfile(linuxReleaseLib) or os.path.isfile(linuxDebugLib):
            print("Detected Linux library file")
            releaseLib = linuxReleaseLib
            debugLib = linuxDebugLib
        elif os.path.isfile(winReleaseLib) or os.path.isfile(winDebugLib):
            print("Detected Windows library file")
            releaseLib = winReleaseLib
            debugLib = winDebugLib
        else:
            print("Library file not found!")
            exit(1)

        loadRelease = True
        if mode == 'Copy':
            gettrace = getattr(sys, 'gettrace', None)
            if gettrace() and os.path.isfile(debugLib):
                loadRelease == False
        elif mode == 'Debug':
            loadRelease = False

        if loadRelease and not os.path.isfile(releaseLib):
            print(f"Requested Release lib: {releaseLib} wasn't found falling back to Debug lib")
            loadRelease = False
        if not loadRelease and not os.path.isfile(debugLib):
            print(f"Requested Debug lib: {debugLib} wasn't found falling back to Release lib")
            loadRelease = True

        if loadRelease:
            libPath = releaseLib
        else:
            libPath = debugLib

        # needed when the lib is linked with non-system-available
        # dependencies
        os.environ['PATH'] = os.pathsep.join(
            pathBackup + [os.path.dirname(libPath)])

        print(f"Lib file to load: {libPath}")

        Parfis.lib = cdll.LoadLibrary(libPath)

        print(f"Successfully loaded lib file")

        Parfis.lib.info.argtypes = None
        Parfis.lib.info.restype = c_char_p

        Parfis.lib.newParfis.argtypes = [c_char_p]
        Parfis.lib.newParfis.restype = c_uint32

        Parfis.lib.deleteParfis.argtypes = [c_uint32]
        Parfis.lib.deleteParfis.restype = c_int

        Parfis.lib.deleteAll.argtypes = None
        Parfis.lib.deleteAll.restype = c_int

        Parfis.lib.getPyCfgData.argtypes = [c_uint32]
        Parfis.lib.getPyCfgData.restype = POINTER(PyCfgData)
        
        Parfis.lib.loadSimData.argtypes = [c_uint32]
        Parfis.lib.loadSimData.restype = c_int

    @staticmethod
    def info() -> str:
        return Parfis.lib.info().decode()
    
    @staticmethod
    def newParfis(cfgStr: str  = "") -> int:
        return Parfis.lib.newParfis(cfgStr.encode())

    @staticmethod
    def deleteParfis(id: int) -> int:
        return Parfis.lib.deleteParfis(id)

    @staticmethod
    def deleteAll() -> int:
        return Parfis.lib.deleteAll()

    @staticmethod
    def getPyCfgData(id: int) -> POINTER(PyCfgData):
        return Parfis.lib.getPyCfgData(id)

    @staticmethod
    def loadSimData(id: int) -> int:
        return Parfis.lib.loadSimData(id)

if __name__ == "__main__":

    Parfis.load_lib("Copy")
    print(Parfis.info())