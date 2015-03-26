// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/browser/ui/context_menu_model.h"

#include "base/strings/utf_string_conversions.h"
#include "ui/base/accelerators/accelerator.h"

SprocketContextMenuModel::SprocketContextMenuModel(
  SprocketWebContents* sprocket_web_contents,
  const content::ContextMenuParams& params)
  : ui::SimpleMenuModel(this),
    sprocket_web_contents_(sprocket_web_contents),
    params_(params)
    {
      AddItem(COMMAND_BACK, base::ASCIIToUTF16("Back"));
      AddItem(COMMAND_FORWARD, base::ASCIIToUTF16("Forward"));
      AddItem(COMMAND_RELOAD, base::ASCIIToUTF16("Reload"));
}

bool SprocketContextMenuModel::IsCommandIdChecked(
    int command_id) const {
  return false;
}

bool SprocketContextMenuModel::IsCommandIdEnabled(
    int command_id) const {
  switch (command_id) {
    case COMMAND_BACK:
      return sprocket_web_contents_->CanGoBack();
    case COMMAND_FORWARD:
      return sprocket_web_contents_->CanGoForward();
    case COMMAND_RELOAD:
      return true;
  };
  return false;
}

bool SprocketContextMenuModel::GetAcceleratorForCommandId(
    int command_id,
    ui::Accelerator* accel) {
  return false;
}

void SprocketContextMenuModel::ExecuteCommand(
    int command_id, int event_flags) {
  switch (command_id) {
    case COMMAND_BACK:
      sprocket_web_contents_->GoBackOrForward(-1);
      break;
    case COMMAND_FORWARD:
      sprocket_web_contents_->GoBackOrForward(1);
      break;
    case COMMAND_RELOAD:
      sprocket_web_contents_->Reload();
      break;
  };
}
