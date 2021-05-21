//
// Created by 赵永飞 on 2019/4/22.
//

#include <fstream>
#include "TrackerLib.h"
#include "json/json.hpp"

namespace iot_track {
    using json = nlohmann::json;

    cv::Point2f Center(const cv::Rect &rect) {
        return {rect.x + rect.width * .5f, rect.y + rect.height * .5f};
    }

    std::vector<cv::Point> Centers(const iot_track::TrackedCoreObjects &detections) {
        std::vector<cv::Point> centers(detections.size());
        for (size_t i = 0; i < detections.size(); i++) {
            centers[i] = Center(detections[i]->rect);
        }
        return centers;
    }

    inline bool IsInRange(float val, float min, float max) {
        return min <= val && val <= max;
    }

    inline bool IsInRange(float val, cv::Vec2f range) {
        return IsInRange(val, range[0], range[1]);
    }

    std::vector<cv::Scalar> GenRandomColors(int colors_num) {
        std::vector<cv::Scalar> colors(colors_num);

        for (int i = 0; i < colors_num; i++) {
            colors[i] = cv::Scalar(static_cast<uchar>(255. * rand() / RAND_MAX),  // NOLINT
                                   static_cast<uchar>(255. * rand() / RAND_MAX),  // NOLINT
                                   static_cast<uchar>(255. * rand() / RAND_MAX));  // NOLINT
        }
        return colors;
    }

    double IoU(const cv::Rect &rect1, const cv::Rect &rect2) {
        //计算两个矩形的交集
        cv::Rect rect3 = rect1 & rect2;
        double iou = rect3.area() * 1.0 / (rect1.area() + rect2.area() - rect3.area());
        return iou;
    }

    float CenterDistance(const cv::Rect &rect1, const cv::Rect &rect2) {
        cv::Point c1 = Center(rect1);
        cv::Point c2 = Center(rect2);
        return sqrt((c1.x - c2.x) * (c1.x - c2.x) + (c1.y - c2.y) * (c1.y - c2.y));
    }


    std::tuple<size_t, cv::Point, cv::Point> GetClosestLocation(const TrackingPtr &track1, const TrackingPtr &track2) {
        auto obj1 = track1->objects.begin();
        auto obj2 = track2->objects.begin();
        int min_frame = 10000;
        cv::Point dc = cv::Point(0, 0);
        cv::Point m = cv::Point(0, 0);
        while(obj1 != track1->objects.end() && obj2 != track2->objects.end()) {
            if (abs(obj1->get()->frame_idx - obj2->get()->frame_idx) < min_frame) {
                min_frame = abs(obj1->get()->frame_idx - obj2->get()->frame_idx);
                dc = Center(obj1->get()->rect) - Center(obj2->get()->rect);
                m = cv::Point(int((obj1->get()->rect.width + obj2->get()->rect.width) / 2),
                              int((obj1->get()->rect.height + obj2->get()->rect.height) / 2));
            }
            if(obj1->get()->frame_idx < obj2->get()->frame_idx) {
                ++obj1;
            }
            else if(obj1->get()->frame_idx > obj2->get()->frame_idx) {
                ++obj2;
            }
            else {
                return std::make_tuple(0, dc, m);

            }
        }
        return std::make_tuple(min_frame, dc, m);

    }

    TrackerLibParams::TrackerLibParams()
            : appearance_feature_type(HISTOGRAM),
              use_hog_feature(false),
              hog_threshold(0.7),
              min_track_duration(0),
              forget_delay(50),
              min_det_conf(0.4),
              bbox_aspect_ratios_range(0.666, 5.0),
              bbox_heights_range(40, 500),
              time_affinity_w(0.0),
              shape_affinity_w(0.5),
              motion_affinity_w(0.1),
              simple_threshold(0.83), // for jincheng head
              predict(25),
              max_num_objects_in_track(300),
              drop_forgotten_tracks(true),
              use_rectify(false),
              rectify_length(20) {
        std::cout << "Init config using default params" << std::endl;
    }

    TrackerLibParams::TrackerLibParams(char *config_path) {
        std::cout << "Init config from file: " << config_path << std::endl;
        std::ifstream fin(config_path);
        assert(fin);

        json params_json;
        fin >> params_json;

        appearance_feature_type = params_json["track_params"]["appearance_feature_type"] < int(FeatureTypeLast) ?
                AppearanceFeatureType(params_json["track_params"]["appearance_feature_type"]) : NONE;
        use_hog_feature = params_json["track_params"]["use_hog_feature"] == 0 ? false : true;
        hog_threshold = params_json["track_params"]["hog_threshold"];
        min_track_duration = params_json["track_params"]["min_track_duration"];
        forget_delay = params_json["track_params"]["forget_delay"];
        predict = params_json["track_params"]["predict"];
        min_det_conf = params_json["track_params"]["min_det_conf"];
        time_affinity_w = params_json["track_params"]["time_affinity_w"];
        shape_affinity_w = params_json["track_params"]["shape_affinity_w"];
        motion_affinity_w = params_json["track_params"]["motion_affinity_w"];
        simple_threshold = params_json["track_params"]["simple_threshold"];
        max_num_objects_in_track = params_json["track_params"]["max_num_objects_in_track"];
        drop_forgotten_tracks = params_json["track_params"]["drop_forgotten_tracks"] == 0 ? false : true;
        bbox_aspect_ratios_range = cv::Vec2f(params_json["track_params"]["bbox_aspect_ratios_range_min"],
                                             params_json["track_params"]["bbox_aspect_ratios_range_max"]);
        bbox_heights_range = cv::Vec2f(params_json["track_params"]["bbox_heights_range_min"],
                                       params_json["track_params"]["bbox_heights_range_max"]);
        use_rectify = params_json["track_params"]["use_rectify"] == 0 ? false : true;
        rectify_length = params_json["track_params"]["rectify_length"];
    }

