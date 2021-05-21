//
// Created by 赵永飞 on 2019/4/25.
//

#include "TrackerLib_api.h"

/*
params.min_track_duration=500;                        //ms
params.forget_delay=400;                              //帧数，25*15=375对应跟踪目标15s保留
params.aff_thr_fast=0.88;                             //快速型特征描述子，大于给定阈值认为完全匹配上
params.aff_thr_strong=0.75;                           //强健型特征描述子，大于给定阈值认为完全匹配上    reid_based_classifier_matches_
params.shape_affinity_w=0.5;                          //形状相似度权重
params.motion_affinity_w=0.2;                         //运动估计相近权重
params.time_affinity_w=0.0;                           //时间度量权重
params.min_det_conf=0.4;                              //依据目标检测输出的置信度阈值过滤选择置信度较高的目标作为跟踪对象
params.bbox_aspect_ratios_range=cv::Vec2f(0.3, 5.0);  //目标检测输出的bounding box宽高比限定范围
params.bbox_heights_range=cv::Vec2f(16, 1000);        //目标检测输出的结果高度像素范围限定，16-1000默认
params.predict=25;                                    //依据给定的帧数来预测跟踪目标
params.strong_affinity_thr=0.4;                       //强健型相似度的阈值，大于此阈值认为跟踪目标匹配上
params.reid_thr=0.65;                                 //强健型特征描述子，相似度距离阈值，大于此阈值认为匹配上  reid_classifier_matches_
params.drop_forgotten_tracks=true;
params.max_num_objects_in_track=600;
*/
namespace iot_track {
    TrackerLib_api::TrackerLib_api() {
        auto params = TrackerLibParams();
        init(params);
    }

    TrackerLib_api::TrackerLib_api(char *config_file) {
        auto params = TrackerLibParams(config_file);
        init(params);
    }

    void TrackerLib_api::init(TrackerLibParams &params) {
        ptracker = new TrackerLib(params);

        if (ptracker->params_.appearance_feature_type == HISTOGRAM) {
            ptracker->set_descriptor(std::make_shared<TrackerHistogramDescriptor>());
            ptracker->set_distance(std::make_shared<TrackerCosDistance>());
        } else if (ptracker->params_.appearance_feature_type == PART_HISTOGRAM) {
            ptracker->set_descriptor(std::make_shared<TrackerPartHistogramDescriptor>());
            ptracker->set_distance(std::make_shared<TrackerCosDistance>());
        }
        else {
            ptracker->set_distance(std::make_shared<TrackerCosDistance>());
        }

        if (ptracker->params_.appearance_feature_type != NONE && ptracker->params_.use_hog_feature) {
            ptracker->set_hog_descriptor(std::make_shared<TrackerHOGDescriptor>());
        }

        m_TrackedObject_extends.clear();

        int height = 128, width = 128;
        int rows = 4, cols = 4;
        showImg = cv::Mat::zeros(height * rows, width * cols, CV_8UC3);
    }

//    TrackedCoreObjects TrackerLib_api::PredictedTracking() {
//        TrackedCoreObjects trackeddetections = ptracker->TrackingPredictedDetections();
//        for (int i = 0; i < trackeddetections.size(); ++i) {
//            //确保rect有效
//            auto tl = trackeddetections[i]->rect.tl(), br = trackeddetections[i]->rect.br();
//            tl.x = std::max(0, std::min(frame_width - 2, tl.x));
//            tl.y = std::max(0, std::min(frame_height - 2, tl.y));
//            br.x = std::max(0, std::min(frame_width - 1, br.x));
//            br.y = std::max(0, std::min(frame_height - 1, br.y));
//            int w = std::max(1, br.x - tl.x);
//            int h = std::max(2, br.y - tl.y);
//            trackeddetections[i]->rect = cv::Rect(tl.x, tl.y, w, h);
//        }
//        return trackeddetections;
//    }

