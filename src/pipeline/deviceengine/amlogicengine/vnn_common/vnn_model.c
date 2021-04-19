/****************************************************************************
 *   Generated by ACUITY 5.16.3
 *   Match ovxlib 1.1.26
 *
 *   Neural Network appliction network definition source file
 ****************************************************************************/
/*-------------------------------------------
                   Includes
 -------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "vsi_nn_pub.h"

#include "vnn_global.h"
#include "vnn_model.h"

uint64_t get_perf_count() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (uint64_t)((uint64_t)ts.tv_nsec + (uint64_t)ts.tv_sec * BILLON);
}

vsi_status vnn_VerifyGraph(vsi_nn_graph_t *graph) {
  vsi_status status = VSI_FAILURE;
  uint64_t tmsStart, tmsEnd, msVal, usVal;

  /* Verify graph */
  // printf("Verify...\n");
  tmsStart = get_perf_count();
  status = vsi_nn_VerifyGraph(graph);
  TEST_CHECK_STATUS(status, final);
  tmsEnd = get_perf_count();
  msVal = (tmsEnd - tmsStart) / 1000000;
  usVal = (tmsEnd - tmsStart) / 1000;
  // printf("Verify Graph: %ldms or %ldus\n", msVal, usVal);
final:
  return status;
}

void vnn_ReleaseModel(vsi_nn_graph_t *graph, vsi_bool release_ctx) {
  vsi_nn_context_t ctx;
  if (NULL != graph) {
    ctx = graph->ctx;
    vsi_nn_ReleaseGraph(&graph);

    /*-----------------------------------------
    Unregister client ops
    -----------------------------------------*/

    if (release_ctx) {
      vsi_nn_ReleaseContext(&ctx);
    }
  }
} /* vnn_ReleaseModel() */

static uint8_t *load_data(FILE *fp, size_t ofst, size_t sz) {
  uint8_t *data;
  int32_t ret;
  data = NULL;
  if (NULL == fp) {
    return NULL;
  }

  ret = fseek(fp, ofst, SEEK_SET);
  if (ret != 0) {
    VSILOGE("blob seek failure.");
    return NULL;
  }

  data = (uint8_t *)malloc(sz);
  if (data == NULL) {
    VSILOGE("buffer malloc failure.");
    return NULL;
  }
  ret = fread(data, 1, sz, fp);
  return data;
} /* load_data() */