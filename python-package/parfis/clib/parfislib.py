import sys
import os
from ctypes import *
import platform

class Parfis:
    """[summary]
    """
    lib = None

    # By default we consider linux os with lib in release
    linuxReleaseLib = os.path.join(os.path.dirname(__file__), "libparfis.so")
    linuxDebugLib = os.path.join(os.path.dirname(__file__), "../../../build/lib/parfis/libparfisd.so")
    winReleaseLib = os.path.join(os.path.dirname(__file__), "../../../build/lib/parfis/parfis.dll")
    winDebugLib = os.path.join(os.path.dirname(__file__), "../../../build/lib/parfis/parfisd.dll")

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

        loadlib = None

        if platform.system() == 'Linux':
            releaseLib = Parfis.linuxReleaseLib
            debugLib = Parfis.linuxDebugLib
        elif platform.system() == 'Windows':
            releaseLib = Parfis.winReleaseLib
            debugLib = Parfis.winDebugLib

        if mode == 'Copy':
            gettrace = getattr(sys, 'gettrace', None)
            if gettrace is None and os.path.isfile(releaseLib):
                    loaddll = releaseLib
            elif gettrace() and os.path.isfile(debugLib):
                loaddll = debugLib
            elif os.path.isfile(releaseLib):
                loaddll = releaseLib
            else:
                print("Library doesn't exists, exiting!")
                exit(0)
        elif mode == 'Release':
            loaddll = releaseLib
        elif mode == 'Debug':
            loaddll = debugLib

        Parfis.lib = cdll.LoadLibrary(loaddll)

        print(f"Successfully loaded: {loaddll}")

        Parfis.lib.info.argtypes = None
        Parfis.lib.info.restype = c_char_p

    @staticmethod
    def info() -> str:
        return Parfis.lib.info().decode()

if __name__ == "__main__":

    Parfis.load_lib("Copy")
    print(Parfis.info())