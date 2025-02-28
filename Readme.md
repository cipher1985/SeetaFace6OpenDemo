使用SeetaFace6Open基础功能制作Demo测试用程序，使用所有基础数据模型实现相应功能并封装集成控制类。  
![程序预览图](https://github.com/cipher1985/SeetaFace6OpenDemo/blob/main/preview.png?raw=true)  
  
## 使用工具
* [SeetaFace6OpenIndex](https://github.com/SeetaFace6Open/index) ([@SeetaFace6Open](https://github.com/SeetaFace6Open))

# 模型可以官方提供的地址下载：
### 百度网盘
模型文件：  
Part I: [Download](https://pan.baidu.com/s/1LlXe2-YsUxQMe-MLzhQ2Aw) code: `ngne`, including: `age_predictor.csta`, `face_landmarker_pts5.csta`, `fas_first.csta`, `pose_estimation.csta`, `eye_state.csta`, `face_landmarker_pts68.csta`, `fas_second.csta`, `quality_lbn.csta`, `face_detector.csta`, `face_recognizer.csta`, `gender_predictor.csta`, `face_landmarker_mask_pts5.csta`, `face_recognizer_mask.csta`, `mask_detector.csta`.   
Part II: [Download](https://pan.baidu.com/s/1xjciq-lkzEBOZsTfVYAT9g) code: `t6j0`，including: `face_recognizer_light.csta`.  

### Dropbox
Model files:  
Part I: [Download](https://www.dropbox.com/s/julk1f16riu0dyp/sf6.0_models.zip?dl=0), including: `age_predictor.csta`, `face_landmarker_pts5.csta`, `fas_first.csta`, `pose_estimation.csta`, `eye_state.csta`, `face_landmarker_pts68.csta`, `fas_second.csta`, `quality_lbn.csta`, `face_detector.csta`, `face_recognizer.csta`, `gender_predictor.csta`, `face_landmarker_mask_pts5.csta`, `face_recognizer_mask.csta`, `mask_detector.csta`.   
Part II: [Download](https://www.dropbox.com/s/d296i7efnz5evbx/face_recognizer_light.csta?dl=0) ，including: `face_recognizer_light.csta`.  

### 模型说明：
模型名称           | 网络结构       | 速度（I7-6700） | 特征长度
-|-|-|-
通用人脸识别       | ResNet-50      | 57ms           | 1024
带口罩人脸识别     | ResNet-50      | 34ms           | 512
通用人脸识别（小） | Mobile FaceNet | 9ms            | 512

模型名称 | 模型功能
-|-|-
age_predictor.csta | 年龄预测  
eye_state.csta | 眼睛状态检测  
face_detector.csta | 人脸检测  
face_landmarker_mask_pts5.csta | 口罩5关键点定位  
face_landmarker_pts5.csta | 人脸5关键点定位  
face_landmarker_pts68.csta | 人脸68关键点定位  
face_recognizer.csta | 通用场景高精度人脸识别  
face_recognizer_light.csta | 轻量级人脸识别  
face_recognizer_mask.csta | 带口罩人脸识别  
fas_first.csta | 局部活体检测  
fas_second.csta | 全局活体检测  
gender_predictor.csta | 性别检测  
mask_detector.csta | 口罩检测  
pose_estimation.csta | 姿态检测  
quality_lbn.csta | 质量检测
