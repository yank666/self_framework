//
// Created by 赵永飞 on 2019/4/25.
//

#ifndef PERSON_DETECT_TRACKER_TEST_TRACKERLIB_API_H
#define PERSON_DETECT_TRACKER_TEST_TRACKERLIB_API_H

#include "TrackerLib.h"

namespace iot_track {
    struct TrackedCoreObject_extend {
        TrackedCoreObject m_TrackedObject;
        cv::Mat m_frame;
        cv::Mat descriptors;
        int flag_segmentation;
    };

    class TrackerLib_api {
    public:
        TrackerLib *ptracker;

        std::vector<TrackedCoreObject_extend> m_TrackedObject_extends;

        cv::Mat showImg;

    public:
        TrackerLib_api();

        explicit TrackerLib_api(char *config_file);

        int frame_width;
        int frame_height;

        void init(TrackerLibParams &params);

        TrackedCoreObjects
        run(const cv::Mat &frame, const TrackedCoreObjects &input_detections, int frame_idx, bool Ifshow);

//        TrackedCoreObjects PredictedTracking();
        TrackedCoreObjects GetLostPrediction();

        void release();

        void ShowResult();
    };

}  // namespace iot_track
#endif //PERSON_DETECT_TRACKER_TEST_TRACKERLIB_API_H
