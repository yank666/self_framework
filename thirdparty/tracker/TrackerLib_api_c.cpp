#include "TrackerLib_api_c.h"
#include "TrackerLib_api.h"

#ifdef __cplusplus
extern "C" {
#endif
/***************************************************************************************************
 * 功  能：跟踪c接口
 * 参  数: 无
 * 返回值: 无
 * 备  注：无
***************************************************************************************************/
using namespace iot_track;

void *tracker_init() {
    auto *tracker = new TrackerLib_api();
    return (void *) tracker;
}

void *tracker_init_with_config(char *config_path) {
    auto *tracker = new TrackerLib_api(config_path);
    return (void *) tracker;
}

int tracker_inference(detect_res *input, detect_res *output, int m_frame_idx, unsigned char *image, int width, int height,
                  int channel, void *lib_handle) {
    //printf("get into tracker inference\n");
    auto *tracker = (TrackerLib_api *) lib_handle;
    TrackedCoreObjects tracker_res;
//    TrackedCoreObjects temp_detections;
    TrackedCoreObjects temp_detections_src;
    //channel表示OpenCV的channel宏，我们暂不用，仅支持BGR三通道
    cv::Mat cv_img(cv::Size(width, height), CV_8UC3, image);

    memcpy(output, input, sizeof(detect_res));

    //将结果存入跟踪输入并排序
    for (int i = 0; i < input->num_detections; i++) {
        float prob = input->detect_res_info[i].prob;
        float x = std::max(input->detect_res_info[i].box.x1, 0.0f);
        x = std::min(x, width * 1.0f - 2.0f);
        float y = std::max(input->detect_res_info[i].box.y1, 0.0f);
        y = std::min(y, height * 1.0f - 2.0f);
        float w = std::min(input->detect_res_info[i].box.x2 - x, width * 1.0f - 1.0f - x);
        float h = std::min(input->detect_res_info[i].box.y2 - y, height * 1.0f - 1.0f - y);
        w = std::max(1.0f, w);
        h = std::max(1.0f, h);
        //将检测的结果存到跟踪结构体中
        cv::Rect rect = cv::Rect(x, y, w, h);
        TrackedCoreObjectPtr detection;

        if (input->detect_res_info[i].feature_len > 0) {
            std::vector<float> appearance_feature(input->detect_res_info[i].feature,
                                       input->detect_res_info[i].feature + input->detect_res_info[i].feature_len);
            detection = std::make_shared<TrackedCoreObject>(rect, prob, m_frame_idx, -1, appearance_feature);
        }
        else {
            detection = std::make_shared<TrackedCoreObject>(rect, prob, m_frame_idx, -1);
        }

        temp_detections_src.push_back(detection);
    }

    tracker_res = tracker->run(cv_img, temp_detections_src, m_frame_idx, false);
    //将检测结果传出
    for (int i = 0; i < tracker_res.size(); i++) {
        output->detect_res_info[i].track_id = tracker_res[i]->object_id;
    }
    output->num_detections = input->num_detections;
    return 0;
}

int tracker_lost_predict(detect_res *output, void *lib_handle) {
    auto *tracker = (TrackerLib_api *) lib_handle;
    TrackedCoreObjects tracker_res = tracker->GetLostPrediction();
    for (int i = 0; i < tracker_res.size(); i++) {
        output->detect_res_info[i].box.x1 = tracker_res[i]->rect.tl().x;
        output->detect_res_info[i].box.y1 = tracker_res[i]->rect.tl().y;
        output->detect_res_info[i].box.x2 = tracker_res[i]->rect.br().x;
        output->detect_res_info[i].box.y2 = tracker_res[i]->rect.br().y;
        output->detect_res_info[i].track_id = tracker_res[i]->object_id;
        output->detect_res_info[i].prob = tracker_res[i]->confidence;
        output->detect_res_info[i].label = tracker_res[i]->label;
        output->detect_res_info[i].feature_len = 0;
    }
    output->num_detections = int(tracker_res.size());
    return 0;
}

int tracker_release(void *lib_handle) {
    int status = 0;
    auto *tracker = (TrackerLib_api *) lib_handle;
    tracker->release();
    tracker = nullptr;
    delete tracker;
    return status;
}

#ifdef __cplusplus
}
#endif
