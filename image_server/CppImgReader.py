import numpy as np
from numpy.ctypeslib import ndpointer
from functools import reduce
import ctypes as ct
import json
from .Mat2Ctype import mat2Ctype

class CppImgReader:
    """class CppImgReader

       responsible for reading an OpenCV image from C++
       and build a corresponding Python OpenCV image (numpy array).

       It is the counterpart of the PyImgWriter class,
       written in C++. PyImgWriter "writes" an image,
       so that CppImgReader can "read" it.
    """    
    
    ## binding to a c++ dynamic library,
    #  containing PyImgWriter's implementation
    lib = None
    boxMat = None
    pathMat = None
    
    
    @staticmethod
    def loadLib(libname = 'libprocess_image.so'): 
        """[STATIC] loadLib(libname = 'libPyImageWriter.so')
           
           Loads a dynamic library containing
           PyImgWriter's implementation and binds it
           to CppImgReader.lib
        """
        CppImgReader.lib = ct.CDLL(libname)


    def __init__(self, cobj, libname = 'libprocess_image.so', lib=None, destroy=False):
        """__init__(self, cobj, libname = 'libprocess_image.so')

           instantiates a CppImgReader
           
           cobj is a pointer of types ctypes.c_void_p
                to a C++ PyImgWriter object
           
           libname is the path to a shared library
                   containing PyImgWriter's implementation.
                   it's only used if CppImgReader.loadLib
                   has not yet been called
        """
        CppImgReader.lib = lib
        #load CppImgReader.lib if its not done so already
        if CppImgReader.lib is None:
            CppImgReader.loadLib(libname)
        
        self.cobj = cobj
        self.existscobj = True
        print("assigned cobj")
        self.read_Box_Mat()
        print("read Box Mat")
        self.read_Path_Mat()
        print("read Path Mat")
        self.existscobj = destroy
        if destroy:
            self.delete()


    def read_Box_Mat(self):
        getBoxDims = self.lib.PyImgW_getBoxDims
        getBoxDims.restype = ct.c_uint32
        getBoxDims.argtypes = [ct.c_void_p]
        self.getBoxDims = getBoxDims(self.cobj)
        print(self.getBoxDims)
        #get the image's shape array
        getBoxShape = self.lib.PyImgW_getBoxShape
        getBoxShape.restype = ndpointer(dtype = np.uint32,
                                     shape = (self.getBoxDims,))
        getBoxShape.argtypes = [ct.c_void_p]
        self.boxShape = getBoxShape(self.cobj)
        print(self.boxShape)
        getBoxType = self.lib.PyImgW_getBoxType
        getBoxType.restype = ct.c_uint32
        getBoxType.argtypes = [ct.c_void_p]

        self.boxctype = mat2Ctype( getBoxType(self.cobj) )

        #self.boxctype = ct.c_char
        print(self.boxctype)

        grabBoxMat = self.lib.PyImgW_getBoxMat
        grabBoxMat.restype = ndpointer(dtype = self.boxctype, 
                                   shape = (reduce(lambda x, y:
                                                x * y,
                                                self.boxShape
                                                  ),
                                           )
                                        )
        grabBoxMat.argtypes = [ct.c_void_p]
        #self.grabBoxMat.restype = ct.c_void_p
        img = grabBoxMat(self.cobj)
        # shape = reduce(lambda x, y:x * y,self.boxShape)
                                        

        # buffer_from_memory = ct.pythonapi.PyBuffer_FromMemory
        # buffer_from_memory.restype = ct.py_object
        # buffer = buffer_from_memory(img, 8*shape)

        print("assigned pointer output")                  
        # #grab the raw data and reshape it
        # a = np.frombuffer(buffer, int)
        # print(type(a))
        self.boxMat = np.reshape(img, tuple(self.boxShape))
        #box_json = json.dump(self.boxMat)
        print(self.boxMat)
        print(len(self.boxMat))
        #delete the C++ object if it's the case
    
    

    def read_Path_Mat(self):
        getPathDims = self.lib.PyImgW_getPathDims
        getPathDims.restype = ct.c_uint32
        getPathDims.argtypes = [ct.c_void_p]
        self.getPathDims = getPathDims(self.cobj)
        print(self.getPathDims)

        getPathShape = self.lib.PyImgW_getPathShape
        getPathShape.restype = ndpointer(dtype = np.uint32,
                                     shape = (self.getPathDims,))
        getPathShape.argtypes = [ct.c_void_p]

        self.PathShape = getPathShape(self.cobj)
        print(self.PathShape)

        getPathType = self.lib.PyImgW_getPathType
        getPathType.restype = ct.c_uint32
        getPathType.argtypes = [ct.c_void_p]

        self.pathctype = mat2Ctype( getPathType(self.cobj) )
        print(self.pathctype)

        grabPathMat = self.lib.PyImgW_getPathMat
        grabPathMat.restype = ndpointer(dtype = self.pathctype, 
                                   shape = (reduce(lambda x, y:
                                                x * y,
                                                self.PathShape
                                                  ),
                                           )
                                        )
        grabPathMat.argtypes = [ct.c_void_p]

                                   
        #grab the raw data and reshape it
        img = grabPathMat(self.cobj)
        self.pathMat = np.reshape(img, tuple(self.PathShape))
        print(self.pathMat)
        
        #delete the C++ object if it's the case




    def getBoxMat(self):
        """getImg(self, destroy = False)
           
           function that gets a C++ OpenCV's image (cv::Mat)
           and returns a corresponding Python OpenCV's image (numpy array)

           if destroy is True, the underlying C++ object (self.cobj)
           is deleted after the image is retrieved
        """


        return self.boxMat
    

    def getPathMat(self):
        """getImg(self, destroy = False)
           
           function that gets a C++ OpenCV's image (cv::Mat)
           and returns a corresponding Python OpenCV's image (numpy array)

           if destroy is True, the underlying C++ object (self.cobj)
           is deleted after the image is retrieved
        """
            

        return self.pathMat

    def delete(self):
        assert self.existscobj, "corresponding C object already deleted"
        deleteFunc = self.lib.PyImgW_delete
        deleteFunc.argtypes = [ct.c_void_p]
        deleteFunc(self.cobj)
        self.existscobj = False

    def debug(self):
        """debug(self)

           Outputs a CppImgReader's info, for debugging
        """
        if not self.existscobj:
            print('ATTENTION: CORRESPONDING C OBJECT ALREADY DELETED')
        print("dims: {}".format(self.dims))
        print("shape: {}".format(self.shape))
        print("ctype: {}".format(self.ctype))