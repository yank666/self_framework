//
// Created by yankai.yan on 2021/6/17.
//

#ifndef SELF_ARCHITECTURE_CNCV_OPENINGTIME_INTERFACE_H
#define SELF_ARCHITECTURE_CNCV_OPENINGTIME_INTERFACE_H


extern "C" {
struct CNCVImageInfo{
  long timeStamp;
  void* BgrBuffer;
  void* YuvBuffer;
  int image_width;
  int image_height;
  int image_channel;
};

const char* CNCVPersonDetectVersion();

int CNCVPersonDetectInit(char *json_str);

char *CNCVPersonDetectProcess(const CNCVImageInfo &img_info, char *json_str);

int CNCVPersonDetectDeinit();
}


#endif  // SELF_ARCHITECTURE_CNCV_OPENINGTIME_INTERFACE_H
