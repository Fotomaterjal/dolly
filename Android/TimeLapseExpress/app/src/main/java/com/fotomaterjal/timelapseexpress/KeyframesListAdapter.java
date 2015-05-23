package com.fotomaterjal.timelapseexpress;

import android.content.Context;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;


import java.util.ArrayList;


public class KeyframesListAdapter extends BaseAdapter {

    String TAG = "com.fotomaterjal.timelapseexpress";

    ArrayList<keyFrame> keyFrameList = new ArrayList<keyFrame>(10);
    String shiSize = String.valueOf(keyFrameList.size());
    TextView keyframeTitleText = null;
    TextView distText = null;
    TextView horDegText = null;
    TextView verDegText = null;
    TextView timeText = null;
    private LayoutInflater keyframeListInflater;

    public KeyframesListAdapter(Context context) {
        keyframeListInflater = LayoutInflater.from(context);
    }

    public void setData(ArrayList<keyFrame> keyframeList){
        this.keyFrameList = keyframeList;
        Log.i(TAG, "im setttdaaataaaa!!!");
    }


    @Override
    public int getCount() {
        Log.i(TAG, "HISIZE: " + shiSize + " other: " + String.valueOf(keyFrameList.size()));
        return keyFrameList.size();

    }

    @Override
    public Object getItem(int position) {
        return null;
    }

    @Override
    public long getItemId(int position) {
        return 0;
    }

    @Override
    public View getView(int position, View view, ViewGroup parent) {
        view =  keyframeListInflater.inflate(R.layout.activity_keyframes_list_adapter, null);
        Log.i(TAG, "get view man");
        keyframeTitleText = (TextView) view.findViewById(R.id.keyframeTitleText);
        distText = (TextView) view.findViewById(R.id.distText);
        horDegText = (TextView) view.findViewById(R.id.horDegText);
        verDegText = (TextView) view.findViewById(R.id.verDegText);
        timeText = (TextView) view.findViewById(R.id.timeText);

        keyFrame mKeyframe = keyFrameList.get(position);


        Log.i(TAG, "NULLPOIUNTER: " + String.valueOf(mKeyframe.mDistance));

        keyframeTitleText.setText("Keyframe " + String.valueOf(position));
        distText.setText("Distance: " + String.valueOf(mKeyframe.mDistance));
        horDegText.setText("Hor degrees: " + String.valueOf(mKeyframe.mHorDeg));
        verDegText.setText("Ver degrees: " + String.valueOf(mKeyframe.mVerDeg));
        timeText.setText("Time: " + String.valueOf(mKeyframe.mTime));

        Log.i(TAG, "Dist: " + String.valueOf(mKeyframe.mDistance));

        return view;

    }
}
