from ctypes import *

class Vec3D(Structure):
    pass

class Vec3DChild(Vec3D):
    pass

class Vec3DBase():

    def asTuple(self):
        return (self.x, self.y, self.z)

class Vec3D_double(Structure, Vec3DBase):
    _fields_ = [
        ('x', c_double),
        ('y', c_double),
        ('z', c_double)
    ]

class Vec3D_float(Structure, Vec3DBase):
    _fields_ = [
        ('x', c_float),
        ('y', c_float),
        ('z', c_float)
    ]

class Vec3D_int(Structure, Vec3DBase):
    _fields_ = [
        ('x', c_int),
        ('y', c_int),
        ('z', c_int)
    ]

def Vec3D(cType = c_int):
    if cType == c_float:
        return Vec3D_float
    elif cType == c_double:
            return Vec3D_double
    elif cType == c_int:
        return Vec3D_int
    else:
        return None

stateId_t = c_uint32
state_t = c_float

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

class PyVecBase:
    def asList(self):
        if self.__class__ == PyVec_char_p:
            return [self.ptr[i].decode() for i in range(self.size)]

class PyVec_char_p(Structure, PyVecBase):
    _fields_ = [
        ('ptr', POINTER(c_char_p)),
        ('size', c_size_t)
    ]

class PyVec_State(Structure, PyVecBase):
    _fields_ = [
        ('ptr', POINTER(State)),
        ('size', c_size_t)
    ]

class PyVec_uint32(Structure, PyVecBase):
    _fields_ = [
        ('ptr', POINTER(c_uint32)),
        ('size', c_size_t)
    ]

class PyVec_Specie(Structure, PyVecBase):
    _fields_ = [
        ('ptr', POINTER(Specie)),
        ('size', c_size_t)
    ]


def PyVec(cType = c_char_p):
    if cType == c_char_p:
        return PyVec_char_p
    elif cType == c_uint32:
            return PyVec_uint32
    elif cType == State:
        return PyVec_State
    elif cType == Specie:
        return PyVec_Specie
    else:
        return None

class PyCfgData(Structure):
    _fields_ = [
        ('geometry', c_char_p),
        ('timestep', c_double),
        ('geometrySize', POINTER(Vec3D(c_double))),
        ('cellSize', POINTER(Vec3D(c_double))),
        ('periodicBoundary', POINTER(Vec3D(c_int))),
        ('cellCount', POINTER(Vec3D(c_int))),
        ('specieNameVec', PyVec(c_char_p))
    ]

class PySimData(Structure):
    _fields_ = [
        ('stateVec', PyVec(State)),
        ('cellIdVec', PyVec(c_uint32)),
        ('specieVec', PyVec(Specie))
    ]

if __name__ == '__main__':
    pass