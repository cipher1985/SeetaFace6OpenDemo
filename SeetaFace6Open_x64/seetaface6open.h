#ifndef SEETAFACE6OPEN_H
#define SEETAFACE6OPEN_H

#include <QObject>
#include <QVector3D>
#include <QImage>

#include <seeta/FaceDetector.h>
#include <seeta/FaceLandmarker.h>
#include <seeta/AgePredictor.h>
#include <seeta/EyeStateDetector.h>
#include <seeta/FaceRecognizer.h>
#include <seeta/GenderPredictor.h>
#include <seeta/MaskDetector.h>
#include <seeta/FaceAntiSpoofing.h>
#include <seeta/QualityOfLBN.h>
#include <seeta/PoseEstimator.h>

//开源版SeetaFace6功能封装
class SeetaFace6 : public QObject
{
    Q_OBJECT
public:
    //脸部关键点定位模式
    enum FaceLandmarkerMode
    {
        FLM_Mark5 = 0,  //5点关键点
        FLM_Mark68,     //68点关键点
        FLM_Mar5Mask,   //带口罩5关键点
    };
    //人脸识别模式
    enum FaceRecognizerMode
    {
        FRM_Light = 0,  //轻量级的人脸识别
        FRM_Normal,     //通用正常人脸识别
        FRM_Mask        //带口罩的人脸识别
    };
    //眼睛开合状态
    enum EyeState
    {
        SES_CLOSE = seeta::EyeStateDetector::EYE_CLOSE,     //闭眼
        SES_OPEN = seeta::EyeStateDetector::EYE_OPEN,       //睁眼
        SES_NONE = seeta::EyeStateDetector::EYE_RANDOM,   //非眼部区域
        SES_UNKNOW = seeta::EyeStateDetector::EYE_UNKNOWN,  //未知状态
    };
    struct SEyeState
    {
        EyeState left = SES_UNKNOW;
        EyeState right = SES_UNKNOW;
    };
    //性别状态
    enum Gender
    {
        GD_UNKNOW = -1,
        GD_MALE = seeta::GenderPredictor::MALE,
        GD_FEMALE = seeta::GenderPredictor::FEMALE,
    };
    //活体识别状态
    enum FakeFaceMode
    {
        FFM_PART = 0,   //局部活体检测
        FFM_GLOBAL      //全局活体检测
    };
    enum FakeState
    {
        FS_UNKNOW = -1,                                     //未初始化
        FS_REAL = seeta::FaceAntiSpoofing::REAL,            //真实人脸
        FS_FAKE = seeta::FaceAntiSpoofing::SPOOF,           //虚假人脸
        FS_FAILED = seeta::FaceAntiSpoofing::FUZZY,         //无法判断
        FS_DETECTING = seeta::FaceAntiSpoofing::DETECTING   //正在检测
    };
public:
    //转换图片数据
    static SeetaImageData createFromImage(QImage& img);
    //释放通过QImage创建的Seeta图像
    static void releaseSeetaImage(SeetaImageData image);
public:
    explicit SeetaFace6(QObject *parent = nullptr);
    //初始化人脸位置检测
    void initFaceDetector();
    //初始化人脸关键点检测
    void initFaceLandmarker(
        FaceLandmarkerMode mode = FLM_Mark5);
    //初始化年龄预测
    void initAgePredictor();
    //初始化眼睛开合状态检测
    void initEyeStateDetector();
    //初始化性别检测
    void initGenderPredictor();
    //初始化口罩检测
    void initMaskDetector();
    //初始化人脸是否模糊检测
    void initBlurDetector();
    //初始化活体识别
    void initFakeFaceDetector(FakeFaceMode mode = FFM_PART);
    //初始化预估人脸角度
    void initEstimateAngle();
    //初始化人脸识别匹配检测
    void initFaceRecognizer(
        FaceRecognizerMode mode = FRM_Light);
public:
    //检测人脸位置
    QVector<SeetaFaceInfo> detectFace(const SeetaImageData &image);
    //检测最大人脸位置
    SeetaFaceInfo detectMaxFace(const SeetaImageData &image);
    //设置检测人脸最小尺寸(默认值为20)
    void setDetectFaceMinSize(int minSize = 20);
    int getDetectFaceMinSize();
    //设置检测器过滤阈值(范围0-1，默认为 0.90)
    void setDetectFaceThreshold(float threshold = 0.9f);
    float getDetectFaceThreshold();
    //设置支持输入图像的最大宽高
    void setDetectFaceMaxWidth(int width);
    void setDetectFaceMaxHeight(int height);
    int getDetectFaceMaxWidth();
    int getDetectFaceMaxHeight();
public:
    //标记人脸关键点
    std::vector<SeetaPointF> markFace(const SeetaImageData &image, const SeetaRect &face);

