// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/browser/browser_main.h"

#include "base/memory/scoped_ptr.h"
#include "content/public/browser/browser_main_runner.h"

#if defined(OS_ANDROID)
#include "base/run_loop.h"
#endif

// Main routine for running as the Browser process.
int BrowserMain(
    const content::MainFunctionParams& parameters,
    const scoped_ptr<content::BrowserMainRunner>& main_runner) {
  int exit_code = main_runner->Initialize(parameters);

  if (exit_code >= 0)
    return exit_code;

#if !defined(OS_ANDROID)
  exit_code = main_runner->Run();

  main_runner->Shutdown();
#endif

  return exit_code;
}
