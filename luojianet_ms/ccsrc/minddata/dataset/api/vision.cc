/**
 * Copyright 2021, 2022 LuoJiaNET Research and Development Group, Wuhan University
 * Copyright 2021, 2022 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "minddata/dataset/include/dataset/vision.h"
#ifdef ENABLE_ACL
#include "minddata/dataset/include/dataset/vision_ascend.h"
#include "minddata/dataset/kernels/ir/vision/ascend_vision_ir.h"
#endif

#include "minddata/dataset/include/dataset/transforms.h"
#include "minddata/dataset/kernels/ir/vision/adjust_gamma_ir.h"
#include "minddata/dataset/kernels/ir/vision/affine_ir.h"
#include "minddata/dataset/kernels/ir/vision/andwi_ir.h"
#include "minddata/dataset/kernels/ir/vision/auto_augment_ir.h"
#include "minddata/dataset/kernels/ir/vision/auto_contrast_ir.h"
#include "minddata/dataset/kernels/ir/vision/awei_ir.h"
#include "minddata/dataset/kernels/ir/vision/bmi_ir.h"
#include "minddata/dataset/kernels/ir/vision/bounding_box_augment_ir.h"
#include "minddata/dataset/kernels/ir/vision/center_crop_ir.h"
#include "minddata/dataset/kernels/ir/vision/ciwi_ir.h"
#include "minddata/dataset/kernels/ir/vision/convert_color_ir.h"
#include "minddata/dataset/kernels/ir/vision/crop_ir.h"
#include "minddata/dataset/kernels/ir/vision/csi_ir.h"
#include "minddata/dataset/kernels/ir/vision/cutmix_batch_ir.h"
#include "minddata/dataset/kernels/ir/vision/cutout_ir.h"
#include "minddata/dataset/kernels/ir/vision/decode_ir.h"
#include "minddata/dataset/kernels/ir/vision/dvi_ir.h"
#include "minddata/dataset/kernels/ir/vision/equalize_ir.h"
#include "minddata/dataset/kernels/ir/vision/evi_ir.h"
#include "minddata/dataset/kernels/ir/vision/ewi_w_ir.h"
#include "minddata/dataset/kernels/ir/vision/ewi_y_ir.h"
#include "minddata/dataset/kernels/ir/vision/fndwi_ir.h"
#include "minddata/dataset/kernels/ir/vision/gaussian_blur_ir.h"
#include "minddata/dataset/kernels/ir/vision/gabor_ir.h"
#include "minddata/dataset/kernels/ir/vision/glcm_ir.h"
#include "minddata/dataset/kernels/ir/vision/gndwi_ir.h"
#include "minddata/dataset/kernels/ir/vision/horizontal_flip_ir.h"
#include "minddata/dataset/kernels/ir/vision/hwc_to_chw_ir.h"
#include "minddata/dataset/kernels/ir/vision/invert_ir.h"
#include "minddata/dataset/kernels/ir/vision/lbp_ir.h"
#include "minddata/dataset/kernels/ir/vision/mbi_ir.h"
#include "minddata/dataset/kernels/ir/vision/mbwi_ir.h"
#include "minddata/dataset/kernels/ir/vision/mciwi_ir.h"
#include "minddata/dataset/kernels/ir/vision/mndwi_ir.h"
#include "minddata/dataset/kernels/ir/vision/mixup_batch_ir.h"
#include "minddata/dataset/kernels/ir/vision/msavi_ir.h"
#include "minddata/dataset/kernels/ir/vision/ndpi_ir.h"
#include "minddata/dataset/kernels/ir/vision/ndvi_ir.h"
#include "minddata/dataset/kernels/ir/vision/ndwi_ir.h"
#include "minddata/dataset/kernels/ir/vision/nwi_ir.h"
#include "minddata/dataset/kernels/ir/vision/normalize_ir.h"
#include "minddata/dataset/kernels/ir/vision/normalize_pad_ir.h"
#include "minddata/dataset/kernels/ir/vision/osavi_ir.h"
#include "minddata/dataset/kernels/ir/vision/pad_ir.h"
#include "minddata/dataset/kernels/ir/vision/psi_ir.h"
#include "minddata/dataset/kernels/ir/vision/random_adjust_sharpness_ir.h"
#include "minddata/dataset/kernels/ir/vision/random_affine_ir.h"
#include "minddata/dataset/kernels/ir/vision/random_auto_contrast_ir.h"
#include "minddata/dataset/kernels/ir/vision/random_color_adjust_ir.h"
#include "minddata/dataset/kernels/ir/vision/random_color_ir.h"
#include "minddata/dataset/kernels/ir/vision/random_crop_decode_resize_ir.h"
#include "minddata/dataset/kernels/ir/vision/random_crop_ir.h"
#include "minddata/dataset/kernels/ir/vision/random_crop_with_bbox_ir.h"
#include "minddata/dataset/kernels/ir/vision/random_equalize_ir.h"
#include "minddata/dataset/kernels/ir/vision/random_horizontal_flip_ir.h"
#include "minddata/dataset/kernels/ir/vision/random_horizontal_flip_with_bbox_ir.h"
#include "minddata/dataset/kernels/ir/vision/random_invert_ir.h"
#include "minddata/dataset/kernels/ir/vision/random_lighting_ir.h"
#include "minddata/dataset/kernels/ir/vision/random_posterize_ir.h"
#include "minddata/dataset/kernels/ir/vision/random_resize_ir.h"
#include "minddata/dataset/kernels/ir/vision/random_resize_with_bbox_ir.h"
#include "minddata/dataset/kernels/ir/vision/random_resized_crop_ir.h"
#include "minddata/dataset/kernels/ir/vision/random_resized_crop_with_bbox_ir.h"
#include "minddata/dataset/kernels/ir/vision/random_rotation_ir.h"
#include "minddata/dataset/kernels/ir/vision/random_select_subpolicy_ir.h"
#include "minddata/dataset/kernels/ir/vision/random_sharpness_ir.h"
#include "minddata/dataset/kernels/ir/vision/random_solarize_ir.h"
#include "minddata/dataset/kernels/ir/vision/random_vertical_flip_ir.h"
#include "minddata/dataset/kernels/ir/vision/random_vertical_flip_with_bbox_ir.h"
#include "minddata/dataset/kernels/ir/vision/rdvi_ir.h"
#include "minddata/dataset/kernels/ir/vision/rescale_ir.h"
#include "minddata/dataset/kernels/ir/vision/resize_ir.h"
#include "minddata/dataset/kernels/ir/vision/resize_preserve_ar_ir.h"
#include "minddata/dataset/kernels/ir/vision/resize_with_bbox_ir.h"
#include "minddata/dataset/kernels/ir/vision/rfdi_ir.h"
#include "minddata/dataset/kernels/ir/vision/rgb_to_bgr_ir.h"
#include "minddata/dataset/kernels/ir/vision/rgb_to_gray_ir.h"
#include "minddata/dataset/kernels/ir/vision/rgba_to_bgr_ir.h"
#include "minddata/dataset/kernels/ir/vision/rgba_to_rgb_ir.h"
#include "minddata/dataset/kernels/ir/vision/rotate_ir.h"
#include "minddata/dataset/kernels/ir/vision/rvi_ir.h"
#include "minddata/dataset/kernels/ir/vision/rvi_sar_ir.h"
#include "minddata/dataset/kernels/ir/vision/savi_ir.h"
#include "minddata/dataset/kernels/ir/vision/slice_patches_ir.h"
#include "minddata/dataset/kernels/ir/vision/softdvpp_decode_random_crop_resize_jpeg_ir.h"
#include "minddata/dataset/kernels/ir/vision/softdvpp_decode_resize_jpeg_ir.h"
#include "minddata/dataset/kernels/ir/vision/srwi_ir.h"
#include "minddata/dataset/kernels/ir/vision/swap_red_blue_ir.h"
#include "minddata/dataset/kernels/ir/vision/tvi_ir.h"
#include "minddata/dataset/kernels/ir/vision/uniform_aug_ir.h"
#include "minddata/dataset/kernels/ir/vision/vertical_flip_ir.h"
#include "minddata/dataset/kernels/ir/vision/vsi_ir.h"
#include "minddata/dataset/kernels/ir/vision/wdrvi_ir.h"
#include "minddata/dataset/kernels/ir/vision/wi_f_ir.h"
#include "minddata/dataset/kernels/ir/vision/wi_h_ir.h"
#include "minddata/dataset/kernels/ir/vision/wndwi_ir.h"
#ifndef ENABLE_ANDROID
#include "utils/log_adapter.h"
#else
#include "luojianet_ms/lite/src/common/log_adapter.h"
#endif
#include "minddata/dataset/kernels/ir/validators.h"

// Kernel image headers (in alphabetical order)

namespace luojianet_ms {
namespace dataset {

// Transform operations for computer vision.
namespace vision {
// CONSTRUCTORS FOR API CLASSES TO CREATE VISION TENSOR TRANSFORM OPERATIONS
// (In alphabetical order)

// Affine Transform Operation.
struct Affine::Data {
  Data(float_t degrees, const std::vector<float> &translation, float scale, const std::vector<float> &shear,
       InterpolationMode interpolation, const std::vector<uint8_t> &fill_value)
      : degrees_(degrees),
        translation_(translation),
        scale_(scale),
        shear_(shear),
        interpolation_(interpolation),
        fill_value_(fill_value) {}
  float degrees_;
  std::vector<float> translation_;
  float scale_;
  std::vector<float> shear_;
  InterpolationMode interpolation_;
  std::vector<uint8_t> fill_value_;
};

Affine::Affine(float_t degrees, const std::vector<float> &translation, float scale, const std::vector<float> &shear,
               InterpolationMode interpolation, const std::vector<uint8_t> &fill_value)
    : data_(std::make_shared<Data>(degrees, translation, scale, shear, interpolation, fill_value)) {}

std::shared_ptr<TensorOperation> Affine::Parse() {
  return std::make_shared<AffineOperation>(data_->degrees_, data_->translation_, data_->scale_, data_->shear_,
                                           data_->interpolation_, data_->fill_value_);
}

#ifndef ENABLE_ANDROID
// AdjustGamma Transform Operation.
struct AdjustGamma::Data {
  Data(float gamma, float gain) : gamma_(gamma), gain_(gain) {}
  float gamma_;
  float gain_;
};

AdjustGamma::AdjustGamma(float gamma, float gain) : data_(std::make_shared<Data>(gamma, gain)) {}

std::shared_ptr<TensorOperation> AdjustGamma::Parse() {
  return std::make_shared<AdjustGammaOperation>(data_->gamma_, data_->gain_);
}

// AutoAugment Transform Operation.
struct AutoAugment::Data {
  Data(AutoAugmentPolicy policy, InterpolationMode interpolation, const std::vector<uint8_t> &fill_value)
      : policy_(policy), interpolation_(interpolation), fill_value_(fill_value) {}
  AutoAugmentPolicy policy_;
  InterpolationMode interpolation_;
  std::vector<uint8_t> fill_value_;
};

AutoAugment::AutoAugment(AutoAugmentPolicy policy, InterpolationMode interpolation,
                         const std::vector<uint8_t> &fill_value)
    : data_(std::make_shared<Data>(policy, interpolation, fill_value)) {}

std::shared_ptr<TensorOperation> AutoAugment::Parse() {
  return std::make_shared<AutoAugmentOperation>(data_->policy_, data_->interpolation_, data_->fill_value_);
}

// AutoContrast Transform Operation.
struct AutoContrast::Data {
  Data(float cutoff, const std::vector<uint32_t> &ignore) : cutoff_(cutoff), ignore_(ignore) {}
  float cutoff_;
  std::vector<uint32_t> ignore_;
};

AutoContrast::AutoContrast(float cutoff, std::vector<uint32_t> ignore)
    : data_(std::make_shared<Data>(cutoff, ignore)) {}

std::shared_ptr<TensorOperation> AutoContrast::Parse() {
  return std::make_shared<AutoContrastOperation>(data_->cutoff_, data_->ignore_);
}

// BoundingBoxAugment Transform Operation.
struct BoundingBoxAugment::Data {
  std::shared_ptr<TensorOperation> transform_;
  float ratio_;
};

BoundingBoxAugment::BoundingBoxAugment(TensorTransform *transform, float ratio) : data_(std::make_shared<Data>()) {
  data_->transform_ = transform ? transform->Parse() : nullptr;
  data_->ratio_ = ratio;
}

BoundingBoxAugment::BoundingBoxAugment(const std::shared_ptr<TensorTransform> &transform, float ratio)
    : data_(std::make_shared<Data>()) {
  data_->transform_ = transform ? transform->Parse() : nullptr;
  data_->ratio_ = ratio;
}

BoundingBoxAugment::BoundingBoxAugment(const std::reference_wrapper<TensorTransform> transform, float ratio)
    : data_(std::make_shared<Data>()) {
  data_->transform_ = transform.get().Parse();
  data_->ratio_ = ratio;
}

std::shared_ptr<TensorOperation> BoundingBoxAugment::Parse() {
  return std::make_shared<BoundingBoxAugmentOperation>(data_->transform_, data_->ratio_);
}
#endif  // not ENABLE_ANDROID

// CenterCrop Transform Operation.
struct CenterCrop::Data {
  explicit Data(const std::vector<int32_t> &size) : size_(size) {}
  std::vector<int32_t> size_;
};

CenterCrop::CenterCrop(std::vector<int32_t> size) : data_(std::make_shared<Data>(size)) {}

std::shared_ptr<TensorOperation> CenterCrop::Parse() { return std::make_shared<CenterCropOperation>(data_->size_); }

std::shared_ptr<TensorOperation> CenterCrop::Parse(const MapTargetDevice &env) {
  if (env == MapTargetDevice::kAscend310) {
#ifdef ENABLE_ACL
    std::vector<uint32_t> usize_;
    usize_.reserve(data_->size_.size());
    std::transform(data_->size_.begin(), data_->size_.end(), std::back_inserter(usize_),
                   [](int32_t i) { return (uint32_t)i; });
    return std::make_shared<DvppCropJpegOperation>(usize_);
#endif  // ENABLE_ACL
  } else if (env == MapTargetDevice::kCpu) {
    return std::make_shared<CenterCropOperation>(data_->size_);
  }
  MS_LOG(ERROR) << "Unsupported MapTargetDevice, only supported kCpu and kAscend310.";
  return nullptr;
}

#ifndef ENABLE_ANDROID
// ConvertColor Transform Operation.
struct ConvertColor::Data {
  explicit Data(ConvertMode convert_mode) : convert_mode_(convert_mode) {}
  ConvertMode convert_mode_;
};

ConvertColor::ConvertColor(ConvertMode convert_mode) : data_(std::make_shared<Data>(convert_mode)) {}

std::shared_ptr<TensorOperation> ConvertColor::Parse() {
  return std::make_shared<ConvertColorOperation>(data_->convert_mode_);
}
#endif  // not ENABLE_ANDROID

// Crop Transform Operation.
struct Crop::Data {
  Data(const std::vector<int32_t> &coordinates, const std::vector<int32_t> &size)
      : coordinates_(coordinates), size_(size) {}
  std::vector<int32_t> coordinates_;
  std::vector<int32_t> size_;
};

Crop::Crop(std::vector<int32_t> coordinates, std::vector<int32_t> size)
    : data_(std::make_shared<Data>(coordinates, size)) {}

std::shared_ptr<TensorOperation> Crop::Parse() {
  return std::make_shared<CropOperation>(data_->coordinates_, data_->size_);
}

#ifndef ENABLE_ANDROID
// CutMixBatch Transform Operation.
struct CutMixBatch::Data {
  Data(ImageBatchFormat image_batch_format, float alpha, float prob)
      : image_batch_format_(image_batch_format), alpha_(alpha), prob_(prob) {}
  float alpha_;
  float prob_;
  ImageBatchFormat image_batch_format_;
};

CutMixBatch::CutMixBatch(ImageBatchFormat image_batch_format, float alpha, float prob)
    : data_(std::make_shared<Data>(image_batch_format, alpha, prob)) {}

std::shared_ptr<TensorOperation> CutMixBatch::Parse() {
  return std::make_shared<CutMixBatchOperation>(data_->image_batch_format_, data_->alpha_, data_->prob_);
}

// CutOutOp.
struct CutOut::Data {
  Data(int32_t length, int32_t num_patches) : length_(length), num_patches_(num_patches) {}
  int32_t length_;
  int32_t num_patches_;
};

CutOut::CutOut(int32_t length, int32_t num_patches) : data_(std::make_shared<Data>(length, num_patches)) {}

std::shared_ptr<TensorOperation> CutOut::Parse() {
  return std::make_shared<CutOutOperation>(data_->length_, data_->num_patches_);
}
#endif  // not ENABLE_ANDROID

// Decode Transform Operation.
struct Decode::Data {
  explicit Data(bool rgb) : rgb_(rgb) {}
  bool rgb_;
};

Decode::Decode(bool rgb) : data_(std::make_shared<Data>(rgb)) {}

std::shared_ptr<TensorOperation> Decode::Parse() { return std::make_shared<DecodeOperation>(data_->rgb_); }

std::shared_ptr<TensorOperation> Decode::Parse(const MapTargetDevice &env) {
  if (env == MapTargetDevice::kAscend310) {
#ifdef ENABLE_ACL
    return std::make_shared<DvppDecodeJpegOperation>();
#endif  // ENABLE_ACL
  } else if (env == MapTargetDevice::kCpu) {
    return std::make_shared<DecodeOperation>(data_->rgb_);
  }
  MS_LOG(ERROR) << "Unsupported MapTargetDevice, only supported kCpu and kAscend310.";
  return nullptr;
}

#ifdef ENABLE_ACL
// DvppDecodeResize Transform Operation.
struct DvppDecodeResizeJpeg::Data {
  explicit Data(const std::vector<uint32_t> &resize) : resize_(resize) {}
  std::vector<uint32_t> resize_;
};

DvppDecodeResizeJpeg::DvppDecodeResizeJpeg(std::vector<uint32_t> resize) : data_(std::make_shared<Data>(resize)) {}

std::shared_ptr<TensorOperation> DvppDecodeResizeJpeg::Parse() {
  return std::make_shared<DvppDecodeResizeOperation>(data_->resize_);
}

std::shared_ptr<TensorOperation> DvppDecodeResizeJpeg::Parse(const MapTargetDevice &env) {
  if (env == MapTargetDevice::kAscend310) {
    return std::make_shared<DvppDecodeResizeOperation>(data_->resize_);
  }
  MS_LOG(ERROR) << "Unsupported MapTargetDevice, only supported kAscend310.";
  return nullptr;
}

// DvppDecodeResizeCrop Transform Operation.
struct DvppDecodeResizeCropJpeg::Data {
  Data(const std::vector<uint32_t> &crop, const std::vector<uint32_t> &resize) : crop_(crop), resize_(resize) {}
  std::vector<uint32_t> crop_;
  std::vector<uint32_t> resize_;
};

DvppDecodeResizeCropJpeg::DvppDecodeResizeCropJpeg(std::vector<uint32_t> crop, std::vector<uint32_t> resize)
    : data_(std::make_shared<Data>(crop, resize)) {}

std::shared_ptr<TensorOperation> DvppDecodeResizeCropJpeg::Parse() {
  return std::make_shared<DvppDecodeResizeCropOperation>(data_->crop_, data_->resize_);
}

std::shared_ptr<TensorOperation> DvppDecodeResizeCropJpeg::Parse(const MapTargetDevice &env) {
  if (env == MapTargetDevice::kAscend310) {
    return std::make_shared<DvppDecodeResizeCropOperation>(data_->crop_, data_->resize_);
  }
  MS_LOG(ERROR) << "Unsupported MapTargetDevice, only supported kAscend310.";
  return nullptr;
}

// DvppDecodePng Transform Operation.
DvppDecodePng::DvppDecodePng() {}

std::shared_ptr<TensorOperation> DvppDecodePng::Parse() { return std::make_shared<DvppDecodePngOperation>(); }

std::shared_ptr<TensorOperation> DvppDecodePng::Parse(const MapTargetDevice &env) {
  if (env == MapTargetDevice::kAscend310) {
    return std::make_shared<DvppDecodePngOperation>();
  }
  MS_LOG(ERROR) << "Unsupported MapTargetDevice, only supported kAscend310.";
  return nullptr;
}
#endif  // ENABLE_ACL

#ifndef ENABLE_ANDROID
// Equalize Transform Operation.
Equalize::Equalize() {}

std::shared_ptr<TensorOperation> Equalize::Parse() { return std::make_shared<EqualizeOperation>(); }
#endif  // not ENABLE_ANDROID

// GaussianBlur Transform Operation.
struct GaussianBlur::Data {
  Data(const std::vector<int32_t> &kernel_size, const std::vector<float> &sigma)
      : kernel_size_(kernel_size), sigma_(sigma) {}
  std::vector<int32_t> kernel_size_;
  std::vector<float> sigma_;
};

GaussianBlur::GaussianBlur(const std::vector<int32_t> &kernel_size, const std::vector<float> &sigma)
    : data_(std::make_shared<Data>(kernel_size, sigma)) {}

std::shared_ptr<TensorOperation> GaussianBlur::Parse() {
  return std::make_shared<GaussianBlurOperation>(data_->kernel_size_, data_->sigma_);
}

#ifndef ENABLE_ANDROID
// HorizontalFlip Transform Operation.
HorizontalFlip::HorizontalFlip() {}

std::shared_ptr<TensorOperation> HorizontalFlip::Parse() { return std::make_shared<HorizontalFlipOperation>(); }

// HwcToChw Transform Operation.
HWC2CHW::HWC2CHW() {}

std::shared_ptr<TensorOperation> HWC2CHW::Parse() { return std::make_shared<HwcToChwOperation>(); }

// Invert Transform Operation.
Invert::Invert() {}

std::shared_ptr<TensorOperation> Invert::Parse() { return std::make_shared<InvertOperation>(); }

// MixUpBatch Transform Operation.
struct MixUpBatch::Data {
  explicit Data(float alpha) : alpha_(alpha) {}
  float alpha_;
};

MixUpBatch::MixUpBatch(float alpha) : data_(std::make_shared<Data>(alpha)) {}

std::shared_ptr<TensorOperation> MixUpBatch::Parse() { return std::make_shared<MixUpBatchOperation>(data_->alpha_); }
#endif  // not ENABLE_ANDROID

// Normalize Transform Operation.
struct Normalize::Data {
  Data(const std::vector<float> &mean, const std::vector<float> &std) : mean_(mean), std_(std) {}
  std::vector<float> mean_;
  std::vector<float> std_;
};

Normalize::Normalize(std::vector<float> mean, std::vector<float> std) : data_(std::make_shared<Data>(mean, std)) {}

std::shared_ptr<TensorOperation> Normalize::Parse() {
  return std::make_shared<NormalizeOperation>(data_->mean_, data_->std_);
}

std::shared_ptr<TensorOperation> Normalize::Parse(const MapTargetDevice &env) {
  if (env == MapTargetDevice::kAscend310) {
#ifdef ENABLE_ACL
    return std::make_shared<DvppNormalizeOperation>(data_->mean_, data_->std_);
#endif  // ENABLE_ACL
  } else if (env == MapTargetDevice::kCpu) {
    return std::make_shared<NormalizeOperation>(data_->mean_, data_->std_);
  }
  MS_LOG(ERROR) << "Unsupported MapTargetDevice, only supported kCpu and kAscend310.";
  return nullptr;
}

#ifndef ENABLE_ANDROID
// NormalizePad Transform Operation.
struct NormalizePad::Data {
  Data(const std::vector<float> &mean, const std::vector<float> &std, const std::string &dtype)
      : mean_(mean), std_(std), dtype_(dtype) {}
  std::vector<float> mean_;
  std::vector<float> std_;
  std::string dtype_;
};

NormalizePad::NormalizePad(const std::vector<float> &mean, const std::vector<float> &std,
                           const std::vector<char> &dtype)
    : data_(std::make_shared<Data>(mean, std, CharToString(dtype))) {}

std::shared_ptr<TensorOperation> NormalizePad::Parse() {
  return std::make_shared<NormalizePadOperation>(data_->mean_, data_->std_, data_->dtype_);
}

// Pad Transform Operation.
struct Pad::Data {
  Data(const std::vector<int32_t> &padding, const std::vector<uint8_t> &fill_value, BorderType padding_mode)
      : padding_(padding), fill_value_(fill_value), padding_mode_(padding_mode) {}
  std::vector<int32_t> padding_;
  std::vector<uint8_t> fill_value_;
  BorderType padding_mode_;
};

Pad::Pad(std::vector<int32_t> padding, std::vector<uint8_t> fill_value, BorderType padding_mode)
    : data_(std::make_shared<Data>(padding, fill_value, padding_mode)) {}

std::shared_ptr<TensorOperation> Pad::Parse() {
  return std::make_shared<PadOperation>(data_->padding_, data_->fill_value_, data_->padding_mode_);
}

// RandomAdjustSharpness Transform Operation.
struct RandomAdjustSharpness::Data {
  Data(float degree, float prob) : degree_(degree), probability_(prob) {}
  float degree_;
  float probability_;
};

RandomAdjustSharpness::RandomAdjustSharpness(float degree, float prob) : data_(std::make_shared<Data>(degree, prob)) {}

std::shared_ptr<TensorOperation> RandomAdjustSharpness::Parse() {
  return std::make_shared<RandomAdjustSharpnessOperation>(data_->degree_, data_->probability_);
}
#endif  // not ENABLE_ANDROID

// RandomAffine Transform Operation.
struct RandomAffine::Data {
  Data(const std::vector<float_t> &degrees, const std::vector<float_t> &translate_range,
       const std::vector<float_t> &scale_range, const std::vector<float_t> &shear_ranges,
       InterpolationMode interpolation, const std::vector<uint8_t> &fill_value)
      : degrees_(degrees),
        translate_range_(translate_range),
        scale_range_(scale_range),
        shear_ranges_(shear_ranges),
        interpolation_(interpolation),
        fill_value_(fill_value) {}
  std::vector<float_t> degrees_;          // min_degree, max_degree
  std::vector<float_t> translate_range_;  // maximum x translation percentage, maximum y translation percentage
  std::vector<float_t> scale_range_;      // min_scale, max_scale
  std::vector<float_t> shear_ranges_;     // min_x_shear, max_x_shear, min_y_shear, max_y_shear
  InterpolationMode interpolation_;
  std::vector<uint8_t> fill_value_;
};

RandomAffine::RandomAffine(const std::vector<float_t> &degrees, const std::vector<float_t> &translate_range,
                           const std::vector<float_t> &scale_range, const std::vector<float_t> &shear_ranges,
                           InterpolationMode interpolation, const std::vector<uint8_t> &fill_value)
    : data_(std::make_shared<Data>(degrees, translate_range, scale_range, shear_ranges, interpolation, fill_value)) {}

std::shared_ptr<TensorOperation> RandomAffine::Parse() {
  return std::make_shared<RandomAffineOperation>(data_->degrees_, data_->translate_range_, data_->scale_range_,
                                                 data_->shear_ranges_, data_->interpolation_, data_->fill_value_);
}

#ifndef ENABLE_ANDROID
// RandomAutoContrast Transform Operation.
struct RandomAutoContrast::Data {
  Data(float cutoff, const std::vector<uint32_t> &ignore, float prob)
      : cutoff_(cutoff), ignore_(ignore), probability_(prob) {}
  float cutoff_;
  std::vector<uint32_t> ignore_;
  float probability_;
};

RandomAutoContrast::RandomAutoContrast(float cutoff, std::vector<uint32_t> ignore, float prob)
    : data_(std::make_shared<Data>(cutoff, ignore, prob)) {}

std::shared_ptr<TensorOperation> RandomAutoContrast::Parse() {
  return std::make_shared<RandomAutoContrastOperation>(data_->cutoff_, data_->ignore_, data_->probability_);
}

// RandomColor Transform Operation.
struct RandomColor::Data {
  Data(float t_lb, float t_ub) : t_lb_(t_lb), t_ub_(t_ub) {}
  float t_lb_;
  float t_ub_;
};

RandomColor::RandomColor(float t_lb, float t_ub) : data_(std::make_shared<Data>(t_lb, t_ub)) {}

std::shared_ptr<TensorOperation> RandomColor::Parse() {
  return std::make_shared<RandomColorOperation>(data_->t_lb_, data_->t_ub_);
}

// RandomColorAdjust Transform Operation.
struct RandomColorAdjust::Data {
  Data(const std::vector<float> &brightness, const std::vector<float> &contrast, const std::vector<float> &saturation,
       const std::vector<float> &hue)
      : brightness_(brightness), contrast_(contrast), saturation_(saturation), hue_(hue) {}
  std::vector<float> brightness_;
  std::vector<float> contrast_;
  std::vector<float> saturation_;
  std::vector<float> hue_;
};

RandomColorAdjust::RandomColorAdjust(std::vector<float> brightness, std::vector<float> contrast,
                                     std::vector<float> saturation, std::vector<float> hue)
    : data_(std::make_shared<Data>(brightness, contrast, saturation, hue)) {}

std::shared_ptr<TensorOperation> RandomColorAdjust::Parse() {
  return std::make_shared<RandomColorAdjustOperation>(data_->brightness_, data_->contrast_, data_->saturation_,
                                                      data_->hue_);
}

// RandomCrop Transform Operation.
struct RandomCrop::Data {
  Data(const std::vector<int32_t> &size, const std::vector<int32_t> &padding, bool pad_if_needed,
       const std::vector<uint8_t> &fill_value, BorderType padding_mode)
      : size_(size),
        padding_(padding),
        pad_if_needed_(pad_if_needed),
        fill_value_(fill_value),
        padding_mode_(padding_mode) {}
  std::vector<int32_t> size_;
  std::vector<int32_t> padding_;
  bool pad_if_needed_;
  std::vector<uint8_t> fill_value_;
  BorderType padding_mode_;
};

RandomCrop::RandomCrop(std::vector<int32_t> size, std::vector<int32_t> padding, bool pad_if_needed,
                       std::vector<uint8_t> fill_value, BorderType padding_mode)
    : data_(std::make_shared<Data>(size, padding, pad_if_needed, fill_value, padding_mode)) {}

std::shared_ptr<TensorOperation> RandomCrop::Parse() {
  return std::make_shared<RandomCropOperation>(data_->size_, data_->padding_, data_->pad_if_needed_, data_->fill_value_,
                                               data_->padding_mode_);
}

// RandomCropDecodeResize Transform Operation.
struct RandomCropDecodeResize::Data {
  Data(const std::vector<int32_t> &size, const std::vector<float> &scale, const std::vector<float> &ratio,
       InterpolationMode interpolation, int32_t max_attempts)
      : size_(size), scale_(scale), ratio_(ratio), interpolation_(interpolation), max_attempts_(max_attempts) {}
  std::vector<int32_t> size_;
  std::vector<float> scale_;
  std::vector<float> ratio_;
  InterpolationMode interpolation_;
  int32_t max_attempts_;
};

RandomCropDecodeResize::RandomCropDecodeResize(std::vector<int32_t> size, std::vector<float> scale,
                                               std::vector<float> ratio, InterpolationMode interpolation,
                                               int32_t max_attempts)
    : data_(std::make_shared<Data>(size, scale, ratio, interpolation, max_attempts)) {}

std::shared_ptr<TensorOperation> RandomCropDecodeResize::Parse() {
  return std::make_shared<RandomCropDecodeResizeOperation>(data_->size_, data_->scale_, data_->ratio_,
                                                           data_->interpolation_, data_->max_attempts_);
}

// RandomCropWithBBox Transform Operation.
struct RandomCropWithBBox::Data {
  Data(const std::vector<int32_t> &size, const std::vector<int32_t> &padding, bool pad_if_needed,
       const std::vector<uint8_t> &fill_value, BorderType padding_mode)
      : size_(size),
        padding_(padding),
        pad_if_needed_(pad_if_needed),
        fill_value_(fill_value),
        padding_mode_(padding_mode) {}
  std::vector<int32_t> size_;
  std::vector<int32_t> padding_;
  bool pad_if_needed_;
  std::vector<uint8_t> fill_value_;
  BorderType padding_mode_;
};

RandomCropWithBBox::RandomCropWithBBox(std::vector<int32_t> size, std::vector<int32_t> padding, bool pad_if_needed,
                                       std::vector<uint8_t> fill_value, BorderType padding_mode)
    : data_(std::make_shared<Data>(size, padding, pad_if_needed, fill_value, padding_mode)) {}

std::shared_ptr<TensorOperation> RandomCropWithBBox::Parse() {
  return std::make_shared<RandomCropWithBBoxOperation>(data_->size_, data_->padding_, data_->pad_if_needed_,
                                                       data_->fill_value_, data_->padding_mode_);
}

// RandomEqualize Transform Operation.
struct RandomEqualize::Data {
  explicit Data(float prob) : probability_(prob) {}
  float probability_;
};

RandomEqualize::RandomEqualize(float prob) : data_(std::make_shared<Data>(prob)) {}

std::shared_ptr<TensorOperation> RandomEqualize::Parse() {
  return std::make_shared<RandomEqualizeOperation>(data_->probability_);
}

// RandomHorizontalFlip.
struct RandomHorizontalFlip::Data {
  explicit Data(float prob) : probability_(prob) {}
  float probability_;
};

RandomHorizontalFlip::RandomHorizontalFlip(float prob) : data_(std::make_shared<Data>(prob)) {}

std::shared_ptr<TensorOperation> RandomHorizontalFlip::Parse() {
  return std::make_shared<RandomHorizontalFlipOperation>(data_->probability_);
}

// RandomHorizontalFlipWithBBox
struct RandomHorizontalFlipWithBBox::Data {
  explicit Data(float prob) : probability_(prob) {}
  float probability_;
};

RandomHorizontalFlipWithBBox::RandomHorizontalFlipWithBBox(float prob) : data_(std::make_shared<Data>(prob)) {}

std::shared_ptr<TensorOperation> RandomHorizontalFlipWithBBox::Parse() {
  return std::make_shared<RandomHorizontalFlipWithBBoxOperation>(data_->probability_);
}

// RandomInvert Operation.
struct RandomInvert::Data {
  explicit Data(float prob) : probability_(prob) {}
  float probability_;
};

RandomInvert::RandomInvert(float prob) : data_(std::make_shared<Data>(prob)) {}

std::shared_ptr<TensorOperation> RandomInvert::Parse() {
  return std::make_shared<RandomInvertOperation>(data_->probability_);
}

// RandomLighting Transform Operation.
struct RandomLighting::Data {
  explicit Data(float alpha) : alpha_(alpha) {}
  float alpha_;
};

RandomLighting::RandomLighting(float alpha) : data_(std::make_shared<Data>(alpha)) {}

std::shared_ptr<TensorOperation> RandomLighting::Parse() {
  return std::make_shared<RandomLightingOperation>(data_->alpha_);
}

// RandomPosterize Transform Operation.
struct RandomPosterize::Data {
  explicit Data(const std::vector<uint8_t> &bit_range) : bit_range_(bit_range) {}
  std::vector<uint8_t> bit_range_;
};

RandomPosterize::RandomPosterize(const std::vector<uint8_t> &bit_range) : data_(std::make_shared<Data>(bit_range)) {}

std::shared_ptr<TensorOperation> RandomPosterize::Parse() {
  return std::make_shared<RandomPosterizeOperation>(data_->bit_range_);
}

// RandomResize Transform Operation.
struct RandomResize::Data {
  explicit Data(const std::vector<int32_t> &size) : size_(size) {}
  std::vector<int32_t> size_;
};

RandomResize::RandomResize(std::vector<int32_t> size) : data_(std::make_shared<Data>(size)) {}

std::shared_ptr<TensorOperation> RandomResize::Parse() { return std::make_shared<RandomResizeOperation>(data_->size_); }

// RandomResizeWithBBox Transform Operation.
struct RandomResizeWithBBox::Data {
  explicit Data(const std::vector<int32_t> &size) : size_(size) {}
  std::vector<int32_t> size_;
};

RandomResizeWithBBox::RandomResizeWithBBox(std::vector<int32_t> size) : data_(std::make_shared<Data>(size)) {}

std::shared_ptr<TensorOperation> RandomResizeWithBBox::Parse() {
  return std::make_shared<RandomResizeWithBBoxOperation>(data_->size_);
}

// RandomResizedCrop Transform Operation.
struct RandomResizedCrop::Data {
  Data(const std::vector<int32_t> &size, const std::vector<float> &scale, const std::vector<float> &ratio,
       InterpolationMode interpolation, int32_t max_attempts)
      : size_(size), scale_(scale), ratio_(ratio), interpolation_(interpolation), max_attempts_(max_attempts) {}
  std::vector<int32_t> size_;
  std::vector<float> scale_;
  std::vector<float> ratio_;
  InterpolationMode interpolation_;
  int32_t max_attempts_;
};

RandomResizedCrop::RandomResizedCrop(std::vector<int32_t> size, std::vector<float> scale, std::vector<float> ratio,
                                     InterpolationMode interpolation, int32_t max_attempts)
    : data_(std::make_shared<Data>(size, scale, ratio, interpolation, max_attempts)) {}

std::shared_ptr<TensorOperation> RandomResizedCrop::Parse() {
  return std::make_shared<RandomResizedCropOperation>(data_->size_, data_->scale_, data_->ratio_, data_->interpolation_,
                                                      data_->max_attempts_);
}

// RandomResizedCrop Transform Operation.
struct RandomResizedCropWithBBox::Data {
  Data(const std::vector<int32_t> &size, const std::vector<float> &scale, const std::vector<float> &ratio,
       InterpolationMode interpolation, int32_t max_attempts)
      : size_(size), scale_(scale), ratio_(ratio), interpolation_(interpolation), max_attempts_(max_attempts) {}
  std::vector<int32_t> size_;
  std::vector<float> scale_;
  std::vector<float> ratio_;
  InterpolationMode interpolation_;
  int32_t max_attempts_;
};

RandomResizedCropWithBBox::RandomResizedCropWithBBox(std::vector<int32_t> size, std::vector<float> scale,
                                                     std::vector<float> ratio, InterpolationMode interpolation,
                                                     int32_t max_attempts)
    : data_(std::make_shared<Data>(size, scale, ratio, interpolation, max_attempts)) {}

std::shared_ptr<TensorOperation> RandomResizedCropWithBBox::Parse() {
  return std::make_shared<RandomResizedCropWithBBoxOperation>(data_->size_, data_->scale_, data_->ratio_,
                                                              data_->interpolation_, data_->max_attempts_);
}

// RandomRotation Transform Operation.
struct RandomRotation::Data {
  Data(const std::vector<float> &degrees, InterpolationMode resample, bool expand, const std::vector<float> &center,
       const std::vector<uint8_t> &fill_value)
      : degrees_(degrees), interpolation_mode_(resample), center_(center), expand_(expand), fill_value_(fill_value) {}
  std::vector<float> degrees_;
  InterpolationMode interpolation_mode_;
  std::vector<float> center_;
  bool expand_;
  std::vector<uint8_t> fill_value_;
};

RandomRotation::RandomRotation(std::vector<float> degrees, InterpolationMode resample, bool expand,
                               std::vector<float> center, std::vector<uint8_t> fill_value)
    : data_(std::make_shared<Data>(degrees, resample, expand, center, fill_value)) {}

std::shared_ptr<TensorOperation> RandomRotation::Parse() {
  return std::make_shared<RandomRotationOperation>(data_->degrees_, data_->interpolation_mode_, data_->expand_,
                                                   data_->center_, data_->fill_value_);
}

// RandomSelectSubpolicy Transform Operation.
struct RandomSelectSubpolicy::Data {
  std::vector<std::vector<std::pair<std::shared_ptr<TensorOperation>, double>>> policy_;
};

RandomSelectSubpolicy::RandomSelectSubpolicy(
  const std::vector<std::vector<std::pair<TensorTransform *, double>>> &policy)
    : data_(std::make_shared<Data>()) {
  for (uint32_t i = 0; i < policy.size(); i++) {
    std::vector<std::pair<std::shared_ptr<TensorOperation>, double>> subpolicy;

    for (uint32_t j = 0; j < policy[i].size(); j++) {
      TensorTransform *op = policy[i][j].first;
      std::shared_ptr<TensorOperation> operation = (op ? op->Parse() : nullptr);
      double prob = policy[i][j].second;
      subpolicy.emplace_back(std::move(std::make_pair(operation, prob)));
    }
    data_->policy_.emplace_back(subpolicy);
  }
}

RandomSelectSubpolicy::RandomSelectSubpolicy(
  const std::vector<std::vector<std::pair<std::shared_ptr<TensorTransform>, double>>> &policy)
    : data_(std::make_shared<Data>()) {
  for (uint32_t i = 0; i < policy.size(); i++) {
    std::vector<std::pair<std::shared_ptr<TensorOperation>, double>> subpolicy;

    for (uint32_t j = 0; j < policy[i].size(); j++) {
      std::shared_ptr<TensorTransform> op = policy[i][j].first;
      std::shared_ptr<TensorOperation> operation = (op ? op->Parse() : nullptr);
      double prob = policy[i][j].second;
      subpolicy.emplace_back(std::move(std::make_pair(operation, prob)));
    }
    data_->policy_.emplace_back(subpolicy);
  }
}

RandomSelectSubpolicy::RandomSelectSubpolicy(
  const std::vector<std::vector<std::pair<std::reference_wrapper<TensorTransform>, double>>> &policy)
    : data_(std::make_shared<Data>()) {
  for (int32_t i = 0; i < policy.size(); i++) {
    std::vector<std::pair<std::shared_ptr<TensorOperation>, double>> subpolicy;

    for (int32_t j = 0; j < policy[i].size(); j++) {
      TensorTransform &op = policy[i][j].first;
      std::shared_ptr<TensorOperation> operation = op.Parse();
      double prob = policy[i][j].second;
      subpolicy.emplace_back(std::move(std::make_pair(operation, prob)));
    }
    data_->policy_.emplace_back(subpolicy);
  }
}

std::shared_ptr<TensorOperation> RandomSelectSubpolicy::Parse() {
  return std::make_shared<RandomSelectSubpolicyOperation>(data_->policy_);
}

// RandomSharpness Transform Operation.
struct RandomSharpness::Data {
  explicit Data(const std::vector<float> &degrees) : degrees_(degrees) {}
  std::vector<float> degrees_;
};

RandomSharpness::RandomSharpness(std::vector<float> degrees) : data_(std::make_shared<Data>(degrees)) {}

std::shared_ptr<TensorOperation> RandomSharpness::Parse() {
  return std::make_shared<RandomSharpnessOperation>(data_->degrees_);
}

// RandomSolarize Transform Operation.
struct RandomSolarize::Data {
  explicit Data(const std::vector<uint8_t> &threshold) : threshold_(threshold) {}
  std::vector<uint8_t> threshold_;
};

RandomSolarize::RandomSolarize(std::vector<uint8_t> threshold) : data_(std::make_shared<Data>(threshold)) {}

std::shared_ptr<TensorOperation> RandomSolarize::Parse() {
  return std::make_shared<RandomSolarizeOperation>(data_->threshold_);
}

// RandomVerticalFlip Transform Operation.
struct RandomVerticalFlip::Data {
  explicit Data(float prob) : probability_(prob) {}
  float probability_;
};

RandomVerticalFlip::RandomVerticalFlip(float prob) : data_(std::make_shared<Data>(prob)) {}

std::shared_ptr<TensorOperation> RandomVerticalFlip::Parse() {
  return std::make_shared<RandomVerticalFlipOperation>(data_->probability_);
}

// RandomVerticalFlipWithBBox Transform Operation.
struct RandomVerticalFlipWithBBox::Data {
  explicit Data(float prob) : probability_(prob) {}
  float probability_;
};

RandomVerticalFlipWithBBox::RandomVerticalFlipWithBBox(float prob) : data_(std::make_shared<Data>(prob)) {}

std::shared_ptr<TensorOperation> RandomVerticalFlipWithBBox::Parse() {
  return std::make_shared<RandomVerticalFlipWithBBoxOperation>(data_->probability_);
}

// Rescale Transform Operation.
struct Rescale::Data {
  Data(float rescale, float shift) : rescale_(rescale), shift_(shift) {}
  float rescale_;
  float shift_;
};

Rescale::Rescale(float rescale, float shift) : data_(std::make_shared<Data>(rescale, shift)) {}

std::shared_ptr<TensorOperation> Rescale::Parse() {
  return std::make_shared<RescaleOperation>(data_->rescale_, data_->shift_);
}
#endif  // not ENABLE_ANDROID

// Resize Transform Operation.
struct Resize::Data {
  Data(const std::vector<int32_t> &size, InterpolationMode interpolation)
      : size_(size), interpolation_(interpolation) {}
  std::vector<int32_t> size_;
  InterpolationMode interpolation_;
};

Resize::Resize(std::vector<int32_t> size, InterpolationMode interpolation)
    : data_(std::make_shared<Data>(size, interpolation)) {}

std::shared_ptr<TensorOperation> Resize::Parse() {
  return std::make_shared<ResizeOperation>(data_->size_, data_->interpolation_);
}

std::shared_ptr<TensorOperation> Resize::Parse(const MapTargetDevice &env) {
  if (env == MapTargetDevice::kAscend310) {
#ifdef ENABLE_ACL
    std::vector<uint32_t> usize_;
    usize_.reserve(data_->size_.size());
    std::transform(data_->size_.begin(), data_->size_.end(), std::back_inserter(usize_),
                   [](int32_t i) { return (uint32_t)i; });
    return std::make_shared<DvppResizeJpegOperation>(usize_);
#endif  // ENABLE_ACL
  } else if (env == MapTargetDevice::kCpu) {
    return std::make_shared<ResizeOperation>(data_->size_, data_->interpolation_);
  }
  MS_LOG(ERROR) << "Unsupported MapTargetDevice, only supported kCpu and kAscend310.";
  return nullptr;
}

// ResizePreserveAR Transform Operation.
struct ResizePreserveAR::Data {
  Data(int32_t height, int32_t width, int32_t img_orientation)
      : height_(height), width_(width), img_orientation_(img_orientation) {}
  int32_t height_;
  int32_t width_;
  int32_t img_orientation_;
};

ResizePreserveAR::ResizePreserveAR(int32_t height, int32_t width, int32_t img_orientation)
    : data_(std::make_shared<Data>(height, width, img_orientation)) {}

std::shared_ptr<TensorOperation> ResizePreserveAR::Parse() {
  return std::make_shared<ResizePreserveAROperation>(data_->height_, data_->width_, data_->img_orientation_);
}

// Rotate Transform Operation.
struct Rotate::Data {
  Data(const float &degrees, InterpolationMode resample, bool expand, const std::vector<float> &center,
       const std::vector<uint8_t> &fill_value)
      : degrees_(degrees), interpolation_mode_(resample), center_(center), expand_(expand), fill_value_(fill_value) {}
  explicit Data(const FixRotationAngle &angle_id) : angle_id_(angle_id), lite_impl_(true) {}
  FixRotationAngle angle_id_{FixRotationAngle::k0Degree};
  bool lite_impl_{false};
  float degrees_{0};
  InterpolationMode interpolation_mode_{InterpolationMode::kNearestNeighbour};
  std::vector<float> center_{{}};
  bool expand_{false};
  std::vector<uint8_t> fill_value_{0, 0, 0};
};

Rotate::Rotate(FixRotationAngle angle_id) : data_(std::make_shared<Data>(angle_id)) {}

Rotate::Rotate(float degrees, InterpolationMode resample, bool expand, std::vector<float> center,
               std::vector<uint8_t> fill_value)
    : data_(std::make_shared<Data>(degrees, resample, expand, center, fill_value)) {}

std::shared_ptr<TensorOperation> Rotate::Parse() {
#ifndef ENABLE_ANDROID
  if (!data_->lite_impl_) {
    return std::make_shared<RotateOperation>(data_->degrees_, data_->interpolation_mode_, data_->expand_,
                                             data_->center_, data_->fill_value_);
  }
#else
  if (data_->lite_impl_) {
    return std::make_shared<RotateOperation>(data_->angle_id_);
  }
#endif  // not ENABLE_ANDROID
  std::string platform = data_->lite_impl_ ? "Cloud" : "Android";
  MS_LOG(ERROR) << "This Rotate API is not supported for " + platform + ", use another Rotate API.";
  return nullptr;
}

#ifndef ENABLE_ANDROID
// ResizeWithBBox Transform Operation.
struct ResizeWithBBox::Data {
  Data(const std::vector<int32_t> &size, InterpolationMode interpolation)
      : size_(size), interpolation_(interpolation) {}
  std::vector<int32_t> size_;
  InterpolationMode interpolation_;
};

ResizeWithBBox::ResizeWithBBox(std::vector<int32_t> size, InterpolationMode interpolation)
    : data_(std::make_shared<Data>(size, interpolation)) {}

std::shared_ptr<TensorOperation> ResizeWithBBox::Parse() {
  return std::make_shared<ResizeWithBBoxOperation>(data_->size_, data_->interpolation_);
}
#endif  // not ENABLE_ANDROID

// RGB2BGR Transform Operation.
std::shared_ptr<TensorOperation> RGB2BGR::Parse() { return std::make_shared<RgbToBgrOperation>(); }

// RGB2GRAY Transform Operation.
std::shared_ptr<TensorOperation> RGB2GRAY::Parse() { return std::make_shared<RgbToGrayOperation>(); }

#ifndef ENABLE_ANDROID
// RgbaToBgr Transform Operation.
RGBA2BGR::RGBA2BGR() {}

std::shared_ptr<TensorOperation> RGBA2BGR::Parse() { return std::make_shared<RgbaToBgrOperation>(); }

// RgbaToRgb Transform Operation.
RGBA2RGB::RGBA2RGB() {}

std::shared_ptr<TensorOperation> RGBA2RGB::Parse() { return std::make_shared<RgbaToRgbOperation>(); }

// SlicePatches Transform Operation.
struct SlicePatches::Data {
  Data(int32_t num_height, int32_t num_width, SliceMode slice_mode, uint8_t fill_value)
      : num_height_(num_height), num_width_(num_width), slice_mode_(slice_mode), fill_value_(fill_value) {}
  int32_t num_height_;
  int32_t num_width_;
  SliceMode slice_mode_;
  uint8_t fill_value_;
};

SlicePatches::SlicePatches(int32_t num_height, int32_t num_width, SliceMode slice_mode, uint8_t fill_value)
    : data_(std::make_shared<Data>(num_height, num_width, slice_mode, fill_value)) {}

std::shared_ptr<TensorOperation> SlicePatches::Parse() {
  return std::make_shared<SlicePatchesOperation>(data_->num_height_, data_->num_width_, data_->slice_mode_,
                                                 data_->fill_value_);
}

// SoftDvppDecodeRandomCropResizeJpeg Transform Operation.
struct SoftDvppDecodeRandomCropResizeJpeg::Data {
  Data(const std::vector<int32_t> &size, const std::vector<float> &scale, const std::vector<float> &ratio,
       int32_t max_attempts)
      : size_(size), scale_(scale), ratio_(ratio), max_attempts_(max_attempts) {}
  std::vector<int32_t> size_;
  std::vector<float> scale_;
  std::vector<float> ratio_;
  int32_t max_attempts_;
};

SoftDvppDecodeRandomCropResizeJpeg::SoftDvppDecodeRandomCropResizeJpeg(std::vector<int32_t> size,
                                                                       std::vector<float> scale,
                                                                       std::vector<float> ratio, int32_t max_attempts)
    : data_(std::make_shared<Data>(size, scale, ratio, max_attempts)) {}

std::shared_ptr<TensorOperation> SoftDvppDecodeRandomCropResizeJpeg::Parse() {
  return std::make_shared<SoftDvppDecodeRandomCropResizeJpegOperation>(data_->size_, data_->scale_, data_->ratio_,
                                                                       data_->max_attempts_);
}

// SoftDvppDecodeResizeJpeg Transform Operation.
struct SoftDvppDecodeResizeJpeg::Data {
  explicit Data(const std::vector<int32_t> &size) : size_(size) {}
  std::vector<int32_t> size_;
};

SoftDvppDecodeResizeJpeg::SoftDvppDecodeResizeJpeg(std::vector<int32_t> size) : data_(std::make_shared<Data>(size)) {}

std::shared_ptr<TensorOperation> SoftDvppDecodeResizeJpeg::Parse() {
  return std::make_shared<SoftDvppDecodeResizeJpegOperation>(data_->size_);
}

// SwapRedBlue Transform Operation.
SwapRedBlue::SwapRedBlue() {}

std::shared_ptr<TensorOperation> SwapRedBlue::Parse() { return std::make_shared<SwapRedBlueOperation>(); }

// UniformAug Transform Operation.
struct UniformAugment::Data {
  std::vector<std::shared_ptr<TensorOperation>> transforms_;
  int32_t num_ops_;
};

UniformAugment::UniformAugment(const std::vector<TensorTransform *> &transforms, int32_t num_ops)
    : data_(std::make_shared<Data>()) {
  (void)std::transform(
    transforms.begin(), transforms.end(), std::back_inserter(data_->transforms_),
    [](TensorTransform *const op) -> std::shared_ptr<TensorOperation> { return op ? op->Parse() : nullptr; });
  data_->num_ops_ = num_ops;
}

UniformAugment::UniformAugment(const std::vector<std::shared_ptr<TensorTransform>> &transforms, int32_t num_ops)
    : data_(std::make_shared<Data>()) {
  (void)std::transform(transforms.begin(), transforms.end(), std::back_inserter(data_->transforms_),
                       [](const std::shared_ptr<TensorTransform> op) -> std::shared_ptr<TensorOperation> {
                         return op ? op->Parse() : nullptr;
                       });
  data_->num_ops_ = num_ops;
}

UniformAugment::UniformAugment(const std::vector<std::reference_wrapper<TensorTransform>> &transforms, int32_t num_ops)
    : data_(std::make_shared<Data>()) {
  (void)std::transform(transforms.begin(), transforms.end(), std::back_inserter(data_->transforms_),
                       [](TensorTransform &op) -> std::shared_ptr<TensorOperation> { return op.Parse(); });
  data_->num_ops_ = num_ops;
}

std::shared_ptr<TensorOperation> UniformAugment::Parse() {
  return std::make_shared<UniformAugOperation>(data_->transforms_, data_->num_ops_);
}

// VerticalFlip Transform Operation.
VerticalFlip::VerticalFlip() {}

std::shared_ptr<TensorOperation> VerticalFlip::Parse() { return std::make_shared<VerticalFlipOperation>(); }
#endif  // not ENABLE_ANDROID

// RS index
// ANDWI Transform Operation.
ANDWI::ANDWI() {}
std::shared_ptr<TensorOperation> ANDWI::Parse() { return std::make_shared<ANDWIOperation>(); }

// AWEI Transform Operation.
AWEI::AWEI() {}
std::shared_ptr<TensorOperation> AWEI::Parse() { return std::make_shared<AWEIOperation>(); }

// BMI Transform Operation.
BMI::BMI() {}
std::shared_ptr<TensorOperation> BMI::Parse() { return std::make_shared<BMIOperation>(); }

// CSI Transform Operation.
CSI::CSI() {}
std::shared_ptr<TensorOperation> CSI::Parse() { return std::make_shared<CSIOperation>(); }

// CIWI Transform Operation.
struct CIWI::Data {
  Data(float digital_C)
      : digital_C_(digital_C) {}
  float digital_C_;
};

CIWI::CIWI(float digital_C)
    : data_(std::make_shared<Data>(digital_C)) {}

std::shared_ptr<TensorOperation> CIWI::Parse() {
  return std::make_shared<CIWIOperation>(data_->digital_C_);
}

// EWI_W Transform Operation.
struct EWI_W::Data {
  Data(float m, float n)
      : m_(m), n_(n) {}
  float m_;
  float n_;
};

EWI_W::EWI_W(float m, float n)
    : data_(std::make_shared<Data>(m, n)) {}

std::shared_ptr<TensorOperation> EWI_W::Parse() {
  return std::make_shared<EWI_WOperation>(data_->m_, data_->n_);
}

// FNDWI Transform Operation.
struct FNDWI::Data {
  Data(int S, int CNIR)
      : S_(S), CNIR_(CNIR) {}
  int S_;
  int CNIR_;
};

FNDWI::FNDWI(int S, int CNIR)
    : data_(std::make_shared<Data>(S, CNIR)) {}

std::shared_ptr<TensorOperation> FNDWI::Parse() {
  return std::make_shared<FNDWIOperation>(data_->S_, data_->CNIR_);
}

// Gabor Transform Operation.
struct Gabor::Data {
  Data(bool if_opencv_kernal)
      : if_opencv_kernal_(if_opencv_kernal) {}
  bool if_opencv_kernal_;
};

Gabor::Gabor(bool if_opencv_kernal)
    : data_(std::make_shared<Data>(if_opencv_kernal)) {}

std::shared_ptr<TensorOperation> Gabor::Parse() {
  return std::make_shared<GaborOperation>(data_->if_opencv_kernal_);
}

// GLCM Transform Operation.
struct GLCM::Data {
  Data(int N)
      : N_(N) {}
  int N_;
};

GLCM::GLCM(int N)
    : data_(std::make_shared<Data>(N)) {}

std::shared_ptr<TensorOperation> GLCM::Parse() {
  return std::make_shared<GLCMOperation>(data_->N_);
}

// GNDWI Transform Operation.
GNDWI::GNDWI() {}
std::shared_ptr<TensorOperation> GNDWI::Parse() { return std::make_shared<GNDWIOperation>(); }

// LBP Transform Operation.
struct LBP::Data {
  Data(int N)
      : N_(N) {}
  int N_;
};

LBP::LBP(int N)
    : data_(std::make_shared<Data>(N)) {}

std::shared_ptr<TensorOperation> LBP::Parse() {
  return std::make_shared<LBPOperation>(data_->N_);
}

// MBI Transform Operation.
struct MBI::Data {
  Data(int32_t s_min, int32_t s_max, int32_t delta_s)
      : s_min_(s_min), s_max_(s_max), delta_s_(delta_s) {}
  int32_t s_min_;
  int32_t s_max_;
  int32_t delta_s_;
};

MBI::MBI(int32_t s_min, int32_t s_max, int32_t delta_s)
    : data_(std::make_shared<Data>(s_min, s_max, delta_s)) {}

std::shared_ptr<TensorOperation> MBI::Parse() {
  return std::make_shared<MBIOperation>(data_->s_min_, data_->s_max_, data_->delta_s_);
}

// MBWI Transform Operation.
MBWI::MBWI() {}
std::shared_ptr<TensorOperation> MBWI::Parse() { return std::make_shared<MBWIOperation>(); }

// MNDWI Transform Operation.
MNDWI::MNDWI() {}
std::shared_ptr<TensorOperation> MNDWI::Parse() { return std::make_shared<MNDWIOperation>(); }

// MCIWI Transform Operation.
MCIWI::MCIWI() {}
std::shared_ptr<TensorOperation> MCIWI::Parse() { return std::make_shared<MCIWIOperation>(); }

// NDPI Transform Operation.
NDPI::NDPI() {}
std::shared_ptr<TensorOperation> NDPI::Parse() { return std::make_shared<NDPIOperation>(); }

// NDVI Transform Operation.
NDVI::NDVI() {}
std::shared_ptr<TensorOperation> NDVI::Parse() { return std::make_shared<NDVIOperation>(); }

// NDWI Transform Operation.
NDWI::NDWI() {}
std::shared_ptr<TensorOperation> NDWI::Parse() { return std::make_shared<NDWIOperation>(); }

// NWI Transform Operation.
NWI::NWI() {}
std::shared_ptr<TensorOperation> NWI::Parse() { return std::make_shared<NWIOperation>(); }

// PSI Transform Operation.
PSI::PSI() {}
std::shared_ptr<TensorOperation> PSI::Parse() { return std::make_shared<PSIOperation>(); }

// RFDI Transform Operation.
RFDI::RFDI() {}
std::shared_ptr<TensorOperation> RFDI::Parse() { return std::make_shared<RFDIOperation>(); }

// RVI Transform Operation.
RVI::RVI() {}
std::shared_ptr<TensorOperation> RVI::Parse() { return std::make_shared<RVIOperation>(); }

// DVI Transform Operation.
DVI::DVI() {}
std::shared_ptr<TensorOperation> DVI::Parse() { return std::make_shared<DVIOperation>(); }

// EVI Transform Operation.
EVI::EVI() {}
std::shared_ptr<TensorOperation> EVI::Parse() { return std::make_shared<EVIOperation>(); }

// EWI_Y Transform Operation.
EWI_Y::EWI_Y() {}
std::shared_ptr<TensorOperation> EWI_Y::Parse() { return std::make_shared<EWI_YOperation>(); }

// OSAVI Transform Operation.
struct OSAVI::Data {
  Data(float theta)
      : theta_(theta) {}
  float theta_;
};

OSAVI::OSAVI(float theta)
    : data_(std::make_shared<Data>(theta)) {}

std::shared_ptr<TensorOperation> OSAVI::Parse() {
  return std::make_shared<OSAVIOperation>(data_->theta_);
}

// MSAVI Transform Operation.
MSAVI::MSAVI() {}
std::shared_ptr<TensorOperation> MSAVI::Parse() { return std::make_shared<MSAVIOperation>(); }

// RDVI Transform Operation.
RDVI::RDVI() {}
std::shared_ptr<TensorOperation> RDVI::Parse() { return std::make_shared<RDVIOperation>(); }

// RVI_SAR Transform Operation.
RVI_SAR::RVI_SAR() {}
std::shared_ptr<TensorOperation> RVI_SAR::Parse() { return std::make_shared<RVI_SAROperation>(); }

// SAVI Transform Operation.
struct SAVI::Data {
  Data(float L)
      : L_(L) {}
  float L_;
};

SAVI::SAVI(float L)
    : data_(std::make_shared<Data>(L)) {}

std::shared_ptr<TensorOperation> SAVI::Parse() {
  return std::make_shared<SAVIOperation>(data_->L_);
}

// SRWI Transform Operation.
SRWI::SRWI() {}
std::shared_ptr<TensorOperation> SRWI::Parse() { return std::make_shared<SRWIOperation>(); }

// TVI Transform Operation.
TVI::TVI() {}
std::shared_ptr<TensorOperation> TVI::Parse() { return std::make_shared<TVIOperation>(); }

// VSI Transform Operation.
VSI::VSI() {}
std::shared_ptr<TensorOperation> VSI::Parse() { return std::make_shared<VSIOperation>(); }

// WDRVI Transform Operation.
struct WDRVI::Data {
  Data(float alpha)
      : alpha_(alpha) {}
  float alpha_;
};

WDRVI::WDRVI(float alpha)
    : data_(std::make_shared<Data>(alpha)) {}

std::shared_ptr<TensorOperation> WDRVI::Parse() {
  return std::make_shared<WDRVIOperation>(data_->alpha_);
}

// WI_F Transform Operation.
WI_F::WI_F() {}
std::shared_ptr<TensorOperation> WI_F::Parse() { return std::make_shared<WI_FOperation>(); }

// WI_H Transform Operation.
WI_H::WI_H() {}
std::shared_ptr<TensorOperation> WI_H::Parse() { return std::make_shared<WI_HOperation>(); }

// WNDWI Transform Operation.
struct WNDWI::Data {
  Data(float alpha)
      : alpha_(alpha) {}
  float alpha_;
};

WNDWI::WNDWI(float alpha)
    : data_(std::make_shared<Data>(alpha)) {}

std::shared_ptr<TensorOperation> WNDWI::Parse() {
  return std::make_shared<WNDWIOperation>(data_->alpha_);
}
}  // namespace vision
}  // namespace dataset
}  // namespace luojianet_ms