    //获得预测年龄
    int predictAge(const SeetaImageData &image, std::vector<SeetaPointF>& faceMark);

    //检测眼睛开合状态
    SEyeState detectEyeState(const SeetaImageData &image, std::vector<SeetaPointF>& faceMark);

    //预测性别
    Gender predictGender(const SeetaImageData &image, std::vector<SeetaPointF>& faceMark);

    //检测口罩(score范围0-1，score>=0.5返回真)
    bool detectMask(const SeetaImageData &image, const SeetaRect &face, float *score = nullptr);

    //假脸活体预测
    FakeState predictFake(const SeetaImageData &image,
        const SeetaRect &face, std::vector<SeetaPointF>& faceMark);
    //视频检测活体
    FakeState predictFakeVideo(const SeetaImageData &image,
        const SeetaRect &face, std::vector<SeetaPointF>& faceMark, bool resetDetect = false);
    //设置视频检测活体帧数,当输入帧数为该值以后才会有返回值
    void setFakePredictVideoCount(int num);
    int getFakePredictVideoCount();
    //设置活体检测阈值(clarity清晰度阈值，默认0.3；reality活体阈值，默认0.8)
    void setFakeThreshold(float clarity = 0.3f, float reality = 0.8f);
    void getFakeThreshold(float* clarity, float* reality);

    //检测人脸是否模糊
    bool detectIsBlur(const SeetaImageData &image, std::vector<SeetaPointF>& faceMark68);
    //设置人脸模糊阈值，默认0.8
    void setBlurThreshold(double blur = 0.8);
    double getBlurThreshold();

    //预估姿势角度
    //X轴旋转控制物体在垂直方向上的上下旋转，pitch（俯仰角）
    //Y轴旋转，控制物体的翻滚动作，roll（翻滚角）
    //Z轴旋转，控制物体在水平方向上的旋转，yaw（偏航角）
    QVector3D estimateAngle(const SeetaImageData &image, const SeetaRect &face);

    //检测获取人脸特征点
    QVector<float> detectFaceFeature(const SeetaImageData &image, std::vector<SeetaPointF>& faceMark);
    //比对人脸特征点匹配程度(范围0-1)
    float matchFeature(QVector<float> feature1, QVector<float> feature2);
private:
    //人脸位置检测
    seeta::FaceDetector* m_faceDetector{};
    //人脸关键点检测
    seeta::FaceLandmarker* m_faceLandmarker{};
    //年龄预测
    seeta::AgePredictor* m_agePredictor{};
    //眼睛开合状态检测
    seeta::EyeStateDetector* m_eyeStateDetector{};
    //性别预测
    seeta::GenderPredictor* m_genderPredictor{};
    //口罩检测
    seeta::MaskDetector* m_maskDetector{};
    //活体检测
    seeta::FaceAntiSpoofing* m_faceAntiSpoofing{};
    //清晰度检测
    seeta::QualityOfLBN* m_qualityOfLBN{};
    //姿势角度检测
    seeta::PoseEstimator* m_poseEstimator{};
    //人脸识别检测
    seeta::FaceRecognizer* m_faceRecognizer{};
private:
    const std::string cstr_model_path = "SeetaModels";
    const std::string cstr_age_predictor = cstr_model_path + "/age_predictor.csta";
    const std::string cstr_eye_state = cstr_model_path + "/eye_state.csta";
    const std::string cstr_face_detector = cstr_model_path + "/face_detector.csta";
    const std::string cstr_face_landmarker_mask_pts5 = cstr_model_path + "/face_landmarker_mask_pts5.csta";
    const std::string cstr_face_landmarker_pts5 = cstr_model_path + "/face_landmarker_pts5.csta";
    const std::string cstr_face_landmarker_pts68 = cstr_model_path + "/face_landmarker_pts68.csta";
    const std::string cstr_face_recognizer = cstr_model_path + "/face_recognizer.csta";
    const std::string cstr_face_recognizer_light = cstr_model_path + "/face_recognizer_light.csta";
    const std::string cstr_face_recognizer_mask = cstr_model_path + "/face_recognizer_mask.csta";
    const std::string cstr_fas_first = cstr_model_path + "/fas_first.csta";
    const std::string cstr_fas_second = cstr_model_path + "/fas_second.csta";
    const std::string cstr_gender_predictor = cstr_model_path + "/gender_predictor.csta";
    const std::string cstr_mask_detector = cstr_model_path + "/mask_detector.csta";
    const std::string cstr_pose_estimation = cstr_model_path + "/pose_estimation.csta";
    const std::string cstr_quality_lbn = cstr_model_path + "/quality_lbn.csta";
};

#endif // SEETAFACE6OPEN_H
