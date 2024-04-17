from celery import shared_task
import redis
from beta_api.models import DataInstance
import os
from typing import List, Optional
#from ml_base.decorator import MLModelDecorator
import redis
import json
import cv2
import torch
from ultralytics import YOLO
import numpy as np
import beta_api.ml_models as ml_models
from beta_api.preprocess_image import preprocess_image
import ctypes as ct
from beta_api.CppImgWriter import CppImgWriter
from beta_api.CppImgReader import CppImgReader
import uuid
from betasprayer_backend.settings import BASE_DIR

#potentially set Prefet Multiplier to 1
@shared_task()
def task_execute(job_params):
    print("In Task Execution with UUID: " + job_params["data_instance_uuid"])
    uuid_obj = uuid.UUID(job_params["data_instance_uuid"])
    instance = DataInstance.objects.get(id=uuid_obj)
    
    img = instance.get_data_as_image()
    
   # img = cv2.resize(decoded_image, (data_instance.image_width, data_instance.image_height), interpolation= cv2.INTER_LINEAR)

    # Close all OpenCV windows
    #cv2.destroyAllWindows()
    #preprocess image before it goes into models and clustering
    img = preprocess_image(img)
    print("Preprocessing image complete")

    #get all processing models
    model_types = json.loads(os.environ.get("PREPROCESSING_MODELS"))
    print(model_types["PREPROCESSING_MODELS"])
    ml_results = {}
    
    #compute all model results one at a time
    for ml_id in model_types["PREPROCESSING_MODELS"]:     
      ml_results[ml_id] = ml_models.load_and_predict(ml_id, img)
    
    print("prediction complete")
    c_lib_location = os.path.join(BASE_DIR, os.environ.get("C_LIB_LOC"))
    
    c_lib = ct.CDLL(c_lib_location)
    print(c_lib_location)

    writer = CppImgWriter(img=img,
                          depth_img = ml_results["DEPTH_MODEL"],
                          human_loc = ml_results["HUMAN_MODEL"],
                          hold_loc  = ml_results["HOLD_MODEL"],
                          lib=c_lib)
    print("CPP model instance created successfully")
    c_lib.process_image.restype = ct.c_void_p
    c_lib.process_image.argtypes = [ct.c_void_p]

    print("Hold Count: " + str(len(ml_results["HOLD_MODEL"])))

    try:
      c_return = c_lib.process_image(writer.sendData())
      c_reader = CppImgReader(c_return, lib=c_lib, destroy=True)

      hold_mat = c_reader.getBoxMat()
      path_mat = c_reader.getPathMat()

      hold_json = json.dumps({'holds' : hold_mat.tolist()})
      path_json = json.dumps({'paths' : path_mat.tolist()})

      instance.hold_data = hold_json
      instance.path_data = path_json
      instance.processing_complete = True
      instance.save()

    except ct.ArgumentError as e:
        print("Argument error:", e)
    #except ct.Error as e:
        #print("CTypes error:", e)
    except Exception as e:
        print("An unexpected error occurred:", e)

    print("processing complete")


    return



    



    


    # calc_data = []
    # data_instance["calc_data"] = calc_data
    # data_instance.save()

