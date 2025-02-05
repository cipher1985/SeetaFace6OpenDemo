#include "seetaface6open.h"

SeetaImageData SeetaFace6::createFromImage(QImage &img)
{
    QImage image = img.convertToFormat(QImage::Format_BGR888);
    SeetaImageData simg;
    simg.width      = image.width();
    simg.height     = image.height();
    simg.channels   = (image.depth() >> 3);
    //创建数据
    int preLine = simg.width * simg.channels;
    uchar* data = new uchar[preLine * simg.height];
    for(int y = 0, offset = 0; y < simg.height; ++y, offset += preLine)
        memcpy(data + offset, image.scanLine(y), preLine);
    simg.data       = data;
    return simg;
}

void SeetaFace6::releaseSeetaImage(SeetaImageData image)
{
    if(!image.data)
        return;
    delete[] image.data;
    image.data = nullptr;
}

SeetaFace6::SeetaFace6(QObject *parent)
    : QObject{parent}
{}

void SeetaFace6::initFaceDetector()
{
    if(m_faceDetector)
        return;
    seeta::ModelSetting setting(cstr_face_detector);
    m_faceDetector = new seeta::FaceDetector(setting);
}

void SeetaFace6::initFaceLandmarker(FaceLandmarkerMode mode)
{
    if(m_faceLandmarker) {
        delete m_faceLandmarker;
        m_faceLandmarker = nullptr;
    }
    std::string modeName;
    switch (mode) {
    case FLM_Mar5Mask:
        modeName = cstr_face_landmarker_mask_pts5;
        break;
    case FLM_Mark68:
        modeName = cstr_face_landmarker_pts68;
        break;
    default:
        modeName = cstr_face_landmarker_pts5;
        break;
    }
    seeta::ModelSetting setting(modeName);
    m_faceLandmarker = new seeta::FaceLandmarker(setting);
}

void SeetaFace6::initAgePredictor()
{
    if(m_agePredictor)
        return;
    seeta::ModelSetting setting(cstr_age_predictor);
    m_agePredictor = new seeta::AgePredictor(setting);
}

void SeetaFace6::initEyeStateDetector()
{
    if(m_eyeStateDetector)
        return;
    seeta::ModelSetting setting(cstr_eye_state);
    m_eyeStateDetector = new seeta::EyeStateDetector(setting);
}

void SeetaFace6::initGenderPredictor()
{
    if(m_genderPredictor)
        return;
    seeta::ModelSetting setting(cstr_gender_predictor);
    m_genderPredictor = new seeta::GenderPredictor(setting);
}

void SeetaFace6::initMaskDetector()
{
    if(m_maskDetector)
        return;
    seeta::ModelSetting setting(cstr_mask_detector);
    m_maskDetector = new seeta::MaskDetector(setting);
}

void SeetaFace6::initBlurDetector()
{
    if(m_qualityOfLBN)
        return;
    seeta::ModelSetting setting(cstr_quality_lbn);
    m_qualityOfLBN = new seeta::QualityOfLBN(setting);

}

void SeetaFace6::initFakeFaceDetector(FakeFaceMode mode)
{
    if(m_faceAntiSpoofing) {
        delete m_faceAntiSpoofing;
        m_faceAntiSpoofing = nullptr;
    }
    seeta::ModelSetting setting(
        mode == FFM_PART ? cstr_fas_first : cstr_fas_second);
    m_faceAntiSpoofing = new seeta::FaceAntiSpoofing(setting);
}

void SeetaFace6::initEstimateAngle()
{
    if(m_poseEstimator)
        return;
    seeta::ModelSetting setting(cstr_pose_estimation);
    m_poseEstimator = new seeta::PoseEstimator(setting);
}

void SeetaFace6::initFaceRecognizer(FaceRecognizerMode mode)
{
    if(m_faceRecognizer) {
        delete m_faceRecognizer;
        m_faceRecognizer = nullptr;
    }
    std::string modeName;
    switch (mode) {
    case FRM_Normal:
        modeName = cstr_face_recognizer;
        break;
    case FRM_Mask:
        modeName = cstr_face_recognizer_mask;
        break;
    default:
        modeName = cstr_face_recognizer_light;
        break;
    }
    seeta::ModelSetting setting(modeName);
    m_faceRecognizer = new seeta::FaceRecognizer(setting);
}

QVector<SeetaFaceInfo> SeetaFace6::detectFace(const SeetaImageData &image)
{
    QVector<SeetaFaceInfo> ret;
    if(!m_faceDetector)
        return ret;
    SeetaFaceInfoArray arr = m_faceDetector->detect(image);
    ret.resize(arr.size);
    memcpy(ret.data(), arr.data, arr.size * sizeof(SeetaFaceInfo));
    return ret;
}

