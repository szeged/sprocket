// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/browser/devtools/devtools_frontend.h"

#include "base/command_line.h"
#include "base/json/json_reader.h"
#include "base/json/json_writer.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "base/values.h"
#include "components/devtools_http_handler/devtools_http_handler.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/content_client.h"
#include "sprocket/browser/ui/window.h"
#include "sprocket/browser/web_contents.h"
#include "sprocket/browser/browser_context.h"
#include "sprocket/browser/browser_main_parts.h"
#include "sprocket/browser/content_browser_client.h"
#include "sprocket/browser/devtools/devtools_manager_delegate.h"
#include "sprocket/common/switches.h"
#include "net/base/io_buffer.h"
#include "net/base/net_errors.h"
#include "net/http/http_response_headers.h"
#include "net/url_request/url_fetcher.h"
#include "net/url_request/url_fetcher_response_writer.h"

namespace {


// ResponseWriter -------------------------------------------------------------

class ResponseWriter : public net::URLFetcherResponseWriter {
 public:
  ResponseWriter(base::WeakPtr<SprocketDevToolsFrontend> sprocket_devtools_,
                 int stream_id);
  ~ResponseWriter() override;

  // URLFetcherResponseWriter overrides:
  int Initialize(const net::CompletionCallback& callback) override;
  int Write(net::IOBuffer* buffer,
            int num_bytes,
            const net::CompletionCallback& callback) override;
  int Finish(const net::CompletionCallback& callback) override;

 private:
  base::WeakPtr<SprocketDevToolsFrontend> sprocket_devtools_;
  int stream_id_;

  DISALLOW_COPY_AND_ASSIGN(ResponseWriter);
};

ResponseWriter::ResponseWriter(
    base::WeakPtr<SprocketDevToolsFrontend> sprocket_devtools,
    int stream_id)
    : sprocket_devtools_(sprocket_devtools),
      stream_id_(stream_id) {
}

ResponseWriter::~ResponseWriter() {
}

int ResponseWriter::Initialize(const net::CompletionCallback& callback) {
  return net::OK;
}

int ResponseWriter::Write(net::IOBuffer* buffer,
                          int num_bytes,
                          const net::CompletionCallback& callback) {
  base::FundamentalValue* id = new base::FundamentalValue(stream_id_);
  base::StringValue* chunk =
      new base::StringValue(std::string(buffer->data(), num_bytes));

  content::BrowserThread::PostTask(
      content::BrowserThread::UI, FROM_HERE,
      base::Bind(&SprocketDevToolsFrontend::CallClientFunction,
                 sprocket_devtools_, "DevToolsAPI.streamWrite",
                 base::Owned(id), base::Owned(chunk), nullptr));
  return num_bytes;
}

int ResponseWriter::Finish(const net::CompletionCallback& callback) {
  return net::OK;
}

}  // namespace

// This constant should be in sync with
// the constant at devtools_ui_bindings.cc.
const size_t kMaxMessageChunkSize = IPC::Channel::kMaximumMessageSize / 4;

// static
SprocketDevToolsFrontend* SprocketDevToolsFrontend::Show(
    content::WebContents* inspected_contents) {
  SprocketWindow* window = SprocketWindow::CreateNewWindow(gfx::Size());
  SprocketWebContents* web_contents =
      SprocketWebContents::CreateSprocketWebContents(window,
                                                     inspected_contents->GetBrowserContext(),
                                                     GURL(),
                                                     gfx::Size());
  SprocketDevToolsFrontend* devtools_frontend = new SprocketDevToolsFrontend(
      web_contents,
      inspected_contents);

  devtools_http_handler::DevToolsHttpHandler* http_handler =
      SprocketContentBrowserClient::Get()
          ->browser_main_parts()
          ->devtools_http_handler();
  web_contents->LoadURL(http_handler->GetFrontendURL("/devtools/inspector.html"));

  return devtools_frontend;
}

