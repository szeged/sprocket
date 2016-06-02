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
#include "sprocket/renderer/content_renderer_client.h"
#include "sprocket/common/switches.h"
#include "ui/base/resource/resource_bundle.h"

#if defined(OS_ANDROID)
#include "base/android/apk_assets.h"
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

  std::string process_type = command_line->GetSwitchValueASCII(switches::kProcessType);
  if (process_type == switches::kBrowserProcess &&
      !command_line->HasSwitch(switches::kUseSandbox)) {
#if !defined(OS_ANDROID)
    command_line->AppendSwitch(switches::kNoSandbox);
#endif
  }

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

content::ContentBrowserClient* SprocketMainDelegate::CreateContentBrowserClient() {
  browser_client_.reset(new SprocketContentBrowserClient);
  return browser_client_.get();
}

content::ContentRendererClient* SprocketMainDelegate::CreateContentRendererClient() {
  renderer_client_.reset(new SprocketContentRendererClient);
  return renderer_client_.get();
}

// static
void SprocketMainDelegate::InitializeResourceBundle() {
#if defined(OS_ANDROID)
  // On Android, the renderer runs with a different UID and can never access
  // the file system. Use the file descriptor passed in at launch time.
  auto* global_descriptors = base::GlobalDescriptors::GetInstance();
  int pak_fd = global_descriptors->MaybeGet(kSprocketPakDescriptor);
  base::MemoryMappedFile::Region pak_region;
  if (pak_fd >= 0) {
    pak_region = global_descriptors->GetRegion(kSprocketPakDescriptor);
  } else {
    pak_fd =
        base::android::OpenApkAsset("assets/sprocket.pak", &pak_region);
    // Loaded from disk for browsertests.
    if (pak_fd < 0) {
      base::FilePath pak_file;
      bool r = PathService::Get(base::DIR_ANDROID_APP_DATA, &pak_file);
      DCHECK(r);
      pak_file = pak_file.Append(FILE_PATH_LITERAL("paks"));
      pak_file = pak_file.Append(FILE_PATH_LITERAL("sprocket.pak"));
      int flags = base::File::FLAG_OPEN | base::File::FLAG_READ;
      pak_fd = base::File(pak_file, flags).TakePlatformFile();
      pak_region = base::MemoryMappedFile::Region::kWholeFile;
    }
    global_descriptors->Set(kSprocketPakDescriptor, pak_fd, pak_region);
  }
  DCHECK_GE(pak_fd, 0);
  // This is clearly wrong. See crbug.com/330930
  ui::ResourceBundle::InitSharedInstanceWithPakFileRegion(base::File(pak_fd),
                                                          pak_region);
  ui::ResourceBundle::GetSharedInstance().AddDataPackFromFileRegion(
      base::File(pak_fd), pak_region, ui::SCALE_FACTOR_100P);
#else
  base::FilePath pak_file;
  bool r = PathService::Get(base::DIR_MODULE, &pak_file);
  DCHECK(r);
  pak_file = pak_file.Append(FILE_PATH_LITERAL("sprocket.pak"));
  ui::ResourceBundle::InitSharedInstanceWithPakPath(pak_file);
#endif
}
