// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package hu.uszeged.sprocket;

import android.app.Activity;
import android.content.Context;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.FrameLayout;

import org.chromium.base.CalledByNative;
import org.chromium.base.JNINamespace;
import org.chromium.base.ThreadUtils;
import org.chromium.base.VisibleForTesting;
import org.chromium.content.browser.ActivityContentVideoViewClient;
import org.chromium.content.browser.ContentVideoViewClient;
import org.chromium.content.browser.ContentViewClient;
import org.chromium.content.browser.ContentViewCore;
import org.chromium.content.browser.ContentViewRenderView;
import org.chromium.ui.base.WindowAndroid;

/**
 * Container and generator of SprocketViews.
 */
//@JNINamespace("content")
public class SprocketManager extends FrameLayout {

    public static final String DEFAULT_SPROCKET_URL = "http://www.google.com";
    private static boolean sStartup = true;
    private WindowAndroid mWindow;
    private SprocketWebContents mActiveSprocketWebContents;

    private String mStartupUrl = DEFAULT_SPROCKET_URL;

    // The target for all content rendering.
    private ContentViewRenderView mContentViewRenderView;
    private ContentViewClient mContentViewClient;

    /**
     * Constructor for inflating via XML.
     */
    public SprocketManager(final Context context, AttributeSet attrs) {
        super(context, attrs);
        nativeInit(this);
        mContentViewClient = new ContentViewClient() {
            @Override
            public ContentVideoViewClient getContentVideoViewClient() {
                return new ActivityContentVideoViewClient((Activity) context) {
                    @Override
                    public void enterFullscreenVideo(View view) {
                        super.enterFullscreenVideo(view);
                        setOverlayVideoMode(true);
                    }

                    @Override
                    public void exitFullscreenVideo() {
                        super.exitFullscreenVideo();
                        setOverlayVideoMode(false);
                    }
                };
            }
        };
    }

    /**
     * @param window The window used to generate all Sprockets.
     */
    public void setWindow(WindowAndroid window) {
        setWindow(window, true);
    }

    /**
     * @param window The window used to generate all Sprockets.
     * @param initialLoadingNeeded Whether initial loading is needed or not.
     */
    @VisibleForTesting
    public void setWindow(WindowAndroid window, final boolean initialLoadingNeeded) {
        assert window != null;
        mWindow = window;
        mContentViewRenderView = new ContentViewRenderView(getContext()) {
            @Override
            protected void onReadyToRender() {
                if (sStartup) {
                    if (initialLoadingNeeded) mActiveSprocketWebContents.loadUrl(mStartupUrl);
                    sStartup = false;
                }
            }
        };
        mContentViewRenderView.onNativeLibraryLoaded(window);
    }

    /**
     * @return The window used to generate all Sprockets.
     */
    public WindowAndroid getWindow() {
        return mWindow;
    }

    /**
     * Get the ContentViewRenderView.
     */
    public ContentViewRenderView getContentViewRenderView() {
        return mContentViewRenderView;
    }

    /**
     * Sets the startup URL for new SprocketWebContents windows.
     */
    public void setStartupUrl(String url) {
        mStartupUrl = url;
    }

    /**
     * @return The currently visible SprocketWebContents view or null if one is not showing.
     */
    public SprocketWebContents getActiveSprocketWebContents() {
        return mActiveSprocketWebContents;
    }

    /**
     * Creates a new SprocketWebContents pointing to the specified URL.
     * @param url The URL the SprocketWebContents should load upon creation.
     */
    public void launchSprocketWebContents(String url) {
        ThreadUtils.assertOnUiThread();
        SprocketWebContents previousSprocketWebContents = mActiveSprocketWebContents;
        nativeLaunchSprocketWebContents(url);
        if (previousSprocketWebContents != null) previousSprocketWebContents.close();
    }

    /**
     * Enter or leave overlay video mode.
     * @param enabled Whether overlay mode is enabled.
     */
    public void setOverlayVideoMode(boolean enabled) {
        if (mContentViewRenderView == null) return;
        mContentViewRenderView.setOverlayVideoMode(enabled);
    }

    @SuppressWarnings("unused")
    @CalledByNative
    private Object createSprocketWebContents(long nativeSprocketWebContentsPtr) {
        assert mContentViewRenderView != null;
        LayoutInflater inflater =
                (LayoutInflater) getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        SprocketWebContents sprocketWebContentsView = (SprocketWebContents) inflater.inflate(R.layout.sprocket_view, null);
        sprocketWebContentsView.initialize(nativeSprocketWebContentsPtr, mWindow, mContentViewClient);

        // TODO(tedchoc): Allow switching back to these inactive SprocketWebContents.
        if (mActiveSprocketWebContents != null) removeSprocketWebContents(mActiveSprocketWebContents);

        showSprocketWebContents(sprocketWebContentsView);
        return sprocketWebContentsView;
    }

    private void showSprocketWebContents(SprocketWebContents sprocketWebContentsView) {
        sprocketWebContentsView.setContentViewRenderView(mContentViewRenderView);
        addView(sprocketWebContentsView, new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.MATCH_PARENT, FrameLayout.LayoutParams.MATCH_PARENT));
        mActiveSprocketWebContents = sprocketWebContentsView;
        ContentViewCore contentViewCore = mActiveSprocketWebContents.getContentViewCore();
        if (contentViewCore != null) {
            mContentViewRenderView.setCurrentContentViewCore(contentViewCore);
            contentViewCore.onShow();
        }
    }

    @CalledByNative
    private void removeSprocketWebContents(SprocketWebContents sprocketWebContentsView) {
        if (sprocketWebContentsView == mActiveSprocketWebContents) mActiveSprocketWebContents = null;
        if (sprocketWebContentsView.getParent() == null) return;
        ContentViewCore contentViewCore = sprocketWebContentsView.getContentViewCore();
        if (contentViewCore != null) contentViewCore.onHide();
        sprocketWebContentsView.setContentViewRenderView(null);
        removeView(sprocketWebContentsView);
    }

    private static native void nativeInit(Object sprocketManagerInstance);
    private static native void nativeLaunchSprocketWebContents(String url);
}
