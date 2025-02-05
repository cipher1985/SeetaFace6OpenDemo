SOURCES += \
    $$PWD/seetaface6open.cpp

HEADERS += \
    $$PWD/seetaface6open.h

LIBS += -L$$PWD/lib/ -lSeetaAgePredictor600
LIBS += -L$$PWD/lib/ -lSeetaAuthorize
LIBS += -L$$PWD/lib/ -lSeetaEyeStateDetector200
LIBS += -L$$PWD/lib/ -lSeetaFaceAntiSpoofingX600
LIBS += -L$$PWD/lib/ -lSeetaFaceDetector600
LIBS += -L$$PWD/lib/ -lSeetaFaceLandmarker600
LIBS += -L$$PWD/lib/ -lSeetaFaceRecognizer610
LIBS += -L$$PWD/lib/ -lSeetaFaceTracking600
LIBS += -L$$PWD/lib/ -lSeetaGenderPredictor600
LIBS += -L$$PWD/lib/ -lSeetaMaskDetector200
LIBS += -L$$PWD/lib/ -lSeetaPoseEstimation600
LIBS += -L$$PWD/lib/ -lSeetaQualityAssessor300
LIBS += -L$$PWD/lib/ -ltennis
LIBS += -L$$PWD/lib/ -ltennis_haswell
LIBS += -L$$PWD/lib/ -ltennis_pentium
LIBS += -L$$PWD/lib/ -ltennis_sandy_bridge

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include
