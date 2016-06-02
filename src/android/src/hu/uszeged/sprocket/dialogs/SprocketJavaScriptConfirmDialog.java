// Copyright (c) 2015 University of Szeged.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package hu.uszeged.sprocket.dialogs;

import hu.uszeged.sprocket.R;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;

public class SprocketJavaScriptConfirmDialog extends SprocketJavaScriptDialog {

    public SprocketJavaScriptConfirmDialog(String messageText, long javaScriptDialogPtr) {
        super(messageText, javaScriptDialogPtr);
    }

    @Override
    public AlertDialog.Builder getBuilder(Context context) {
      return super.getBuilder(context).setNegativeButton(context.getResources()
                                                                .getString(R.string.button_cancel),
                                                         new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        nativeCancelDialog(mJavaScriptDialogPtr);
                    }
               });
    }
}
