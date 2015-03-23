// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "minibrowser/browser/browser_main_parts.h"

#include "base/base_switches.h"
#include "base/bind.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/message_loop/message_loop.h"
#include "content/public/common/main_function_params.h"
#include "content/public/common/url_constants.h"
#include "minibrowser/browser/ui/minibrowser.h"
#include "minibrowser/browser/browser_context.h"
#include "net/base/filename_util.h"
#include "net/base/net_module.h"
#include "net/grit/net_resources.h"
#include "ui/base/ime/input_method_initializer.h"
#include "ui/base/resource/resource_bundle.h"
#include "url/gurl.h"

namespace {

GURL GetStartupURL() {
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  const base::CommandLine::StringVector& args = command_line->GetArgs();

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


MiniBrowserBrowserMainParts::MiniBrowserBrowserMainParts(
  const content::MainFunctionParams& parameters)
  : parameters_(parameters),
    run_message_loop_(true) {
}

MiniBrowserBrowserMainParts::~MiniBrowserBrowserMainParts() {
}

void MiniBrowserBrowserMainParts::PreMainMessageLoopStart() {
}

void MiniBrowserBrowserMainParts::PostMainMessageLoopStart() {
}

void MiniBrowserBrowserMainParts::PreEarlyInitialization() {
  ui::InitializeInputMethodForTesting();
}

void MiniBrowserBrowserMainParts::InitializeBrowserContexts() {
  set_browser_context(new MiniBrowserBrowserContext(false));
  set_off_the_record_browser_context(
    new MiniBrowserBrowserContext(true));
}

void MiniBrowserBrowserMainParts::InitializeMessageLoopContext() {
  MiniBrowser::CreateNewWindow(browser_context_.get(),
               GetStartupURL(),
               NULL,
               gfx::Size());
}

void MiniBrowserBrowserMainParts::PreMainMessageLoopRun() {
  InitializeBrowserContexts();

  MiniBrowser::Initialize();

  net::NetModule::SetResourceProvider(NetResourceProvider);

  InitializeMessageLoopContext();
}

bool MiniBrowserBrowserMainParts::MainMessageLoopRun(int* result_code)  {
  return !run_message_loop_;
}

void MiniBrowserBrowserMainParts::PostMainMessageLoopRun() {
  browser_context_.reset();
  off_the_record_browser_context_.reset();
}