    void ValidateParams(const TrackerLibParams &p) {
        assert(p.min_track_duration >= static_cast<size_t>(0));
        assert(p.min_track_duration <= static_cast<size_t>(250));

        assert(p.forget_delay <= static_cast<size_t>(10000));

        assert(p.shape_affinity_w >= 0.0f);
        assert(p.shape_affinity_w <= 100.0f);

        assert(p.motion_affinity_w >= 0.0f);
        assert(p.motion_affinity_w <= 100.0f);

        assert(p.time_affinity_w >= 0.0f);
        assert(p.time_affinity_w <= 100.0f);

        assert(p.min_det_conf >= 0.0f);
        assert(p.min_det_conf <= 1.0f);

        assert(p.bbox_aspect_ratios_range[0] >= 0.0f);
        assert(p.bbox_aspect_ratios_range[1] <= 100.0f);
        assert(p.bbox_aspect_ratios_range[0] < p.bbox_aspect_ratios_range[1]);

        assert(p.bbox_heights_range[0] >= 3.0f);
        assert(p.bbox_heights_range[1] <= 2048.0f);
        assert(p.bbox_heights_range[0] < p.bbox_heights_range[1]);

        assert(p.predict >= 0);
        assert(p.predict <= 10000);

        if (p.max_num_objects_in_track > 0) {
            int min_required_track_length = 10;
            assert(p.max_num_objects_in_track >= min_required_track_length);
            assert(p.max_num_objects_in_track <= 10000);
        }
    }

// Returns confusion matrix as:
//   |tp fn|
//   |fp tn|
// confusion matrix ，混淆矩阵又称为可能性表格或是错误矩阵
//    cv::Mat TrackerLib::ConfusionMatrix(const std::vector<Match> &matches) {
//        const bool kNegative = false;
//        cv::Mat conf_mat(2, 2, CV_32F, cv::Scalar(0));
//        for (const auto &m : matches) {
//            conf_mat.at<float>(m.gt_label == kNegative, m.pr_label == kNegative)++;
//        }
//
//        return conf_mat;
//    }

    TrackerLib::TrackerLib(TrackerLibParams &params)
            : params_(std::move(params)),
              tracks_counter_(1),
              valid_tracks_counter_(0),
              frame_size_(0, 0),
              prev_timestamp_(std::numeric_limits<uint64_t>::max()) {
        ValidateParams(params_);
    }

// Pipeline parameters getter.
    const TrackerLibParams &TrackerLib::params() const { return params_; }

// Pipeline parameters setter.
    void TrackerLib::set_params(const TrackerLibParams &params) {
        ValidateParams(params);
        params_ = params;
    }

// Descriptor fast getter.
    const TrackerLib::Descriptor &TrackerLib::descriptor() const {
        return descriptor_;
    }

// Descriptor fast setter.
    void TrackerLib::set_descriptor(const Descriptor &val) {
         descriptor_ = val;
    }

    void TrackerLib::set_hog_descriptor(const Descriptor &val) {
        hog_descriptor_ = val;
    }

// Distance fast getter.
    const TrackerLib::Distance &TrackerLib::distance() const {
        return distance_;
    }

// Distance fast setter.
    void TrackerLib::set_distance(const Distance &val) {
        distance_ = val;
    }

// Returns all tracks including forgotten (lost too many frames ago).
    const std::unordered_map<size_t, TrackingPtr> &TrackerLib::tracks() const {
        return tracks_;
    }

// Returns indexes of active tracks only.
    const std::set<size_t> &TrackerLib::active_track_ids() const {
        return active_track_ids_;
    }


//依据宽高比，检测置信度，高度范围限制条件过滤待跟踪目标
    TrackedCoreObjects TrackerLib::PreProcess(const TrackedCoreObjects &detections, const cv::Mat &frame,
                                              uint64_t timestamp) const {
        TrackedCoreObjects filtered_detections;
        for (const auto &det : detections) {
            float aspect_ratio = static_cast<float>(det->rect.height) / det->rect.width;

            if (det->confidence > params_.min_det_conf
                && IsInRange(aspect_ratio, params_.bbox_aspect_ratios_range)
                && IsInRange(det->rect.height, params_.bbox_heights_range)) {
                if(params_.appearance_feature_type != NONE && params_.appearance_feature_type != INPUT) {
                    descriptor_->Compute(frame(det->rect), det->appearance_feature);
                }
                if (params_.appearance_feature_type != NONE && params_.use_hog_feature) {
                    hog_descriptor_->Compute(frame(det->rect), det->hog_feature);
                }

                det->timestamp = timestamp;
                filtered_detections.emplace_back(det);
            }
        }
        return filtered_detections;
    }

//速度及跟踪物体大小估测滤波器
    cv::Rect TrackerLib::PredictRectSmoothed(size_t id, size_t k, size_t s) const//平滑预测跟踪位置
    {
        if (IsTracks_IdValid(id)) {
            const auto &track = tracks_.at(id);

            if (track->size() == 1) {
                return (*track)[0]->rect;
            }

            size_t start_i = track->size() > k ? track->size() - k : 0;

            float width = 0, height = 0;


            assert(track->size() - start_i > 0);


            float delim = 0;

            float width_p = 0;
            float height_p = 0;

            std::vector<float> dx_list;
            std::vector<float> dy_list;
            float sum = 0.0;
            for (size_t i = start_i + 1; i < track->size(); i++) {
                sum += static_cast<float>(i - start_i);
                auto _time = static_cast<float>((*track)[i]->frame_idx - (*track)[start_i]->frame_idx);

                float dx = Center((*track)[i]->rect).x - Center((*track)[start_i]->rect).x;
                dx_list.push_back(dx / _time);

                float dy = Center((*track)[i]->rect).y - Center((*track)[start_i]->rect).y;
                dy_list.push_back(dy / _time);

                width_p += float((*track)[i]->rect.width - (*track)[i - 1]->rect.width);
                height_p += float((*track)[i]->rect.height - (*track)[i - 1]->rect.height);
                delim += static_cast<float>((*track)[i]->frame_idx - (*track)[i - 1]->frame_idx);
            }

            float ave_dx = 0;
            float ave_dy = 0;


            for (int i = 0; i < dx_list.size(); ++i) {
                ave_dx += dx_list[i] * (i + 1.0f) / sum;
                ave_dy += dy_list[i] * (i + 1.0f) / sum;
            }

            if (fabs(ave_dx) < 0.01) {
                ave_dx = 0.0;
            }
            if (fabs(ave_dy) < 0.01) {
                ave_dy = 0.0;
            }

            if (delim > 0) {
                width_p /= delim;
                height_p /= delim;
            }

            s += 1;

            width = track->back()->rect.width;
            height = track->back()->rect.height;
            width = width + width_p * s;
            height = height + height_p * s;

            cv::Point c = Center(track->back()->rect);//对应跟踪目标的上一次检测出的位置
            return {static_cast<int>(c.x - width / 2 + ave_dx * s),
                    static_cast<int>(c.y - height / 2 + ave_dy * s),
                    static_cast<int>(width), static_cast<int>(height)};
        }

        return {0, 0, 0, 0};
    }

