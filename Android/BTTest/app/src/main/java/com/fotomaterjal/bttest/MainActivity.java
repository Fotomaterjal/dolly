package com.fotomaterjal.bttest;

import android.annotation.TargetApi;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Build;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.ArrayAdapter;
import android.widget.ListAdapter;
import android.widget.ListView;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Set;
import java.util.UUID;


public class MainActivity extends ActionBarActivity {

    int REQUEST_ENABLE_BT = 1;

    //for debug filter
    String TAG = "com.fotomaterjal.bttest";
    private static String BTaddress = "98:D3:31:80:38:A9";
    private OutputStream outStream = null;
    private BluetoothAdapter mBluetoothAdapter = null;
    private BluetoothSocket btSocket = null;
    BluetoothDevice BTdevice = null;
    ConnectedThread conBT = null;
    //private BluetoothAdapter btAdapter = null;
    // Well known SPP UUID
    private static final UUID MY_UUID =
            UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    @TargetApi(Build.VERSION_CODES.GINGERBREAD_MR1)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        //for listview display
        final ArrayAdapter<String> myArrayAdapter = new ArrayAdapter<String>
                (this, android.R.layout.simple_list_item_1);
        final ListView myListView = (ListView) findViewById(R.id.myListView);

        //Check wether BT is supported
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (mBluetoothAdapter == null) {
            // Device does not support Bluetooth
            Log.i(TAG, "BT not supported");
        }else{
            Log.i(TAG, "I support BT!");
        }

        //Check if BT is switched on or off
        if (!mBluetoothAdapter.isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        }

        // Set up a pointer to the remote node using it's address.
        BTdevice = mBluetoothAdapter.getRemoteDevice(BTaddress);
        mBluetoothAdapter.cancelDiscovery();

        try {


            //btSocket.connect();
            btSocket = BTdevice.createInsecureRfcommSocketToServiceRecord (MY_UUID);
        } catch (IOException e) {
            Log.i(TAG,"Fatal Error " + "In onResume() and socket create failed: " + e.getMessage() + ".");
        }


        // Establish the connection.  This will block until it connects.
        Log.d(TAG, "...Connecting to Remote...");
        try {
            Method m = null;
            try {
                m = BTdevice.getClass().getMethod("createInsecureRfcommSocket", new Class[] {int.class});
            } catch (NoSuchMethodException e) {
                e.printStackTrace();
            }
            try {
                btSocket = (BluetoothSocket) m.invoke(BTdevice, 1);
            } catch (IllegalAccessException e) {
                e.printStackTrace();
            } catch (InvocationTargetException e) {
                e.printStackTrace();
            }
            mBluetoothAdapter.cancelDiscovery();
            //btSocket.
            String msdd;
            msdd = btSocket.getRemoteDevice().getName();
            Log.i(TAG, "MY DEVICE : " + msdd);
            btSocket.connect();
            Log.d(TAG, "...Connection established and data link opened...");
        } catch (IOException e) {
            Log.i(TAG, "socket error: " + e.getMessage());
            try {
                btSocket.close();
            } catch (IOException e2) {
                Log.i(TAG, "Fatal Error " + "In onResume() and unable to close socket " +
                        "during connection failure" + e2.getMessage() + ".");
            }
        }

        conBT = new ConnectedThread(btSocket);
        byte[] asi = new byte[1];
        asi[0] = '1';
        //int bytes = 0x6B000001;
        conBT.write(asi);


        /*try {
            outStream = btSocket.getOutputStream();
        } catch (IOException e) {
            Log.i(TAG,"Fatal Error " + "In onResume() and output stream creation failed:" + e.getMessage() + ".");
        }*/

        //sendData("1");

        //start looking for devices
        //mBluetoothAdapter.startDiscovery();


        // Register broadcastreceiver for discovering devices
        registerReceiver(mReceiver, filter); // Don't forget to unregister during onDestroy

        //Check for (PAIRED) devices
        Set<BluetoothDevice> pairedDevices = mBluetoothAdapter.getBondedDevices();
        // If there are paired devices
        if (pairedDevices.size() > 0) {
            String devName;
            String devAdr;

            // Loop through paired devices
            for (BluetoothDevice device : pairedDevices) {
                // Add the name and address to an array adapter to show in a ListView
                devName = device.getName();
                devAdr = device.getAddress();
                Log.i(TAG, "Paired devide: " + devName + ", " + devAdr);
                //myArrayAdapter.add("PAIRED: \n" + devName+ "\n" + devAdr);
            }
            //myListView.setAdapter(myArrayAdapter);
        }
        Log.i(TAG, String.valueOf(pairedDevices.size()) + " oli masinaid");

    }

    //Discover devices
    // Create a BroadcastReceiver for ACTION_FOUND
    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            // When discovery finds a device
            if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                // Get the BluetoothDevice object from the Intent
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                // Add the name and address to an array adapter to show in a ListView
                Log.i(TAG, "Found a device with name: " + device.getName());
                Log.i(TAG, "Address is: " + device.getAddress());
                //mArrayAdapter.add(device.getName() + "\n" + device.getAddress());
            }
        }
    };
    // Register the BroadcastReceiver
    IntentFilter filter = new IntentFilter(BluetoothDevice.ACTION_FOUND);

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    @Override
    public Intent registerReceiver(BroadcastReceiver receiver, IntentFilter filter) {
        return super.registerReceiver(receiver, filter);
    }

    /*private void sendData(String message) {
        byte[] msgBuffer = message.getBytes();

        Log.d(TAG, "...Sending data: " + message + "...");



        try {
            outStream.write(msgBuffer);
        } catch (IOException e) {
            String msg = "In onResume() and an exception occurred during write: " + e.getMessage();
            Log.i(TAG, "jouki " + msg);
            //msg = msg +  ".\n\nCheck that the SPP UUID: " + MY_UUID.toString() + " exists on server.\n\n";


        }
    }*/


    private class ConnectedThread extends Thread {
        private final BluetoothSocket mmSocket;
        private final InputStream mmInStream;
        private final OutputStream mmOutStream;

        public ConnectedThread(BluetoothSocket socket) {
            mmSocket = socket;
            InputStream tmpIn = null;
            OutputStream tmpOut = null;

            // Get the input and output streams, using temp objects because
            // member streams are final
            try {
                tmpIn = socket.getInputStream();
                tmpOut = socket.getOutputStream();
            } catch (IOException e) {
                Log.i(TAG, "NO out or inputstream!");
            }

            mmInStream = tmpIn;
            mmOutStream = tmpOut;
        }

        public void run() {
            byte[] buffer = new byte[1024];  // buffer store for the stream
            int bytes; // bytes returned from read()

            // Keep listening to the InputStream until an exception occurs
            while (true) {
                try {
                    // Read from the InputStream
                    bytes = mmInStream.read(buffer);
                    // Send the obtained bytes to the UI activity
                    //mHandler.obtainMessage(MESSAGE_READ, bytes, -1, buffer)
                    //      .sendToTarget();
                } catch (IOException e) {
                    break;
                }
            }
        }

        /* Call this from the main activity to send data to the remote device */
        public void write(byte[] bytes) {
            try {
                mmOutStream.write(bytes);
            } catch (IOException e) {

                Log.i(TAG, "I just cant send stuff!");
                Log.i(TAG, e.getMessage());
            }
        }

        /* Call this from the main activity to shutdown the connection */
        public void cancel() {
            try {
                mmSocket.close();
            } catch (IOException e) { }
        }
    }
}


