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

class Vec3D_float(Structure, Vec3D):
    _fields_ = [
        ('x', c_float),
        ('y', c_float),
        ('z', c_float)
    ]

class Vec3D_int(Structure, Vec3D):
    _fields_ = [
        ('x', c_int),
        ('y', c_int),
        ('z', c_int)
    ]

stateId_t = c_uint32
state_t = c_float

def Vec3D(cType):
    if cType == c_float:
        return Vec3D_float

class State(Structure):
    _fields_ = [
        ('next', stateId_t),
        ('prev', stateId_t),
        ('pos', Vec3D(state_t)),
        ('vel', Vec3D(state_t))
    ]

class Specie(Structure):
    _fields_ = [
        ('id', c_uint32),
        ('name', c_char_p),
        ('statesPerCell', c_int)        
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

class PyVec_State(Structure, PyVec):
    _fields_ = [
        ('ptr', POINTER(State)),
        ('size', c_size_t)
    ]

class PyVec_uint32(Structure, PyVec):
    _fields_ = [
        ('ptr', POINTER(c_uint32)),
        ('size', c_size_t)
    ]

class PyVec_Specie(Structure, PyVec):
    _fields_ = [
        ('ptr', POINTER(Specie)),
        ('size', c_size_t)
    ]

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

class PySimData(Structure):
    _fields_ = [
        ('stateVec', PyVec_State),
        ('cellIdVec', PyVec_uint32),
        ('specieVec', PyVec_Specie)
    ]