    cv::Rect TrackerLib::PredictRect2(size_t id, size_t s) const {
        int k = params().predict;
        if (IsTracks_IdValid(id)) {
            const auto &track = tracks_.at(id);
            assert(!track->empty());

            if (track->size() == 1) {
                return (*track)[0]->rect;
            }
            float width = 0, height = 0;
            float width_p = 0, height_p = 0;
            cv::Point2f d(0, 0);

            if (s == 1) {
                size_t start_i = track->size() > k ? track->size() - k : 0;
                assert(track->size() - start_i > 0);

                float delim = 0;

                for (size_t i = start_i + 1; i < track->size(); i++) {
                    d += cv::Point2f(Center((*track)[i]->rect) - Center((*track)[i - 1]->rect));
                    width_p += static_cast<float>((*track)[i]->rect.width - (*track)[i - 1]->rect.width);
                    height_p += static_cast<float>((*track)[i]->rect.height - (*track)[i - 1]->rect.height);
                    delim += static_cast<float>((*track)[i]->frame_idx - (*track)[i - 1]->frame_idx);
                }

                if (delim > 0) {
                    d /= delim;
                    width_p /= delim;
                    height_p /= delim;
                }
                track->d = d;
                track->width_p = width_p;
                track->height_p = height_p;
            } else {
                d = track->d;
                width_p = track->width_p;
                height_p = track->height_p;
            }

            s += 1;
            width = track->back()->rect.width;
            height = track->back()->rect.height;
            width = width + width_p * s;
            height = height + height_p * s;

            cv::Point c = Center(track->back()->rect); //对应跟踪目标的上一次检测出的位置
            int out_x = static_cast<int>(c.x - width / 2 + d.x * s);
            int out_y = static_cast<int>(c.y - height / 2 + d.y * s);
            return {out_x, out_y, static_cast<int>(width), static_cast<int>(height)};
        }
        return {0, 0, 0, 0};
    }

    void TrackerLib::PredictRect3(size_t id) const {
        int k = params().predict;
        auto cur_track = tracks_.at(id);
        cur_track->predicted_score -= 1.0 / params_.forget_delay;
        cur_track->predicted_score = std::max(0.0f, cur_track->predicted_score);
        if (IsTracks_IdValid(id)) {
            assert(!cur_track->empty());

            if (cur_track->size() == 1) {
                cur_track->predicted_rect = (*cur_track)[0]->rect;
                return;
            }
            float width = 0, height = 0;
            float width_avg = 0, height_avg = 0;
            cv::Point2f d(0, 0);

            if (cur_track->lost == 1) {
                size_t start_i = cur_track->size() > k ? cur_track->size() - k : 0;
                assert(cur_track->size() - start_i > 0);

                float delim = 0;
                width_avg = cur_track->back()->rect.width;
                height_avg = cur_track->back()->rect.height;

                for (size_t i = start_i + 1; i < cur_track->size(); i++) {
                    d += cv::Point2f(Center((*cur_track)[i]->rect) - Center((*cur_track)[i - 1]->rect));
                    width_avg += static_cast<float>((*cur_track)[i - 1]->rect.width);
                    height_avg += static_cast<float>((*cur_track)[i - 1]->rect.height);
                    delim += static_cast<float>((*cur_track)[i]->frame_idx - (*cur_track)[i - 1]->frame_idx);
                }

                if (delim > 0) {
                    d /= delim;
                    width_avg /= float(cur_track->size() - start_i);
                    height_avg /= float(cur_track->size() - start_i);
                }
                cur_track->d = d;
                cur_track->width_avg = width_avg;
                cur_track->height_avg = height_avg;
            } else {
                d = cur_track->d;
                width_avg = cur_track->width_avg;
                height_avg = cur_track->height_avg;
            }

            width = width_avg;
            height = height_avg;

            cv::Point c = Center(cur_track->back()->rect); //对应跟踪目标的上一次检测出的位置
            int out_x = static_cast<int>(c.x - width / 2 + d.x * float(cur_track->lost + 1));
            int out_y = static_cast<int>(c.y - height / 2 + d.y * float(cur_track->lost + 1));
            tracks_.at(id)->predicted_rect = cv::Rect(out_x, out_y, static_cast<int>(width), static_cast<int>(height));
            return;
        }
        tracks_.at(id)->predicted_rect = cv::Rect(0, 0, 0, 0);
    }

    bool TrackerLib::EraseTrackIfBBoxIsOutOfFrame(size_t track_id) {
        if (IsTracks_IdValid(track_id)) {
            if (tracks_.find(track_id) == tracks_.end()) {
                return true;
            }
            if (tracks_.at(track_id)->predicted_rect.width <= 0 || tracks_.at(track_id)->predicted_rect.height <= 0) {
                tracks_.at(track_id)->lost = params_.forget_delay + 1;
//                for (auto id : active_track_ids()) {
//                    size_t min_id = std::min(id, track_id);
//                    size_t max_id = std::max(id, track_id);
//                    tracks_dists_.erase(std::pair<size_t, size_t>(min_id, max_id));
//                }
                active_track_ids_.erase(track_id);
                tracks_.erase(track_id);
                return true;
            }

//            auto c = Center(tracks_.at(track_id)->predicted_rect);
            int xmin = tracks_.at(track_id)->predicted_rect.x;
            int ymin = tracks_.at(track_id)->predicted_rect.y;
            int xmax = xmin + tracks_.at(track_id)->predicted_rect.width;
            int ymax = ymin + tracks_.at(track_id)->predicted_rect.height;

            bool flag = false;
            if (xmin < 0) {
                tracks_.at(track_id)->predicted_rect.x = 0;
                flag = true;
            }
            if (ymin < 0) {
                tracks_.at(track_id)->predicted_rect.y = 0;
                flag = true;
            }
            if (xmax > prev_frame_size_.width - 1) {
                tracks_.at(track_id)->predicted_rect.width = prev_frame_size_.width - 1 - xmin;
                flag = true;
            }
            if (ymax > prev_frame_size_.height - 1) {
                tracks_.at(track_id)->predicted_rect.height = prev_frame_size_.height - 1 - ymin;
                flag = true;
            }
            if (tracks_.at(track_id)->lost > params_.forget_delay / 3 && flag == true) {
                tracks_.at(track_id)->lost = params_.forget_delay + 1;
//                for (auto id : active_track_ids()) {
//                    size_t min_id = std::min(id, track_id);
//                    size_t max_id = std::max(id, track_id);
//                    tracks_dists_.erase(std::pair<size_t, size_t>(min_id, max_id));
//                }
                active_track_ids_.erase(track_id);
                tracks_.erase(track_id);
                return true;
            }
        }

        return false;
    }

