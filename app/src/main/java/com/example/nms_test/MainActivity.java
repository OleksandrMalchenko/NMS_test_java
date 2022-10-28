package com.example.nms_test;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.os.Bundle;
import android.util.Log;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import com.example.nms_test.YOLOv5Method;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        float[] preds = ReadFromfile("filename1824.txt", this.getApplicationContext());
        Log.e("MainActivity", "1111111111: ");
        int ret = YOLOv5Method.nms(preds, 0.25f, 0.45f, null, 0,
                0, 0, 300, 0);
        Log.e("MainActivity", "NMS return: " + ret);
    }

    public float[] ReadFromfile(String fileName, Context context) {
        float[] preds = new float[30240];
        InputStream fIn = null;
        InputStreamReader isr = null;
        BufferedReader input = null;
        try {
            fIn = context.getResources().getAssets()
                    .open(fileName, Context.MODE_WORLD_READABLE);
            isr = new InputStreamReader(fIn);
            input = new BufferedReader(isr);
            String line = "";
            int idx = 0;
            while ((line = input.readLine()) != null) {
                preds[idx] = Float.parseFloat(line);
                idx++;
            }
        } catch (Exception e) {
            e.getMessage();
        } finally {
            try {
                if (isr != null)
                    isr.close();
                if (fIn != null)
                    fIn.close();
                if (input != null)
                    input.close();
            } catch (Exception e2) {
                e2.getMessage();
            }
        }
        return preds;
    }
}