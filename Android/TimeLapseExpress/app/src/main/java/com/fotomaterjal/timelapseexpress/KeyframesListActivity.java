package com.fotomaterjal.timelapseexpress;

import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.content.Intent;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.ListView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.Map;


public class KeyframesListActivity extends BaseActivity {

    String TAG = "com.fotomaterjal.timelapseexpress";
    static ArrayList<keyFrame> keyFrameList = new ArrayList<keyFrame>(10);
    int duration = 1000;
    public Button doneEditingButton;




    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_keyframes_list);
        doneEditingButton = (Button) findViewById(R.id.doneEditingButton);

        Bundle keyFrameData = getIntent().getExtras();
        if(keyFrameData != null){
            duration = keyFrameData.getInt("duration");
            int nrOfKeyframes = keyFrameData.getInt("nrOfKeyframes");

            Log.i(TAG, "im here now biaatch2" + String.valueOf(duration));
            Log.i(TAG, "im here now biaatch2" + String.valueOf(nrOfKeyframes));

            //ArrayList<keyFrame> keyFrameList = new ArrayList<keyFrame>(10);
            if((keyFrameList.size() == 0)) {
                for (int i = 0; i < nrOfKeyframes; i++) {
                    keyFrameList.add(new keyFrame());
                }
            }else if(nrOfKeyframes >= keyFrameList.size()){
                // add some extra keyframes
                int toAdd = 0;
                toAdd = nrOfKeyframes - keyFrameList.size();
                for(int k = 0; k < toAdd; k++){
                    keyFrameList.add(new keyFrame());
                }
            }else{
                keyFrameList.clear();
                for (int i = 0; i < nrOfKeyframes; i++) {
                    keyFrameList.add(new keyFrame());
                }
            }
        }







        final KeyframesListAdapter myAdapter = new KeyframesListAdapter(this);

        myAdapter.setData(keyFrameList);
        ListView myListView = (ListView) findViewById(R.id.keyFrameListView);
        myListView.setAdapter(myAdapter);
        Log.i(TAG, "im here now biaatch");


        myListView.setOnItemClickListener(
                new AdapterView.OnItemClickListener(){
                    @Override
                    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                        Log.i(TAG, "joujoujoujououjou " + String.valueOf(keyFrameList.get(position).mDistance));
                        keyFrame mKeyframe = keyFrameList.get(position);

                        int mDistance = mKeyframe.mDistance;
                        int mHorDeg = mKeyframe.mHorDeg;
                        int mVerDeg = mKeyframe.mVerDeg;
                        int mTime = mKeyframe.mTime;

                        Intent intent = new Intent(KeyframesListActivity.this, EditKeyFrame.class);
                        intent.putExtra("mDistance", mDistance);
                        intent.putExtra("mHorDeg", mHorDeg);
                        intent.putExtra("mVerDeg", mVerDeg);
                        intent.putExtra("mTime", mTime);
                        intent.putExtra("position", position);
                        intent.putExtra("maxDuration", duration);
                        startActivity(intent);
                    }
                }
        );

        doneEditingButton.setOnClickListener(
                new View.OnClickListener() {
                    @Override
                    public void onClick(View arg0) {
                        Intent intent = new Intent(KeyframesListActivity.this, StartSequenceActivity.class);
                        startActivity(intent);
                    }
                }
        );

    }



    public static void setKeyFrameListElement(int position, keyFrame mKeyFrame) {
        keyFrameList.set(position, mKeyFrame);
    }

    public static ArrayList<keyFrame> getKeyFrameList(){
        return keyFrameList;
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

    @Override
    protected void onResume() {
        super.onResume();
        final KeyframesListAdapter myAdapter = new KeyframesListAdapter(this);

        myAdapter.setData(keyFrameList);
        ListView myListView = (ListView) findViewById(R.id.keyFrameListView);
        myListView.setAdapter(myAdapter);
        Log.i(TAG, "im here now yeah buiii");
    }


}