    bool TrackerLib::EraseTrackIfItWasLostTooManyFramesAgo(size_t track_id) {
        if (IsTracks_IdValid(track_id)) {
            if (tracks_.find(track_id) == tracks_.end()) {
                return true;
            }
            auto cur_track = tracks_.at(track_id);
            if (cur_track->lost > params_.forget_delay || (cur_track->length < 3 && cur_track->lost > params_.forget_delay / 3)) {
//                for (auto id : active_track_ids()) {
//                    size_t min_id = std::min(id, track_id);
//                    size_t max_id = std::max(id, track_id);
//                    tracks_dists_.erase(std::pair<size_t, size_t>(min_id, max_id));
//                }
                active_track_ids_.erase(track_id);
                tracks_.erase(track_id);
                return true;
            }
        }

        return false;
    }

    bool TrackerLib::UpdateLostTrackAndEraseIfItsNeeded(size_t track_id) {
        if (IsTracks_IdValid(track_id)) {
            tracks_.at(track_id)->lost++;
//        tracks_.at(track_id)->predicted_rect = PredictRect(track_id, tracks_.at(track_id)->lost);
            PredictRect3(track_id);
            bool erased = EraseTrackIfBBoxIsOutOfFrame(track_id);
            if (!erased) erased = EraseTrackIfItWasLostTooManyFramesAgo(track_id);
            return erased;
        }
        return false;

    }

    void TrackerLib::UpdateLostTracks(const std::set<size_t> &track_ids) {
        for (auto track_id : track_ids) {
            UpdateLostTrackAndEraseIfItsNeeded(track_id);
        }
    }

    void TrackerLib::Process(const cv::Mat &frame, TrackedCoreObjects &input_detections, uint64_t timestamp) {
        if (timestamp > 0 && prev_timestamp_ < std::numeric_limits<uint64_t>::max()) {
            assert(prev_timestamp_ < timestamp);
        }

        if (frame_size_ == cv::Size(0, 0)) {
            frame_size_ = frame.size();
        } else {
            assert(frame_size_ == frame.size());
        }

        TrackedCoreObjects detections = PreProcess(input_detections, frame, timestamp);

//        ComputeDistancesWithOthers(detections);

        auto active_tracks = active_track_ids_;
        if (!active_tracks.empty() && !detections.empty()) {
            std::set<size_t> unmatched_tracks, unmatched_detections;
            std::set<std::tuple<size_t, size_t, float>> matches;

            unmatched_tracks.insert(-1);
            unmatched_detections.insert(-1);
            matches.emplace(-1, -1, -1.0);

            SolveAssignmentProblem(active_tracks,
                                   detections,
                                   &unmatched_tracks,
                                   &unmatched_detections,
                                   &matches);//匈牙利算法最优跟踪求解

//        clock_t end2 = clock();
//        time = 1.0 * (end2 - end) / CLOCKS_PER_SEC;
//        std::cout << "SolveAssignmentProblem cost " << time * 1000 << " ms" << std::endl;

            for (const auto &match : matches) {
                size_t track_id = std::get<0>(match);
                size_t det_id = std::get<1>(match);
                float conf = std::get<2>(match);

                if (IsTracks_IdValid(track_id)) {

                    AppendToTrack(frame,
                                  track_id,
                                  detections[det_id]);
                }
            }

            AddNewTracks(frame, detections, unmatched_detections);

            UpdateLostTracks(unmatched_tracks);
            for (size_t id : active_tracks) {
                EraseTrackIfBBoxIsOutOfFrame(id);
            }
        } else {
            AddNewTracks(frame, detections);
            UpdateLostTracks(active_tracks);
        }

        prev_frame_size_ = frame.size();

        if (params_.drop_forgotten_tracks) DropForgottenTracks();

        if (params_.appearance_feature_type != NONE && params_.use_rectify) {
            Rectify();
        }
        prev_timestamp_ = timestamp;
    }

    void TrackerLib::Rectify() {

        std::set<size_t> new_ids;
        std::set<size_t> lost_ids;
        for (const auto &id : active_track_ids_) {
            auto cur_track = tracks_.at(id);
            if (cur_track->lost == 0) {
                if (cur_track->length == params_.rectify_length ) {
                    new_ids.emplace(id);
                }
            }
            else {
                if (IsTrackValid(id)) {
                    lost_ids.emplace(id);
                }
            }
        }
        if (!new_ids.empty() && !lost_ids.empty()) {
            std::vector<std::tuple<size_t, size_t, float>> merge_list;
            for(const auto &new_id : new_ids) {
                auto new_track = tracks_.at(new_id);
                for(const auto &lost_id : lost_ids) {
                    auto lost_track = tracks_.at(lost_id);
                    auto info = GetClosestLocation(new_track, lost_track);
                    int min_frame = std::get<0>(info);
                    if (min_frame == 0) {
                        continue;
                    }
                    else {
                        cv::Point dc = std::get<1>(info);
                        cv::Point m = std::get<2>(info);
                        if (fabs(dc.x) / (1.0f * std::min(min_frame, 3)) > m.x / 2.0f ||
                            fabs(dc.y) / (1.0f * std::min(min_frame, 3)) > m.y / 2.0f) {
                            continue;
                        }
                        else {
                            float aff = 1 - distance_->Compute(new_track->appearance_feature_mean(),
                                                               lost_track->appearance_feature_mean());
                            float aff2 = 1 - distance_->Compute(new_track->objects.back()->appearance_feature,
                                                               lost_track->appearance_feature_mean());
                            float score = 0.7f * aff2 + 0.3f * aff;
                            float s = 1.0;
                            if(fabs(dc.x) < 0.15f * m.x && fabs(dc.y) < 0.15f * m.y) {
                                s = 1.125;
                            }
                            score *= s;
                            if (score > params_.simple_threshold * 1.13) {
                                merge_list.emplace_back(std::tuple<size_t, size_t, float>(lost_id, new_id, aff));
                            }
                        }
                    }
                }
            }
            Solve_speed2(merge_list);

            std::set<size_t> merged_lost_list, merged_new_list;

            for (auto &merge : merge_list) {
                size_t lost_id = std::get<0>(merge);
                size_t new_id = std::get<1>(merge);
                if (merged_lost_list.find(lost_id) == merged_lost_list.end() &&
                    merged_new_list.find(new_id) == merged_new_list.end()) {
                    MergeTrack(lost_id, new_id);
                    std::cout << "Merge " << lost_id << " and " << new_id << std::endl;
                    merged_lost_list.emplace(lost_id);
                    merged_new_list.emplace(new_id);
                }
            }

        }
    }

