//
// Created by 赵永飞 on 2019/4/22.
//

#ifndef PERSON_DETECT_TRACKER_TEST_TRACKERDESCRIPTOR_H
#define PERSON_DETECT_TRACKER_TEST_TRACKERDESCRIPTOR_H

#include "opencv2/opencv.hpp"

//############################openvino #################
//#include <samples/ocv_common.hpp>
//#include <inference_engine.hpp>
//#include <ext_list.hpp>
//using namespace InferenceEngine;
//############################openvino #################

/*
class openvino_model
{
public:
    //############################openvino #################
    mutable InferenceEngine::InferRequest  infer_request;
    mutable InferenceEngine::Blob::Ptr input_blob_;
    InferenceEngine::OutputsDataMap outInfo_;
    InferenceEngine::BlobMap outputs_;
    void model_init(std::string path_model_xml);
    void model_infer(cv::Mat input_image, cv::Mat& output_probMat);
    void model_InferBatch(std::vector<cv::Mat>& images, std::vector<cv::Mat>* vectors);

    InferenceEngine::Core ie;
    InferenceEngine::ExecutableNetwork executable_network;
    //############################openvino #################

};
*/
namespace iot_track {
//    class no_openvino_model {
//    public:
//        static void no_model_ResizedImage_infer(const cv::Mat &input_image, cv::Mat &output_probMat);
//
//        void no_model_ResizedImage_InferBatch(std::vector<cv::Mat> &images, std::vector<cv::Mat> *vectors);
//
//        void no_model_Handler_InferBatch(std::vector<cv::Mat> &images, std::vector<cv::Mat> *vectors);
//    };

///
/// \brief The IImageDescriptor class declares base class for image
/// descriptor.
///
    class TrackerImageDescriptor {
    public:

        ///
        /// \brief Descriptor size getter.
        /// \return Descriptor size.
        ///
        virtual cv::Size size() const = 0;

        ///
        /// \brief Computes image descriptor.
        /// \param[in] mat Color image.
        /// \param[out] descr Computed descriptor.
        ///
        virtual void Compute(const cv::Mat &mat, cv::Mat &descr) = 0;

        ///
        /// \brief Computes image descriptors in batches.
        /// \param[in] mats Images of interest.
        /// \param[out] descrs Matrices to store the computed descriptors.
        ///
        virtual void Compute(const std::vector<cv::Mat> &mats,
                             std::vector<cv::Mat> &descrs) = 0;

        ///
        /// \brief Prints performance counts for CNN-based descriptors
        ///
        virtual void PrintPerformanceCounts() {}

        virtual ~TrackerImageDescriptor() = default;
    };


///
/// \brief Uses resized image as descriptor.
///
    class TrackerHistogramDescriptor : public TrackerImageDescriptor {
    public:
        TrackerHistogramDescriptor() = default;

        /// \param resize_w
        /// \param resize_h
        explicit TrackerHistogramDescriptor(int resize_w, int resize_h) :
        resize_w_(resize_w), resize_h_(resize_h) {}

        ///
        /// \brief Returns descriptor size.
        /// \return Number of elements in the descriptor.
        ///
        cv::Size size() const override { return descr_size_; }

        ///
        /// \brief Computes image descriptor.
        /// \param[in] mat Frame containing the image of interest.
        /// \param[out] descr Matrix to store the computed descriptor.
        ///
        void Compute(const cv::Mat &mat, cv::Mat &descr) override;

        ///
        /// \brief Computes images descriptors.
        /// \param[in] mats Frames containing images of interest.
        /// \param[out] descrs Matrices to store the computed descriptors.
        //
        void Compute(const std::vector<cv::Mat> &mats, std::vector<cv::Mat> &descrs) override;

        ~TrackerHistogramDescriptor() override {};

    public:
        int resize_w_ = 64, resize_h_ = 64;
        cv::Size descr_size_;
    };

    ///
/// \brief Uses resized image as descriptor.
///
    class TrackerPartHistogramDescriptor : public TrackerImageDescriptor {
    public:
        TrackerPartHistogramDescriptor() = default;

        /// \param resize_w
        /// \param resize_h
        explicit TrackerPartHistogramDescriptor(int resize_w, int resize_h) :
                resize_w_(resize_w), resize_h_(resize_h) {}

        ///
        /// \brief Returns descriptor size.
        /// \return Number of elements in the descriptor.
        ///
        cv::Size size() const override { return descr_size_; }

        ///
        /// \brief Computes image descriptor.
        /// \param[in] mat Frame containing the image of interest.
        /// \param[out] descr Matrix to store the computed descriptor.
        ///
        void Compute(const cv::Mat &mat, cv::Mat &descr) override;

        ///
        /// \brief Computes images descriptors.
        /// \param[in] mats Frames containing images of interest.
        /// \param[out] descrs Matrices to store the computed descriptors.
        //
        void Compute(const std::vector<cv::Mat> &mats, std::vector<cv::Mat> &descrs) override;

        ~TrackerPartHistogramDescriptor() override {};

    public:
        int resize_w_ = 64, resize_h_ = 64;
        cv::Size descr_size_;
    };


    class TrackerHOGDescriptor : public TrackerImageDescriptor {
    public:
        TrackerHOGDescriptor() {
            hog = new cv::HOGDescriptor(cv::Size(resize_w_, resize_h_), block_size_, block_stride_, cell_size_, nbins_);
        };

        /// \param resize_w
        /// \param resize_h
        explicit TrackerHOGDescriptor(int resize_w, int resize_h) :
                resize_w_(resize_w), resize_h_(resize_h) {
            hog = new cv::HOGDescriptor(cv::Size(resize_w_, resize_h_), block_size_, block_stride_, cell_size_, nbins_);
        }

        ///
        /// \brief Returns descriptor size.
        /// \return Number of elements in the descriptor.
        ///
        cv::Size size() const override { return descr_size_; }

        ///
        /// \brief Computes image descriptor.
        /// \param[in] mat Frame containing the image of interest.
        /// \param[out] descr Matrix to store the computed descriptor.
        ///
        void Compute(const cv::Mat &mat, cv::Mat &descr) override;

        ///
        /// \brief Computes images descriptors.
        /// \param[in] mats Frames containing images of interest.
        /// \param[out] descrs Matrices to store the computed descriptors.
        //
        void Compute(const std::vector<cv::Mat> &mats, std::vector<cv::Mat> &descrs) override;

        ~TrackerHOGDescriptor() override {
            delete(hog);
            hog = nullptr;
        };

    public:
        int resize_w_ = 64, resize_h_ = 64;
        cv::Size block_size_ = cv::Size(8, 8);
        cv::Size block_stride_ = cv::Size(4, 4);
        cv::Size cell_size_ = cv::Size(4, 4);
        int nbins_ = 9;
        cv::Size padding_ = cv::Size(1, 1);
        cv::HOGDescriptor *hog = nullptr;
        cv::Size descr_size_;
    };



}  // namespace iot_track
#endif //PERSON_DETECT_TRACKER_TEST_TRACKERDESCRIPTOR_H
