// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SPROCKET_BROWSER_RESOURCE_DISPATCHER_HOST_DELEGATE_H_
#define SPROCKET_BROWSER_RESOURCE_DISPATCHER_HOST_DELEGATE_H_

#include "content/public/browser/resource_dispatcher_host_delegate.h"
#include "content/public/browser/resource_dispatcher_host_login_delegate.h"

#include "url/gurl.h"

// Interface for getting login credentials for HTTP auth requests. If the
// login delegate obtains credentials, it should call the URLRequest's SetAuth
// method. If the user cancels, the login delegate should call the URLRequest's
// CancelAuth instead. And in either case, it must make a call to
// ResourceDispatcherHost::ClearLoginDelegateForRequest.
class SprocketResourceDispatcherHostLoginDelegate
    : public content::ResourceDispatcherHostLoginDelegate {
public:
  SprocketResourceDispatcherHostLoginDelegate(
      net::AuthChallengeInfo* auth_info,
      net::URLRequest* request);

  // Overridden from content::ResourceDispatcherHostLoginDelegate:

  // Notify the login delegate that the request was cancelled.
  // This function can only be called from the IO thread.
  virtual void OnRequestCancelled() override;



  // Triggers the dialog on UI thread
  void TriggerDialog();
  // Username/password arrives from the user on UI thread
  void InputProvided(const base::string16& username, const base::string16& password);
  // Sets credentials for the request on IO thread
  void SetCredentials(const base::string16& username, const base::string16& password);

private:
  GURL url_;
  std::string realm_;
  std::string host_;
  bool proxy_;

  int render_process_id_;
  int render_frame_id_;

  net::URLRequest* request_;
};

// Interface that the embedder provides to ResourceDispatcherHost to allow
// observing and modifying requests.
class SprocketResourceDispatcherHostDelegate
    : public content::ResourceDispatcherHostDelegate {
public:
  // Overridden from content::ResourceDispatcherHostDelegate:

  // Creates a ResourceDispatcherHostLoginDelegate that asks the user for a
  // username and password.
  virtual content::ResourceDispatcherHostLoginDelegate* CreateLoginDelegate(
              net::AuthChallengeInfo* auth_info,
              net::URLRequest* request) override;
};

#endif // SPROCKET_BROWSER_RESOURCE_DISPATCHER_HOST_DELEGATE_H_
