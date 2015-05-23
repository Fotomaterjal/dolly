package com.fotomaterjal.timelapseexpress;

import android.content.Intent;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.SeekBar;
import android.widget.Toast;


public class EditKeyFrame extends BaseActivity {

    EditText distEditText = null;
    EditText horDegEditText = null;
    EditText verDegEditText = null;
    EditText timeEditText = null;
    Button saveKeyframeButton = null;
    SeekBar distSeekBar = null;
    SeekBar horDegSeekBar = null;
    SeekBar verDegSeekBar = null;
    SeekBar timeEditSeekBar = null;


    int mDistance;
    int mHorDeg;
    int mVerDeg;
    int mTime;
    int position;
    int maxDuration;

    @Override
    public void onBackPressed() {
        super.onBackPressed();

        keyFrame mKeyFrame = new keyFrame();
        mKeyFrame.mDistance = Integer.valueOf(String.valueOf(distEditText.getText()));
        mKeyFrame.mHorDeg = Integer.valueOf(String.valueOf(horDegEditText.getText()));
        mKeyFrame.mVerDeg = Integer.valueOf(String.valueOf(verDegEditText.getText()));
        mKeyFrame.mTime = Integer.valueOf(String.valueOf(timeEditText.getText()));

        if(mKeyFrame.mTime > maxDuration){
            mKeyFrame.mTime = maxDuration;
            Toast.makeText(getApplicationContext(), "Time since start too big, changed to max value",
                    Toast.LENGTH_LONG).show();
        }

        KeyframesListActivity.setKeyFrameListElement(position, mKeyFrame);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_edit_key_frame);

        distEditText = (EditText) findViewById(R.id.distEditText);
        horDegEditText = (EditText) findViewById(R.id.horDegEditText);
        verDegEditText = (EditText) findViewById(R.id.verDegEditText);
        timeEditText = (EditText) findViewById(R.id.timeEditText);
        saveKeyframeButton = (Button) findViewById(R.id.saveKeyframeButton);

        distSeekBar = (SeekBar) findViewById(R.id.distSeekBar);
        horDegSeekBar = (SeekBar) findViewById(R.id.horDegSeekBar);
        verDegSeekBar = (SeekBar) findViewById(R.id.verDegSeekBar);
        timeEditSeekBar = (SeekBar) findViewById(R.id.timeEditSeekBar);


        // get data from intent extra
        Bundle elementDetails = getIntent().getExtras();
        mDistance = (int) elementDetails.get("mDistance");
        mHorDeg = (int) elementDetails.get("mHorDeg");
        mVerDeg = (int) elementDetails.get("mVerDeg");
        mTime = (int) elementDetails.get("mTime");
        position = (int) elementDetails.get("position");
        maxDuration = (int) elementDetails.get("maxDuration");

        distEditText.setText(String.valueOf(mDistance));
        horDegEditText.setText(String.valueOf(mHorDeg));
        verDegEditText.setText(String.valueOf(mVerDeg));
        timeEditText.setText(String.valueOf(mTime));

        distSeekBar.setMax(110);
        horDegSeekBar.setMax(360);
        verDegSeekBar.setMax(360);
        timeEditSeekBar.setMax(maxDuration);

        distSeekBar.setProgress(mDistance);
        horDegSeekBar.setProgress(mHorDeg);
        verDegSeekBar.setProgress(mVerDeg);
        timeEditSeekBar.setProgress(mTime);




        //Do stuff
        saveKeyframeButton.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View arg0) {
                    onBackPressed();
                }
            }
        );

        distSeekBar.setOnSeekBarChangeListener(
                new SeekBar.OnSeekBarChangeListener() {
                    @Override
                    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                        distEditText.setText(String.valueOf(progress));
                    }
                    @Override
                    public void onStartTrackingTouch(SeekBar seekBar) {}
                    @Override
                    public void onStopTrackingTouch(SeekBar seekBar) {}
                }
        );

        horDegSeekBar.setOnSeekBarChangeListener(
                new SeekBar.OnSeekBarChangeListener() {
                    @Override
                    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                        horDegEditText.setText(String.valueOf(progress));
                    }
                    @Override
                    public void onStartTrackingTouch(SeekBar seekBar) {}
                    @Override
                    public void onStopTrackingTouch(SeekBar seekBar) {}
                }
        );

        verDegSeekBar.setOnSeekBarChangeListener(
                new SeekBar.OnSeekBarChangeListener() {
                    @Override
                    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                        verDegEditText.setText(String.valueOf(progress));
                    }
                    @Override
                    public void onStartTrackingTouch(SeekBar seekBar) {}
                    @Override
                    public void onStopTrackingTouch(SeekBar seekBar) {}
                }
        );

        timeEditSeekBar.setOnSeekBarChangeListener(
                new SeekBar.OnSeekBarChangeListener() {
                    @Override
                    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                        timeEditText.setText(String.valueOf(progress));
                    }
                    @Override
                    public void onStartTrackingTouch(SeekBar seekBar) {}
                    @Override
                    public void onStopTrackingTouch(SeekBar seekBar) {}
                }
        );

        distEditText.setOnFocusChangeListener(new View.OnFocusChangeListener() {
            @Override
            public void onFocusChange(View v, boolean hasFocus) {
                distSeekBar.setProgress(Integer.valueOf(String.valueOf(distEditText.getText())));
            }
        });

        horDegEditText.setOnFocusChangeListener(new View.OnFocusChangeListener() {
            @Override
            public void onFocusChange(View v, boolean hasFocus) {
                horDegSeekBar.setProgress(Integer.valueOf(String.valueOf(horDegEditText.getText())));
            }
        });

        verDegEditText.setOnFocusChangeListener(new View.OnFocusChangeListener() {
            @Override
            public void onFocusChange(View v, boolean hasFocus) {
                verDegSeekBar.setProgress(Integer.valueOf(String.valueOf(verDegEditText.getText())));
            }
        });

        timeEditText.setOnFocusChangeListener(new View.OnFocusChangeListener() {
            @Override
            public void onFocusChange(View v, boolean hasFocus) {
                timeEditSeekBar.setProgress(Integer.valueOf(String.valueOf(timeEditText.getText())));
            }
        });
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_base, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        return super.onOptionsItemSelected(item);
    }
}
