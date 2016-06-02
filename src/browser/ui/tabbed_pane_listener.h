// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SPROCKET_BROWSER_UI_TABBED_PANE_LISTENER_H_
#define SPROCKET_BROWSER_UI_TABBED_PANE_LISTENER_H_

// An interface implemented by an object to let it know that a tabbed pane was
// selected by the user at the specified index.
class TabbedPaneListener {
 public:
  // Called when the tab at |index| is selected by the user.
  virtual void TabSelectedAt(int index) = 0;
  virtual void LastTabClosed() = 0;
  virtual void OpenNewEmptyTab() = 0;

 protected:
  virtual ~TabbedPaneListener() {}
};

#endif  // SPROCKET_BROWSER_UI_TABBED_PANE_LISTENER_H_