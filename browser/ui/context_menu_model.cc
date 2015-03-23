// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "minibrowser/browser/ui/context_menu_model.h"

#include "base/strings/utf_string_conversions.h"
#include "ui/base/accelerators/accelerator.h"

MiniBrowserContextMenuModel::MiniBrowserContextMenuModel(
  MiniBrowser* minibrowser,
  const content::ContextMenuParams& params)
  : ui::SimpleMenuModel(this),
    minibrowser_(minibrowser),
    params_(params)
    {
      AddItem(COMMAND_BACK, base::ASCIIToUTF16("Back"));
      AddItem(COMMAND_FORWARD, base::ASCIIToUTF16("Forward"));
      AddItem(COMMAND_RELOAD, base::ASCIIToUTF16("Reload"));
}

bool MiniBrowserContextMenuModel::IsCommandIdChecked(
    int command_id) const {
  return false;
}

bool MiniBrowserContextMenuModel::IsCommandIdEnabled(
    int command_id) const {
  switch (command_id) {
    case COMMAND_BACK:
      return minibrowser_->CanGoBack();
    case COMMAND_FORWARD:
      return minibrowser_->CanGoForward();
    case COMMAND_RELOAD:
      return true;
  };
  return false;
}

bool MiniBrowserContextMenuModel::GetAcceleratorForCommandId(
    int command_id,
    ui::Accelerator* accel) {
  return false;
}

void MiniBrowserContextMenuModel::ExecuteCommand(
    int command_id, int event_flags) {
  switch (command_id) {
    case COMMAND_BACK:
      minibrowser_->GoBackOrForward(-1);
      break;
    case COMMAND_FORWARD:
      minibrowser_->GoBackOrForward(1);
      break;
    case COMMAND_RELOAD:
      minibrowser_->Reload();
      break;
  };
}