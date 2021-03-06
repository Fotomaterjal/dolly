package com.fotomaterjal.timelapseexpress;

import android.app.Activity;
import android.content.Intent;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;


public class BaseActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_base);
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

        // Go to Bluetooth settings
        if (id == R.id.action_bt_settings) {
            Intent i = new Intent(this, btActivity.class);
            startActivity(i);
            return true;
            // Go to Time Lapse Settings activity
        }else if(id == R.id.action_time_lapse_settings) {
            Intent i = new Intent(this, TimeLapseSettingsActivity.class);
            startActivity(i);
            return true;
        }else if(id == R.id.action_main_screen) {
            Intent i = new Intent(this, MainActivity.class);
            startActivity(i);
            return true;
        }
        return super.onOptionsItemSelected(item);
    }
}
