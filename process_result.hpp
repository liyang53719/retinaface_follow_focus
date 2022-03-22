/*
 * Copyright 2019 Xilinx Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include "serial_moter.hpp"

rs2::pipeline p; 

cv::Mat process_result(cv::Mat &m1, const vitis::ai::RetinaFaceResult &result,
                       bool is_jpeg) {
  cv::Mat image;
  cv::resize(m1, image, cv::Size{result.width, result.height});
  serial_moter my_serial_moter("/dev/ttyUSB0");  
//   my_serial_moter.openloop(255);  
  for (const auto &r : result.bboxes) {
    LOG_IF(INFO, is_jpeg) << " " << r.score << " "  //
                          << r.x << " "             //
                          << r.y << " "             //
                          << r.width << " "         //
                          << r.height;
    cv::rectangle(image,
                  cv::Rect{cv::Point(r.x * image.cols, r.y * image.rows),
                           cv::Size{(int)(r.width * image.cols),
                                    (int)(r.height * image.rows)}},
                  0xff);
//     std::cout  << " " << r.score*100 << " "   //
//             << r.x * image.cols << " "    //
//             << r.y * image.rows << " "    //
//             << r.width * image.cols << " " //
//             << r.height * image.rows
//             << std::endl;                   
  }

    int p_eye_x = 0, p_eye_y = 0 ;
  for (const auto &l : result.landmarks) {
    for (auto j = 0; j < 5; ++j) {
      auto px = l[j].first * image.cols;
      auto py = l[j].second * image.rows;
        
      LOG_IF(INFO, is_jpeg) << "p[" << j << "]: " << px << " " << py; //
      //std::cout << "p[" << j << "]: " << px << " " << py << std::endl; //
      cv::circle(image, cv::Point(px, py), 1, cv::Scalar(0, 255, 0), 1);
        
      if (j==0){          
//           auto p_eye_x = l[0].first * image.cols;
//           auto p_eye_y = l[0].second * image.rows;
          p_eye_x = px;
          p_eye_y = py;
      }
    }
  }
    
    // Block program until frames arrive
    rs2::frameset frames = p.wait_for_frames();

    // Try to get a frame of a depth image
    rs2::depth_frame depth = frames.get_depth_frame();

    // Get the depth frame's dimensions
    auto width = depth.get_width();
    auto height = depth.get_height();

    // Query the distance from the camera to the object in the center of the image
    float dist_eye = depth.get_distance(p_eye_x, p_eye_y*height/image.rows);
    int moter_angle = (int)(dist_eye*10);
    std::cout << "eye: " << p_eye_x <<" " << p_eye_y 
              <<";depth: " << dist_eye 
              <<";angle: " << moter_angle 
              << std::endl; 
    
//     my_serial_moter.openloop_demo();
    my_serial_moter.moveAngle(moter_angle);

////    image.cols 640 depth.get_width 640
////    image.rows 384 depth.get_height 480
//     std::cout << "image.cols " << image.cols << " depth.get_width " << width 
//        << "image.rows " << image.rows << " depth.get_height " << height 
//        << std::endl; 
    
//     open loop to nearist:
//     3E A0 01 02 E1 00 01 01
//     Angle to 90:
//     3E A3 01 08 EA 28 23 00 00 00 00 00 00 4B
//     Angel to 0:
//     3E A3 01 08 EA 00 00 00 00 00 00 00 00 00
  return image;
}
