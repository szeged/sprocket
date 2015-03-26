// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SPROCKET_COMMON_CONTENT_CLIENT_H_
#define SPROCKET_COMMON_CONTENT_CLIENT_H_

#include <string>
#include <vector>

#include "content/public/common/content_client.h"

std::string GetSprocketUserAgent();

class SprocketContentClient : public content::ContentClient {

public:

  std::string GetUserAgent() const override;
  base::string16 GetLocalizedString(int message_id) const override;
  base::StringPiece GetDataResource(
      int resource_id,
      ui::ScaleFactor scale_factor) const override;
  base::RefCountedStaticMemory* GetDataResourceBytes(
      int resource_id) const override;
  gfx::Image& GetNativeImageNamed(int resource_id) const override;
};

#endif  // SPROCKET_COMMON_CONTENT_CLIENT_H_
