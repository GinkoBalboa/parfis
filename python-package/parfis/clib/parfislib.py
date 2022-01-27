import sys
import os
from ctypes import *
import platform

class Parfis:
    lib = None

    # By default we consider linux os with lib in release
    linuxReleaseLib = os.path.join(os.path.dirname(__file__), "libparfis.so")
    linuxDebugLib = os.path.join(os.path.dirname(__file__), "libparfisd.so")
    winReleaseLib = os.path.join(os.path.dirname(__file__), "parfis.dll")
    winDebugLib = os.path.join(os.path.dirname(__file__), "parfisd.dll")

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

        # Checking platform.system() gets it wrong under docker, so just look for files
        if os.path.isfile(Parfis.linuxReleaseLib) or os.path.isfile(Parfis.linuxDebugLib):
            releaseLib = Parfis.linuxReleaseLib
            debugLib = Parfis.linuxDebugLib
        elif os.path.isfile(Parfis.winReleaseLib) or os.path.isfile(Parfis.winDebugLib):
            releaseLib = Parfis.winReleaseLib
            debugLib = Parfis.winDebugLib

        if mode == 'Copy':
            gettrace = getattr(sys, 'gettrace', None)
            if gettrace is None and os.path.isfile(releaseLib):
                loadlib = releaseLib
            elif gettrace() and os.path.isfile(debugLib):
                loadlib = debugLib
            elif os.path.isfile(releaseLib):
                loadlib = releaseLib
            else:
                print("Library doesn't exists, exiting!")
                exit(0)
        elif mode == 'Release':
            loadlib = releaseLib
        elif mode == 'Debug':
            loadlib = debugLib

        print(f"Lib file to load: {loadlib}")

        Parfis.lib = cdll.LoadLibrary(loadlib)

        print(f"Successfully loaded: {loadlib}")

        Parfis.lib.info.argtypes = None
        Parfis.lib.info.restype = c_char_p

    @staticmethod
    def info() -> str:
        return Parfis.lib.info().decode()

if __name__ == "__main__":

    Parfis.load_lib("Copy")
    print(Parfis.info())