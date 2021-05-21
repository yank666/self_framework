//
// Created by 赵永飞 on 2019/4/22.
//

#ifndef PERSON_DETECT_TRACKER_TEST_TRACKERLIB_H
#define PERSON_DETECT_TRACKER_TEST_TRACKERLIB_H
#include <memory>
#include <utility>
#include "TrackerCore.h"
#include "TrackerDescriptor.h"
#include "TrackerDistance.h"

///
/// \brief The TrackerParams struct stores parameters of PedestrianTracker
///
namespace iot_track {

    enum AppearanceFeatureType {
        NONE,
        INPUT,
        HISTOGRAM,
        PART_HISTOGRAM,
        FeatureTypeLast
    };
    struct TrackerLibParams {
        AppearanceFeatureType appearance_feature_type;

        bool use_hog_feature;  /// 是否使用hog 特征来判断遮挡状态。

        float hog_threshold;  /// hog 特征的阈值，当 use_hog_feature = true， 且使用外观特征时有效

        size_t min_track_duration;  ///< Min track duration in milliseconds.  最小跟踪持续时间

        size_t forget_delay;  ///< Forget about track if the last bounding box in
        /// track was detected more than specified number of frames ago.  如果检测到跟踪中的最后一个边界框超过指定的帧数，遗忘跟踪目标
        int predict;  ///< How many frames are used to predict bounding box in case of lost track. 在丢失目标的情况下，使用多少帧来预测边界框

        float min_det_conf;  ///< Min confidence of detection. 检测的最小置信度

        float time_affinity_w;  ///< Time affinity weight. 时间权重
        float shape_affinity_w;  ///< Shape affinity weight. 形状权重
        float motion_affinity_w;  ///< Motion affinity weight. 运动权重
        float simple_threshold;  /// 简化后的阈值

        int max_num_objects_in_track;  ///< The number of objects in track is
        bool drop_forgotten_tracks;  ///< Drop forgotten tracks. If it's enabled it

        cv::Vec2f bbox_aspect_ratios_range;  ///< Bounding box aspect ratios range.  边界框宽高比范围
        cv::Vec2f bbox_heights_range;  ///< Bounding box heights range.    边界框高度范围

        bool use_rectify;
        float rectify_length;

        ///
        /// Default constructor.
        ///
        TrackerLibParams();

        explicit TrackerLibParams(char *config);
    };

///
/// \brief The Track struct describes tracks.
///
    struct Tracking {
        ///
        /// \brief Track constructor.  //跟踪构造函数
        /// \param objs Detected objects sequence.
        /// \param last_image Image of last image in the detected object sequence.
        ///
        Tracking(const TrackedCoreObjects &objs, cv::Mat last_image)
                : objects(objs),
                  predicted_rect(!objs.empty() ? objs.back()->rect : cv::Rect()),
                  predicted_score(!objs.empty()? objs.back()->confidence: 0),
                  last_image(std::move(last_image)),
                  lost(0),
                  length(1) {
            assert(!objs.empty());
            first_object = objs[0];
        }


        ///
        /// \brief empty returns if track does not contain objects. 如果track不包含对象，则返回empty
        /// \return true if track does not contain objects.
        ///
        bool empty() const { return objects.empty(); }

        ///
        /// \brief size returns number of detected objects in a track.  size 返回跟踪中检测到的对象的数量
        /// \return number of detected objects in a track.
        ///
        size_t size() const { return objects.size(); }

        ///
        /// \brief operator [] return const reference to detected object with
        ///        specified index.
        /// \param i Index of object.
        /// \return const reference to detected object with specified index.
        ///
        const TrackedCoreObjectPtr &operator[](size_t i) const { return objects[i]; }

        ///
        /// \brief operator [] return non-const reference to detected object with
        ///        specified index.
        /// \param i Index of object.
        /// \return non-const reference to detected object with specified index.
        ///
        TrackedCoreObjectPtr &operator[](size_t i) { return objects[i]; }

        ///
        /// \brief back returns const reference to last object in track.    back将const引用的track中的最后一个对象返回
        /// \return const reference to last object in track.
        ///
        const TrackedCoreObjectPtr &back() const {
            assert(!empty());
            return objects.back();
        }

