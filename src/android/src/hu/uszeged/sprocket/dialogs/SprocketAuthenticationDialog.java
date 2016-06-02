// Copyright (c) 2016 University of Szeged.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package hu.uszeged.sprocket.dialogs;

import hu.uszeged.sprocket.R;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.text.InputType;
import android.widget.EditText;
import android.widget.LinearLayout;

import org.chromium.base.annotations.CalledByNative;

public class SprocketAuthenticationDialog {

    protected String mMessageText;
    protected long mAuthenticationDialogPtr;

    @CalledByNative
    public static SprocketAuthenticationDialog create(String message, long authenticationDialogPtr) {
        return new SprocketAuthenticationDialog(message, authenticationDialogPtr);
    }

    public SprocketAuthenticationDialog(String message, long authenticationDialogPtr) {
        super();
        this.mMessageText = message;
        this.mAuthenticationDialogPtr = authenticationDialogPtr;
    }

    public AlertDialog.Builder getBuilder(Context context) {
        LinearLayout layout = new LinearLayout(context);
        layout.setOrientation(LinearLayout.VERTICAL);
        final EditText userEdit = new EditText(context);
        userEdit.setInputType(InputType.TYPE_TEXT_FLAG_CAP_SENTENCES);
        userEdit.setHint(context.getResources().getString(R.string.name_placeholder));
        layout.addView(userEdit);
        final EditText passEdit = new EditText(context);
        passEdit.setInputType(InputType.TYPE_NUMBER_VARIATION_PASSWORD);
        passEdit.setHint(context.getResources().getString(R.string.pass_placeholder));
        layout.addView(passEdit);

        return new AlertDialog.Builder(context).setMessage(mMessageText)
             .setPositiveButton(context.getResources().getString(R.string.button_ok),
                                new DialogInterface.OnClickListener() {
                  public void onClick(DialogInterface dialog, int id) {
                      nativeAcceptAuthenticationDialog(
                        mAuthenticationDialogPtr,
                        userEdit.getText().toString(),
                        passEdit.getText().toString());
                  }
             })
            .setNegativeButton(context.getResources().getString(R.string.button_cancel),
                               new DialogInterface.OnClickListener() {
                  public void onClick(DialogInterface dialog, int id) {
                      nativeCancelAuthenticationDialog(mAuthenticationDialogPtr);
                  }
               })
            .setView(layout);
    }

    public void show(Context context) {
        getBuilder(context).create().show();
    }

    private static native void nativeAcceptAuthenticationDialog(
        long authenticationDialogPtr, String username, String password);
    protected static native void nativeCancelAuthenticationDialog(long authenticationDialogPtr);
}
