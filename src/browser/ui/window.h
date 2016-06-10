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
#endif

class SprocketWebContents;
class GURL;

namespace content {
class WebContents;
struct ContextMenuParams;
struct NativeWebKeyboardEvent;
}

class SprocketWindow {

 public:

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
  void PlatformSetContents(SprocketWebContents* sprocket_web_contents);
  // Set the title of window
  void PlatformSetTitle(const base::string16& title);

  void PlatformToggleFullscreenModeForTab(bool enter_fullscreen);
  bool PlatformIsFullscreenForTabOrPending() const;
#if defined(USE_AURA)
  bool PlatformWasFullscreenForTab() const;
#endif
  void PlatformHandleKeyboardEvent(const content::NativeWebKeyboardEvent& event);

  SprocketWebContents* sprocket_web_contents_;

#if defined(OS_ANDROID)
  base::android::ScopedJavaGlobalRef<jobject> java_object_;
#elif defined(USE_AURA)
  gfx::NativeWindow window_;
  gfx::Size content_size_;
  static views::ViewsDelegate* views_delegate_;
  views::Widget* window_widget_;

  friend class SprocketWindowDelegateView;

  bool is_fullscreen_;
  bool was_fullscreen_;
#endif

  static std::set<SprocketWindow*> windows_;

  friend class SprocketWebContents;
};

#endif // SPROCKET_BROWSER_UI_WINDOW_H_
