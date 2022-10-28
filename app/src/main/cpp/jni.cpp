//
// Created by Dev on 10/14/2022.
//
#include <android/asset_manager_jni.h>
#include <android/bitmap.h>
#include <android/log.h>

#include <jni.h>
#include <string>
#include <vector>

#define LOG_TAG             "Engine"

#define LOG_INFO(...)       __android_log_print(ANDROID_LOG_INFO,   LOG_TAG, __VA_ARGS__)
#define LOG_DEBUG(...)      __android_log_print(ANDROID_LOG_DEBUG,  LOG_TAG, __VA_ARGS__)
#define LOG_WARN(...)       __android_log_print(ANDROID_LOG_WARN,   LOG_TAG, __VA_ARGS__)
#define LOG_ERR(...)        __android_log_print(ANDROID_LOG_ERROR,  LOG_TAG, __VA_ARGS__)

extern "C"
{
struct Object {
    float x;
    float y;
    float w;
    float h;
    int label;
    float prob;
};

static inline float intersection_area(const Object &a, const Object &b) {
    if (a.x > b.x + b.w || a.x + a.w < b.x || a.y > b.y + b.h || a.y + a.h < b.y) {
        // no intersection
        return 0.f;
    }

    float inter_width = std::min(a.x + a.w, b.x + b.w) - std::max(a.x, b.x);
    float inter_height = std::min(a.y + a.h, b.y + b.h) - std::max(a.y, b.y);

    return inter_width * inter_height;
}

static void nms_sorted_bboxes(const std::vector<Object> &faceobjects, std::vector<int> &picked,
                              float nms_threshold) {
    picked.clear();

    const int n = faceobjects.size();

    std::vector<float> areas(n);
    for (int i = 0; i < n; i++) {
        areas[i] = faceobjects[i].w * faceobjects[i].h;
    }

    for (int i = 0; i < n; i++) {
        const Object &a = faceobjects[i];

        int keep = 1;
        for (int j = 0; j < (int) picked.size(); j++) {
            const Object &b = faceobjects[picked[j]];

            // intersection over union
            float inter_area = intersection_area(a, b);
            float union_area = areas[i] + areas[picked[j]] - inter_area;
            // float IoU = inter_area / union_area
            if (inter_area / union_area > nms_threshold)
                keep = 0;
        }

        if (keep)
            picked.push_back(i);
    }
}

JNIEXPORT int JNICALL Java_com_example_nms_1test_YOLOv5Method_nms(
        JNIEnv *env, jobject thiz, jfloatArray preds, float conf_thresh,
        float iou_thresh, jintArray classes, int agnostic, int multilabels, int labels, int maxDet,
        int nm) {

    std::vector<Object> proposals;
    float *preds_ptr = (float *) env->GetFloatArrayElements(preds, 0);
    for (int i = 0 ; i < 30240; i+=6) {
        Object obj;
        obj.x = preds_ptr[6*i];
        obj.y = preds_ptr[6*i+1];
        obj.w = preds_ptr[6*i+2];
        obj.h = preds_ptr[6*i+3];
        obj.label = preds_ptr[6*i+4];
        obj.prob = preds_ptr[6*i+5];

        proposals.push_back(obj);
    }
    std::vector<Object> objects;

    std::vector<int> picked;
    nms_sorted_bboxes(proposals, picked, conf_thresh);

    int count = picked.size();

    LOG_DEBUG("=========: %d", count);

    objects.resize(count);
    int width = 480;
    int height = 640;

    int wpad = (width + 31) / 32 * 32 - width;
    int hpad = (height + 31) / 32 * 32 - height;

    float scale = 1.f;
    int w = width;
    int h = height;
    const int target_size = 640;

    if (w > h)
    {
        scale = (float)target_size / w;
        w = target_size;
        h = h * scale;
    }
    else
    {
        scale = (float)target_size / h;
        h = target_size;
        w = w * scale;
    }

    for (int i = 0; i < count; i++)
    {
        objects[i] = proposals[picked[i]];

        // adjust offset to original unpadded
        float x0 = (objects[i].x - (wpad / 2)) / scale;
        float y0 = (objects[i].y - (hpad / 2)) / scale;
        float x1 = (objects[i].x + objects[i].w - (wpad / 2)) / scale;
        float y1 = (objects[i].y + objects[i].h - (hpad / 2)) / scale;

        // clip
        x0 = std::max(std::min(x0, (float)(width - 1)), 0.f);
        y0 = std::max(std::min(y0, (float)(height - 1)), 0.f);
        x1 = std::max(std::min(x1, (float)(width - 1)), 0.f);
        y1 = std::max(std::min(y1, (float)(height - 1)), 0.f);

        objects[i].x = x0;
        objects[i].y = y0;
        objects[i].w = x1 - x0;
        objects[i].h = y1 - y0;
    }

    return 0;
}

}