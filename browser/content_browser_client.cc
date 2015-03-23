// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "minibrowser/browser/content_browser_client.h"

#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/path_service.h"
#include "content/public/browser/resource_dispatcher_host.h"
#include "content/public/common/content_descriptors.h"
#include "content/public/common/url_constants.h"
#include "minibrowser/browser/browser_context.h"
#include "minibrowser/browser/browser_main_parts.h"
#include "gin/public/isolate_holder.h"
#include "net/url_request/url_request_context_getter.h"
#include "url/gurl.h"


MiniBrowserContentBrowserClient::MiniBrowserContentBrowserClient()
  : v8_natives_fd_(-1),
    v8_snapshot_fd_(-1),
    browser_main_parts_(NULL) {
}

MiniBrowserContentBrowserClient::~MiniBrowserContentBrowserClient() {
}

content::BrowserMainParts*
MiniBrowserContentBrowserClient::CreateBrowserMainParts(
  const content::MainFunctionParams& parameters) {
  browser_main_parts_ = new MiniBrowserBrowserMainParts(parameters);
  return browser_main_parts_;
}

net::URLRequestContextGetter*
MiniBrowserContentBrowserClient::CreateRequestContext(
  content::BrowserContext* browser_context,
  content::ProtocolHandlerMap* protocol_handlers,
  content::URLRequestInterceptorScopedVector request_interceptors) {
    MiniBrowserBrowserContext* minibrowser_browser_context =
      MiniBrowserBrowserContextForBrowserContext(browser_context);
    return minibrowser_browser_context->CreateRequestContext(
      protocol_handlers, request_interceptors.Pass());
}

bool MiniBrowserContentBrowserClient::IsHandledURL(const GURL& url) {
  if (!url.is_valid()) {
    return false;
  }

  static const char* const kProtocolList[] = {
    url::kBlobScheme,
    url::kFileSystemScheme,
    content::kChromeUIScheme,
    url::kDataScheme,
    url::kFileScheme,
  };
  for (size_t i = 0; i < arraysize(kProtocolList); ++i) {
    if (url.scheme() == kProtocolList[i])
      return true;
  }
  return false;
}

std::string MiniBrowserContentBrowserClient::GetDefaultDownloadName() {
  return "download";
}

void MiniBrowserContentBrowserClient::GetAdditionalMappedFilesForChildProcess(
    const base::CommandLine& command_line,
    int child_process_id,
    content::FileDescriptorInfo* mappings) {
  if (v8_snapshot_fd_.get() == -1 && v8_natives_fd_.get() == -1) {
    base::FilePath v8_data_path;
    PathService::Get(gin::IsolateHolder::kV8SnapshotBasePathKey, &v8_data_path);
    DCHECK(!v8_data_path.empty());

    int file_flags = base::File::FLAG_OPEN | base::File::FLAG_READ;
    base::FilePath v8_natives_data_path =
        v8_data_path.AppendASCII(gin::IsolateHolder::kNativesFileName);
    base::FilePath v8_snapshot_data_path =
        v8_data_path.AppendASCII(gin::IsolateHolder::kSnapshotFileName);
    base::File v8_natives_data_file(v8_natives_data_path, file_flags);
    base::File v8_snapshot_data_file(v8_snapshot_data_path, file_flags);
    DCHECK(v8_natives_data_file.IsValid());
    DCHECK(v8_snapshot_data_file.IsValid());
    v8_natives_fd_.reset(v8_natives_data_file.TakePlatformFile());
    v8_snapshot_fd_.reset(v8_snapshot_data_file.TakePlatformFile());
  }
  mappings->Share(kV8NativesDataDescriptor, v8_natives_fd_.get());
  mappings->Share(kV8SnapshotDataDescriptor, v8_snapshot_fd_.get());
}

MiniBrowserBrowserContext*
MiniBrowserContentBrowserClient::browser_context() {
  return browser_main_parts_->browser_context();
}

MiniBrowserBrowserContext*
MiniBrowserContentBrowserClient::off_the_record_browser_context() {
  return browser_main_parts_->off_the_record_browser_context();
}

MiniBrowserBrowserContext*
MiniBrowserContentBrowserClient::MiniBrowserBrowserContextForBrowserContext(
    content::BrowserContext* content_browser_context) {
  if (content_browser_context == browser_context())
    return browser_context();
  DCHECK_EQ(content_browser_context, off_the_record_browser_context());
  return off_the_record_browser_context();
}