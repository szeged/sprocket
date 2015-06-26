// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SPROCKET_BROWSER_WEB_CONTENTS_H_
#define SPROCKET_BROWSER_WEB_CONTENTS_H_

#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_delegate.h"

class SprocketWindow;
#if defined(USE_AURA)
class Tab;
#endif

namespace content {
  class BrowserContext;
  class SiteInstance;
}

// WebContentsDelegate: Objects implement this interface to get notified about
// changes in the WebContents and to provide necessary functionality.

class SprocketWebContents : public content::WebContentsDelegate {

public:
  ~SprocketWebContents() override;

  static SprocketWebContents* CreateSprocketWebContents(
    SprocketWindow* window,
    content::BrowserContext* browser_context,
    const GURL& url,
    const gfx::Size& initial_size);
  static SprocketWebContents* AdoptWebContents(
    SprocketWindow* window,
    content::WebContents* web_contents);

  SprocketWindow* Window() { return window_; }

  void LoadURL(const GURL& url);
  bool CanGoBack();
  bool CanGoForward();
  void GoBackOrForward(int offset);
  void Reload();
  void Stop();
  void UpdateNavigationControls(bool to_different_document);
  void Close();

  // content::WebContentsDelegate overrides.

  // Opens a new URL inside the passed in WebContents (if |source| is 0 open
  // in the current front-most tab), unless |disposition| indicates the url
  // should be opened in a new tab or window. Returns the WebContents the URL
  // is opened in, or nullptr if the URL wasn't opened immediately.
  content::WebContents* OpenURLFromTab(
    content::WebContents* source,
    const content::OpenURLParams& params) override;

  // Called to inform the delegate that the WebContents's navigation state
  // changed. The |changed_flags| indicates the parts of the navigation state
  // that have been updated.
  void NavigationStateChanged(content::WebContents* source,
            content::InvalidateTypes changed_flags) override;

  // Creates a new tab with the already-created WebContents 'new_contents'.
  // The window for the added contents should be reparented correctly when this
  // method returns. If |disposition| is NEW_POPUP, |initial_rect| should hold
  // the initial position and size. If |was_blocked| is non-nullptr, then
  // |*was_blocked| will be set to true if the popup gets blocked, and left
  // unchanged otherwise.
  void AddNewContents(content::WebContents* source,
            content::WebContents* new_contents,
            WindowOpenDisposition disposition,
            const gfx::Rect& initial_rect,
            bool user_gesture,
            bool* was_blocked) override;

  // Notifies the delegate that this contents is starting or is done loading
  // some resource. The delegate should use this notification to represent
  // loading feedback. See WebContents::IsLoading()
  // |to_different_document| will be true unless the load is a fragment
  // navigation, or triggered by history.pushState/replaceState.
  void LoadingStateChanged(content::WebContents* source,
              bool to_different_document) override;

  // Notifies the delegate that the page has made some progress loading.
  // |progress| is a value between 0.0 (nothing loaded) to 1.0 (page fully
  // loaded).
  void LoadProgressChanged(content::WebContents* source, double progress) override;

  void EnterFullscreenModeForTab(content::WebContents* web_contents,
                                 const GURL& origin) override;
  void ExitFullscreenModeForTab(content::WebContents* web_contents) override;
  bool IsFullscreenForTabOrPending(
      const content::WebContents* web_contents) const override;

  // Request the delegate to close this web contents, and do whatever cleanup
  // it needs to do.
  void CloseContents(content::WebContents* source) override;

  // Called to determine if the WebContents can be overscrolled with touch/wheel
  // gestures.
  bool CanOverscrollContent() const override;

  // Invoked when a main fram navigation occurs.
  void DidNavigateMainFramePostCommit(content::WebContents* web_contents) override;

  // Selects the specified contents, bringing its container to the front.
  void ActivateContents(content::WebContents* contents) override;

  // Deactivates the specified contents by deactivating its container and
  // potentialy moving it to the back of the Z order.
  void DeactivateContents(content::WebContents* contents) override;

  // Returns true if the context menu operation was handled by the delegate.
  bool HandleContextMenu(const content::ContextMenuParams& params) override;



  content::WebContents* web_contents() const { return web_contents_.get(); }
#if defined(USE_AURA)
  Tab* tab() const { return tab_; }
  void SetTab(Tab* tab) { tab_ = tab; }
#endif
private:
  explicit SprocketWebContents(SprocketWindow* window,
                               content::WebContents* web_contents);

  void ToggleFullscreenModeForTab(content::WebContents* web_contents,
                                  bool enter_fullscreen);

  scoped_ptr<content::WebContents> web_contents_;
  SprocketWindow* window_;
#if defined(USE_AURA)
  Tab* tab_;
#endif

  bool is_fullscreen_;
};

#endif // SPROCKET_BROWSER_UI_WEB_CONTENTS_H_
