//
// Created by yankai.yan on 2021/6/17.
//

#ifndef SELF_ARCHITECTURE_CNCV_OPENINGTIME_INTERFACE_H
#define SELF_ARCHITECTURE_CNCV_OPENINGTIME_INTERFACE_H

constexpr int CNCV_VERSION_MAJOR = 1;
constexpr int CNCV_VERSION_MINOR = 0;
constexpr int CNCV_VERSION_PATCH = 0;
constexpr char CNCV_BUILD_TIME[] = " Built:2021-06-30 00:00:00";

inline std::string CNCVOpeningTimeVersion() {
  return "v" + std::to_string(CNCV_VERSION_MAJOR) + "." +
         std::to_string(CNCV_VERSION_MINOR) + "." +
         std::to_string(CNCV_VERSION_PATCH) + CNCV_BUILD_TIME;
}

int CNCVOpeningTimeInit(char *json_str);

int CNCVOpeningTimeProcess(char *json_str);
#endif  // SELF_ARCHITECTURE_CNCV_OPENINGTIME_INTERFACE_H