        ///
        /// \brief back returns non-const reference to last object in track. back将track中的最后一个对象返回
        /// \return non-const reference to last object in track.
        ///
        TrackedCoreObjectPtr &back() {
            assert(!empty());
            return objects.back();
        }

        cv::Mat appearance_feature_mean() {
            return cv::Mat(appearance_feature_sum_ / length);
        }

        TrackedCoreObjects objects;   ///< Detected objects; //已检测到的对象
        cv::Rect predicted_rect;  ///< Rectangle that represents predicted position  表示预测的对象矩形位置
        /// and size of bounding box if track has been lost 如果跟踪丢失，则为边界框的大小
        float predicted_score;
        cv::Mat last_image;       ///< Image of last detected object in track.  正在跟踪的上一帧检测到的对象对应的图像
//        cv::Mat descriptor_fast;  ///< Fast descriptor.   快速特征描述子
//        cv::Mat descriptor_strong;  ///< Strong descriptor (reid embedding). reid加持的强特征描述子
        size_t lost;                ///< How many frames ago track has been lost. 多少帧之前跟踪已丢失
        TrackedCoreObjectPtr first_object;  ///< First object in track. 跟踪的第一个对象
        size_t length;  ///< Length of a track including number of objects that were
        /// removed from track in order to avoid memory usage growth. 跟踪的对象数量，包括从已跟踪的对象中为避免内存使用增长移除的目标
        float width_p = 0, height_p = 0;
        float width_avg = 0, height_avg = 0;
        cv::Point2f d = cv::Point2f(0, 0);
        cv::Mat appearance_feature_sum_;
    };

    typedef std::shared_ptr<Tracking> TrackingPtr;

///
/// \brief Online pedestrian tracker algorithm implementation.
///
/// This class is implementation of pedestrian tracking system. It uses two
/// different appearance measures to compute affinity between bounding boxes:
/// some fast descriptor and some strong descriptor. Each time the assignment
/// problem is solved. The assignment problem in our case is how to establish
/// correspondence between existing tracklets and recently detected objects.
/// First step is to compute an affinity matrix between tracklets and
/// detections. The affinity equals to
///       appearance_affinity * motion_affinity * shape_affinity.
/// Where appearance is 1 - distance(tracklet_fast_dscr, detection_fast_dscr).
/// Second step is to solve the assignment problem using Kuhn-Munkres
/// algorithm. If correspondence between some tracklet and detection is
/// established with low confidence (affinity) then the strong descriptor is
/// used to determine if there is correspondence between tracklet and detection.
///
/*
 * 这个是具体是对象跟踪系统的实施策略。
 * 它使用两种不同的策略计算边界框之间的相似性：一些快速描述符和一些强描述符。
 * 每一次调用分配（跟踪）问题都会被解决。
 * 分配问题在我们的系统中具体是如何在现有的tracklet（已跟踪到的对象）和最近检测到的对象之间建立对应关系。
 * 第一步是计算tracklet（已跟踪到的对象）和检测到的对象的相似度矩阵。
 * 公式为：
 *      相似度affinity = appearance_affinity * motion_affinity * shape_affinity。
 *      appearance = 1  - distance（tracklet_fast_dscr，detection_fast_dscr）。
 *
 * 第二步是使用Kuhn-Munkres 匈牙利算法解决策略分配问题。  匈牙利算法是一种在多项式时间内求解任务分配问题的组合优化算法，并推动了后来的原始对偶方法
 *
 * 如果一些轨迹和检测之间的对应关系是比较低的置信度，那么我们使用强描述符来确定轨迹和检测之间是否存在对应关系。
 */

    class TrackerLib {
    public:
        using Descriptor = std::shared_ptr<TrackerImageDescriptor>;
        using Distance = std::shared_ptr<TrackerDistance>;

        ///
        /// \brief Constructor that creates an instance of the pedestrian tracker with
        /// parameters.  通过参数实体化跟踪类对象
        /// \param[in] params - the pedestrian tracker parameters.
        ///
        explicit TrackerLib(TrackerLibParams &params);

