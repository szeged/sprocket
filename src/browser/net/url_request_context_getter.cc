// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/browser/net/url_request_context_getter.h"

#include "base/memory/ptr_util.h"
#include "base/threading/worker_pool.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/cookie_store_factory.h"
#include "sprocket/common/content_client.h"
#include "net/base/cache_type.h"
#include "net/base/network_delegate_impl.h"
#include "net/cert/cert_verifier.h"
#include "net/dns/host_resolver.h"
#include "net/http/http_auth_handler_factory.h"
#include "net/http/http_cache.h"
#include "net/http/http_network_session.h"
#include "net/http/http_server_properties_impl.h"
#include "net/http/transport_security_state.h"
#include "net/proxy/proxy_service.h"
#include "net/ssl/channel_id_service.h"
#include "net/ssl/default_channel_id_store.h"
#include "net/ssl/ssl_config_service_defaults.h"
#include "net/url_request/data_protocol_handler.h"
#if !defined(DISABLE_FILE_SUPPORT)
#include "net/url_request/file_protocol_handler.h"
#endif
#include "net/url_request/static_http_user_agent_settings.h"
#include "net/url_request/url_request_context.h"
#include "net/url_request/url_request_context_storage.h"
#include "net/url_request/url_request_job_factory_impl.h"


SprocketURLRequestContextGetter::SprocketURLRequestContextGetter(
    bool ignore_certificate_errors,
    const base::FilePath& base_path,
    content::ProtocolHandlerMap* protocol_handlers)
    : ignore_certificate_errors_(ignore_certificate_errors),
      base_path_(base_path) {
  // Must first be created on the UI thread.
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));

  std::swap(protocol_handlers_, *protocol_handlers);

  // We must create the proxy config service on the UI loop on Linux because it
  // must synchronously run on the glib message loop. This will be passed to
  // the URLRequestContextStorage on the IO thread in GetURLRequestContext().
  proxy_config_service_ =
      net::ProxyService::CreateSystemProxyConfigService(
          content::BrowserThread::UnsafeGetMessageLoopForThread(content::BrowserThread::IO)->task_runner(),
          content::BrowserThread::UnsafeGetMessageLoopForThread(content::BrowserThread::FILE)->task_runner());
}

SprocketURLRequestContextGetter::~SprocketURLRequestContextGetter() {
}

#include "net/cookies/cookie_store.h"

net::URLRequestContext* SprocketURLRequestContextGetter::GetURLRequestContext() {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::IO));

  // Create a new URLRequestContext if it does not exists yet.
  if (!url_request_context_) {
    url_request_context_.reset(new net::URLRequestContext());
    network_delegate_.reset(new net::NetworkDelegateImpl);
    url_request_context_->set_network_delegate(network_delegate_.get());


    // Generating URLRequestContextStorage
    storage_.reset(new net::URLRequestContextStorage(url_request_context_.get()));

    std::unique_ptr<net::HostResolver> host_resolver(net::HostResolver::CreateDefaultResolver(NULL));

    storage_->set_channel_id_service(base::WrapUnique(
        new net::ChannelIDService(new net::DefaultChannelIDStore(NULL),
                                  base::WorkerPool::GetTaskRunner(true))));
    storage_->set_cert_verifier(net::CertVerifier::CreateDefault());
    storage_->set_transport_security_state(base::WrapUnique(new net::TransportSecurityState));
    storage_->set_proxy_service(net::ProxyService::CreateUsingSystemProxyResolver(std::move(proxy_config_service_), 0, NULL));
    storage_->set_ssl_config_service(new net::SSLConfigServiceDefaults);
    storage_->set_http_auth_handler_factory( net::HttpAuthHandlerFactory::CreateDefault(host_resolver.get()));
    storage_->set_http_server_properties(base::WrapUnique<net::HttpServerProperties>(new net::HttpServerPropertiesImpl()));
    // Give |storage_| ownership at the end in case it's |mapped_host_resolver|.
    storage_->set_host_resolver(std::move(host_resolver));


    // Generating cookie store
    // CookieStoreConfig(): Convenience constructor for an in-memory cookie store with no delegate.
    std::unique_ptr<net::CookieStore> cookie_store = content::CreateCookieStore(content::CookieStoreConfig());
    storage_->set_cookie_store(std::move(cookie_store));


    // Setting HTTP user agent
    storage_->set_http_user_agent_settings(base::WrapUnique(
        new net::StaticHttpUserAgentSettings(
            "en-us,en", GetSprocketUserAgent())));


    // Cache settings
    base::FilePath cache_path = base_path_.Append(FILE_PATH_LITERAL("Cache"));
    std::unique_ptr<net::HttpCache::DefaultBackend> main_backend(
        new net::HttpCache::DefaultBackend(
            net::DISK_CACHE,
#if defined(OS_ANDROID)
            net::CACHE_BACKEND_SIMPLE,
#else
            net::CACHE_BACKEND_DEFAULT,
#endif
            cache_path,
            0,    // If |max_bytes| is  zero, a default value will be calculated automatically.
            content::BrowserThread::GetMessageLoopProxyForThread(content::BrowserThread::CACHE)));

    net::HttpNetworkSession::Params network_session_params;
    network_session_params.cert_verifier =
        url_request_context_->cert_verifier();
    network_session_params.transport_security_state =
        url_request_context_->transport_security_state();
    network_session_params.channel_id_service =
        url_request_context_->channel_id_service();
    network_session_params.proxy_service =
        url_request_context_->proxy_service();
    network_session_params.ssl_config_service =
        url_request_context_->ssl_config_service();
    network_session_params.http_auth_handler_factory =
        url_request_context_->http_auth_handler_factory();
    network_session_params.http_server_properties =
        url_request_context_->http_server_properties();
    network_session_params.ignore_certificate_errors =
        ignore_certificate_errors_;
    network_session_params.host_resolver =
        url_request_context_->host_resolver();

    storage_->set_http_network_session(base::WrapUnique(new net::HttpNetworkSession(network_session_params)));
    storage_->set_http_transaction_factory(base::WrapUnique(new net::HttpCache(storage_->http_network_session(), std::move(main_backend), true)));


    // Generate job factory
    std::unique_ptr<net::URLRequestJobFactoryImpl> job_factory(new net::URLRequestJobFactoryImpl());

    bool set_protocol = false;
    // Keep ProtocolHandlers added in sync with SprocketContentBrowserClient::IsHandledURL().
    for (auto it = protocol_handlers_.begin(); it != protocol_handlers_.end(); ++it) {
      set_protocol = job_factory->SetProtocolHandler(it->first, base::WrapUnique(it->second.release()));
      DCHECK(set_protocol);
    }
    protocol_handlers_.clear();

    set_protocol = job_factory->SetProtocolHandler(
        url::kDataScheme, base::WrapUnique(new net::DataProtocolHandler));
    DCHECK(set_protocol);

#if !defined(DISABLE_FILE_SUPPORT)
    set_protocol = job_factory->SetProtocolHandler(
        url::kFileScheme,
        base::WrapUnique(new net::FileProtocolHandler(
            content::BrowserThread::GetBlockingPool()->GetTaskRunnerWithShutdownBehavior(
                base::SequencedWorkerPool::SKIP_ON_SHUTDOWN))));
    DCHECK(set_protocol);
#endif
    storage_->set_job_factory(std::move(job_factory));
  }

  return url_request_context_.get();
}

scoped_refptr<base::SingleThreadTaskRunner> SprocketURLRequestContextGetter::GetNetworkTaskRunner() const {
  return content::BrowserThread::GetMessageLoopProxyForThread(content::BrowserThread::IO);
}
