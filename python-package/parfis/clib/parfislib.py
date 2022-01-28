import sys
import os
from ctypes import *

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

        if mode == 'Copy':
            gettrace = getattr(sys, 'gettrace', None)
            if gettrace is None and os.path.isfile(releaseLib):
                libPath = releaseLib
            elif gettrace() and os.path.isfile(debugLib):
                libPath = debugLib
            elif os.path.isfile(releaseLib):
                libPath = releaseLib
        elif mode == 'Release':
            libPath = releaseLib
        elif mode == 'Debug':
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

    @staticmethod
    def info() -> str:
        return Parfis.lib.info().decode()

if __name__ == "__main__":

    Parfis.load_lib("Copy")
    print(Parfis.info())