        virtual ~TrackerLib() = default;

        ///
        /// \brief Process given frame.
        /// \param[in] frame Colored image (CV_8UC3).
        /// \param[in] detections Detected objects on the frame.
        /// \param[in] timestamp Timestamp must be positive and measured in
        /// milliseconds
        ///  核心跟踪处理函数
        void Process(const cv::Mat &frame, TrackedCoreObjects &detections, uint64_t timestamp);

        ///
        /// \brief Pipeline parameters getter.
        /// \return Parameters of pipeline.
        ///  获取跟踪参数
        const TrackerLibParams &params() const;

        ///
        /// \brief Pipeline parameters setter.
        /// \param[in] params Parameters of pipeline.
        ///  设置跟踪参数
        void set_params(const TrackerLibParams &params);

        ///
        /// \brief Fast descriptor getter.
        /// \return Fast descriptor used in pipeline.
        ///  获取快速型特征描述子
        const Descriptor &descriptor() const;

        ///
        /// \brief Fast descriptor setter.
        /// \param[in] val Fast descriptor used in pipeline.
        /// 设置快速型特征描述子
        void set_descriptor(const Descriptor &val);

        void set_hog_descriptor(const Descriptor &val);

        ///
        /// \brief Fast distance getter.
        /// \return Fast distance used in pipeline.
        ///  获取快速型特征描述子的距离
        const Distance &distance() const;

        ///
        /// \brief Fast distance setter.
        /// \param[in] val Fast distance used in pipeline.
        ///  设置快速型特征描述子的距离
        void set_distance(const Distance &val);

        ///
        /// \brief Strong distance getter.
        /// \return Strong distance used in pipeline.
        ///  获取稳定型特征描述子的距离
//        const Distance &distance_strong() const;

        ///
        /// \brief Strong distance setter.
        /// \param[in] val Strong distance used in pipeline.
        ///  设置稳定型特征描述子的距离
//        void set_distance_strong(const Distance &val);

        ///
        /// \brief Returns number of counted people.
        /// \return a number of counted people.
        ///  获取已计数的对象数量
        size_t Count() const;

        ///
        /// \brief Returns a detection log which is used for tracks saving.
        /// \param[in] valid_only If it is true the method returns valid track only.
        /// \return a detection log which is used for tracks saving.
        ///

        //DetectionLog GetDetectionLog(const bool valid_only) const;

        ///
        /// \brief Get active tracks to draw
        /// \return Active tracks.
        ///  获取激活的正在跟踪的对象id，及对应中心点坐标
        std::unordered_map<size_t, std::vector<cv::Point> > GetActiveTracks() const;

        ///
        /// \brief Get tracked detections.
        /// \return Tracked detections.
        ///  获取已跟踪到的对象
        TrackedCoreObjects TrackedDetections() const;


        /// 获取丢失目标的预测框
        TrackedCoreObjects GetLostPrediction() const;

        ///
        /// \brief Draws active tracks on a given frame.
        /// \param[in] frame Colored image (CV_8UC3).
        /// \return Colored image with drawn active tracks.
        ///  跟踪结果demo效果演示
        cv::Mat DrawActiveTracks(const cv::Mat &frame);

        ///
        /// \brief IsTrackForgotten returns true if track is forgotten.
        /// \param id Track ID.
        /// \return true if track is forgotten.
        ///  如果id对应目标被遗忘了，将返回true 1
        bool IsTrackForgotten(size_t id) const;

        ///
        /// \brief tracks Returns all tracks including forgotten (lost too many frames
        /// ago).
        /// \return Set of tracks {id, track}.
        /// 返回所有跟踪的结果，包含被遗忘的跟踪对象结果
        const std::unordered_map<size_t, TrackingPtr> &tracks() const;

        ///
        /// \brief IsTrackValid Checks whether track is valid (duration > threshold).
        /// \param track_id Index of checked track.
        /// \return True if track duration exceeds some predefined value.
        /// 检查对应 id 跟踪是否有效（持续时间>阈值）
        bool IsTrackValid(size_t track_id) const;

