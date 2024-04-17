import sys
import os
sys.path.append(os.getcwd() + 'beta_api/midas/')
import inspect



import torch
from ultralytics import YOLO
from beta_api.midas.model_loader import load_model
from beta_api.midas.dpt_depth import DPTDepthModel
from beta_api.midas.transforms import Resize, NormalizeImage, PrepareForNet
import os
from torchvision.transforms import Compose
import json
from django.core.cache import cache
import cv2
import numpy as np
from betasprayer_backend.settings import BASE_DIR



def load_depth_model(model_path):
  device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
  model = DPTDepthModel(
            path=model_path,
            backbone="vitb_rn50_384",
            non_negative=True,
        )
  net_w, net_h = 384, 384
  resize_mode = "minimal"
  normalization = NormalizeImage(mean=[0.5, 0.5, 0.5], std=[0.5, 0.5, 0.5])

  transform = Compose(
        [
            Resize(
                net_w,
                net_h,
                resize_target=None,
                keep_aspect_ratio=True,
                ensure_multiple_of=32,
                resize_method=resize_mode,
                image_interpolation_method=cv2.INTER_CUBIC,
            ),
            normalization,
            PrepareForNet(),
        ]
    )
  model.eval()
  model.to(device)  
  return (model, transform)

def load_yolo(model_path):
  #print("in loading yolo model")
  return YOLO(model_path)


def load_all_models():
        prepross_env= os.environ.get("PREPROCESSING_MODELS")
    
        model_list = json.loads(prepross_env)["PREPROCESSING_MODELS"]
        for model_key in model_list:
          get_model(model_key, False)


def get_model(chach_key, return_model=False):
    
    #model = cache.get(chach_key) # get model from cache
    model = None
    if model is None:
        model_item_env = os.environ.get(chach_key)
        
        model_item = json.loads(model_item_env)
        #print("items")
        #print(model_item)
        # your model isn't in the cache
        # so `set` it
        abs_path = os.path.join(BASE_DIR,model_item["LOC"])
        model = function_dict[model_item["LOAD_METHOD"]](abs_path) # load model
        #cache.set(chach_key, model, None) # save in the cache
    
    if return_model:
        return model
    
    return

def load_and_predict(model_id, img):
  
  model_config = json.loads(os.environ.get(model_id))
  model =  get_model(model_id, True)
  ml_results = function_dict[model_config["EVAL_METHOD"]](model, img)
  

  return ml_results

def get_depth_map(model, img):
  
  model, transform = model
  with torch.no_grad():
    if img.ndim == 2:
          img = cv2.cvtColor(img, cv2.COLOR_GRAY2BGR)

    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB) / 255.0
    image = transform({"image": img})["image"]
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    sample = torch.from_numpy(image).to(device).unsqueeze(0)
    height, width = sample.shape[2:]
    prediction = model.forward(sample)
    new_size = image.shape[1::]
    print(new_size)
    height, width = sample.shape[2:]
    print(f"    Input resized to {width}x{height} before entering the encoder")
    prediction = (
              torch.nn.functional.interpolate(
                  prediction.unsqueeze(1),
                  size=new_size[::-1],
                  mode="bicubic",
                  align_corners=False,
              )
              .squeeze()
              .cpu()
              .numpy()
          )
    depth = prediction
    if not np.isfinite(prediction).all():
        depth=np.nan_to_num(prediction, nan=0.0, posinf=0.0, neginf=0.0)
        print("WARNING: Non-finite depth values present")

    depth_min = depth.min()
    depth_max = depth.max()

    #max_val = (2**(8*1))-1
    max_val = 1


    if depth_max - depth_min > np.finfo("float").eps:
        out = max_val - (max_val * (depth - depth_min) / (depth_max - depth_min))
    else:
        out = np.zeros(depth.shape, dtype=np.uint8)
    print("Out complet")
    return out
    # min_value = np.min(out)
    # max_value = np.max(out)

    # # Scale the values to the range [0, 255]
    # scaled_image = (image - min_value) * (255.0 / (max_value - min_value))
    # scaled_image = np.reshape(scaled_image, (height, width, -1))
    # #image_array = np.resize(scaled_image,(height, width, 3))
    # img = np.uint8(scaled_image)
    # cv2.imwrite("test.png", img)
    # return img
  

def yolo_predict(model, data, conf=0):
  results = model.predict(source=data, save=False, verbose=False, conf=conf)
  #print(results[0].boxes)
  if results[0].boxes.shape[0] != 0:
    results = results[0].boxes.xyxy.detach().numpy()
  else:
     shape = (1, 4)  # Specify the number of rows and columns
     results = np.full(shape, -1)
  #print("results len: " , len(results))
              #print(results)
  return results

global_vars = globals()

function_dict = {name: obj for name, obj in global_vars.items() if inspect.isfunction(obj)}
