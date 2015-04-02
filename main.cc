// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/public/app/content_main.h"
#include "sprocket/main_delegate.h"

int main(int argc, const char** argv) {
  // The embedder can also pass in NULL for |delegate| if they don't want to
  // override default startup.
  SprocketMainDelegate delegate;
  content::ContentMainParams params(&delegate);
  params.argc = argc;
  params.argv = argv;

  // ContentMain should be called from the embedder's main() function to do the
  // initial setup for every process.
  return content::ContentMain(params);
}