    TrackedCoreObjects TrackerLib_api::GetLostPrediction() {
        TrackedCoreObjects lost_predict = ptracker->GetLostPrediction();
        return lost_predict;
    }

    TrackedCoreObjects
    TrackerLib_api::run(const cv::Mat &frame, const TrackedCoreObjects &input_detections, int frame_idx, bool Ifshow) {
//    double start= (double)cv::getTickCount();

        uint64_t cur_timestamp = 1000.0 / 25 * frame_idx;

        frame_width = frame.cols;
        frame_height = frame.rows;
        TrackedCoreObjects input_detections_tmp;
        //input_detections 检测框边界保护
        for (int i = 0; i < input_detections.size(); ++i) {
            //确保rect有效
            auto tl = input_detections[i]->rect.tl(), br = input_detections[i]->rect.br();
            tl.x = std::max(0, std::min(frame_width - 2, tl.x));
            tl.y = std::max(0, std::min(frame_height - 2, tl.y));
            br.x = std::max(0, std::min(frame_width - 1, br.x));
            br.y = std::max(0, std::min(frame_height - 1, br.y));
            int w = std::max(1, br.x - tl.x);
            int h = std::max(1, br.y - tl.y);
            cv::Rect rect(tl.x, tl.y, w, h);
            input_detections_tmp.push_back(std::make_shared<TrackedCoreObject>(
                    TrackedCoreObject(rect, input_detections[i]->confidence,
                                      frame_idx, input_detections[i]->object_id,
                                      input_detections[i]->appearance_feature)));
        }

        ptracker->Process(frame, input_detections_tmp, cur_timestamp);
        TrackedCoreObjects tracker_res = ptracker->TrackedDetections();

        TrackedCoreObjects output;
        //输入输出结果对齐，size一致
        for (int j = 0; j < input_detections_tmp.size(); j++) {
            cv::Rect rect(input_detections[j]->rect);

            int object_id = input_detections[j]->object_id;
            for (int i = 0; i < tracker_res.size(); i++) {
                if (tracker_res[i]->rect.x == input_detections_tmp[j]->rect.x &&
                    tracker_res[i]->rect.y == input_detections_tmp[j]->rect.y
                    && tracker_res[i]->rect.width == input_detections_tmp[j]->rect.width &&
                    tracker_res[i]->rect.height == input_detections_tmp[j]->rect.height
                    && tracker_res[i]->confidence == input_detections_tmp[j]->confidence) {
                    object_id = tracker_res[i]->object_id;
                    break;
                }
            }
            output.push_back(std::make_shared<TrackedCoreObject>(TrackedCoreObject(
                    rect, input_detections[j]->confidence, frame_idx, object_id,
                    input_detections[j]->appearance_feature)));
        }

        if (Ifshow) {
            //提取目标
            //添加新目标id,更新置信度及图像
            for (int i = 0; i < tracker_res.size(); ++i) {
                int flag = -1;
                for (int j = 0; j < m_TrackedObject_extends.size(); ++j) {
                    //update frame
                    if (tracker_res[i]->object_id == m_TrackedObject_extends[j].m_TrackedObject.object_id) {
                        if (tracker_res[i]->confidence > m_TrackedObject_extends[j].m_TrackedObject.confidence) {
                            m_TrackedObject_extends[j].m_TrackedObject = *(tracker_res[i]);
                            frame.copyTo(m_TrackedObject_extends[j].m_frame);
                        }
                        flag = 1;
                    }

                }
                //添加新目标id
                if (flag == -1) {
                    TrackedCoreObject_extend TrackedObject_extend_;
                    TrackedObject_extend_.m_TrackedObject = *(tracker_res[i]);
                    TrackedObject_extend_.flag_segmentation = -1;
                    frame.copyTo(TrackedObject_extend_.m_frame);

                    //确保rect有效
                    auto tl = TrackedObject_extend_.m_TrackedObject.rect.tl(), br = TrackedObject_extend_.m_TrackedObject.rect.br();
                    tl.x = std::max(0, std::min(TrackedObject_extend_.m_frame.cols - 1, tl.x));
                    tl.y = std::max(0, std::min(TrackedObject_extend_.m_frame.rows - 1, tl.y));
                    br.x = std::max(0, std::min(TrackedObject_extend_.m_frame.cols, br.x));
                    br.y = std::max(0, std::min(TrackedObject_extend_.m_frame.rows, br.y));
                    int w = std::max(0, br.x - tl.x);
                    int h = std::max(0, br.y - tl.y);
                    TrackedObject_extend_.m_TrackedObject.rect = cv::Rect(tl.x, tl.y, w, h);

                    if (TrackedObject_extend_.m_TrackedObject.rect.width > 5 &&
                        TrackedObject_extend_.m_TrackedObject.rect.height > 5) {
                        m_TrackedObject_extends.push_back(TrackedObject_extend_);
                    }
                }
            }

            ShowResult();
            cv::waitKey(1);
        }

        return output;
    }

