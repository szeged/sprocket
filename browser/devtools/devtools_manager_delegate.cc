// Copyright (c) 2016 University of Szeged.
// Copyright (c) 2016 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/browser/devtools/devtools_manager_delegate.h"

#include "base/command_line.h"
#include "base/strings/string_number_conversions.h"
#include "components/devtools_http_handler/devtools_http_handler.h"
#include "content/public/browser/devtools_frontend_host.h"
#include "content/public/common/content_switches.h"
#include "grit/sprocket_resources.h"
#include "net/base/net_errors.h"
#include "net/socket/tcp_server_socket.h"
#include "sprocket/common/content_client.h"
#include "ui/base/resource/resource_bundle.h"

#if defined(OS_ANDROID)
#include "base/strings/stringprintf.h"
#include "content/public/browser/android/devtools_auth.h"
#include "content/public/common/user_agent.h"
#include "net/socket/unix_domain_server_socket_posix.h"
#endif

using devtools_http_handler::DevToolsHttpHandler;

namespace {

#if defined(OS_ANDROID)
const char kFrontEndURL[] =
    "http://chrome-devtools-frontend.appspot.com/serve_rev/%s/inspector.html";
#endif

const char kAddress[] = "127.0.0.1";
const int kBackLog = 10;
const int kPort = 5000;

#if defined(OS_ANDROID)
class UnixDomainServerSocketFactory
    : public DevToolsHttpHandler::ServerSocketFactory {
 public:
  explicit UnixDomainServerSocketFactory(const std::string& socket_name)
      : socket_name_(socket_name) {}

 private:
  // DevToolsHttpHandler::ServerSocketFactory.
  scoped_ptr<net::ServerSocket> CreateForHttpServer() override {
    scoped_ptr<net::ServerSocket> socket(
        new net::UnixDomainServerSocket(
            base::Bind(&content::CanUserConnectToDevTools),
            true /* use_abstract_namespace */));
    if (socket->ListenWithAddressAndPort(socket_name_, 0, kBackLog) != net::OK)
      return scoped_ptr<net::ServerSocket>();

    return socket;
  }

  std::string socket_name_;

  DISALLOW_COPY_AND_ASSIGN(UnixDomainServerSocketFactory);
};
#else
class TCPServerSocketFactory
    : public DevToolsHttpHandler::ServerSocketFactory {
 public:
  TCPServerSocketFactory(const std::string& address, uint16_t port)
      : address_(address), port_(port) {
  }

 private:
  // DevToolsHttpHandler::ServerSocketFactory.
  scoped_ptr<net::ServerSocket> CreateForHttpServer() override {
    scoped_ptr<net::ServerSocket> socket(
        new net::TCPServerSocket(nullptr, net::NetLog::Source()));
    if (socket->ListenWithAddressAndPort(address_, port_, kBackLog) != net::OK)
      return scoped_ptr<net::ServerSocket>();

    return socket;
  }

  std::string address_;
  uint16_t port_;

  DISALLOW_COPY_AND_ASSIGN(TCPServerSocketFactory);
};
#endif

scoped_ptr<DevToolsHttpHandler::ServerSocketFactory> CreateSocketFactory() {
#if defined(OS_ANDROID)
  const base::CommandLine& command_line =
      *base::CommandLine::ForCurrentProcess();
  std::string socket_name = "sprocket_devtools_remote";
  if (command_line.HasSwitch(switches::kRemoteDebuggingSocketName)) {
    socket_name = command_line.GetSwitchValueASCII(
        switches::kRemoteDebuggingSocketName);
  }
  return scoped_ptr<DevToolsHttpHandler::ServerSocketFactory>(
      new UnixDomainServerSocketFactory(socket_name));
#else
  return scoped_ptr<DevToolsHttpHandler::ServerSocketFactory>(
      new TCPServerSocketFactory(
            SprocketDevToolsHttpHandlerDelegate::host(),
            SprocketDevToolsHttpHandlerDelegate::port()));
#endif
}

}  // namespace


// SprocketDevToolsHttpHandlerDelegate ----------------------------------------------------

// static
std::string SprocketDevToolsHttpHandlerDelegate::host() {
  return std::string(kAddress);
}

// static
int SprocketDevToolsHttpHandlerDelegate::port() {
  int port = kPort;
  const base::CommandLine& command_line = *base::CommandLine::ForCurrentProcess();
  if (command_line.HasSwitch(switches::kRemoteDebuggingPort)) {
    int temp_port;
    std::string port_str = command_line.GetSwitchValueASCII(switches::kRemoteDebuggingPort);
    if (base::StringToInt(port_str, &temp_port) && temp_port > 0 && temp_port < 65535) {
      port = temp_port;
    } else {
      DLOG(WARNING) << "Invalid http debugger port number " << temp_port;
    }
  }
  return port;
}

// static
GURL SprocketDevToolsHttpHandlerDelegate::discoveryPageURL() {
  return GURL("http://" + host() + ":" + std::to_string(port()));
}

SprocketDevToolsHttpHandlerDelegate::SprocketDevToolsHttpHandlerDelegate() {
}

SprocketDevToolsHttpHandlerDelegate::~SprocketDevToolsHttpHandlerDelegate() {
}

std::string SprocketDevToolsHttpHandlerDelegate::GetDiscoveryPageHTML() {
#if defined(OS_ANDROID)
  return std::string();
#else
  return ResourceBundle::GetSharedInstance().GetRawDataResource(
      IDR_SPROCKET_DEVTOOLS_DISCOVERY_PAGE).as_string();
#endif
}

std::string SprocketDevToolsHttpHandlerDelegate::GetFrontendResource(
    const std::string& path) {
  return content::DevToolsFrontendHost::GetFrontendResource(path).as_string();
}

std::string SprocketDevToolsHttpHandlerDelegate::GetPageThumbnailData(const GURL& url) {
  return std::string();
}

content::DevToolsExternalAgentProxyDelegate*
SprocketDevToolsHttpHandlerDelegate::HandleWebSocketConnection(const std::string& path) {
  return nullptr;
}


// SprocketDevToolsManagerDelegate ----------------------------------------------

// static
DevToolsHttpHandler* SprocketDevToolsManagerDelegate::CreateHttpHandler() {
  std::string frontend_url;
#if defined(OS_ANDROID)
  frontend_url = base::StringPrintf(kFrontEndURL, content::GetWebKitRevision().c_str());
#endif
  return new DevToolsHttpHandler(
      CreateSocketFactory(),
      frontend_url,
      new SprocketDevToolsHttpHandlerDelegate,
      base::FilePath(),
      base::FilePath(),
      std::string(),
      GetSprocketUserAgent());
}

SprocketDevToolsManagerDelegate::SprocketDevToolsManagerDelegate() {
}

SprocketDevToolsManagerDelegate::~SprocketDevToolsManagerDelegate() {
}

base::DictionaryValue* SprocketDevToolsManagerDelegate::HandleCommand(
    content::DevToolsAgentHost* agent_host,
    base::DictionaryValue* command) {
  return NULL;
}