    void TrackerLib::MergeTrack(size_t id1, size_t id2) {
        auto track1 = tracks_.at(id1);
        auto track2 = tracks_.at(id2);
        track1->lost = 0;
        track1->length += track2->length;
        track1->appearance_feature_sum_ += track2->appearance_feature_sum_;
        track1->predicted_rect = track2->predicted_rect;
        track1->last_image = track2->last_image;
        if (track1->objects.front().get()->frame_idx > track2->objects.front().get()->frame_idx) {
            track1->first_object = track2->first_object;
        }
        TrackedCoreObjects tmp;
        auto obj1 = track1->objects.begin();
        auto obj2 = track2->objects.begin();
        int track_id = obj1->get()->object_id;
        while(obj1 != track1->objects.end() && obj2 != track2->objects.end()) {
            if(obj1->get()->frame_idx < obj2->get()->frame_idx) {
                tmp.emplace_back(*obj1);
                ++obj1;
            }
            else if(obj1->get()->frame_idx > obj2->get()->frame_idx) {
                obj2->get()->object_id = track_id;
                tmp.emplace_back(*obj2);
                ++obj2;
            }
            if (params_.max_num_objects_in_track > 0) {
                while (tmp.size() > static_cast<size_t>(params_.max_num_objects_in_track)) {
                    tmp.erase(tmp.begin());
                }
            }
        }
        while(obj1 != track1->objects.end()) {
            tmp.emplace_back(*obj1);
            ++obj1;
            if (params_.max_num_objects_in_track > 0) {
                while (tmp.size() > static_cast<size_t>(params_.max_num_objects_in_track)) {
                    tmp.erase(tmp.begin());
                }
            }
        }
        while(obj2 != track2->objects.end()) {
            obj2->get()->object_id = track_id;
            tmp.push_back(*obj2);
            ++obj2;
            if (params_.max_num_objects_in_track > 0) {
                while (tmp.size() > static_cast<size_t>(params_.max_num_objects_in_track)) {
                    tmp.erase(tmp.begin());
                }
            }
        }
        track1->objects = tmp;
        active_track_ids_.erase(id2);
        tracks_.erase(id2);
    }


    void TrackerLib::DropForgottenTracks() {
        auto pair = tracks_.begin();
        while (pair != tracks_.end()) {
            if (IsTrackForgotten(pair->first)) {
                active_track_ids_.erase(pair->first);
                pair = tracks_.erase(pair);
            }
            else {
                ++pair;
            }
        }
    }

    void TrackerLib::DropForgottenTrack(size_t track_id) {
        assert(IsTrackForgotten(track_id));
        assert(active_track_ids_.count(track_id) == 0);
        active_track_ids_.erase(track_id);
        tracks_.erase(track_id);
    }

    float TrackerLib::ShapeAffinity(float weight, const cv::Rect &trk, const cv::Rect &det) {
        auto w_dist = static_cast<float>(std::fabs(trk.width - det.width) / (trk.width + det.width));
        auto h_dist = static_cast<float>(std::fabs(trk.height - det.height) / (trk.height + det.height));

        return static_cast<float>(exp(-weight * (w_dist + h_dist)));
    }

//    float TrackerLib::MotionAffinity(float weight, const cv::Rect &trk, const cv::Rect &det) {
//        auto x_dist = static_cast<float>(std::fabs((trk.x + trk.width / 2.0f - det.width / 2.0f - det.x)) / (det.width + trk.width));
//        auto y_dist = static_cast<float>(std::fabs((trk.y + trk.height / 2.0f - det.height / 2.0f - det.y)) / (det.height + trk.height));
//
//        cv::Rect rect = trk;
//
//        cv::Rect rect1 = det;
//
//        double iou = IoU(rect, rect1);
//        double IOU_back = 1.0 - iou;
//
//        return static_cast<float>(exp(-weight * ((x_dist + y_dist) * IOU_back)));
//    }

    float TrackerLib::MotionAffinity(float weight, const cv::Rect &trk, const cv::Rect &det) {
        auto x_dist = static_cast<float>(std::fabs(trk.x + trk.width / 2.0f - det.width / 2.0f - det.x) /
                float(det.width + trk.width));
        auto y_dist = static_cast<float>(std::fabs(trk.y + trk.height / 2.0f - det.height / 2.0f - det.y) /
                float(det.height + trk.height));

        cv::Rect rect = trk;

        cv::Rect rect1 = det;

        double iou = IoU(rect, rect1);
        double IOU_back = 1.0 - iou;

        return static_cast<float>(exp(-weight * (x_dist + y_dist) * IOU_back));
    }

    float TrackerLib::TimeAffinity(float weight, const float &trk_time, const float &det_time) {
        return static_cast<float>(exp(-weight * std::fabs(trk_time - det_time)));
    }


    void TrackerLib::ComputeDistancesWithOthers(TrackedCoreObjects &detections) {
        for (size_t i = 0; i < detections.size(); ++i) {
            auto det = detections[i];
            cv::Rect rect = det->rect;
            float min_dist = -1;
            for (size_t j = 0; j < detections.size(); ++j) {
                if (j == i) continue;
                cv::Rect rect2 = detections[j]->rect;
                if (IoU(rect, rect2) > 0) {
                    min_dist = -1;
                    break;
                }
                float distance = CenterDistance(rect, rect2) / rect.width;
                if (min_dist == -1 || distance < min_dist) {
                    min_dist = distance;
                }
            }
            detections[i]->distances_with_others = min_dist;
        }

    }


