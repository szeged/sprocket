// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/app/main_delegate.h"

#include "base/command_line.h"
#include "base/cpu.h"
#include "base/files/file.h"
#include "base/files/file_path.h"
#include "base/path_service.h"
#include "content/public/browser/browser_main_runner.h"
#include "content/public/common/content_switches.h"
#include "sprocket/browser/browser_main.h"
#include "sprocket/browser/content_browser_client.h"
#include "ui/base/resource/resource_bundle.h"

#if defined(OS_ANDROID)
#include "base/posix/global_descriptors.h"
#include "sprocket/android/descriptors.h"
#endif

SprocketMainDelegate::SprocketMainDelegate() {
}

SprocketMainDelegate::~SprocketMainDelegate() {
}

bool SprocketMainDelegate::BasicStartupComplete(int* exit_code) {
#if defined(ARCH_CPU_ARM_FAMILY) && (defined(OS_ANDROID) || defined(OS_LINUX))
  // Create an instance of the CPU class to parse /proc/cpuinfo and cache
  // cpu_brand info.
  base::CPU cpu_info;
#endif

  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();

  // We do not support the sandbox for now.
  command_line->AppendSwitch(switches::kNoSandbox);

  // TODO: Do we really need this?
#if defined(OS_ANDROID)
    command_line->AppendSwitch(
        switches::kDisableGestureRequirementForMediaPlayback);
#endif

  InitializeResourceBundle();

  // The client should be set early, before any content code is called.
  content::SetContentClient(&content_client_);
  return false;
}

int SprocketMainDelegate::RunProcess(
    const std::string& process_type,
    const content::MainFunctionParams& main_function_params) {

    // When |process_type| is empty, that means we should start a browser process.
    // We use the default behavior in the case of other types of processes.
    if (!process_type.empty())
      return -1;

#if !defined(OS_ANDROID)
  // Android stores the BrowserMainRunner instance as a scoped member pointer
  // on the SprocketMainDelegate class because of different object lifetime.
  scoped_ptr<content::BrowserMainRunner> browser_runner_;
#endif

    browser_runner_.reset(content::BrowserMainRunner::Create());

    return BrowserMain(main_function_params, browser_runner_);
}

content::ContentBrowserClient*
SprocketMainDelegate::CreateContentBrowserClient() {
  browser_client_.reset(new SprocketContentBrowserClient);
  return browser_client_.get();
}

// static
void SprocketMainDelegate::InitializeResourceBundle() {
#if defined(OS_ANDROID)
  int pak_fd =
      base::GlobalDescriptors::GetInstance()->MaybeGet(kSprocketPakDescriptor);
  if (pak_fd >= 0) {
    ui::ResourceBundle::InitSharedInstanceWithPakFileRegion(
        base::File(pak_fd), base::MemoryMappedFile::Region::kWholeFile);
    ResourceBundle::GetSharedInstance().AddDataPackFromFile(
        base::File(pak_fd), ui::SCALE_FACTOR_100P);
    return;
  }
#endif
  base::FilePath pak_file;
  base::FilePath pak_dir;

#if defined(OS_ANDROID)
  bool got_path = PathService::Get(base::DIR_ANDROID_APP_DATA, &pak_dir);
  DCHECK(got_path);
  pak_dir = pak_dir.Append(FILE_PATH_LITERAL("paks"));
#else
  PathService::Get(base::DIR_MODULE, &pak_dir);
#endif
  pak_file = pak_dir.Append(FILE_PATH_LITERAL("sprocket.pak"));
  ui::ResourceBundle::InitSharedInstanceWithPakPath(pak_file);
}
