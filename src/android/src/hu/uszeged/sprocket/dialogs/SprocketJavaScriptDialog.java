// Copyright (c) 2015 University of Szeged.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package hu.uszeged.sprocket.dialogs;

import hu.uszeged.sprocket.R;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;

import org.chromium.base.annotations.CalledByNative;

public class SprocketJavaScriptDialog {

    enum JavaScriptMessageType {
        JAVASCRIPT_MESSAGE_TYPE_ALERT,
        JAVASCRIPT_MESSAGE_TYPE_CONFIRM,
        JAVASCRIPT_MESSAGE_TYPE_PROMPT,
    };

    protected String mMessageText;
    protected long mJavaScriptDialogPtr;

    @CalledByNative
    public static SprocketJavaScriptDialog create(int type,
                                                  String messageText,
                                                  String defaultPromptText,
                                                  long javaScriptDialogPtr) {
        SprocketJavaScriptDialog dialog = null;
        switch (JavaScriptMessageType.values()[type]) {
            case JAVASCRIPT_MESSAGE_TYPE_ALERT:
                dialog = new SprocketJavaScriptDialog(messageText, javaScriptDialogPtr);
                break;
            case JAVASCRIPT_MESSAGE_TYPE_CONFIRM:
                dialog = new SprocketJavaScriptConfirmDialog(messageText, javaScriptDialogPtr);
                break;
            case JAVASCRIPT_MESSAGE_TYPE_PROMPT:
                dialog = new SprocketJavaScriptPromptDialog(messageText,
                                                            defaultPromptText,
                                                            javaScriptDialogPtr);
                break;
            default:
                break;
        }

        return dialog;
    }

    public SprocketJavaScriptDialog(String messageText, long javaScriptDialogPtr) {
        super();
        this.mMessageText = messageText;
        this.mJavaScriptDialogPtr = javaScriptDialogPtr;
    }

    public AlertDialog.Builder getBuilder(Context context) {
      return new AlertDialog.Builder(context).setMessage(mMessageText)
             .setPositiveButton(context.getResources().getString(R.string.button_ok),
                  new DialogInterface.OnClickListener() {
                  public void onClick(DialogInterface dialog, int id) {
                      nativeAcceptDialog(mJavaScriptDialogPtr);
                  }
             });
    }

    public void show(Context context) {
        getBuilder(context).create().show();
    }

    private static native void nativeAcceptDialog(long javaScriptDialogPtr);
    protected static native void nativeCancelDialog(long javaScriptDialogPtr);
    protected static native void nativeAcceptPromptDialog(
        long javaScriptDialogPtr, String promptText);
}
