//
// Created by 赵永飞 on 2019/4/22.
//

#ifndef PERSON_DETECT_TRACKER_TEST_TRACKERDISTANCE_H
#define PERSON_DETECT_TRACKER_TEST_TRACKERDISTANCE_H
#include <opencv2/opencv.hpp>
//#include<omp.h>
///
/// \brief The IDescriptorDistance class declares an interface for distance
/// computation between reidentification descriptors.
///
namespace iot_track {
    class TrackerDistance {
    public:
        ///
        /// \brief Computes distance between two descriptors.
        /// \param[in] descr1 First descriptor.
        /// \param[in] descr2 Second descriptor.
        /// \return Distance between two descriptors.
        ///
        virtual float Compute(const cv::Mat &descr1, const cv::Mat &descr2) = 0;

        ///
        /// \brief Computes distances between two descriptors in batches.
        /// \param[in] descrs1 Batch of first descriptors.
        /// \param[in] descrs2 Batch of second descriptors.
        /// \return Distances between descriptors.
        ///
        virtual std::vector<float> Compute(const std::vector<cv::Mat> &descrs1,
                                           const std::vector<cv::Mat> &descrs2) = 0;

        virtual ~TrackerDistance() {}
    };

///
/// \brief The CosDistance class allows computing cosine distance between two
/// reidentification descriptors.
///
    class TrackerCosDistance : public TrackerDistance {
    public:
        ///
        /// \brief CosDistance constructor.
        /// \param[in] descriptor_size Descriptor size.
        ///
        explicit TrackerCosDistance(const cv::Size &descriptor_size = cv::Size(1, 256));

        ///
        /// \brief Computes distance between two descriptors.
        /// \param descr1 First descriptor.
        /// \param descr2 Second descriptor.
        /// \return Distance between two descriptors.
        ///
        float Compute(const cv::Mat &descr1, const cv::Mat &descr2) override;

        ///
        /// \brief Computes distances between two descriptors in batches.
        /// \param[in] descrs1 Batch of first descriptors.
        /// \param[in] descrs2 Batch of second descriptors.
        /// \return Distances between descriptors.
        ///
        std::vector<float> Compute(
                const std::vector<cv::Mat> &descrs1,
                const std::vector<cv::Mat> &descrs2) override;

    public:
        cv::Size descriptor_size_;
    };


///
/// \brief Computes distance between images
///        using MatchTemplate function from OpenCV library
///        and its cross-correlation computation method in particular.
///
    class TrackerMatchTemplateDistance : public TrackerDistance {
    public:
        ///
        /// \brief Constructs the distance object.
        ///
        /// \param[in] type Method of MatchTemplate function computation.
        /// \param[in] scale Scale parameter for the distance.
        ///            Final distance is computed as:
        ///            scale * distance + offset.
        /// \param[in] offset Offset parameter for the distance.
        ///            Final distance is computed as:
        ///            scale * distance + offset.
        ///
//        TrackerMatchTemplateDistance(int type = cv::TemplateMatchModes::TM_CCORR_NORMED,
//                                     float scale = -1, float offset = 1)
//                : type_(type), scale_(scale), offset_(offset) {}

        ///
        /// \brief Computes distance between image descriptors.
        /// \param[in] descr1 First image descriptor.
        /// \param[in] descr2 Second image descriptor.
        /// \return Distance between image descriptors.
        ///
        float Compute(const cv::Mat &descr1, const cv::Mat &descr2) override;

        ///
        /// \brief Computes distances between two descriptors in batches.
        /// \param[in] descrs1 Batch of first descriptors.
        /// \param[in] descrs2 Batch of second descriptors.
        /// \return Distances between descriptors.
        ///
        std::vector<float> Compute(const std::vector<cv::Mat> &descrs1,
                                   const std::vector<cv::Mat> &descrs2) override;

        virtual ~TrackerMatchTemplateDistance() = default;

    public:
        int type_;      ///< Method of MatchTemplate function computation.
        float scale_;   ///< Scale parameter for the distance. Final distance is
        /// computed as: scale * distance + offset.
        float offset_;  ///< Offset parameter for the distance. Final distance is
        /// computed as: scale * distance + offset.
    };
}  // namespace iot_track
#endif //PERSON_DETECT_TRACKER_TEST_TRACKERDISTANCE_H
