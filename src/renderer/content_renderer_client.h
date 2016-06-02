// Copyright (c) 2016 University of Szeged.
// Copyright (c) 2016 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SPROCKET_RENDERER_CONTENT_RENDERER_CLIENT_H_
#define SPROCKET_RENDERER_CONTENT_RENDERER_CLIENT_H_

#include "content/public/renderer/content_renderer_client.h"

// Embedder API for participating in renderer logic.
class SprocketContentRendererClient : public content::ContentRendererClient {
 public:
  SprocketContentRendererClient();
  ~SprocketContentRendererClient() override;

  // ContentRendererClient overrides

  // Returns true if the embedder has an error page to show for the given http
  // status code. If so |error_domain| should be set to according to WebURLError
  // and the embedder's GetNavigationErrorStrings will be called afterwards to get
  // the error html.
  bool HasErrorPage(int http_status_code,
                    std::string* error_domain) override;

  // Returns true if the embedder prefers not to show an error page for a failed
  // navigation to |url| in |render_frame|.
  bool ShouldSuppressErrorPage(content::RenderFrame* render_frame,
                               const GURL& url) override;

  // Returns the information to display when a navigation error occurs.
  // If |error_html| is not null then it may be set to a HTML page containing
  // the details of the error and maybe links to more info.
  // If |error_description| is not null it may be set to contain a brief
  // message describing the error that has occurred.
  // Either of the out parameters may be not written to in certain cases
  // (lack of information on the error code) so the caller should take care to
  // initialize the string values with safe defaults before the call.
  void GetNavigationErrorStrings(
      content::RenderFrame* render_frame,
      const blink::WebURLRequest& failed_request,
      const blink::WebURLError& error,
      std::string* error_html,
      base::string16* error_description) override;
};

#endif  // SPROCKET_RENDERER_CONTENT_RENDERER_CLIENT_H_