    void TrackerLib::ComputeSimilarityMatrix(
            const std::set<size_t> &active_tracks,
            const TrackedCoreObjects &detections,
            std::vector<std::tuple<size_t, size_t, float>> &similarity) {
        size_t i = 0;
        for (auto id : active_tracks) {
            auto cur_track = tracks_.at(id);
            auto last_det =  cur_track->objects.back();
            TrackedCoreObjectPtr last_obj = std::make_shared<TrackedCoreObject>(*last_det);
            last_obj->rect =  cur_track->predicted_rect;

            for (size_t j = 0; j < detections.size(); j++) {

                auto score = Affinity(last_obj, detections[j]);
                float shp_aff, mot_aff, time_aff;
                shp_aff = std::get<0>(score);
                mot_aff = std::get<1>(score);
                time_aff = std::get<2>(score);

                if (cur_track->lost < 5 &&
                ((last_obj->rect.x < frame_size_.width * 0.01 && detections[j]->rect.x < frame_size_.width * 0.01) ||
                    (last_obj->rect.x + last_obj->rect.width > frame_size_.width * 0.99 &&
                     detections[j]->rect.x + detections[j]->rect.width > frame_size_.width * 0.99) ||
                    (last_obj->rect.y < frame_size_.height * 0.01 &&
                     detections[j]->rect.y < frame_size_.height * 0.01) ||
                    (last_obj->rect.y + last_obj->rect.height > frame_size_.height * 0.99 &&
                     detections[j]->rect.y + detections[j]->rect.height > frame_size_.height * 0.99))) {
                    shp_aff = 1;
                }

                float sim = shp_aff * mot_aff * time_aff;

                if(0.54 * sim + 0.4 < params_.simple_threshold) {
                    continue;
                }

                float app_aff = 1.0f;
                if (params_.appearance_feature_type != NONE) {
                    app_aff = 1.0f - distance_->Compute(last_obj->appearance_feature, detections[j]->appearance_feature);

                    if (params_.use_hog_feature && detections[j]->frame_idx - last_obj->frame_idx <= 2) {
                        cv::Mat hog_diff = detections[j]->hog_feature - last_obj->hog_feature;
                        double hog_score = hog_diff.dot(hog_diff);
                        if (hog_score > params_.hog_threshold) {
                            app_aff = 1.0f - distance_->Compute(cur_track->appearance_feature_mean(),
                                                                detections[j]->appearance_feature);
                        }
                    }
                }
                sim *= app_aff;

                if (mot_aff > 0.9998) {
                    sim *= 1.08;
                    if (shp_aff > 0.998) {
                        sim *= 1.08;
                    }
                } else if (mot_aff > 0.998) {
                    sim *= 1.05;
                    if (shp_aff > 0.998) {
                        sim *= 1.05;
                    }
                }
                if (1.0 - sim <= 1e-6) {
                    sim = 1.0;
                }
                sim = sim * 0.5 + 0.4; //相似度整体提高0.4的台阶，不影响匹配结果，有效避免算法KM最优匹配时陷入死循环
                if (sim > params_.simple_threshold) {
                    similarity.emplace_back(std::tuple<int, int, float>(id, j, sim));
                }
            }
            i++;
        }
    }


    void TrackerLib::AddNewTracks(
            const cv::Mat &frame,
            const TrackedCoreObjects &detections) {
        for(auto & iter : detections) {
            AddNewTrack(frame, iter);
        }
    }

    void TrackerLib::AddNewTracks(
            const cv::Mat &frame,
            const TrackedCoreObjects &detections,
            const std::set<size_t> &ids) {
        for (size_t i : ids) {
            assert(i < detections.size());
            AddNewTrack(frame, detections[i]);
        }
    }

    void TrackerLib::AddNewTrack(
            const cv::Mat &frame,
            const TrackedCoreObjectPtr &detection) {
        TrackedCoreObjectPtr detection_with_id = std::make_shared<TrackedCoreObject>(*detection);
        detection_with_id->object_id = tracks_counter_;
        TrackingPtr track_ptr = std::make_shared<Tracking>(Tracking({detection_with_id},
                                                                    frame(detection->rect).clone()));
        tracks_.emplace(std::pair<size_t, TrackingPtr>(tracks_counter_, track_ptr));

//        for (size_t id : active_track_ids_) {
//            tracks_dists_.emplace(std::pair<size_t, size_t>(id, tracks_counter_), std::numeric_limits<float>::max());
//        }
        track_ptr->appearance_feature_sum_ = detection_with_id->appearance_feature;
        track_ptr->length = 1;
        active_track_ids_.insert(tracks_counter_);
        tracks_counter_++;
        // 上限为 int 型的最大值，防止 object_id 溢出
        if (tracks_counter_ >= std::numeric_limits<int>::max()) {
            tracks_counter_ = 1;
        }
    }

    void TrackerLib::AppendToTrack(const cv::Mat &frame,
                                   size_t track_id,
                                   const TrackedCoreObjectPtr &detection) {

        if (IsTracks_IdValid(track_id)) {
            assert(!IsTrackForgotten(track_id));

            auto detection_with_id = std::make_shared<TrackedCoreObject>(*detection);
            auto &cur_track = tracks_.at(track_id);

            auto last_det = cur_track->objects.back();

            float max_length = std::max(detection->rect.height, detection->rect.width);
            float min_length = std::min(detection->rect.height, detection->rect.width);
//        float dx=(Center(last_det.rect).x-Center(detection.rect).x);
//        float dy=(Center(last_det.rect).y-Center(detection.rect).y);
//        float dist=sqrt(dx*dx+dy*dy);
            float dist = CenterDistance(last_det->rect, detection->rect);

            int flag = -1; //跟踪对象建立条件必须首先满足持续稳定跟上目标 params_.min_track_duration/2.0 帧数
            if (cur_track->size() < params_.min_track_duration / 2.0) {
                if (dist > min_length * 1.0) {
                    //删除track,去除噪声轨迹
                    active_track_ids_.erase(track_id);
                    tracks_.erase(track_id);
                    return;
                }
            }
//        if(dist < max_length * 2 && flag==-1)//防止距离过远，id 跳变
            if (1)//防止距离过远，id 跳变
            {
                detection_with_id->object_id = track_id;
                cur_track->objects.emplace_back(detection_with_id);

                cur_track->predicted_rect = detection->rect;
                cur_track->predicted_score = detection->confidence;
                cur_track->lost = 0;
                cur_track->last_image = frame(detection->rect).clone();
                cur_track->length++;
                if (params_.appearance_feature_type != NONE) {
                    cur_track->appearance_feature_sum_ += detection_with_id->appearance_feature;
                }
                if (params_.max_num_objects_in_track > 0) {
                    while (cur_track->size() > static_cast<size_t>(params_.max_num_objects_in_track)) {
                        cur_track->objects.erase(cur_track->objects.begin());
                    }
                }
            }
        }
    }

