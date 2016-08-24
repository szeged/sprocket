// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SPROCKET_BROWSER_NET_URL_REQUEST_CONTEXT_GETTER_H_
#define SPROCKET_BROWSER_NET_URL_REQUEST_CONTEXT_GETTER_H_

#include "base/compiler_specific.h"
#include "base/files/file_path.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "content/public/browser/content_browser_client.h"
#include "net/proxy/proxy_config_service.h"
#include "net/url_request/url_request_context_getter.h"
#include "net/url_request/url_request_job_factory.h"

#include "content/public/browser/browser_context.h"

namespace base {
class MessageLoop;
}

namespace net {
class HostResolver;
class MappedHostResolver;
class NetworkDelegate;
class NetLog;
class ProxyConfigService;
class ProxyService;
class URLRequestContextStorage;
}

// URLRequestContextGetter: Interface for retrieving an net::URLRequestContext.
class SprocketURLRequestContextGetter : public net::URLRequestContextGetter {
 public:
  SprocketURLRequestContextGetter(
      bool ignore_certificate_errors,
      const base::FilePath& base_path,
      content::ProtocolHandlerMap* protocol_handlers);

  // net::URLRequestContextGetter implementation.

  net::URLRequestContext* GetURLRequestContext() override;

  // Returns a SingleThreadTaskRunner corresponding to the thread on
  // which the network IO happens (the thread on which the returned
  // net::URLRequestContext may be used).
  scoped_refptr<base::SingleThreadTaskRunner> GetNetworkTaskRunner()
      const override;


 protected:
  ~SprocketURLRequestContextGetter() override;

 private:
  bool ignore_certificate_errors_;
  base::FilePath base_path_;

  scoped_ptr<net::ProxyConfigService> proxy_config_service_;
  scoped_ptr<net::NetworkDelegate> network_delegate_;
  scoped_ptr<net::URLRequestContextStorage> storage_;
  scoped_ptr<net::URLRequestContext> url_request_context_;
  content::ProtocolHandlerMap protocol_handlers_;

  DISALLOW_COPY_AND_ASSIGN(SprocketURLRequestContextGetter);
};

#endif  // SPROCKET_BROWSER_NET_URL_REQUEST_CONTEXT_GETTER_H_