void SprocketDevToolsFrontend::Activate() {
  frontend_contents_->ActivateContents(web_contents());
}

void SprocketDevToolsFrontend::Focus() {
  web_contents()->Focus();
}

void SprocketDevToolsFrontend::InspectElementAt(int x, int y) {
  if (agent_host_)
    agent_host_->InspectElement(x, y);
}

void SprocketDevToolsFrontend::Close() {
  frontend_contents_->Close();
}

void SprocketDevToolsFrontend::DisconnectFromTarget() {
  if (!agent_host_)
    return;
  agent_host_->DetachClient();
  agent_host_ = NULL;
}

SprocketDevToolsFrontend::SprocketDevToolsFrontend(SprocketWebContents* frontend_contents,
                                                   content::WebContents* inspected_contents)
    : content::WebContentsObserver(frontend_contents->web_contents()),
      frontend_contents_(frontend_contents),
      inspected_contents_(inspected_contents),
      weak_factory_(this) {
}

SprocketDevToolsFrontend::~SprocketDevToolsFrontend() {
  for (const auto& pair : pending_requests_)
    delete pair.first;
}

void SprocketDevToolsFrontend::RenderViewCreated(
    content::RenderViewHost* render_view_host) {
  if (!frontend_host_) {
    frontend_host_.reset(
        content::DevToolsFrontendHost::Create(web_contents()->GetMainFrame(), this));
  }
}

void SprocketDevToolsFrontend::DocumentAvailableInMainFrame() {
  agent_host_ = content::DevToolsAgentHost::GetOrCreateFor(inspected_contents_);
  agent_host_->AttachClient(this);
}

void SprocketDevToolsFrontend::WebContentsDestroyed() {
  if (agent_host_)
    agent_host_->DetachClient();
  delete this;
}

void SprocketDevToolsFrontend::HandleMessageFromDevToolsFrontend(
    const std::string& message) {
  if (!agent_host_)
    return;
  std::string method;
  base::ListValue* params = NULL;
  base::DictionaryValue* dict = NULL;
  scoped_ptr<base::Value> parsed_message = base::JSONReader::Read(message);
  if (!parsed_message ||
      !parsed_message->GetAsDictionary(&dict) ||
      !dict->GetString("method", &method)) {
    return;
  }
  int request_id = 0;
  dict->GetInteger("id", &request_id);
  dict->GetList("params", &params);

  std::string browser_message;
  if (method == "sendMessageToBrowser" && params &&
      params->GetSize() == 1 && params->GetString(0, &browser_message)) {
    agent_host_->DispatchProtocolMessage(browser_message);
  } else if (method == "loadCompleted") {
    web_contents()->GetMainFrame()->ExecuteJavaScript(
        base::ASCIIToUTF16("DevToolsAPI.setUseSoftMenu(true);"));
  } else if (method == "loadNetworkResource" && params->GetSize() == 3) {
    // TODO(pfeldman): handle some of the embedder messages in content.
    std::string url;
    std::string headers;
    int stream_id;
    if (!params->GetString(0, &url) ||
        !params->GetString(1, &headers) ||
        !params->GetInteger(2, &stream_id)) {
      return;
    }

    GURL gurl(url);
    if (!gurl.is_valid()) {
      base::DictionaryValue response;
      response.SetInteger("statusCode", 404);
      SendMessageAck(request_id, &response);
      return;
    }

    net::URLFetcher* fetcher =
        net::URLFetcher::Create(gurl, net::URLFetcher::GET, this).release();
    pending_requests_[fetcher] = request_id;
    fetcher->SetRequestContext(web_contents()->GetBrowserContext()->
        GetRequestContext());
    fetcher->SetExtraRequestHeaders(headers);
    fetcher->SaveResponseWithWriter(scoped_ptr<net::URLFetcherResponseWriter>(
        new ResponseWriter(weak_factory_.GetWeakPtr(), stream_id)));
    fetcher->Start();
    return;
  } else if (method == "getPreferences") {
    SendMessageAck(request_id, &preferences_);
    return;
  } else if (method == "setPreference") {
    std::string name;
    std::string value;
    if (!params->GetString(0, &name) ||
        !params->GetString(1, &value)) {
      return;
    }
    preferences_.SetStringWithoutPathExpansion(name, value);
  } else if (method == "removePreference") {
    std::string name;
    if (!params->GetString(0, &name))
      return;
    preferences_.RemoveWithoutPathExpansion(name, nullptr);
  } else {
    return;
  }

  if (request_id)
    SendMessageAck(request_id, nullptr);
}

