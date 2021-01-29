#include <iostream>
#include <vector>
#include "glog/logging.h"

int main(int argc, char* argv[]) {

  LOG(INFO) << "info: hello world!";
  LOG(WARNING) << "warning: hello world!";
  LOG(ERROR) << "error: hello world!";

  return 0;

}
