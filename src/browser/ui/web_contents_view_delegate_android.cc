// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/browser/ui/web_contents_view_delegate.h"

#include "content/public/browser/android/content_view_core.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/context_menu_params.h"

content::WebContentsViewDelegate* CreateSprocketWebContentsViewDelegate(
    content::WebContents* web_contents) {
  return new SprocketWebContentsViewDelegate(web_contents);
}


SprocketWebContentsViewDelegate::SprocketWebContentsViewDelegate(
    content::WebContents* web_contents)
    : web_contents_(web_contents) {
}

SprocketWebContentsViewDelegate::~SprocketWebContentsViewDelegate() {
}

void SprocketWebContentsViewDelegate::ShowContextMenu(
    content::RenderFrameHost* render_frame_host,
    const content::ContextMenuParams& params) {
  if (params.is_editable && params.selection_text.empty()) {
    content::ContentViewCore* content_view_core =
        content::ContentViewCore::FromWebContents(web_contents_);
    if (content_view_core) {
      content_view_core->ShowPastePopup(params.selection_start.x(),
                                        params.selection_start.y());
    }
  }
}