    std::tuple<float, float, float> TrackerLib::Affinity(const TrackedCoreObjectPtr &obj1,
                                                         const TrackedCoreObjectPtr &obj2) const {

        std::tuple<float, float, float> res(0, 0, 0);
//    clock_t start = clock();
        float mot_aff = MotionAffinity(params_.motion_affinity_w, obj1->rect, obj2->rect);

//    clock_t end = clock();
//    double time = 1.0 * (end - start) / CLOCKS_PER_SEC;
//    std::cout << "MotionAffinity cost " << time * 1000 << "ms" << std::endl;
        if (mot_aff < 2 * (params_.simple_threshold - 0.4)) {
            return res;
        }

        const float eps = 1e-6;
        float shp_aff = ShapeAffinity(params_.shape_affinity_w, obj1->rect, obj2->rect);
//    std::cout << "shp_aff = " << shp_aff << std::endl;
        if (shp_aff < 1.86 * (params_.simple_threshold - 0.4)) {
            return res;
        }
//    clock_t end1 = clock();
//    time = 1.0 * (end1 - end) / CLOCKS_PER_SEC;
//    std::cout << "ShapeAffinity cost " << time * 1000 << "ms" << std::endl;

//    float time_aff = TimeAffinity(params_.time_affinity_w, obj1->frame_idx, obj2->frame_idx);
        float time_aff = 1.0f;


//    clock_t end3 = clock();
//    time = 1.0 * (end3 - end1) / CLOCKS_PER_SEC;
//    std::cout << "app_aff cost " << time * 1000 << "ms" << std::endl;

//    float res = shp_aff * mot_aff * time_aff * app_aff;

        res = std::tuple<float, float, float>(shp_aff, mot_aff, time_aff);
        return res;
    }

    bool TrackerLib::IsTrackValid(size_t id) const {
        const auto &track = tracks_.at(id);
        const auto &objects = track->objects;

        if (objects.empty()) {
            return false;
        }

//        int64_t duration_ms = objects.back()->frame_idx - track->first_object->frame_idx;
        int64_t duration_ms = track->length;
        //zyf
        if (duration_ms <= static_cast<int64_t>(params_.min_track_duration)) //params_.min_track_duration
        {
            return false;
        }
        return true;
    }

    bool TrackerLib::IsTrackForgotten(size_t id) const {
        return IsTrackForgotten(tracks_.at(id));
    }

    bool TrackerLib::IsTrackForgotten(const TrackingPtr &track) const {
        return (track->lost > params_.forget_delay);
    }

    size_t TrackerLib::Count() const {
        size_t count = valid_tracks_counter_;

        for (const auto &pair : tracks_) {
            count += (IsTrackValid(pair.first) ? 1 : 0);
        }
        return count;
    }

    std::unordered_map<size_t, std::vector<cv::Point>> TrackerLib::GetActiveTracks() const {
        std::unordered_map<size_t, std::vector<cv::Point>> active_tracks;
        for (size_t idx : active_track_ids()) {
            auto track = tracks().at(idx);

            if (IsTrackValid(idx) && !IsTrackForgotten(idx)) {
                active_tracks.emplace(idx, Centers(track->objects));
            }
        }
        return active_tracks;
    }

    bool TrackerLib::IsTracks_IdValid(size_t track_id) const {
        bool flag = false;
        for (auto &track : tracks_) {
            if (track.first == track_id) {
                flag = true;
            }
        }
        if (!flag) {
            //std::cout << "unmatched_track_id  = " << track_id << "  out of tracks_!!!" << std::endl;
        }
        return flag;

    }

    TrackedCoreObjects TrackerLib::TrackedDetections() const {
        TrackedCoreObjects detections;
        for (size_t idx : active_track_ids()) {
            auto track = tracks().at(idx);
            if (IsTrackValid(idx) && !track->lost) {
                detections.emplace_back(track->objects.back());
            }
        }
        return detections;
    }

    TrackedCoreObjects TrackerLib::GetLostPrediction() const {
        TrackedCoreObjects lost_prediction;
        for (size_t idx : active_track_ids()) {
            auto track = tracks().at(idx);
            if (IsTrackValid(idx) && track->lost > 0) {
                TrackedCoreObjectPtr tmp = std::make_shared<TrackedCoreObject>();
                tmp->rect = cv::Rect(track->predicted_rect);
                tmp->confidence = track->predicted_score;
                tmp->object_id = idx;
                tmp->label = track->first_object->label;
                lost_prediction.emplace_back(tmp);
            }
        }
        return lost_prediction;
    }

    void
    TrackerLib::DrawPolyline(const std::vector<cv::Point> &polyline, const cv::Scalar &color, cv::Mat *image, int lwd) {
        assert(image);
        assert(!image->empty());
        assert(image->type() == CV_8UC3);
        assert(lwd >= 0);
        assert(lwd < 20);

        for (size_t i = 1; i < polyline.size(); i++) {
            cv::line(*image, polyline[i - 1], polyline[i], color, lwd);
        }
    }

    cv::Mat TrackerLib::DrawActiveTracks(const cv::Mat &frame) {
        cv::Mat out_frame = frame.clone();

        if (colors_.empty()) {
            int num_colors = 100;
            colors_ = GenRandomColors(num_colors);
        }

        auto active_tracks = GetActiveTracks();
        for (auto active_track : active_tracks) {
            size_t idx = active_track.first;
            auto centers = active_track.second;
            DrawPolyline(centers, colors_[idx % colors_.size()], &out_frame);
            std::stringstream ss;
            ss << idx;
            cv::putText(out_frame, ss.str(), centers.back(), cv::FONT_HERSHEY_SCRIPT_COMPLEX, 2.0,
                        colors_[idx % colors_.size()], 3);
            auto track = tracks().at(idx);
            if (track->lost) {
                cv::line(out_frame, active_track.second.back(), Center(track->predicted_rect), cv::Scalar(0, 0, 0), 4);
            }
        }

        return out_frame;
    }

