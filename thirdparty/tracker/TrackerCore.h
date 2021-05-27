//
// Created by 赵永飞 on 2019/4/22.
//

#ifndef PERSON_DETECT_TRACKER_TEST_TRACKERCORE_H
#define PERSON_DETECT_TRACKER_TEST_TRACKERCORE_H
#include <opencv2/opencv.hpp>
#include <deque>
#include <memory>
#include <unordered_map>

namespace iot_track {
///
/// \brief The TrackedObject struct defines properties of detected object.
///
    struct TrackedCoreObject {
        cv::Rect rect;       ///< Detected object ROI (zero area if N/A).
        double confidence = 0.0;   ///< Detection confidence level (-1 if N/A).
        double detect_quality = 0.0; ///< Detection quality level (-1 if N/A).
        int frame_idx = 0;       ///< Frame index where object was detected (-1 if N/A).
        int object_id = 0;       ///< Unique object identifier (-1 if N/A).
        uint64_t timestamp = 0;  ///< Timestamp in milliseconds.
        int label = 0;
        float distances_with_others = -1.0;
        cv::Mat appearance_feature;
        cv::Mat hog_feature;

        TrackedCoreObject()
                : confidence(-1),
                  frame_idx(0),
                  object_id(-1),
                  timestamp(0),
                  distances_with_others(-1.0),
                  detect_quality(-1) {}
        ///
        /// \brief Constructor with parameters.
        /// \param rect Bounding box of detected object.
        /// \param confidence Confidence of detection.
        /// \param frame_idx Index of frame.
        /// \param object_id Object ID.
        ///
        TrackedCoreObject(cv::Rect &rect, float confidence, int frame_idx,
                          int object_id, float quality=-1.0)
                : rect(std::move(rect)),
                  confidence(confidence),
                  frame_idx(frame_idx),
                  object_id(object_id),
                  detect_quality(quality),
                  timestamp(0){}

        TrackedCoreObject(cv::Rect &rect, float confidence, int frame_idx,
                          int object_id, std::vector<float>& feature, float quality=-1.0)
                : rect(std::move(rect)),
                  confidence(confidence),
                  frame_idx(frame_idx),
                  object_id(object_id),
                  appearance_feature(cv::Mat(feature, true)),
                  detect_quality(quality),
                  timestamp(0){}

        TrackedCoreObject(cv::Rect &rect, float confidence, int frame_idx,
                          int object_id, cv::Mat& feature, float quality=-1.0)
                : rect(std::move(rect)),
                  confidence(confidence),
                  frame_idx(frame_idx),
                  object_id(object_id),
                  detect_quality(quality),
                  timestamp(0){
            appearance_feature = feature.clone();
        }

        TrackedCoreObject(cv::Rect &rect, float confidence, int frame_idx,
                          int object_id, cv::Mat& feature, uint64_t timestamp, float quality=-1.0)
                : rect(std::move(rect)),
                  confidence(confidence),
                  frame_idx(frame_idx),
                  object_id(object_id),
                  detect_quality(quality),
                  timestamp(timestamp){
            appearance_feature = feature.clone();
        }

        TrackedCoreObject(const TrackedCoreObject& other)
                : rect(other.rect),
                  confidence(other.confidence),
                  frame_idx(other.frame_idx),
                  object_id(other.object_id),
                  detect_quality(other.detect_quality),
                  timestamp(other.timestamp){
            appearance_feature = other.appearance_feature.clone();
            hog_feature = other.hog_feature.clone();
        }
    };

    using TrackedCoreObjectPtr = std::shared_ptr<TrackedCoreObject>;
    using TrackedCoreObjects = std::deque<TrackedCoreObjectPtr>;

    bool operator==(const TrackedCoreObject &first, const TrackedCoreObject &second);

    bool operator!=(const TrackedCoreObject &first, const TrackedCoreObject &second);
/// (object id, detected objects) pairs collection.
    using ObjectCoreTracks = std::unordered_map<int, TrackedCoreObjects>;

}  // namespace iot_track
#endif //PERSON_DETECT_TRACKER_TEST_TRACKERCORE_H
