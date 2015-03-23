// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_BROWSER_BROWSER_UI_MINI_BROWSER_H_
#define MINI_BROWSER_BROWSER_UI_MINI_BROWSER_H_


#include "base/callback_forward.h"
#include "base/memory/scoped_ptr.h"
#include "content/public/browser/web_contents_delegate.h"
#include "content/public/browser/web_contents_observer.h"
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
class MiniBrowserPlatformDataAura;

// This represents one window of the MiniBrowser, i.e. all the UI including
// buttons and url bar, as well as the web content area.

class MiniBrowser : public content::WebContentsDelegate,
          public content::WebContentsObserver {

public:

  ~MiniBrowser() override;

  void LoadURL(const GURL& url);
  bool CanGoBack();
  bool CanGoForward();
  void GoBackOrForward(int offset);
  void Reload();
  void Stop();
  void UpdateNavigationControls(bool to_different_document);
  void Close();

  static void Initialize();
  static MiniBrowser* CreateNewWindow(content::BrowserContext* browser_context,
                  const GURL& url,
                  content::SiteInstance* site_instance,
                  const gfx::Size& initial_size);

  static std::vector<MiniBrowser*>& windows() { return windows_; }
  content::WebContents* web_contents() const { return web_contents_.get(); }
  gfx::NativeWindow window() { return window_; }

    // content::WebContentsDelegate overrides.
  content::WebContents* OpenURLFromTab(
    content::WebContents* source,
    const content::OpenURLParams& params) override;
  void AddNewContents(content::WebContents* source,
            content::WebContents* new_contents,
            WindowOpenDisposition disposition,
            const gfx::Rect& initial_rect,
            bool user_gesture,
            bool* was_blocked) override;
  void LoadingStateChanged(content::WebContents* source,
              bool to_different_document) override;
  void CloseContents(content::WebContents* source) override;
  bool CanOverscrollContent() const override;
  void DidNavigateMainFramePostCommit(content::WebContents* web_contents) override;
  void ActivateContents(content::WebContents* contents) override;
  void DeactivateContents(content::WebContents* contents) override;
  bool HandleContextMenu(const content::ContextMenuParams& params) override;
  void WebContentsFocused(content::WebContents* contents) override;

  static gfx::Size GetMiniBrowserDefaultSize();
private:
  enum UIControl {
    BACK_BUTTON,
    FORWARD_BUTTON,
    STOP_BUTTON
  };

  explicit MiniBrowser(content::WebContents* web_contents);

  static MiniBrowser* CreateMiniBrowser(content::WebContents* web_contents,
                const gfx::Size& initial_size);
  // Helper for one time initialization of application
  static void PlatformInitialize(const gfx::Size& default_window_size);
  // Helper for one time deinitialization of platform specific state.
  static void PlatformExit();

  static gfx::Size AdjustWindowSize(const gfx::Size& initial_size);

  // All the methods that begin with Platform need to be implemented by the
  // platform specific MiniBrowser implementation.
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

  // content::WebContentsObserver overrides.
  void TitleWasSet(content::NavigationEntry* entry, bool explicit_set) override;

  scoped_ptr<content::WebContents> web_contents_;

  gfx::NativeWindow window_;
  gfx::Size content_size_;

  static views::ViewsDelegate* views_delegate_;
  views::Widget* window_widget_;

  static std::vector<MiniBrowser*> windows_;

  // True if the destructur of MiniBrowser should post a quit closure on the current
  // message loop if the destructed MiniBrowser object was the last one.
  static bool quit_message_loop_;
};

#endif // MINI_BROWSER_BROWSER_UI_MINI_BROWSER_H_