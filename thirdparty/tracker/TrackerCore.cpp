//
// Created by 赵永飞 on 2019/4/22.
//

#include "TrackerCore.h"

namespace iot_track {
    bool operator==(const TrackedCoreObject &first, const TrackedCoreObject &second) {
        return ((first.rect == second.rect)
                && (first.confidence == second.confidence)
                && (first.frame_idx == second.frame_idx)
                && (first.object_id == second.object_id)
                && (first.timestamp == second.timestamp));
    }

    bool operator!=(const TrackedCoreObject &first, const TrackedCoreObject &second) {
        return !(first == second);
    }

}  // namespace iot_track