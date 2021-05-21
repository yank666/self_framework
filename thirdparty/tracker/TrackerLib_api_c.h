#ifndef TRACKERLIB_API_C_H
#define TRACKERLIB_API_C_H


#ifdef __cplusplus
extern "C"{
#endif

struct detect_bbox{
   float x1, y1, x2, y2;
};

struct detect_bbox_info{
   detect_bbox box;
   int label;
   int track_id;
   float prob;
   float feature[256];
   int feature_len;
};

struct detect_res{
   int num_detections;
   detect_bbox_info detect_res_info[300];
};

/***************************************************************************************************
 * 功  能：跟踪器初始化
 * 参  数:  无
 * 返回值:  无
 * 备  注： 无
***************************************************************************************************/
void *tracker_init();

void *tracker_init_with_config(char* config_path);

/***************************************************************************************************
 * 功  能：跟踪器推理接口
 * 参  数:
 *         input:       检测结果
 *         output:      跟踪结果
 *         m_frame_idx: 跟踪器计时
 *         image:       图像裸数据
 *         width:       图像宽
 *         height:      图像高
 *         channel:     图像通道
 *         lib_handle:  算法具柄
 * 返回值: 错误码
 * 备  注：无
***************************************************************************************************/
int tracker_inference(detect_res *input,
                      detect_res *output,
                      int m_frame_idx,
                      unsigned char *image,
                      int width,
                      int height,
                      int channel,
                      void *lib_handle);

/***************************************************************************************************
 * 功  能：跟踪器释放接口
 * 参  数:
 *         lib_handle:算法具柄
 * 返回值: 错误码
 * 备  注：无
***************************************************************************************************/
int tracker_release(void *lib_handle);

#ifdef __cplusplus
}
#endif
#endif //TRACKERLIB_API_C_H