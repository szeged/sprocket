// Copyright (c) 2015 University of Szeged.
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

public class SprocketJavaScriptPromptDialog extends SprocketJavaScriptDialog {

    private String mDefaultPromptText;

    public SprocketJavaScriptPromptDialog(String messageText,
                                          String defaultPromptText,
                                          long javaScriptDialogPtr) {
        super(messageText, javaScriptDialogPtr);
        this.mDefaultPromptText = defaultPromptText;
    }

    @Override
    public AlertDialog.Builder getBuilder(Context context) {
        final EditText prompt = new EditText(context);
        prompt.setInputType(InputType.TYPE_TEXT_FLAG_CAP_SENTENCES);
        prompt.setText(mDefaultPromptText);

        return new AlertDialog.Builder(context).setMessage(mMessageText)
             .setPositiveButton(context.getResources().getString(R.string.button_ok),
                                new DialogInterface.OnClickListener() {
                  public void onClick(DialogInterface dialog, int id) {
                      nativeAcceptPromptDialog(mJavaScriptDialogPtr, prompt.getText().toString());
                  }
             })
            .setNegativeButton(context.getResources().getString(R.string.button_cancel),
                               new DialogInterface.OnClickListener() {
                  public void onClick(DialogInterface dialog, int id) {
                      nativeCancelDialog(mJavaScriptDialogPtr);
                  }
               })
            .setView(prompt);
    }
}
