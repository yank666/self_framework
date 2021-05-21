//
// Created by 赵永飞 on 2019/4/22.
//

#include "TrackerDescriptor.h"

namespace iot_track {
    void CalculateHistogram(const cv::Mat &img_patch, int resize_w, int resize_h, cv::Mat &histogram) {
        cv::Size descr_size = cv::Size(resize_w, resize_h);
        cv::Mat gray;
        //    cv::resize(input_image, gray, descr_size, 0, 0, cv::InterpolationFlags::INTER_CUBIC);
        cv::resize(img_patch, gray, descr_size, 0, 0, cv::InterpolationFlags::INTER_NEAREST);

        cv::cvtColor(gray, gray, cv::COLOR_BGR2GRAY);

        /// 设定bin数目
        int histSize = 256;

        /// 设定取值范围 ( R,G,B) )
        float range[] = {0, 256};
        const float *histRange = {range};

        bool uniform = true;
        bool accumulate = false;

        /// 计算直方图:
        cv::calcHist(&gray, 1, 0, cv::Mat(), histogram, 1, &histSize, &histRange, uniform, accumulate);
    }

    void CalculateHistogram_part(const cv::Mat &img_patch, int resize_w, int resize_h, cv::Mat &histogram) {
        cv::Size descr_size = cv::Size(resize_w, resize_h);
        cv::Mat gray;
        //    cv::resize(input_image, gray, descr_size, 0, 0, cv::InterpolationFlags::INTER_CUBIC);
        cv::resize(img_patch, gray, descr_size, 0, 0, cv::InterpolationFlags::INTER_NEAREST);

        cv::cvtColor(gray, gray, cv::COLOR_BGR2GRAY);

        std::vector<cv::Mat> img_part_list;
        img_part_list.emplace_back(cv::Mat(gray, cv::Rect(0, 0, resize_w / 2, resize_h / 2)));
        img_part_list.emplace_back(cv::Mat(gray, cv::Rect(resize_w / 2, 0, resize_w / 2, resize_h / 2)));
        img_part_list.emplace_back(cv::Mat(gray, cv::Rect(0, resize_h / 2, resize_w / 2, resize_h / 2)));
        img_part_list.emplace_back(cv::Mat(gray, cv::Rect(resize_w / 2, resize_h / 2, resize_w / 2, resize_h / 2)));
        histogram = cv::Mat(cv::Size(4, 256), CV_32FC1);

        for(int i = 0; i < 4; ++i) {
            cv::Mat img = img_part_list[i];
            /// 设定bin数目
            int histSize = 256;

            /// 设定取值范围 ( R,G,B) )
            float range[] = {0, 256};
            const float *histRange = {range};

            bool uniform = true;
            bool accumulate = false;

            cv::Mat hist_tmp;
            /// 计算直方图:
            cv::calcHist(&img, 1, 0, cv::Mat(), hist_tmp, 1, &histSize, &histRange, uniform, accumulate);
            hist_tmp.copyTo(histogram.col(i));
        }
        histogram = histogram.reshape(0, 1024);
    }

    void TrackerHistogramDescriptor::Compute(const std::vector<cv::Mat> &mats, std::vector<cv::Mat> &descrs) {
        //assert( &descrs!= nullptr );
        descrs.resize(mats.size());

        for (int i = 0; i < mats.size(); ++i) {
            CalculateHistogram(mats[i], resize_w_, resize_h_, descrs[i]);
        }
        descr_size_ = descrs[0].size();
    }

    void TrackerHistogramDescriptor::Compute(const cv::Mat &mat, cv::Mat &descr) {
        CalculateHistogram(mat, resize_w_, resize_h_, descr);
        descr_size_ = descr.size();
    }


    void TrackerPartHistogramDescriptor::Compute(const std::vector<cv::Mat> &mats, std::vector<cv::Mat> &descrs) {
        //assert( &descrs!= nullptr );
        descrs.resize(mats.size());

        for (int i = 0; i < mats.size(); ++i) {
            CalculateHistogram_part(mats[i], resize_w_, resize_h_, descrs[i]);
        }
        descr_size_ = descrs[0].size();
    }

    void TrackerPartHistogramDescriptor::Compute(const cv::Mat &mat, cv::Mat &descr) {
        CalculateHistogram_part(mat, resize_w_, resize_h_, descr);
        descr_size_ = descr.size();
    }


    void TrackerHOGDescriptor::Compute(const std::vector<cv::Mat> &mats, std::vector<cv::Mat> &descrs) {
        //assert( &descrs!= nullptr );
        descrs.resize(mats.size());

        for (int i = 0; i < mats.size(); ++i) {
            cv::Mat patch;
            cv::resize(mats[i], patch, cv::Size(resize_w_, resize_h_), 0, 0, cv::InterpolationFlags::INTER_NEAREST);
            cv::Mat gray;
            cv::cvtColor(patch, gray, cv::COLOR_BGR2GRAY);
            std::vector<float> hog_des;
            hog->compute(gray, hog_des, padding_);
            descrs[i] = cv::Mat(hog_des, true);
            descrs[i] /= sqrt(descrs[i].dot(descrs[i]));
        }
        descr_size_ = descrs[0].size();
    }

    void TrackerHOGDescriptor::Compute(const cv::Mat &mat, cv::Mat &descr) {
        cv::Mat patch;
        cv::resize(mat, patch, cv::Size(resize_w_, resize_h_), 0, 0, cv::InterpolationFlags::INTER_NEAREST);
        cv::Mat gray;
        cv::cvtColor(patch, gray, cv::COLOR_BGR2GRAY);
        std::vector<float> hog_des;
        hog->compute(gray, hog_des, block_stride_);
        descr = cv::Mat(hog_des, true);
        descr /= sqrt(descr.dot(descr));
        descr_size_ = descr.size();
    }
}  // namespace iot_track