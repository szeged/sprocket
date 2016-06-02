// Copyright (c) 2016 University of Szeged.
// Copyright (c) 2016 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/renderer/content_renderer_client.h"

#include "base/values.h"
#include "grit/sprocket_resources.h"
#include "net/base/net_errors.h"
#include "public/platform/WebURLError.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/base/webui/jstemplate_builder.h"


SprocketContentRendererClient::SprocketContentRendererClient() {
}

SprocketContentRendererClient::~SprocketContentRendererClient() {
}

bool SprocketContentRendererClient::HasErrorPage(int http_status_code,
                                                 std::string* error_domain) {
  return true;
}

bool SprocketContentRendererClient::ShouldSuppressErrorPage(content::RenderFrame* render_frame,
                                                            const GURL& url) {
  return false;
}


void SprocketContentRendererClient::GetNavigationErrorStrings(
      content::RenderFrame* render_frame,
      const blink::WebURLRequest& failed_request,
      const blink::WebURLError& error,
      std::string* error_html,
      base::string16* error_description) {
  if (error_html) {
    const base::StringPiece template_html(ui::ResourceBundle::GetSharedInstance().GetRawDataResource(IDR_SPROCKET_ERROR_PAGE));
    if (template_html.empty())
      NOTREACHED() << "Unable to load template. ID: " << IDR_SPROCKET_ERROR_PAGE;
    else {
      base::DictionaryValue error_strings;
      error_strings.SetString("error_message", net::ErrorToShortString(error.reason));
      *error_html = webui::GetTemplatesHtml(template_html, &error_strings, "error_page_root_node");
    }
  }
}
