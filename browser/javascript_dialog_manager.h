// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SPROCKET_BROWSER_JAVASCRIPT_DIALOG_MANAGER_H_
#define SPROCKET_BROWSER_JAVASCRIPT_DIALOG_MANAGER_H_

#include "content/public/browser/javascript_dialog_manager.h"

// An interface consisting of methods that can be called to produce and manage
// JavaScript dialogs.
class SprocketJavaScriptDialogManager : public content::JavaScriptDialogManager {
 public:
  // Displays a JavaScript dialog. |did_suppress_message| will not be nil; if
  // |true| is returned in it, the caller will handle faking the reply.
  void RunJavaScriptDialog(
      content::WebContents* web_contents,
      const GURL& origin_url,
      const std::string& accept_lang,
      content::JavaScriptMessageType javascript_message_type,
      const base::string16& message_text,
      const base::string16& default_prompt_text,
      const DialogClosedCallback& callback,
      bool* did_suppress_message) override;

  // Displays a dialog asking the user if they want to leave a page.
  void RunBeforeUnloadDialog(content::WebContents* web_contents,
                                     const base::string16& message_text,
                                     bool is_reload,
                                     const DialogClosedCallback& callback) override;

  // Accepts or dismisses the active JavaScript dialog, which must be owned
  // by the given |web_contents|. If |prompt_override| is not null, the prompt
  // text of the dialog should be set before accepting. Returns true if the
  // dialog was handled.
  bool HandleJavaScriptDialog(content::WebContents* web_contents,
                                      bool accept,
                                      const base::string16* prompt_override) override;

  // Cancels all active and pending dialogs for the given WebContents.
  void CancelActiveAndPendingDialogs(content::WebContents* web_contents) override;

  // Reset any saved state tied to the given WebContents.
  void ResetDialogState(content::WebContents* web_contents) override;
};

#endif  // SPROCKET_BROWSER_JAVASCRIPT_DIALOG_MANAGER_H_
