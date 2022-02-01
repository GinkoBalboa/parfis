from ctypes import *

class Vec3D:
    def asTuple(self):
        return (self.x, self.y, self.z)

class Vec3D_double(Structure, Vec3D):
    _fields_ = [
        ('x', c_double),
        ('y', c_double),
        ('z', c_double)
    ]

class Vec3D_int(Structure, Vec3D):
    _fields_ = [
        ('x', c_int),
        ('y', c_int),
        ('z', c_int)
    ]

class PyVec:
    def asList(self):
        if self.__class__ == PyVec_char_p:
            return [self.ptr[i].decode() for i in range(self.size)]

class PyVec_char_p(Structure, PyVec):
    _fields_ = [
        ('ptr', POINTER(c_char_p)),
        ('size', c_size_t)
    ]
    def __init__(self):
        self.type = "c_char_p"

class PyCfgData(Structure):
    _fields_ = [
        ('geometry', c_char_p),
        ('timestep', c_double),
        ('geometrySize', POINTER(Vec3D_double)),
        ('cellSize', POINTER(Vec3D_double)),
        ('periodicBoundary', POINTER(Vec3D_int)),
        ('cellCount', POINTER(Vec3D_int)),
        ('specieNameVec', PyVec_char_p)
    ]
