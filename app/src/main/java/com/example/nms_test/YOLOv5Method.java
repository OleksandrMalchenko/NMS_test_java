package com.example.nms_test;

import android.content.Context;
import android.content.ContextWrapper;

public class YOLOv5Method extends ContextWrapper{

    public YOLOv5Method(Context context) {
        super(context);
    }

    public static native int nms(
            float[] prediction,
            float confThresh,
            float iouThresh,
            int[] classes,
            int agnostic,
            int multiLabels,
            int labels,
            int maxDet,
            int nm);

    static {
        System.loadLibrary("yolo-jni");
    }
}
