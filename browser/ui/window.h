// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SPROCKET_BROWSER_UI_WINDOW_H_
#define SPROCKET_BROWSER_UI_WINDOW_H_

#include "base/memory/scoped_ptr.h"
#include "base/strings/string16.h"
#include "ui/gfx/geometry/size.h"
#include <set>

#if defined(OS_ANDROID)
#include "base/android/scoped_java_ref.h"
#elif defined(USE_AURA)
#include "ui/gfx/native_widget_types.h"
namespace views {
  class ViewsDelegate;
  class Widget;
}

class Tab;
#endif

class SprocketWebContents;
class GURL;

namespace content {
  class WebContents;
  struct ContextMenuParams;
}

class SprocketWindow {

public:
  enum UIControl {
    BACK_BUTTON,
    FORWARD_BUTTON,
    STOP_BUTTON
  };

  static void Initialize();
  static void Deinitialize();

  ~SprocketWindow();

  static SprocketWindow* CreateNewWindow(const gfx::Size& initial_size);

  void Close();

  static gfx::Size AdjustWindowSize(const gfx::Size& initial_size);

  static gfx::Size GetSprocketWindowDefaultSize();

#if defined(OS_ANDROID)
  static void CloseSprocketWebContents(JNIEnv*, jclass, jlong);
  // Registers the Android Java to native methods.
  static bool Register(JNIEnv* env);
#endif

private:
  explicit SprocketWindow();

  // All the methods that begin with Platform need to be implemented by the
  // platform specific SprocketWindow implementation.

  // Helper for one time initialization of application
  static void PlatformInitialize();
  // Helper for one time deinitialization of platform specific state.
  static void PlatformExit();
  // Called from the destructor to let each platform do any necessary cleanup.
  void PlatformCleanUp();
  // Creates the main window GUI.
  void PlatformCreateWindow(int width, int height);
  // Closes the window.
  void PlatformCloseWindow();
  // Links the WebContents into the newly created window.
  void PlatformAddTab(SprocketWebContents* sprocket_web_contents);
  void PlatformSelectTabAt(int index);
#if defined(USE_AURA)
  void PlatformSelectTab(Tab* tab);
#endif
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
  // Changes the load progress
  void PlatformLoadProgressChanged(double progress);

#if defined(OS_ANDROID)
  void PlatformToggleFullscreenModeForTab(content::WebContents* web_contents,
                                          bool enter_fullscreen);
  bool PlatformIsFullscreenForTabOrPending(
      const content::WebContents* web_contents) const;
#endif

  SprocketWebContents* sprocket_web_contents_;

#if defined(OS_ANDROID)
  base::android::ScopedJavaGlobalRef<jobject> java_object_;
#elif defined(USE_AURA)
  gfx::NativeWindow window_;
  gfx::Size content_size_;
  static views::ViewsDelegate* views_delegate_;
  views::Widget* window_widget_;

  friend class SprocketWindowDelegateView;
#endif

  static std::set<SprocketWindow*> windows_;

  friend class SprocketWebContents;
};

#endif // SPROCKET_BROWSER_UI_WINDOW_H_
