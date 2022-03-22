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
#include <glog/logging.h>

#include <iostream>
#include <memory>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vitis/ai/demo.hpp>
#include <vitis/ai/retinaface.hpp>
//#include <vitis/ai/nnpp/retinaface.hpp>

#include "./process_result.hpp"
#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
// #include "./serial_moter.hpp"
using namespace std;
// Create a Pipeline - this serves as a top-level API for streaming and processing frames
extern rs2::pipeline p; 
// serial_moter my_serial_moter("/dev/ttyUSB0"); 

int main(int argc, char *argv[]) {
  string model = argv[1];
 
  // Configure and start the pipeline
  p.start();  
  return vitis::ai::main_for_video_demo(
      argc, argv, [model] { return vitis::ai::RetinaFace::create(model); },
      process_result, 2);
}

