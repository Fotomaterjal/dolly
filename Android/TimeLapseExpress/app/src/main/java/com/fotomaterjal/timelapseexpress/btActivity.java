package com.fotomaterjal.timelapseexpress;

import android.app.Activity;
import android.app.ProgressDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import java.util.ArrayList;
import java.util.Set;


public class btActivity extends BaseActivity {

    String TAG = "com.fotomaterjal.timelapseexpress";
    private BluetoothAdapter mBluetoothAdapter = null;
    private ProgressDialog scanDialog;
    public Button btEnableDisableButton = null;
    public Button btScanButton = null;
    public Button btPairedButton = null;
    public Button btContinueButton = null;
    //public Button btPairButton = null;
    public TextView btOnOffText = null;
    SharedPreferences myPrefs = null;
    private ArrayList<BluetoothDevice> btDeviceList = new ArrayList<BluetoothDevice>();
    int REQUEST_ENABLE_BT = 1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_bt);

        btOnOffText = (TextView) findViewById(R.id.btOnOffText);
        btEnableDisableButton = (Button) findViewById(R.id.btEnableDisableButton);
        btScanButton = (Button) findViewById(R.id.btScanButton);
        btPairedButton = (Button) findViewById(R.id.btPairedButton);
        btContinueButton = (Button) findViewById(R.id.btContinueButton);
        //btPairButton = (Button) findViewById(R.id.btPairButton);

        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        myPrefs = this.getSharedPreferences(
                "com.fotomaterjal.timelapseexpress", Context.MODE_PRIVATE);

        // Progress dialog for asynchronous Bluetooth device scanning
        scanDialog = new ProgressDialog(this);
        scanDialog.setMessage("Looking for devices...");
        scanDialog.setCancelable(false);
        scanDialog.setButton(DialogInterface.BUTTON_NEGATIVE, "Cancel", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
                mBluetoothAdapter.cancelDiscovery();
            }
        });


        // Check whether BT Adapter exists
        if (mBluetoothAdapter == null) {
            // Device does not support Bluetooth
            Log.i(TAG, "BT not supported");
            showToast("BT not supported");
            return;
        }else{
            Log.i(TAG, "I support BT!");
        }

        // Check if BT is switched on or off
        if (!mBluetoothAdapter.isEnabled()) {
            btOnOffText.setText("Bluetooth is not enabled");
            btEnableDisableButton.setText("Enable Bluetooth");
        }else{
            btOnOffText.setText("Bluetooth is enabled");
            btEnableDisableButton.setText("Disable Bluetooth");
        }

        showToast("Make sure you are ONLY paired to the time lapse device");

        // Do stuff to satisfy user BT addiction
        btEnableDisableButton.setOnClickListener(
                new View.OnClickListener() {
                    @Override
                    public void onClick(View arg0) {
                        if (!mBluetoothAdapter.isEnabled()) {
                            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
                        }else{
                            mBluetoothAdapter.disable();
                        }
                    }
                }
        );


        // Scan nearby Bluetooth devices
        btScanButton.setOnClickListener(
                new View.OnClickListener() {
                    @Override
                    public void onClick(View arg0) {
                        mBluetoothAdapter.startDiscovery();
                    }
                }
        );

        // See paired Bluetooth Devices
        btPairedButton.setOnClickListener(
                new View.OnClickListener() {
                    @Override
                    public void onClick(View arg0) {
                        Set<BluetoothDevice> pairedDevices = mBluetoothAdapter.getBondedDevices();
                        if (pairedDevices == null || pairedDevices.size() == 0) {
                            showToast("No Paired Devices Found");
                        } else {
                            ArrayList<BluetoothDevice> list = new ArrayList<BluetoothDevice>();
                            list.addAll(pairedDevices);

                            Intent intent = new Intent(btActivity.this, btDevicesListActivity.class);
                            intent.putParcelableArrayListExtra("device.list", list);
                            startActivity(intent);
                        }
                    }
                }
        );

        // Continue on to Time Lapse Settings Activity
        btContinueButton.setOnClickListener(
                new View.OnClickListener() {
                    @Override
                    public void onClick(View arg0) {
                        Intent intent = new Intent(btActivity.this, TimeLapseSettingsActivity.class);
                        startActivity(intent);
                    }
                }
        );

        // Stuff for BT action change broadcast listeners
        IntentFilter btFilter = new IntentFilter();
        btFilter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED);
        btFilter.addAction(BluetoothDevice.ACTION_FOUND);
        btFilter.addAction(BluetoothAdapter.ACTION_DISCOVERY_STARTED);
        btFilter.addAction(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);
        btFilter.addAction(BluetoothDevice.ACTION_BOND_STATE_CHANGED);
        registerReceiver(btReceiver, btFilter);
    }





    // BroadcastReceiver for listening BT state changes
    private final BroadcastReceiver btReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            Log.i(TAG, action.toString());
            if (BluetoothAdapter.ACTION_STATE_CHANGED.equals(action)) {
                // Change UI elements' texts
                if (intent.getIntExtra(BluetoothAdapter.EXTRA_STATE, -1)
                        == BluetoothAdapter.STATE_OFF) {
                    btOnOffText.setText("Bluetooth is disabled");
                    btEnableDisableButton.setText("Enable Bluetooth");
                    Log.i(TAG, "BT changed");
                } else {
                    btOnOffText.setText("Bluetooth is enabled");
                    btEnableDisableButton.setText("Disable Bluetooth");
                    Log.i(TAG, "BT changed");
                }

            }else if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                // Get the BluetoothDevice object from the Intent
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                // Add the name and address to an array adapter to show in a ListView
                btDeviceList.add(device);
                Log.i(TAG, "found device");
                Log.i(TAG, device.getName() + ", " + device.getAddress());
                // Hardcoded at the moment
                if(device.getName().equals("HC-06")){
                    SharedPreferences.Editor editor = myPrefs.edit();
                    editor.putString("btAdapterAddress", device.getAddress());
                    editor.commit();
                }

            }else if(BluetoothAdapter.ACTION_DISCOVERY_STARTED.equals(action)){
                scanDialog.show();
                //btDeviceList = new ArrayList<BluetoothDevice>();
            }else if(BluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals(action)){
                scanDialog.dismiss();
                Intent newIntent = new Intent(btActivity.this, btDevicesListActivity.class);
                newIntent.putParcelableArrayListExtra("device.list", btDeviceList);
                // Go and display the found results
                startActivity(newIntent);
            }else if(BluetoothDevice.ACTION_BOND_STATE_CHANGED.equals(action)){
                Intent newIntent = new Intent(btActivity.this, btActivity.class);
                startActivity(newIntent);
            }
        }
    };



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

    private void showToast(String message) {
        Toast.makeText(getApplicationContext(), message, Toast.LENGTH_SHORT).show();
    }


}
