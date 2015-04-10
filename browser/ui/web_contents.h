// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SPROCKET_BROWSER_UI_WEB_CONTENTS_H_
#define SPROCKET_BROWSER_UI_WEB_CONTENTS_H_


#include "base/callback_forward.h"
#include "base/memory/scoped_ptr.h"
#include "content/public/browser/web_contents_delegate.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/native_widget_types.h"

namespace views {
class Widget;
class ViewsDelegate;
}

namespace content {
class BrowserContext;
class SiteInstance;
class WebContents;
}

class GURL;
class SprocketPlatformDataAura;

// This represents one window of the SprocketWebContents, i.e. all the UI including
// buttons and url bar, as well as the web content area.

// WebContentsDelegate: Objects implement this interfacee to get notified about
// changes in the WebContents and to provide necessary functionality.

class SprocketWebContents : public content::WebContentsDelegate {

public:

  ~SprocketWebContents() override;

  void LoadURL(const GURL& url);
  bool CanGoBack();
  bool CanGoForward();
  void GoBackOrForward(int offset);
  void Reload();
  void Stop();
  void UpdateNavigationControls(bool to_different_document);
  void Close();

  static void Initialize();
  static SprocketWebContents* CreateNewWindow(content::BrowserContext* browser_context,
                  const GURL& url,
                  content::SiteInstance* site_instance,
                  const gfx::Size& initial_size);

  static std::vector<SprocketWebContents*>& windows() { return windows_; }
  content::WebContents* web_contents() const { return web_contents_.get(); }
  gfx::NativeWindow window() { return window_; }

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

  // Notification that |contents| has gained focus.
  void WebContentsFocused(content::WebContents* contents) override;

  // TODO: This should not be a member.
  static gfx::Size GetSprocketWebContentsDefaultSize();
private:
  enum UIControl {
    BACK_BUTTON,
    FORWARD_BUTTON,
    STOP_BUTTON
  };

  explicit SprocketWebContents(content::WebContents* web_contents);

  static SprocketWebContents* CreateSprocketWebContents(content::WebContents* web_contents,
                const gfx::Size& initial_size);
  // Helper for one time initialization of application
  static void PlatformInitialize(const gfx::Size& default_window_size);
  // Helper for one time deinitialization of platform specific state.
  static void PlatformExit();

  static gfx::Size AdjustWindowSize(const gfx::Size& initial_size);

  // All the methods that begin with Platform need to be implemented by the
  // platform specific SprocketWebContents implementation.
  // Called from the destructor to let each platform do any necessary cleanup.
  void PlatformCleanUp();
  // Creates the main window GUI.
  void PlatformCreateWindow(int width, int height);
  // Links the WebContents into the newly created window.
  void PlatformSetContents();
  // Resize the content area and GUI.
  void PlatformResizeSubViews();
  // Enable/disable a button.
  void PlatformEnableUIControl(UIControl control, bool is_enabled);
  // Updates the url in the url bar.
  void PlatformSetAddressBarURL(const GURL& url);
  // Sets whether the spinner is spinning.
  void PlatformSetIsLoading(bool loading);
  // Set the title of window
  void PlatformSetTitle(const base::string16& title);
  // User right-clicked on the web view
  bool PlatformHandleContextMenu(const content::ContextMenuParams& params);

  void PlatformWebContentsFocused(content::WebContents* contents);

  gfx::NativeView GetContentView();


  scoped_ptr<content::WebContents> web_contents_;

  gfx::NativeWindow window_;
  gfx::Size content_size_;

  static views::ViewsDelegate* views_delegate_;
  views::Widget* window_widget_;

  static std::vector<SprocketWebContents*> windows_;

  // True if the destructur of SprocketWebContents should post a quit closure on the current
  // message loop if the destructed SprocketWebContents object was the last one.
  static bool quit_message_loop_;
};

#endif // SPROCKET_BROWSER_UI_WEB_CONTENTS_H_
