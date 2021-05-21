//
// Created by 赵永飞 on 2019/4/22.
//

#include "TrackerDistance.h"

namespace iot_track {
    TrackerCosDistance::TrackerCosDistance(const cv::Size &descriptor_size) : descriptor_size_(descriptor_size)
    {
        assert(descriptor_size.area() != 0);
    }

    float TrackerCosDistance::Compute(const cv::Mat &descr1, const cv::Mat &descr2)
    {
        assert(!descr1.empty());
        assert(!descr2.empty());
        assert(descr1.size() == descr2.size());
        double xy = descr1.dot(descr2);
        double xx = descr1.dot(descr1);
        double yy = descr2.dot(descr2);
        double norm = sqrt(xx * yy) + 1e-6;
        return 0.5f * static_cast<float>(1.0 - xy / norm);
    }

    std::vector<float> TrackerCosDistance::Compute(const std::vector<cv::Mat> &descrs1, const std::vector<cv::Mat> &descrs2)
    {
        assert(descrs1.size() != 0);
        assert(descrs1.size() == descrs2.size());

        std::vector<float> distances(descrs1.size(), 1.f);
    #pragma omp parallel for
        for (int i = 0; i < descrs1.size(); i++)
        {
            distances.at(i) = Compute(descrs1.at(i), descrs2.at(i));
        }

        return distances;
    }


    float TrackerMatchTemplateDistance::Compute(const cv::Mat &descr1, const cv::Mat &descr2)
    {
        assert(!descr1.empty() && !descr2.empty());
        assert(descr1.size() == descr2.size());
        assert(descr1.type() == descr2.type());
        cv::Mat res;
        cv::matchTemplate(descr1, descr2, res, type_);
        assert(res.size() == cv::Size(1, 1));
        float dist = res.at<float>(0, 0);
        return scale_ * dist + offset_;
    }

    std::vector<float> TrackerMatchTemplateDistance::Compute(const std::vector<cv::Mat> &descrs1, const std::vector<cv::Mat> &descrs2)
    {
        std::vector<float> result(descrs1.size());
    #pragma omp parallel for
        for (int i = 0; i < descrs1.size(); i++)
        {
            result.at(i) =Compute(descrs1[i], descrs2[i]);
        }
        return result;
    }

}  // namespace iot_track