//
// Created by yankai.yan on 2021/4/18.
//

#ifndef SELF_ARCHITECTURE_ARITHMETICAL_UTILS_H
#define SELF_ARCHITECTURE_ARITHMETICAL_UTILS_H

inline float sigmoid(float x) { return (1 / (1 + exp(-x))); }

void softmax(float *x, int n) {
  float sum = 0.0;
  for (int i = 0; i < n; ++i) {
    x[i] = exp(x[i]);
    sum += x[i];
  }
  for (int i = 0; i < n; ++i) {
    x[i] /= sum;
  }
}
#endif  // SELF_ARCHITECTURE_ARITHMETICAL_UTILS_H
