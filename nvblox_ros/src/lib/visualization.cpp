// SPDX-FileCopyrightText: NVIDIA CORPORATION & AFFILIATES
// Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0

#include <nvblox/core/types.h>

#include <string>
#include <vector>

#include "nvblox_ros/visualization.hpp"

namespace nvblox
{

inline std::string toString(SliceLimitMarkerType marker_type)
{
  switch (marker_type) {
    case SliceLimitMarkerType::kTopSliceLimit:
      return "top_slice_limit";
      break;
    case SliceLimitMarkerType::kBottomSliceLimit:
      return "bottom_slice_limit";
      break;
    default:
      return "unknown_type";
  }
}

visualization_msgs::msg::Marker sliceLimitsToMarker(
  const Transform & T_G_PB, const float slice_visualization_side_length,
  const rclcpp::Time & timestamp, const std::string & global_frame_id,
  const float height, const SliceLimitMarkerType slice_limit_type)
{
  // Corners of the plane in the plane-body frame.
  // NOTE: We attach the z value later because this is specified in the odom
  // frame.
  const float square_half_side_length_m =
    slice_visualization_side_length / 2.0f;
  Vector3f p0_PB(square_half_side_length_m, square_half_side_length_m, 0.0f);
  Vector3f p1_PB(-square_half_side_length_m, square_half_side_length_m, 0.0f);
  Vector3f p2_PB(square_half_side_length_m, -square_half_side_length_m, 0.0f);
  Vector3f p3_PB(-square_half_side_length_m, -square_half_side_length_m, 0.0f);

  // 6 triangle corners ([0,1,2], [1,2,3])
  std::vector<Vector3f> vertices_PB_vec{p0_PB, p1_PB, p2_PB,
    p1_PB, p2_PB, p3_PB};

  // Create marker message
  visualization_msgs::msg::Marker marker;
  marker.header.frame_id = global_frame_id;
  marker.header.stamp = timestamp;
  marker.ns = toString(slice_limit_type);
  marker.id = 0;
  marker.type = visualization_msgs::msg::Marker::TRIANGLE_LIST;
  marker.action = visualization_msgs::msg::Marker::ADD;
  marker.scale.x = 1.0;
  marker.scale.y = 1.0;
  marker.scale.z = 1.0;
  marker.color.a = 0.25;

  for (const Vector3f & vertex_PB : vertices_PB_vec) {
    // Transform to global frame
    Vector3f vertex_G = T_G_PB * vertex_PB;

    // Add point with z-height
    geometry_msgs::msg::Point msg;
    msg.x = vertex_G.x();
    msg.y = vertex_G.y();
    msg.z = height;
    marker.points.push_back(msg);

    // Add color to point
    std_msgs::msg::ColorRGBA color_msg;
    if (slice_limit_type == SliceLimitMarkerType::kTopSliceLimit) {
      color_msg.r = 1.0;
    } else {
      color_msg.g = 1.0;
    }
    color_msg.a = 0.8;
    marker.colors.push_back(color_msg);
  }

  return marker;
}

}  // namespace nvblox
