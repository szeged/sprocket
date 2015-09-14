// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package hu.uszeged.sprocket;

import android.content.Context;
import android.util.AttributeSet;
import android.text.TextUtils;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.LinearLayout;

import org.chromium.base.annotations.CalledByNative;
import org.chromium.content.browser.ContentView;
import org.chromium.content.browser.ContentViewClient;
import org.chromium.content.browser.ContentViewCore;
import org.chromium.content.browser.ContentViewRenderView;
import org.chromium.content_public.browser.LoadUrlParams;
import org.chromium.content_public.browser.NavigationController;
import org.chromium.content_public.browser.WebContents;
import org.chromium.ui.base.WindowAndroid;

/**
 * Container for the various UI components that make up a sprocket window.
 */
public class SprocketWindow extends LinearLayout {

    private ContentViewCore mContentViewCore;
    private WebContents mWebContents;
    private NavigationController mNavigationController;
    private ContentViewClient mContentViewClient;

    private long mNativeSprocketWindow;
    private ContentViewRenderView mContentViewRenderView;
    private WindowAndroid mWindow;

    private boolean mIsFullscreen = false;

    /**
     * Constructor for inflating via XML.
     */
    public SprocketWindow(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    /**
     * Set the SurfaceView being renderered to as soon as it is available.
     */
    public void setContentViewRenderView(ContentViewRenderView contentViewRenderView) {
        FrameLayout contentViewHolder = (FrameLayout) findViewById(R.id.contentview_holder);
        if (contentViewRenderView == null) {
            if (mContentViewRenderView != null) {
                contentViewHolder.removeView(mContentViewRenderView);
            }
        } else {
            contentViewHolder.addView(contentViewRenderView,
                    new FrameLayout.LayoutParams(
                            FrameLayout.LayoutParams.MATCH_PARENT,
                            FrameLayout.LayoutParams.MATCH_PARENT));
        }
        mContentViewRenderView = contentViewRenderView;
    }

    /**
     * Initializes the SprocketWindow for use.
     *
     * @param nativeSprocketWindow The pointer to the native SprocketWindow object.
     * @param window The owning window for this SprocketWindow.
     * @param client The {@link ContentViewClient} to be bound to any current or new
     *               {@link ContentViewCore}s associated with this SprocketWindow.
     */
    public void initialize(long nativeSprocketWindow, WindowAndroid window, ContentViewClient client) {
        mNativeSprocketWindow = nativeSprocketWindow;
        mWindow = window;
        mContentViewClient = client;
    }

    /**
     * Closes the SprocketWindow and cleans up the native instance, which will handle destroying all
     * dependencies.
     */
    public void close() {
        if (mNativeSprocketWindow == 0) return;
        nativeCloseSprocketWindow(mNativeSprocketWindow);
    }

    @CalledByNative
    private void onNativeDestroyed() {
        mWindow = null;
        mNativeSprocketWindow = 0;
        mContentViewCore.destroy();
    }

    /**
     * @return Whether the SprocketWindow has been destroyed.
     * @see #onNativeDestroyed()
     */
    public boolean isDestroyed() {
        return mNativeSprocketWindow == 0;
    }

    /**
     * Loads an URL.  This will perform minimal amounts of sanitizing of the URL to attempt to
     * make it valid.
     *
     * @param url The URL to be loaded by the SprocketWindow.
     */
    public void loadUrl(String url) {
        if (url == null) return;

        if (TextUtils.equals(url, mWebContents.getUrl())) {
            mNavigationController.reload(true);
        } else {
            mNavigationController.loadUrl(new LoadUrlParams(sanitizeUrl(url)));
        }
        // TODO(aurimas): Remove this when crbug.com/174541 is fixed.
        mContentViewCore.getContainerView().clearFocus();
        mContentViewCore.getContainerView().requestFocus();
    }

    /**
     * Given an URL, this performs minimal sanitizing to ensure it will be valid.
     * @param url The url to be sanitized.
     * @return The sanitized URL.
     */
    public static String sanitizeUrl(String url) {
        if (url == null) return null;
        if (url.startsWith("www.") || url.indexOf(":") == -1) url = "http://" + url;
        return url;
    }

    @CalledByNative
    private void toggleFullscreenModeForTab(boolean enterFullscreen) {
        mIsFullscreen = enterFullscreen;
    }

    @CalledByNative
    private boolean isFullscreenForTabOrPending() {
        return mIsFullscreen;
    }

    /**
     * Initializes the ContentView based on the native tab contents pointer passed in.
     * @param webContents A {@link WebContents} object.
     */
    @SuppressWarnings("unused")
    @CalledByNative
    private void initFromNativeTabContents(WebContents webContents) {
        Context context = getContext();
        mContentViewCore = new ContentViewCore(context);
        ContentView cv = new ContentView(context, mContentViewCore);
        mContentViewCore.initialize(cv, cv, webContents, mWindow);
        mContentViewCore.setContentViewClient(mContentViewClient);
        mWebContents = mContentViewCore.getWebContents();
        mNavigationController = mWebContents.getNavigationController();
        if (getParent() != null) mContentViewCore.onShow();
        ((FrameLayout) findViewById(R.id.contentview_holder)).addView(cv,
                new FrameLayout.LayoutParams(
                        FrameLayout.LayoutParams.MATCH_PARENT,
                        FrameLayout.LayoutParams.MATCH_PARENT));
        cv.requestFocus();
        mContentViewRenderView.setCurrentContentViewCore(mContentViewCore);
    }

    /**
     * @return The {@link ViewGroup} currently shown by this SprocketWindow.
     */
    public ViewGroup getContentView() {
        return mContentViewCore.getContainerView();
    }

    /**
     * @return The {@link ContentViewCore} currently managing the view shown by this SprocketWindow.
     */
    public ContentViewCore getContentViewCore() {
        return mContentViewCore;
    }

     /**
     * @return The {@link WebContents} currently managing the content shown by this SprocketWindow.
     */
    public WebContents getWebContents() {
        return mWebContents;
    }

    private static native void nativeCloseSprocketWindow(long sprocketWindowPtr);
}
