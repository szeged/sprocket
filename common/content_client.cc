// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "minibrowser/common/content_client.h"

#include "content/public/common/user_agent.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/resource/resource_bundle.h"

std::string GetMiniBrowserUserAgent() {
  std::string product = "MiniBrowser/0.1.0.0";
  return content::BuildUserAgentFromProduct(product);
}

std::string MiniBrowserContentClient::GetUserAgent() const {
  return GetMiniBrowserUserAgent();
}

base::string16 MiniBrowserContentClient::GetLocalizedString(int message_id) const {
  return l10n_util::GetStringUTF16(message_id);
}

base::StringPiece MiniBrowserContentClient::GetDataResource(
    int resource_id,
    ui::ScaleFactor scale_factor) const {
  return ResourceBundle::GetSharedInstance().GetRawDataResourceForScale(
    resource_id, scale_factor);
}

base::RefCountedStaticMemory* MiniBrowserContentClient::GetDataResourceBytes(
    int resource_id) const {
  return ResourceBundle::GetSharedInstance().LoadDataResourceBytes(resource_id);
}

gfx::Image& MiniBrowserContentClient::GetNativeImageNamed(int resource_id) const {
  return ResourceBundle::GetSharedInstance().GetNativeImageNamed(resource_id);
}