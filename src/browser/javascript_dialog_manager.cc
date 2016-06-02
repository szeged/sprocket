// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/browser/javascript_dialog_manager.h"

#include "base/strings/utf_string_conversions.h"
#include "sprocket/browser/ui/window.h"
#include "sprocket/browser/ui/javascript_dialog.h"
#include "sprocket/browser/web_contents.h"

using namespace base;
using namespace content;

void SprocketJavaScriptDialogManager::RunJavaScriptDialog(
    WebContents* web_contents,
    const GURL& origin_url,
    const std::string& accept_lang,
    JavaScriptMessageType javascript_message_type,
    const string16& message_text,
    const string16& default_prompt_text,
    const DialogClosedCallback& callback,
    bool* did_suppress_message) {
  if (SprocketWebContents* sprocket_web_contents = SprocketWebContents::From(web_contents)) {
    SprocketWindow* window = sprocket_web_contents->window();
    window->PlatformShowJavaScriptDialog(new SprocketJavaScriptDialog(
        javascript_message_type,
        message_text,
        default_prompt_text,
        callback));
  }
}

void SprocketJavaScriptDialogManager::RunBeforeUnloadDialog(
  WebContents* web_contents,
   const string16& message_text,
   bool is_reload,
   const DialogClosedCallback& callback) {
  if (SprocketWebContents* sprocket_web_contents = SprocketWebContents::From(web_contents)) {
    SprocketWindow* window = sprocket_web_contents->window();
    window->PlatformShowJavaScriptDialog(new SprocketJavaScriptDialog(
        content::JAVASCRIPT_MESSAGE_TYPE_CONFIRM,
        string16(UTF8ToUTF16("Are you sure you want to leave this page?")),
        string16(),
        callback));
  }
}

bool SprocketJavaScriptDialogManager::HandleJavaScriptDialog(
    content::WebContents* web_contents,
    bool accept,
    const base::string16* prompt_override) {
  return false;
}

void SprocketJavaScriptDialogManager::CancelActiveAndPendingDialogs(WebContents* web_contents) {
}

void SprocketJavaScriptDialogManager::ResetDialogState(WebContents* web_contents) {
}
