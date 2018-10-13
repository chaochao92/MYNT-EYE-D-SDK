// Copyright 2018 Slightech Co., Ltd. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "dataset/dataset.h"

#ifdef USE_OPENCV2
#include <opencv2/highgui/highgui.hpp>
#else
#include <opencv2/imgcodecs/imgcodecs.hpp>
#endif

#include <iomanip>
#include <limits>
#include <stdexcept>
#include <utility>

#define FULL_PRECISION \
  std::fixed << std::setprecision(std::numeric_limits<double>::max_digits10)
#define OS_SEP "/"

namespace d1000_tools {

Dataset::Dataset(std::string outdir) : outdir_(std::move(outdir)) {
}

Dataset::~Dataset() {
  if (motion_writer_) {
    motion_writer_->ofs.flush();
    motion_writer_->ofs.close();
  }
}

void Dataset::SaveMotionData(const mynteye::MotionData &data) {
  auto &&writer = GetMotionWriter();
  auto seq = motion_count_;

  writer->ofs << seq << ", " << static_cast<int>(data.imu->flag) << ", "
    << data.imu->timestamp << ", "<< data.imu->accel[0] << ", "
    << data.imu->accel[1] << ", " << data.imu->accel[2] << ", "
    << data.imu->gyro[0] << ", " << data.imu->gyro[1] << ", "
    << data.imu->gyro[2] << ", " << data.imu->temperature << std::endl;
  ++motion_count_;
}

void Dataset::SaveStreamData(const mynteye::StreamData &data) {
  auto &&writer = GetStreamWriter();
  auto seq = stream_count_;

  writer->ofs << seq << ", " << data.img_info->frame_id << ", "
    << data.img_info->timestamp << ", "
    << data.img_info->exposure_time << std::endl;
  ++stream_count_;
}

Dataset::writer_t Dataset::GetMotionWriter() {
  if (motion_writer_ == nullptr) {
    writer_t writer = std::make_shared<Writer>();
    writer->outdir = outdir_;
    writer->outfile = writer->outdir + OS_SEP "motion.txt";

    writer->ofs.open(writer->outfile, std::ofstream::out);
    writer->ofs << "seq, flag, timestamp, "
                   "accel_x, accel_y, accel_z, "
                   "gyro_x, gyro_y, gyro_z, temperature" << std::endl;
    writer->ofs << FULL_PRECISION;

    motion_writer_ = writer;
    motion_count_ = 0;
  }
  return motion_writer_;
}

Dataset::writer_t Dataset::GetStreamWriter() {
  if (stream_writer_ == nullptr) {
    writer_t writer = std::make_shared<Writer>();
    writer->outdir = outdir_;
    writer->outfile = writer->outdir + OS_SEP "left" + OS_SEP "stream.txt";

    writer->ofs.open(writer->outfile, std::ofstream::out);
    writer->ofs << "seq, frame_id, timestamp, exposure_time" << std::endl;
    writer->ofs << FULL_PRECISION;

    stream_writer_ = writer;
    stream_count_ = 0;
  }

  return stream_writer_;
}

}  // namespace d1000_tools
