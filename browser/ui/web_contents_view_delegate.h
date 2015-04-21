// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_SPROCKET_BROWSER_SPROCKET_WEB_CONTENTS_VIEW_DELEGATE_H_
#define CONTENT_SPROCKET_BROWSER_SPROCKET_WEB_CONTENTS_VIEW_DELEGATE_H_

#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_view_delegate.h"
#include "content/public/common/context_menu_params.h"

namespace content {
class WebContents;
class WebContentsViewDelegate;
}

content::WebContentsViewDelegate* CreateSprocketWebContentsViewDelegate(
    content::WebContents* web_contents);

class SprocketWebContentsViewDelegate : public content::WebContentsViewDelegate {
 public:
  explicit SprocketWebContentsViewDelegate(content::WebContents* web_contents);
  ~SprocketWebContentsViewDelegate() override;

  // Overridden from WebContentsViewDelegate:
  void ShowContextMenu(content::RenderFrameHost* render_frame_host,
                       const content::ContextMenuParams& params) override;

 private:
  content::WebContents* web_contents_;
  content::ContextMenuParams params_;

  DISALLOW_COPY_AND_ASSIGN(SprocketWebContentsViewDelegate);
};

#endif  // CONTENT_SPROCKET_BROWSER_SPROCKET_WEB_CONTENTS_VIEW_DELEGATE_H_
