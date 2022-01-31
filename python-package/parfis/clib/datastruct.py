from ctypes import *

class Vec3D_double(Structure):
    _fields_ = [
        ('x', c_double),
        ('y', c_double),
        ('z', c_double)
    ]

class PyCfgData(Structure):
    _fields_ = [
        ('geometry', c_char_p),
        ('timestep', c_double),
        ('geometrySize', POINTER(Vec3D_double))
    ]
