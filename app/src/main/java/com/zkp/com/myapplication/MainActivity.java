package com.zkp.com.myapplication;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends Activity implements View.OnClickListener {

    private TextView textView;
    private Button btnCheckIfPng, btnHighLevelRead, btnLowLevelRead;
    private Button btnHighLevelWrite, btnLowLevelWrite;
    private boolean isPng = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        textView = (TextView) findViewById(R.id.tv);
        btnCheckIfPng = (Button) findViewById(R.id.check_if_png);
        btnCheckIfPng.setOnClickListener(this);
        btnHighLevelRead = (Button) findViewById(R.id.high_level_read);
        btnHighLevelRead.setOnClickListener(this);
        btnLowLevelRead = (Button) findViewById(R.id.low_level_read);
        btnLowLevelRead.setOnClickListener(this);
        btnHighLevelWrite = (Button) findViewById(R.id.high_level_write);
        btnHighLevelWrite.setOnClickListener(this);
        btnLowLevelWrite = (Button) findViewById(R.id.low_level_write);
        btnLowLevelWrite.setOnClickListener(this);
    }


    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.check_if_png:
                checkIfPng();
                break;
            case R.id.high_level_read:
                highLevelRead();
                break;
            case R.id.low_level_read:
                lowLevelReadLowLevelWrite();
                break;
            case R.id.high_level_write:
                highLevelReadHighLevelWrite();
                break;
            case R.id.low_level_write:
                break;
        }
    }

    public void checkIfPng() {
        isPng = PngUtils.checkIsPng("/sdcard/Download/ic_launcher.png");
        textView.setText(isPng + "");
        if (isPng) {
            btnHighLevelRead.setEnabled(true);
            btnLowLevelRead.setEnabled(true);
        } else {
            btnHighLevelRead.setEnabled(false);
            btnLowLevelRead.setEnabled(false);
        }
    }

    public void highLevelRead() {
        String msg = PngUtils.readPngHighLevel("/sdcard/Download/temp2.png");
        textView.setText(msg);
    }

    private void lowLevelReadLowLevelWrite() {
        String msg = PngUtils.lowLevelReadLowLevelWrite("/sdcard/Download/ic_launcher.png", "/sdcard/Download/temp2.png");
        textView.setText(msg);
    }

    public void highLevelReadHighLevelWrite() {
        String msg = PngUtils.highLevelReadHighLevelWrite("/sdcard/Download/ic_launcher.png", "/sdcard/Download/temp3.png");
        textView.setText(msg);
    }
}
