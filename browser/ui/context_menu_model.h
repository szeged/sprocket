// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SPROCKET_BROWSER_UI_CONTEXT_MENU_MODEL_H_
#define SPROCKET_BROWSER_UI_CONTEXT_MENU_MODEL_H_

#include "sprocket/browser/web_contents.h"

#include "content/public/common/context_menu_params.h"
#include "ui/base/models/simple_menu_model.h"

class SprocketContextMenuModel : public ui::SimpleMenuModel,
                                    public ui::SimpleMenuModel::Delegate {
public:
  explicit SprocketContextMenuModel(
      SprocketWebContents* sprocket_web_contents,
      const content::ContextMenuParams& params);

  // ui::SimpleMenuModel::Delegate:
  bool IsCommandIdChecked(int command_id) const override;
  bool IsCommandIdEnabled(int command_id) const override;
  bool GetAcceleratorForCommandId(int command_id,
                                  ui::Accelerator* accelerator) override;
  void ExecuteCommand(int command_id, int event_flags) override;

private:
  enum CommandID {
    COMMAND_BACK,
    COMMAND_FORWARD,
    COMMAND_RELOAD
  };

  SprocketWebContents* sprocket_web_contents_;
  content::ContextMenuParams params_;

  DISALLOW_COPY_AND_ASSIGN(SprocketContextMenuModel);
};

#endif // SPROCKET_BROWSER_UI_CONTEXT_MENU_MODEL_H_
