import ctypes as ct
import cv2

from .NpArr2CArr import npArr2CArr

class CppImgWriter:
    """class CppImgWriter

       responsible for searializing a Python OpenCV image (numpy array)
       so it can be read by a C++ function and made into
       a C++ Opencv image (cv::Mat)

       It is the counterpart of the PyImgReader class,
       written in C++. CppImgWriter "writes" an image,
       so that PyImgReader can "read" it.
    """
    
    ## binding to a c++ dynamic library,
    #  containing PyImgReader's implementation
    lib = None
    
    @staticmethod
    def loadLib(libname = 'libPyImageReader.so'): 
        """[STATIC] loadLib(libname = 'libPyImageReader.so')
           
           Loads a dynamic library containing
           PyImgReader's implementation and binds it
           to CppImgReader.lib
        """

        CppImgWriter.lib = ct.CDLL(libname)
    
    
    def __init__(self,
                 img = None,
                 depth_img = None,
                 human_loc = None,
                 hold_loc = None,                
                 libname = 'libPyImageReader.so',
                 lib=None):
        """__init__(self,
                    img = None,
                    imgpath = '',
                    libname = 'libPyImageReader.so'
                   )

           instantiates a CppImgWriter
           
           img is an OpenCV image (numpy array)

           imgpath is the path containing an image to be loaded,
                   in case img argument was not provided
           
           libname is the path to a shared library
                   containing PyImgReader's implementation.
                   it's only used if CppImgWriter.loadLib
                   has not yet been called
        """
        CppImgWriter.lib = lib
       # assert (img is not None or depth_img is not None, hold_loc = None), "Not ALL MATS Provided"
        print("loading lib")
        # load CppImgWriter.lib if it has not been done so
        if CppImgWriter.lib is None:
            
            CppImgWriter.loadLib(libname)
            
        print("lib loaded")
        
        
        # serialize img's data, storing its original shape
        # and OpenCV Mat type
        (arr_img, shape_img, cvtype_img) = npArr2CArr(img)
        (arr_depth_img, _, cvtype_depth_img) = npArr2CArr(depth_img)
        (arr_human_loc, shape_human_loc, cvtype_human_loc) = npArr2CArr(human_loc)
        (arr_hold_loc, shape_hold_loc, cvtype_hold_loc) = npArr2CArr(hold_loc)
        print("data transform complete")
        # instantiate a C++ PyImgReader object and store
        # its reference
        newwriter = CppImgWriter.lib.PyImgR_new
        newwriter.restype = ct.c_void_p
        print("new writer created")
        
        # a reference to a C++ PyImgReader object
        self.cobj = newwriter(arr_img, shape_img[0], shape_img[1], cvtype_img, 
                              arr_depth_img, cvtype_depth_img,
                              arr_hold_loc, shape_hold_loc[0],shape_hold_loc[1],cvtype_hold_loc,
                              arr_human_loc,shape_human_loc[0],shape_human_loc[1],cvtype_human_loc
                              )
        print("self.obj assigned")

    
    def sendData(self):
        """sendImg(self)
           
           To be used
        """
        return self.cobj
    
    
    def getImg(self):
        """getImg(self)
           
           for debugging purposes.

           grabs a pointer to a cv::Mat of type ctypes.c_void_p
           This is the image stored by C++ PyImgReader object,
           stored in self.cobj
        """
        
        grabimg = CppImgWriter.lib.PyImgR_getImg
        grabimg.restype = ct.c_void_p
        return grabimg(self.cobj)