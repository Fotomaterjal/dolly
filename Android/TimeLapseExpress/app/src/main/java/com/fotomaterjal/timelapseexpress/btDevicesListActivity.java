package com.fotomaterjal.timelapseexpress;

import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.ListAdapter;
import android.widget.ListView;
import android.widget.Toast;

import java.util.ArrayList;


public class btDevicesListActivity extends BaseActivity {

    private btDevicesAdapter btDevicesAdapter;
    private ArrayList<BluetoothDevice> btDeviceList;



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_bt_devices_list);
        btDevicesAdapter = new btDevicesAdapter(this);
        btDeviceList = getIntent().getExtras().getParcelableArrayList("device.list");
        btDevicesAdapter.setData(btDeviceList);
        ListView myListView = (ListView) findViewById(R.id.btListView);
        myListView.setAdapter(btDevicesAdapter);

    }
}
