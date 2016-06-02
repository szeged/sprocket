// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/browser/ui/window.h"

#include "base/message_loop/message_loop.h"
#include "sprocket/browser/web_contents.h"

const int kDefaultWindowWidth = 800;
const int kDefaultWindowHeight = 600;

std::set<SprocketWindow*> SprocketWindow::windows_;

// static
gfx::Size SprocketWindow::GetSprocketWindowDefaultSize() {
  static gfx::Size default_window_size;
  if (!default_window_size.IsEmpty())
    return default_window_size;

  default_window_size = gfx::Size(
      kDefaultWindowWidth, kDefaultWindowHeight);

  return default_window_size;
}

// static
gfx::Size SprocketWindow::AdjustWindowSize(const gfx::Size& initial_size) {
  if (!initial_size.IsEmpty())
    return initial_size;
  return SprocketWindow::GetSprocketWindowDefaultSize();
}

// static
SprocketWindow* SprocketWindow::CreateNewWindow(const gfx::Size& initial_size) {
  SprocketWindow* window = new SprocketWindow;

  const gfx::Size& size = SprocketWindow::AdjustWindowSize(initial_size);
  window->PlatformCreateWindow(size.width(), size.height());

  windows_.insert(window);
  return window;
}

void SprocketWindow::Close() {
  PlatformCloseWindow();
}

// static
void SprocketWindow::Initialize() {
  PlatformInitialize();
}

// static
void SprocketWindow::Deinitialize() {
  PlatformExit();
}

SprocketWindow::SprocketWindow()
    : is_fullscreen_(false) {
}

SprocketWindow::~SprocketWindow() {
  PlatformCleanUp();

  windows_.erase(this);
  if (windows_.empty())
    base::MessageLoop::current()->PostTask(FROM_HERE, base::MessageLoop::QuitWhenIdleClosure());
}
