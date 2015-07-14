// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/browser/browser_main_parts.h"

#include "base/base_switches.h"
#include "base/bind.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/message_loop/message_loop.h"
#include "content/public/common/main_function_params.h"
#include "content/public/common/url_constants.h"
#include "sprocket/browser/web_contents.h"
#include "sprocket/browser/ui/window.h"
#include "sprocket/browser/browser_context.h"
#include "net/base/filename_util.h"
#include "net/base/net_module.h"
#include "net/grit/net_resources.h"
#include "ui/base/resource/resource_bundle.h"
#include "url/gurl.h"

#if defined(OS_ANDROID)
#include "net/android/network_change_notifier_factory_android.h"
#include "net/base/network_change_notifier.h"
#endif

#if defined(USE_AURA) && defined(OS_LINUX)
#include "ui/base/ime/input_method_initializer.h"
#endif

namespace {

GURL GetStartupURL() {
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  const base::CommandLine::StringVector& args = command_line->GetArgs();

#if defined(OS_ANDROID)
  // Delay renderer creation on Android until surface is ready.
  return GURL();
#endif

  if (args.empty())
    return GURL("https://www.google.com/");

  GURL url(args[0]);
  if (url.is_valid() && url.has_scheme())
    return url;

  return net::FilePathToFileURL(base::FilePath(args[0]));
}

base::StringPiece NetResourceProvider(int key) {
  if (key == IDR_DIR_HEADER_HTML) {
    base::StringPiece html_data =
      ui::ResourceBundle::GetSharedInstance().GetRawDataResource(
        IDR_DIR_HEADER_HTML);
    return html_data;
  }
  return base::StringPiece();
}

} // namespace


SprocketBrowserMainParts::SprocketBrowserMainParts(
    const content::MainFunctionParams& parameters)
    : parameters_(parameters),
  run_message_loop_(true) {
}

SprocketBrowserMainParts::~SprocketBrowserMainParts() {
}

void SprocketBrowserMainParts::PreEarlyInitialization() {
#if defined(USE_AURA) && defined(OS_LINUX)
  ui::InitializeInputMethodForTesting();
#endif
#if defined(OS_ANDROID)
  net::NetworkChangeNotifier::SetFactory(
      new net::NetworkChangeNotifierFactoryAndroid());
#endif
}

void SprocketBrowserMainParts::PostMainMessageLoopStart() {
#if defined(OS_ANDROID)
  base::MessageLoopForUI::current()->Start();
#endif
}

void SprocketBrowserMainParts::InitializeBrowserContexts() {
  set_browser_context(new SprocketBrowserContext(false));
  set_off_the_record_browser_context(new SprocketBrowserContext(true));
}

void SprocketBrowserMainParts::InitializeMessageLoopContext() {
  SprocketWindow* window = SprocketWindow::CreateNewWindow(gfx::Size());
  SprocketWebContents::CreateSprocketWebContents(window,
                                                 browser_context_.get(),
                                                 GetStartupURL(),
                                                 gfx::Size());
}

void SprocketBrowserMainParts::PreMainMessageLoopRun() {
  InitializeBrowserContexts();

  SprocketWindow::Initialize();

  net::NetModule::SetResourceProvider(NetResourceProvider);

  InitializeMessageLoopContext();
}

bool SprocketBrowserMainParts::MainMessageLoopRun(int* result_code) {
  return !run_message_loop_;
}

void SprocketBrowserMainParts::PostMainMessageLoopRun() {
  SprocketWindow::Deinitialize();
  browser_context_.reset();
  off_the_record_browser_context_.reset();
}