        bool IsTracks_IdValid(size_t track_id) const;

        ///
        /// \brief DropForgottenTracks Removes tracks from memory that were lost too
        /// many frames ago.
        /// 从内存中删除多帧之前丢失的跟踪对象
        void DropForgottenTracks();

        ///
        /// \brief DropForgottenTracks Check that the track was lost too many frames
        /// ago
        /// and removes it frm memory.
        /// 检查id对应的track是否被丢失多帧，如果是，从内存中删除对应跟踪对象
        void DropForgottenTrack(size_t track_id);

        ///
        /// \brief Prints reid performance counter
        ///
        //void PrintReidPerformanceCounts() const;

        void Rectify();

        void MergeTrack(size_t id1, size_t id2);

    public:
        struct Match {
            int frame_idx1;
            int frame_idx2;
            cv::Rect rect1;
            cv::Rect rect2;
            cv::Rect pr_rect1;
            bool pr_label;
            bool gt_label;

            Match(const TrackedCoreObject &a, cv::Rect &a_pr_rect,
                  const TrackedCoreObject &b, bool pr_label)
                    : frame_idx1(a.frame_idx),
                      frame_idx2(b.frame_idx),
                      rect1(a.rect),
                      rect2(b.rect),
                      pr_rect1(std::move(a_pr_rect)),
                      pr_label(pr_label),
                      gt_label(a.object_id == b.object_id) {
                assert(frame_idx1 != frame_idx2);
            }
        };

        const ObjectCoreTracks all_tracks(bool valid_only) const;

        // Returns shape affinity.  得到形状的相似度
        static float ShapeAffinity(float w, const cv::Rect &trk, const cv::Rect &det);

        // Returns motion affinity. 得到运动的相似度
        static float MotionAffinity(float w, const cv::Rect &trk, const cv::Rect &det);

        // Returns time affinity.   得到时间的相似度
        static float TimeAffinity(float w, const float &trk, const float &det);

//        cv::Rect PredictRect(size_t id, size_t s) const;//预测跟踪位置
        cv::Rect PredictRect2(size_t id, size_t s) const;//预测跟踪位置
        void PredictRect3(size_t id) const;//预测跟踪位置

        cv::Rect PredictRectSmoothed(size_t id, size_t k, size_t s) const;//平滑预测跟踪位置

//    cv::Rect PredictRectSimple(size_t id, size_t k, size_t s) const;//简单预测跟踪位置

        //跟踪问题解决核心函数
        void SolveAssignmentProblem(
                const std::set<size_t> &track_ids,
                const TrackedCoreObjects &detections,
                std::set<size_t> *unmatched_tracks,
                std::set<size_t> *unmatched_detections,
                std::set<std::tuple<size_t, size_t, float>> *matches);


        std::vector<size_t> Solve_speed(cv::Mat &dissimilarity_matrix);

        void Solve_speed2(std::vector<std::tuple<size_t, size_t, float>> &dissimilarity_matrix);

        //计算快型速描述子
//        void ComputeFastDesciptors(const cv::Mat &frame,
//                                   const TrackedCoreObjects &detections,
//                                   std::vector<cv::Mat> *desriptors);

        //计算与其他目标的位置关系
        void ComputeDistancesWithOthers(TrackedCoreObjects &detections);

        //计算相似度矩阵
//    void ComputeDissimilarityMatrix(const std::set<size_t> &active_track_ids,
//                                    const TrackedCoreObjects &detections,
//                                    const std::vector<cv::Mat> &fast_descriptors,
//                                    cv::Mat *dissimilarity_matrix);

        void ComputeSimilarityMatrix(const std::set<size_t> &active_tracks,
                                     const TrackedCoreObjects &detections,
                                     std::vector<std::tuple<size_t, size_t, float>> &similarity);

        //相似度计算
        std::tuple<float, float, float> Affinity(const TrackedCoreObjectPtr &obj1,
                                                 const TrackedCoreObjectPtr &obj2) const;

        //增加新的跟踪对象
        void AddNewTrack(const cv::Mat &frame, const TrackedCoreObjectPtr &detection);

