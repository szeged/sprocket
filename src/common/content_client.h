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
  // Returns the user agent.
  std::string GetUserAgent() const override;

  // Returns a string resource given its id.
  base::string16 GetLocalizedString(int message_id) const override;

  // Return the contents of a resource in a StringPiece given the resource id.
  base::StringPiece GetDataResource(
      int resource_id,
      ui::ScaleFactor scale_factor) const override;

  // Returns the raw bytes of a scale independent data resource.
  base::RefCountedMemory* GetDataResourceBytes(
      int resource_id) const override;

  // Returns a native image given its id.
  gfx::Image& GetNativeImageNamed(int resource_id) const override;
};

#endif  // SPROCKET_COMMON_CONTENT_CLIENT_H_
