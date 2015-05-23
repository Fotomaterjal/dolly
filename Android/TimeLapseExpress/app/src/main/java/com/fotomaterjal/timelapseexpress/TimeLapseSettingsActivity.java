package com.fotomaterjal.timelapseexpress;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;


public class TimeLapseSettingsActivity extends BaseActivity {

    String TAG = "com.fotomaterjal.timelapseexpress";
    String nrOfKeyFramesString = "";
    String durationString = "";
    Button editKeyframesButton = null;
    Button readyButton = null;
    EditText nrOfKeyramesEditText = null;
    EditText durationEditText = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_time_lapse_settings);

        editKeyframesButton = (Button) findViewById(R.id.horDegEditText);
        nrOfKeyramesEditText = (EditText) findViewById(R.id.nrOfKeyramesEditText);
        durationEditText = (EditText) findViewById(R.id.durationEditText);
        readyButton = (Button) findViewById(R.id.startButton);

        //Do stuff to satisfy user BT addiction
        editKeyframesButton.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View arg0) {
                    Log.i(TAG, "have continue54 button");
                    nrOfKeyFramesString = String.valueOf(nrOfKeyramesEditText.getText()).trim();
                    durationString = String.valueOf(durationEditText.getText()).trim();
                    Log.i(TAG, "AYY LMAO");
                    Log.i(TAG, nrOfKeyFramesString);
                    Log.i(TAG, durationString);
                    if(!(nrOfKeyFramesString.equals("")) && !(durationString.equals(""))) {

                        int nrOfKeyframes = Integer.valueOf(String.valueOf(nrOfKeyramesEditText.getText()));
                        int duration = Integer.valueOf(String.valueOf(durationEditText.getText()));
                        Intent intent = new Intent(TimeLapseSettingsActivity.this, KeyframesListActivity.class);
                        intent.putExtra("nrOfKeyframes", nrOfKeyframes);
                        intent.putExtra("duration", duration);
                        startActivity(intent);
                    }else{
                        Toast.makeText(getApplicationContext(), "Both duration and number of keyframes must be set",
                                Toast.LENGTH_LONG).show();
                    }
                }
            }
        );

        //Do stuff to satisfy user BT addiction
        Log.i(TAG, "today");
        readyButton.setOnClickListener(
                new View.OnClickListener() {
                    @Override
                    public void onClick(View arg0) {
                        Intent intent = new Intent(TimeLapseSettingsActivity.this, StartSequenceActivity.class);
                        startActivity(intent);
                    }
                }
        );
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
