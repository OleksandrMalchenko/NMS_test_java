package com.example.nms_test;

import android.graphics.Bitmap;
import android.content.Context;
import android.content.ContextWrapper;

/**
 * Created by demid on 2/23/2017.
 */

public class YOLOv5Method extends ContextWrapper{

    public YOLOv5Method(Context context) {
        super(context);
    }

    public static native int nms_test(
            int prediction,
            int confThresh,
            float iouThresh,
            String classes,
            int agnostic,
            int multiLabels,
            int labels,
            int maxDet,
            int nm);

    static {
        System.loadLibrary("yolo-jni");
    }
}
