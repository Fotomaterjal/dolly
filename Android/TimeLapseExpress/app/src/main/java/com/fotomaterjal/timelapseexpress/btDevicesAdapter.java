package com.fotomaterjal.timelapseexpress;

import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;


public class btDevicesAdapter extends BaseAdapter {

    String TAG = "com.fotomaterjal.timelapseexpress";
    TextView btDevName = null;
    Button btPairButton = null;
    Context myContext = null;


    private List<BluetoothDevice> btDevicesList;
    private LayoutInflater btListInflater;
    private List<BluetoothDevice> btDeviceList;

    public btDevicesAdapter(Context context, ArrayList<BluetoothDevice> btList) {
        btDevicesList = btList;

    }

    public btDevicesAdapter(Context context) {
        btListInflater = LayoutInflater.from(context);
        this.myContext = context;
    }

    public void setData(List<BluetoothDevice> data) {
        btDeviceList = data;
    }

    public int getCount() {
        return (btDeviceList == null) ? 0 : btDeviceList.size();
    }

    public BluetoothDevice getItem(int position) {
        return null;
    }

    public long getItemId(int position) {
        return position;
    }

    public View getView(final int position, View view, ViewGroup parent) {
        view =  btListInflater.inflate(R.layout.bt_devices_row, null);
        btDevName = (TextView) view.findViewById(R.id.btDevName);
        btPairButton = (Button) view.findViewById(R.id.btPairButton);
        BluetoothDevice device	= btDeviceList.get(position);
        btDevName.setText(device.getName());
        btPairButton.setText((device.getBondState() == BluetoothDevice.BOND_BONDED) ? "Unpair" : "Pair");


        btPairButton.setOnClickListener(
                new View.OnClickListener() {
                    @Override
                    public void onClick(View arg0) {
                        BluetoothDevice device = btDeviceList.get(position);
                        Log.i(TAG, String.valueOf(device.getBondState()));
                        if (device.getBondState() == BluetoothDevice.BOND_BONDED) {
                            unpairDevice(device);

                        } else {
                            pairDevice(device);

                        }
                    }
                }
        );
        return view;
    }

    private void pairDevice(BluetoothDevice device) {
        try {
            Method method = device.getClass().getMethod("createBond", (Class[]) null);
            method.invoke(device, (Object[]) null);
            Log.i(TAG, "pairing successful");
            showToast("Pairing successful");
        } catch (Exception e) {
            e.printStackTrace();
        }

    }


    private void unpairDevice(BluetoothDevice device) {
        try {
            Method method = device.getClass().getMethod("removeBond", (Class[]) null);
            method.invoke(device, (Object[]) null);
            //btDeviceList.remove(device);
            Log.i(TAG, "unpairing successful");
            showToast("Unpairing successful");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void showToast(String message) {
        Toast.makeText(myContext, message, Toast.LENGTH_SHORT).show();
    }
}
