// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_BROWSER_BROWSER_UI_CONTEXT_MENU_MODEL_H_
#define MINI_BROWSER_BROWSER_UI_CONTEXT_MENU_MODEL_H_

#include "minibrowser/browser/ui/minibrowser.h"

#include "content/public/common/context_menu_params.h"
#include "ui/base/models/simple_menu_model.h"

class MiniBrowserContextMenuModel : public ui::SimpleMenuModel,
                                    public ui::SimpleMenuModel::Delegate {
public:
  explicit MiniBrowserContextMenuModel(
      MiniBrowser* minibrowser,
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

  MiniBrowser* minibrowser_;
  content::ContextMenuParams params_;

  DISALLOW_COPY_AND_ASSIGN(MiniBrowserContextMenuModel);
};

#endif // MINI_BROWSER_BROWSER_UI_CONTEXT_MENU_MODEL_H_