        //批量增加新的跟踪对象
        void AddNewTracks(const cv::Mat &frame, const TrackedCoreObjects &detections);

        //批量增加新的跟踪对象
        void AddNewTracks(const cv::Mat &frame, const TrackedCoreObjects &detections, const std::set<size_t> &ids);


        //跟踪对象被跟踪上，向对应对象列表中增加跟踪信息
        void AppendToTrack(const cv::Mat &frame, size_t track_id, const TrackedCoreObjectPtr &detection);

        //移除跟踪对象，如果对应box超出视频帧像素边界
        bool EraseTrackIfBBoxIsOutOfFrame(size_t track_id);

        //移除跟踪对象，如果检测丢失多帧以后
        bool EraseTrackIfItWasLostTooManyFramesAgo(size_t track_id);

        //更新丢失的跟踪对象，必要时删除
        bool UpdateLostTrackAndEraseIfItsNeeded(size_t track_id);

        //批量更新丢失的跟踪对象
        void UpdateLostTracks(const std::set<size_t> &track_ids);

        const std::set<size_t> &active_track_ids() const;//


        TrackedCoreObjects PreProcess(const TrackedCoreObjects &detections, const cv::Mat& frame,
                                      uint64_t timestamp) const;

        bool IsTrackForgotten(const TrackingPtr &track) const;


        // Parameters of the pipeline.
        TrackerLibParams params_;

        // Indexes of active tracks.
        std::set<size_t> active_track_ids_;//激活的跟踪目标id列表

        // Distance fast (base classifer).
        Distance distance_;

        // Descriptor fast (base classifer).
        Descriptor descriptor_;

        Descriptor hog_descriptor_;

        // All tracks.
        std::unordered_map<size_t, TrackingPtr> tracks_;//所有的跟踪链路

    public:

        // Previous frame image.
        cv::Size prev_frame_size_;

        struct pair_hash {
            std::size_t operator()(const std::pair<size_t, size_t> &p) const {
                assert(p.first < 1e6 && p.second < 1e6);
                return p.first * 1e6 + p.second;
            }
        };

        // Distance between current active tracks.  当前跟踪对象之间的距离
//        std::unordered_map<std::pair<size_t, size_t>, float, pair_hash> tracks_dists_;

        // Whether collect matches and compute confusion matrices for
        // track-detection
        // association task (base classifier, reid-based classifier,
        // reid-classiifer).
//        bool collect_matches_;//为跟踪检测关联任务收集匹配和计算混淆矩阵

        // This vector contains decisions made by
        // fast_apperance-motion-shape affinity model.
//        std::vector<Match> base_classifier_matches_;//此向量包含由fast_apperance-motion-shape相似度模型做出的决策

        // This vector contains decisions made by
        // strong_apperance(cnn-reid)-motion-shape affinity model.
//        std::vector<Match> reid_based_classifier_matches_;//此向量包含由strong_apperance(cnn-reid)-motion-shape相似度模型做出的决策

        // This vector contains decisions made by
        // strong_apperance(cnn-reid) affinity model only.
//        std::vector<Match> reid_classifier_matches_;//此向量包含由strong_apperance(cnn-reid)相似度模型做出的决策

        // Number of all current tracks.
        size_t tracks_counter_;//当前跟踪目标的数量

        // Number of dropped valid tracks.
        size_t valid_tracks_counter_;//已删除的有效跟踪个数

        cv::Size frame_size_;//视频帧大小

        std::vector<cv::Scalar> colors_;

        uint64_t prev_timestamp_;//时间戳

        void DrawPolyline(const std::vector<cv::Point> &polyline, const cv::Scalar &color, cv::Mat *image, int lwd = 5);

        void drawDashRect(cv::Mat &img, cv::Rect blob, int linelength = 10, int dashlength = 20,
                          cv::Scalar color = cv::Scalar(0, 255, 0), int thickness = 3);

    };

    std::tuple<size_t, cv::Point, cv::Point> GetClosestLocation(const TrackingPtr &track1, const TrackingPtr &track2);
}  // namespace iot_track

#endif //PERSON_DETECT_TRACKER_TEST_TRACKERLIB_H