SeetaFaceInfo SeetaFace6::detectMaxFace(const SeetaImageData &image)
{
    SeetaFaceInfo ret;
    ret.score = 0;
    if(!m_faceDetector)
        return ret;
    //人脸位置检测
    SeetaFaceInfoArray infos = m_faceDetector->detect(image);
    if(infos.size == 0)
        return ret;
    //获取最大的人脸定位
    int index = 0;
    for (int i = 1; i < infos.size; i++)
    {
        if (infos.data[i].pos.width >
            infos.data[index].pos.width)
            index = i;
    }
    ret = infos.data[index];
    return ret;
}

void SeetaFace6::setDetectFaceMinSize(int minSize)
{
    if(!m_faceDetector)
        return;
    m_faceDetector->set(
        seeta::v6::FaceDetector::PROPERTY_MIN_FACE_SIZE, (double)minSize);
}

int SeetaFace6::getDetectFaceMinSize()
{
    if(!m_faceDetector)
        return 0;
    double minSize = m_faceDetector->get(
        seeta::v6::FaceDetector::PROPERTY_MIN_FACE_SIZE);
    return (int)minSize;
}

void SeetaFace6::setDetectFaceThreshold(float threshold)
{
    if(!m_faceDetector)
        return;
    m_faceDetector->set(
        seeta::v6::FaceDetector::PROPERTY_THRESHOLD, (double)threshold);
}

float SeetaFace6::getDetectFaceThreshold()
{
    if(!m_faceDetector)
        return 0;
    double threshold = m_faceDetector->get(
        seeta::v6::FaceDetector::PROPERTY_THRESHOLD);
    return (float)threshold;
}

void SeetaFace6::setDetectFaceMaxWidth(int width)
{
    if(!m_faceDetector)
        return;
    m_faceDetector->set(
        seeta::v6::FaceDetector::PROPERTY_MAX_IMAGE_WIDTH, (double)width);
}

void SeetaFace6::setDetectFaceMaxHeight(int height)
{
    if(!m_faceDetector)
        return;
    m_faceDetector->set(
        seeta::v6::FaceDetector::PROPERTY_MAX_IMAGE_HEIGHT, (double)height);
}

int SeetaFace6::getDetectFaceMaxWidth()
{
    if(!m_faceDetector)
        return 0;
    double maxWidth = m_faceDetector->get(
        seeta::v6::FaceDetector::PROPERTY_MAX_IMAGE_WIDTH);
    return (int)maxWidth;
}

int SeetaFace6::getDetectFaceMaxHeight()
{
    if(!m_faceDetector)
        return 0;
    double maxHeight = m_faceDetector->get(
        seeta::v6::FaceDetector::PROPERTY_MAX_IMAGE_HEIGHT);
    return (int)maxHeight;
}

std::vector<SeetaPointF> SeetaFace6::markFace(
    const SeetaImageData &image, const SeetaRect &face)
{
    std::vector<SeetaPointF> ret;
    if(!m_faceLandmarker)
        return ret;
    ret = m_faceLandmarker->mark(image, face);
    return ret;
}

int SeetaFace6::predictAge(const SeetaImageData &image,
    std::vector<SeetaPointF>& faceMark)
{
    if(!m_agePredictor)
        return 0;
    int ret = 0;
    if(!m_agePredictor->PredictAgeWithCrop(image, faceMark.data(), ret))
        return 0;
    return ret;
}

bool SeetaFace6::detectMask(const SeetaImageData &image, const SeetaRect &face, float *score)
{
    if(!m_maskDetector)
        return false;
    bool ret = m_maskDetector->detect(image, face, score);
    return ret;
}

SeetaFace6::FakeState SeetaFace6::predictFake(const SeetaImageData &image, const SeetaRect &face, std::vector<SeetaPointF> &faceMark)
{
    FakeState ret = FS_UNKNOW;
    if(!m_faceAntiSpoofing)
        return ret;
    seeta::FaceAntiSpoofing::Status state =
        m_faceAntiSpoofing->Predict(image, face, faceMark.data());
    switch(state)
    {
    case seeta::FaceAntiSpoofing::REAL:
        ret = FS_REAL;
        break;
    case seeta::FaceAntiSpoofing::SPOOF:
        ret = FS_FAKE;
        break;
    case seeta::FaceAntiSpoofing::FUZZY:
        ret = FS_FAILED;
        break;
    case seeta::FaceAntiSpoofing::DETECTING:
        ret = FS_DETECTING;
        break;
    }
    return ret;
}

