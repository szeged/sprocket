// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package hu.uszeged.sprocket_apk;

import org.chromium.base.CommandLine;
import org.chromium.base.PathUtils;
import org.chromium.base.ResourceExtractor;

import org.chromium.content.app.ContentApplication;

/**
 * Entry point for the sprocket application.  Handles initialization of information that needs
 * to be shared across the main activity and the child services created.
 */
public class SprocketApplication extends ContentApplication {

    public static final String COMMAND_LINE_FILE = "/data/local/tmp/sprocket-command-line";
    private static final String PRIVATE_DATA_DIRECTORY_SUFFIX = "sprocket";

    @Override
    public void onCreate() {
        super.onCreate();
    }

    @Override
    protected void initializeLibraryDependencies() {
        PathUtils.setPrivateDataDirectorySuffix(PRIVATE_DATA_DIRECTORY_SUFFIX, this);
    }

    @Override
    public void initCommandLine() {
        if (!CommandLine.isInitialized()) {
            CommandLine.initFromFile(COMMAND_LINE_FILE);
        }
    }

}