void SprocketDevToolsFrontend::HandleMessageFromDevToolsFrontendToBackend(
    const std::string& message) {
  if (agent_host_)
    agent_host_->DispatchProtocolMessage(message);
}

void SprocketDevToolsFrontend::DispatchProtocolMessage(
    content::DevToolsAgentHost* agent_host,
    const std::string& message) {
  if (message.length() < kMaxMessageChunkSize) {
    base::string16 javascript = base::UTF8ToUTF16(
        "DevToolsAPI.dispatchMessage(" + message + ");");
    web_contents()->GetMainFrame()->ExecuteJavaScript(javascript);
    return;
  }

  base::FundamentalValue total_size(static_cast<int>(message.length()));
  for (size_t pos = 0; pos < message.length(); pos += kMaxMessageChunkSize) {
    std::string param;
    base::JSONWriter::Write(
        base::StringValue(message.substr(pos, kMaxMessageChunkSize)), &param);
    std::string code = "DevToolsAPI.dispatchMessageChunk(" + param + ");";
    base::string16 javascript = base::UTF8ToUTF16(code);
    web_contents()->GetMainFrame()->ExecuteJavaScript(javascript);
  }
}

void SprocketDevToolsFrontend::OnURLFetchComplete(const net::URLFetcher* source) {
  // TODO(pfeldman): this is a copy of chrome's devtools_ui_bindings.cc.
  // We should handle some of the commands including this one in content.
  DCHECK(source);
  PendingRequestsMap::iterator it = pending_requests_.find(source);
  DCHECK(it != pending_requests_.end());

  base::DictionaryValue response;
  base::DictionaryValue* headers = new base::DictionaryValue();
  net::HttpResponseHeaders* rh = source->GetResponseHeaders();
  response.SetInteger("statusCode", rh ? rh->response_code() : 200);
  response.Set("headers", headers);

  void* iterator = NULL;
  std::string name;
  std::string value;
  while (rh && rh->EnumerateHeaderLines(&iterator, &name, &value))
    headers->SetString(name, value);

  SendMessageAck(it->second, &response);
  pending_requests_.erase(it);
  delete source;
}

void SprocketDevToolsFrontend::CallClientFunction(
    const std::string& function_name,
    const base::Value* arg1,
    const base::Value* arg2,
    const base::Value* arg3) {
  std::string javascript = function_name + "(";
  if (arg1) {
    std::string json;
    base::JSONWriter::Write(*arg1, &json);
    javascript.append(json);
    if (arg2) {
      base::JSONWriter::Write(*arg2, &json);
      javascript.append(", ").append(json);
      if (arg3) {
        base::JSONWriter::Write(*arg3, &json);
        javascript.append(", ").append(json);
      }
    }
  }
  javascript.append(");");
  web_contents()->GetMainFrame()->ExecuteJavaScript(
      base::UTF8ToUTF16(javascript));
}

void SprocketDevToolsFrontend::SendMessageAck(int request_id,
                                              const base::Value* arg) {
  base::FundamentalValue id_value(request_id);
  CallClientFunction("DevToolsAPI.embedderMessageAck",
      &id_value, arg, nullptr);
}

void SprocketDevToolsFrontend::AgentHostClosed(
    content::DevToolsAgentHost* agent_host,
    bool replaced) {
  frontend_contents_->Close();
}