    void TrackerLib_api::release() {
//        for (int i = 0; i < m_TrackedObject_extends.size(); i++) {
//            cv::rectangle(m_TrackedObject_extends[i].m_frame, m_TrackedObject_extends[i].m_TrackedObject.rect,
//                          cv::Scalar(0, 0, 255), 3);
//            std::string text = " conf: " + std::to_string(m_TrackedObject_extends[i].m_TrackedObject.confidence);
//            cv::putText(m_TrackedObject_extends[i].m_frame, text, m_TrackedObject_extends[i].m_TrackedObject.rect.tl(),
//                        cv::FONT_HERSHEY_COMPLEX, 1.0, cv::Scalar(0, 0, 255), 2);
//
//            std::string image_path = std::to_string(m_TrackedObject_extends[i].m_TrackedObject.object_id) + ".jpg";
//            cv::imwrite(image_path, m_TrackedObject_extends[i].m_frame);
//        }

        m_TrackedObject_extends.clear();
        showImg.release();

        delete ptracker;
    }

    void TrackerLib_api::ShowResult() {
        int height = 128, width = 128;
        int rows = 4, cols = 4;
        //画布绘制
        for (int i = 0; i < m_TrackedObject_extends.size(); i++) {
            int index = i % 16;

            cv::Mat img(cv::Size(width, height), CV_8UC3);
            img.setTo(cv::Scalar::all(0));

            cv::Mat temp_img;
            m_TrackedObject_extends[i].m_frame(m_TrackedObject_extends[i].m_TrackedObject.rect).copyTo(temp_img);

            cv::resize(temp_img, temp_img,
                       cv::Size(
                               m_TrackedObject_extends[i].m_TrackedObject.rect.width *
                               (height * 1.0 / m_TrackedObject_extends[i].m_TrackedObject.rect.height),
                               height));
            if (width - temp_img.cols > 1) {
                temp_img.copyTo(img(cv::Rect((width - temp_img.cols) / 2, 0, temp_img.cols, height)));
            } else {
                cv::resize(temp_img, img, img.size());
            }
            std::string text = std::to_string(m_TrackedObject_extends[i].m_TrackedObject.object_id);
            cv::putText(img, text, cv::Point(img.cols / 2 - 10, img.rows / 2), cv::FONT_HERSHEY_COMPLEX, 1.0,
                        cv::Scalar(0, 0, 255), 3);
            cv::Mat roi = cv::Mat(showImg, cv::Rect(index % cols * width, index / cols * height, width, height));

            img.copyTo(roi);
        }
        //绘制分割线
        //水平分割线
        for (int row = 1; row < rows; row++) {
            cv::line(showImg, cv::Point(0, row * height), cv::Point(width * cols, row * height), cv::Scalar(0, 0, 0));
        }
        //垂直分割线
        for (int col = 1; col < cols; col++) {
            cv::line(showImg, cv::Point(col * width, 0), cv::Point(col * width, height * rows), cv::Scalar(0, 0, 0));
        }

    }
}  // namespace iot_track