    void
    TrackerLib::drawDashRect(cv::Mat &img, cv::Rect blob, int linelength, int dashlength, cv::Scalar color,
                             int thickness) {
        int w = cvRound(blob.width);//width
        int h = cvRound(blob.height);//height


        int tl_x = cvRound(blob.x);//top left x
        int tl_y = cvRound(blob.y);//top  left y


        int totallength = dashlength + linelength;
        int nCountX = w / totallength;//
        int nCountY = h / totallength;//


        cv::Point start, end;//start and end point of each dash


        //draw the horizontal lines
        start.y = tl_y;
        start.x = tl_x;


        end.x = tl_x;
        end.y = tl_y;


        for (int i = 0; i < nCountX; i++) {
            end.x = tl_x + (i + 1) * totallength - dashlength;//draw top dash line
            end.y = tl_y;
            start.x = tl_x + i * totallength;
            start.y = tl_y;
            cv::line(img, start, end, color, thickness);
        }
        for (int i = 0; i < nCountX; i++) {
            start.x = tl_x + i * totallength;
            start.y = tl_y + h;
            end.x = tl_x + (i + 1) * totallength - dashlength;//draw bottom dash line
            end.y = tl_y + h;
            cv::line(img, start, end, color, thickness);
        }
        for (int i = 0; i < nCountY; i++) {
            start.x = tl_x;
            start.y = tl_y + i * totallength;
            end.y = tl_y + (i + 1) * totallength - dashlength;//draw left dash line
            end.x = tl_x;
            cv::line(img, start, end, color, thickness);
        }
        for (int i = 0; i < nCountY; i++) {
            start.x = tl_x + w;
            start.y = tl_y + i * totallength;
            end.y = tl_y + (i + 1) * totallength - dashlength;//draw right dash line
            end.x = tl_x + w;
            cv::line(img, start, end, color, thickness);
        }
    }

    std::vector<size_t> TrackerLib::Solve_speed(cv::Mat &dissimilarity_matrix) {
        std::vector<size_t> results(dissimilarity_matrix.rows, -1);
        //取每列中最小的距离对应的id赋值到results[i]
        for (int i = 0; i < dissimilarity_matrix.rows; i++) {
            auto ptr = dissimilarity_matrix.ptr<float>(i);
            float min_dst = ptr[0];
            results[i] = 0;
            for (int j = 1; j < dissimilarity_matrix.cols; j++) {
                if (ptr[j] < min_dst) {
                    min_dst = ptr[j];
                    results[i] = j;
                }
            }

        }
        //以距离度量去除重复跟踪的id
        for (int i = 0; i < dissimilarity_matrix.rows - 1; i++) {
            if (results[i] < dissimilarity_matrix.cols) {
                auto ptr_i = dissimilarity_matrix.ptr<float>(i);
                for (int j = i + 1; j < dissimilarity_matrix.rows; j++) {
                    if (results[i] == results[j]) {
                        auto ptr_j = dissimilarity_matrix.ptr<float>(j);
                        if (ptr_i[results[i]] > ptr_j[results[j]]) {
                            results[i] = dissimilarity_matrix.cols + 1;
                        } else {
                            results[j] = dissimilarity_matrix.cols + 1;
                        }
                    }

                }
            }

        }
        return results;
    }

    void TrackerLib::Solve_speed2(std::vector<std::tuple<size_t, size_t, float>> &similarity) {
        sort(similarity.begin(), similarity.end(),
             [](const std::tuple<size_t, size_t, float> &x, const std::tuple<size_t, size_t, float> &y) {
                 return std::get<2>(x) > std::get<2>(y);
             });
    }


    void TrackerLib::SolveAssignmentProblem(
            const std::set<size_t> &track_ids,
            const TrackedCoreObjects &detections,
            std::set<size_t> *unmatched_tracks,
            std::set<size_t> *unmatched_detections,
            std::set<std::tuple<size_t, size_t, float>> *matches) {
        assert(unmatched_tracks);
        assert(unmatched_detections);
        unmatched_tracks->clear();
        unmatched_detections->clear();

        assert(!track_ids.empty());
        assert(!detections.empty());
        assert(matches);
        matches->clear();

        std::vector<std::tuple<size_t, size_t, float>> similarity;

        ComputeSimilarityMatrix(track_ids, detections, similarity);

        Solve_speed2(similarity);
//    clock_t end2 = clock();
//    time = 1.0 * (end2 - end) / CLOCKS_PER_SEC;
//    std::cout <<  "Solve_speed2 cost "<< time * 1000 << " ms" << std::endl;

        for (auto &id : track_ids) {
            unmatched_tracks->insert(id);
        }

        for (size_t i = 0; i < detections.size(); ++i) {
            unmatched_detections->insert(i);
        }

        for (auto &match : similarity) {
            size_t id = std::get<0>(match);
            size_t det_id = std::get<1>(match);
            if (unmatched_tracks->find(id) != unmatched_tracks->end() &&
                unmatched_detections->find(det_id) != unmatched_detections->end()) {
                matches->emplace(match);
                unmatched_tracks->erase(id);
                unmatched_detections->erase(det_id);
            }
        }

    }

    const ObjectCoreTracks TrackerLib::all_tracks(bool valid_only) const {
        ObjectCoreTracks all_objects;
        size_t counter = 0;

        std::set<size_t> sorted_ids;
        for (const auto &pair : tracks()) {
            sorted_ids.emplace(pair.first);
        }

        for (size_t id : sorted_ids) {
            if (!valid_only || IsTrackValid(id)) {
                TrackedCoreObjects filtered_objects;
                for (const auto &object : tracks().at(id)->objects) {
                    filtered_objects.emplace_back(object);
                    filtered_objects.back()->object_id = counter;
                }
                all_objects.emplace(counter++, filtered_objects);
            }
        }
        return all_objects;
    }
}  // namespace iot_track