SeetaFace6::FakeState SeetaFace6::predictFakeVideo(const SeetaImageData &image,
    const SeetaRect &face, std::vector<SeetaPointF> &faceMark, bool resetDetect)
{
    FakeState ret = FS_UNKNOW;
    if(!m_faceAntiSpoofing)
        return ret;
    if(resetDetect)
        m_faceAntiSpoofing->ResetVideo();
    seeta::FaceAntiSpoofing::Status state =
        m_faceAntiSpoofing->PredictVideo(image, face, faceMark.data());
    switch(state)
    {
    case seeta::FaceAntiSpoofing::REAL:
        ret = FS_REAL;
        break;
    case seeta::FaceAntiSpoofing::SPOOF:
        ret = FS_FAKE;
        break;
    case seeta::FaceAntiSpoofing::FUZZY:
        ret = FS_FAILED;
        break;
    case seeta::FaceAntiSpoofing::DETECTING:
        ret = FS_DETECTING;
        break;
    }
    return ret;
}

void SeetaFace6::setFakePredictVideoCount(int num)
{
    if(!m_faceAntiSpoofing)
        return;
    m_faceAntiSpoofing->SetVideoFrameCount((int32_t)num);
}

int SeetaFace6::getFakePredictVideoCount()
{
    if(!m_faceAntiSpoofing)
        return 0;
    return (int)m_faceAntiSpoofing->GetVideoFrameCount();
}

void SeetaFace6::setFakeThreshold(float clarity, float reality)
{
    if(!m_faceAntiSpoofing)
        return;
    m_faceAntiSpoofing->SetThreshold(clarity, reality);
}

void SeetaFace6::getFakeThreshold(float *clarity, float *reality)
{
    if(!m_faceAntiSpoofing)
        return;
    m_faceAntiSpoofing->GetThreshold(clarity, reality);
}

bool SeetaFace6::detectIsBlur(const SeetaImageData &image, std::vector<SeetaPointF> &faceMark68)
{
    if(!m_qualityOfLBN)
        return true;
    int light = 0, blur = 0, noise = 0;
    m_qualityOfLBN->Detect(image, faceMark68.data(), &light, &blur, &noise);
    return (blur == 1);
}

void SeetaFace6::setBlurThreshold(double blur)
{
    if(!m_qualityOfLBN)
        return;
    m_qualityOfLBN->set(seeta::QualityOfLBN::PROPERTY_BLUR_THRESH, blur);
}

double SeetaFace6::getBlurThreshold()
{
    if(!m_qualityOfLBN)
        return 0;
    return m_qualityOfLBN->get(seeta::QualityOfLBN::PROPERTY_BLUR_THRESH);
}

QVector3D SeetaFace6::estimateAngle(const SeetaImageData &image, const SeetaRect &face)
{
    QVector3D ret;
    if(!m_poseEstimator)
        return ret;
    float yaw = 0, pitch = 0, roll = 0;
    m_poseEstimator->Estimate(image, face, &yaw, &pitch, &roll);
    ret.setX(pitch);
    ret.setY(roll);
    ret.setZ(yaw);
    return ret;
}

SeetaFace6::SEyeState SeetaFace6::detectEyeState(const SeetaImageData &image, std::vector<SeetaPointF> &faceMark)
{
    SEyeState ret;
    if(!m_eyeStateDetector)
        return ret;
    seeta::EyeStateDetector::EYE_STATE left;
    seeta::EyeStateDetector::EYE_STATE right;
    m_eyeStateDetector->Detect(image, faceMark.data(), left, right);
    ret.left = (EyeState)left;
    ret.right = (EyeState)right;
    return ret;
}

SeetaFace6::Gender SeetaFace6::predictGender(const SeetaImageData &image, std::vector<SeetaPointF> &faceMark)
{
    Gender ret = GD_UNKNOW;
    if(!m_genderPredictor)
        return ret;
    seeta::GenderPredictor::GENDER gender;
    if(m_genderPredictor->PredictGenderWithCrop(image, faceMark.data(), gender))
        ret = (Gender)gender;
    return ret;
}

QVector<float> SeetaFace6::detectFaceFeature(const SeetaImageData &image, std::vector<SeetaPointF>& faceMark)
{
    QVector<float> features;
    if(!m_faceRecognizer)
        return features;
    //根据人脸关键点裁剪出人脸区域
    seeta::ImageData face = m_faceRecognizer->CropFaceV2(image, faceMark.data());
    //通过将人脸区域输入特征提取网络提取特征
    features.resize(m_faceRecognizer->GetExtractFeatureSize());
    m_faceRecognizer->ExtractCroppedFace(face, features.data());
    return features;
}

float SeetaFace6::matchFeature(QVector<float> feature1, QVector<float> feature2)
{
    if(!m_faceRecognizer)
        return 0;
    float score = m_faceRecognizer->CalculateSimilarity(
        feature1.constData(), feature2.constData());
    return score;
}
