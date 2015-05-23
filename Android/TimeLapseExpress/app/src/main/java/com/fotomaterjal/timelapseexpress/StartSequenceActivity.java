package com.fotomaterjal.timelapseexpress;

import android.annotation.TargetApi;
import android.app.ActionBar;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Build;
import android.support.annotation.Nullable;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.Toast;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.UUID;


public class StartSequenceActivity extends BaseActivity {

    int speedPercentage = 100;
    Button startButton = null;
    ArrayList<keyFrame> keyFrameList = null;
    int REQUEST_ENABLE_BT = 1;
    int btSocketConnected = 0;
    String btAdapterAddress = null;
    private OutputStream outStream = null;
    private BluetoothAdapter mBluetoothAdapter = null;
    private BluetoothSocket btSocket = null;
    BluetoothDevice BTdevice = null;
    ConnectedThread conBT = null;
    SharedPreferences sp = null;
    //for debug filter
    String TAG = "com.fotomaterjal.bttest";
    private static final UUID MY_UUID =
            UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    @TargetApi(Build.VERSION_CODES.GINGERBREAD_MR1)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_start_sequence);

        startButton = (Button) findViewById(R.id.startButton);

        ImageButton moveRightButton = (ImageButton) findViewById(R.id.moveRightButton);
        ImageButton moveLeftButton = (ImageButton) findViewById(R.id.moveLeftButton );
        ImageButton moveUpButton = (ImageButton) findViewById(R.id.moveUpButton);
        ImageButton moveDownButton = (ImageButton) findViewById(R.id.moveDownButton);
        ImageButton moveLeftRotateButton = (ImageButton) findViewById(R.id.moveLeftRotateButton);
        ImageButton moveRightRotateButton = (ImageButton) findViewById(R.id.moveRightRotateButton);

        final ArrayList<keyFrame> keyFrameList = KeyframesListActivity.getKeyFrameList();


        //STUFF FOR BT INITIALIZATION
        sp = getSharedPreferences("com.fotomaterjal.timelapseexpress", Activity.MODE_PRIVATE);
        btAdapterAddress = sp.getString("btAdapterAddress", "98:D3:31:80:38:A9");
        Log.i(TAG, "MY ADAPTER MAN: " + btAdapterAddress);

        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (mBluetoothAdapter == null) {
            // Device does not support Bluetooth
            Log.i(TAG, "BT not supported");
            Toast.makeText(getApplicationContext(), "Bluetooth not supported",
                    Toast.LENGTH_LONG).show();
            // Go back to home screen and end activity and clear the back stack
            Intent intent = new Intent(this, MainActivity.class);
            intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_NEW_TASK);
            startActivity(intent);
            finish(); // call this to finish the current activity
        }else{
            Log.i(TAG, "I support BT!");
        }

        //Check if BT is switched on or off
        if (!mBluetoothAdapter.isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        }
        BTdevice = mBluetoothAdapter.getRemoteDevice(btAdapterAddress);
        mBluetoothAdapter.cancelDiscovery();

        try {
            //btSocket.connect();
            btSocket = BTdevice.createInsecureRfcommSocketToServiceRecord (MY_UUID);
        } catch (IOException e) {
            Log.i(TAG,"Fatal Error " + "In onResume() and socket create failed: " + e.getMessage() + ".");
            btSocketConnected = 0;
        }
        // Establish the connection.  This will block until it connects.
        Log.d(TAG, "...Connecting to Remote...");
        try {
            Method m = null;
            try {
                m = BTdevice.getClass().getMethod("createInsecureRfcommSocket", new Class[] {int.class});
            } catch (NoSuchMethodException e) {
                e.printStackTrace();
                btSocketConnected = 0;
            }
            try {
                btSocket = (BluetoothSocket) m.invoke(BTdevice, 1);
            } catch (IllegalAccessException e) {
                e.printStackTrace();
                btSocketConnected = 0;
            } catch (InvocationTargetException e) {
                e.printStackTrace();
                btSocketConnected = 0;
            }
            mBluetoothAdapter.cancelDiscovery();
            String myBtDeviceName;
            myBtDeviceName = btSocket.getRemoteDevice().getName();
            Log.i(TAG, "MY DEVICE : " + myBtDeviceName);
            btSocket.connect();
            Log.d(TAG, "...Connection established and data link opened...");
            btSocketConnected = 1;
        }catch (IOException e) {
            Log.i(TAG, "socket error: " + e.getMessage());
            try {
                btSocket.close();
                btSocketConnected = 0;
            } catch (IOException e2) {
                Log.i(TAG, "Fatal Error " + "In onResume() and unable to close socket " +
                        "during connection failure" + e2.getMessage() + ".");
                btSocketConnected = 0;
            }
        }

        if(btSocketConnected != 1){
            Toast.makeText(getApplicationContext(), "Bluetooth connection error, try again",
                    Toast.LENGTH_LONG).show();
            onBackPressed();
        }

        conBT = new ConnectedThread(btSocket);

        //Do stuff to satisfy user BT addiction

        startButton.setOnClickListener(
                new View.OnClickListener() {
                    @Override
                    public void onClick(View arg0) {
                        sendBtData(keyFrameList);
                        sendStartSequenceCommand();
                    }
                }
        );

        moveRightButton.setOnTouchListener(new View.OnTouchListener() {
            public boolean onTouch(View yourButton, MotionEvent theMotion) {
                switch (theMotion.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        //send data to distTimer, direction = 1
                        sendMoveCommand(1, 1, speedPercentage, 0);
                        Log.i(TAG, "I JUST ACTION DOWN");
                        break;
                    case MotionEvent.ACTION_UP:
                        //send data to distTimer, halt timer
                        sendMoveCommand(1, 0, speedPercentage, 0);
                        Log.i(TAG, "I JUST ACTION UP");
                        break;
                }
                return true;
            }
        });

        moveLeftButton.setOnTouchListener(new View.OnTouchListener() {
            public boolean onTouch(View yourButton, MotionEvent theMotion) {
                switch (theMotion.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        //send data to distTimer, direction = 0
                        sendMoveCommand(1, 1, speedPercentage, 1);
                        Log.i(TAG, "I JUST ACTION DOWN l");
                        break;
                    case MotionEvent.ACTION_UP:
                        //send data to distTimer, halt timer
                        sendMoveCommand(1, 0, speedPercentage, 1);
                        Log.i(TAG, "I JUST ACTION UP l");
                        break;
                }
                return true;
            }
        });

        moveRightRotateButton.setOnTouchListener(new View.OnTouchListener() {
            public boolean onTouch(View yourButton, MotionEvent theMotion) {
                switch (theMotion.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        //send data to horTimer, direction = 1 (clockwise)
                        sendMoveCommand(2, 1, speedPercentage/2, 1);
                        Log.i(TAG, "I JUST ACTION DOWN l");
                        break;
                    case MotionEvent.ACTION_UP:
                        //send data to distTimer, halt timer
                        sendMoveCommand(2, 0, speedPercentage/2, 1);
                        Log.i(TAG, "I JUST ACTION UP l");
                        break;
                }
                return true;
            }
        });

        moveLeftRotateButton.setOnTouchListener(new View.OnTouchListener() {
            public boolean onTouch(View yourButton, MotionEvent theMotion) {
                switch (theMotion.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        //send data to horTimer, direction = 0 (anticlockwise)
                        sendMoveCommand(2, 1, speedPercentage/2, 0);
                        Log.i(TAG, "I JUST ACTION DOWN l");
                        break;
                    case MotionEvent.ACTION_UP:
                        //send data to distTimer, halt timer
                        sendMoveCommand(2, 0, speedPercentage/2, 0);
                        Log.i(TAG, "I JUST ACTION UP l");
                        break;
                }
                return true;
            }
        });

        moveUpButton.setOnTouchListener(new View.OnTouchListener() {
            public boolean onTouch(View yourButton, MotionEvent theMotion) {
                switch (theMotion.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        //send data to horTimer, direction = 1 (anticlockwise)
                        sendMoveCommand(3, 1, 50, 1);
                        Log.i(TAG, "I JUST ACTION DOWN l");
                        break;
                    case MotionEvent.ACTION_UP:
                        //send data to distTimer, halt timer
                        sendMoveCommand(3, 0, 50, 1);
                        Log.i(TAG, "I JUST ACTION UP l");
                        break;
                }
                return true;
            }
        });

        moveDownButton.setOnTouchListener(new View.OnTouchListener() {
            public boolean onTouch(View yourButton, MotionEvent theMotion) {
                switch (theMotion.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        //send data to horTimer, direction = 0 (anticlockwise)
                        sendMoveCommand(3, 1, 50, 0);
                        Log.i(TAG, "I JUST ACTION DOWN l");
                        break;
                    case MotionEvent.ACTION_UP:
                        //send data to distTimer, halt timer
                        sendMoveCommand(3, 0, 50, 0);
                        Log.i(TAG, "I JUST ACTION UP l");
                        break;
                }
                return true;
            }
        });



    }

    private void sendMoveCommand(int timerNr, int onOff, int speed, int direction){
        byte[] moveCommandPacket = new byte[5];
        moveCommandPacket[0] = 0x6D;    // in hardware 'm' involves moving the device
        moveCommandPacket[1] = (byte) (timerNr);     // lower byte
        moveCommandPacket[2] = (byte) (onOff);       // lower byte
        moveCommandPacket[3] = (byte) (speed);       // lower byte
        moveCommandPacket[4] = (byte) (direction);   // lower byte
        Log.i(TAG, "MY onOff value is " + String.valueOf(Integer.valueOf(moveCommandPacket[2])));
        // ... and away we go
        try {
            conBT.write(moveCommandPacket);
        } catch (IOException e) {
            e.printStackTrace();
        }

    }


    private void sendStartSequenceCommand(){
        byte[] dataToTransfer = new byte[1];
        dataToTransfer[0] = (byte) 0x7A;    // in hardware 'z' starts sequence
        try {
            conBT.write(dataToTransfer);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void sendBtOrderNr(int curNumber){
        byte[] dataToTransfer = new byte[2];
        dataToTransfer[0] = (byte) 0x73;               // starts with 's'
        dataToTransfer[1] = (byte) (curNumber & 0xFF); // lower byte
        Log.i(TAG, "sent byte: " + String.valueOf(curNumber));
        try {
            conBT.write(dataToTransfer);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void sendBtData(ArrayList<keyFrame> keyFrameList) {
        for(int i = 0; i<keyFrameList.size(); i++){
            byte[] dataToTransfer = new byte[9];
            //notify board that the i-th keyFrame is coming
            sendBtOrderNr(i);

            dataToTransfer[0] = (byte) 0x6B;            // send char 'k'

            dataToTransfer[1] = (byte) ((keyFrameList.get(i).mDistance >> 8) & 0xFF);//higher byte
            dataToTransfer[2] = (byte) (keyFrameList.get(i).mDistance & 0xFF);   //lower byte
            dataToTransfer[3] = (byte) ((keyFrameList.get(i).mHorDeg>> 8) & 0xFF);//higher byte
            dataToTransfer[4] = (byte) (keyFrameList.get(i).mHorDeg & 0xFF);   //lower byte
            dataToTransfer[5] = (byte) ((keyFrameList.get(i).mVerDeg >> 8) & 0xFF);//higher byte
            dataToTransfer[6] = (byte) (keyFrameList.get(i).mVerDeg & 0xFF);   //lower byte
            dataToTransfer[7] = (byte) ((keyFrameList.get(i).mTime>> 8) & 0xFF);//higher byte
            dataToTransfer[8] = (byte) (keyFrameList.get(i).mTime & 0xFF);   //lower byte

            try {
                conBT.write(dataToTransfer);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
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
                    Log.i(TAG, "JOUKIIASDASD: " + bytes);
                    // Send the obtained bytes to the UI activity
                    //mHandler.obtainMessage(MESSAGE_READ, bytes, -1, buffer)
                    //      .sendToTarget();
                } catch (IOException e) {
                    break;
                }
            }
        }

        /* Call this from the main activity to send data to the remote device */
        public void write(byte[] bytes) throws IOException {
            try {
                mmOutStream.write(bytes);
            } catch (IOException e) {

                Log.i(TAG, "I just cant send stuff!");
                Log.i(TAG, e.getMessage());
                cancel();
                onBackPressed();
            }
        }

        /* Call this from the main activity to shutdown the connection */
        public void cancel() {
            try {
                mmSocket.close();
            } catch (IOException e) { }
        }
    }


    @Override
    public void onBackPressed() {
        super.onBackPressed();
        try{
            conBT.cancel();
        }catch (Exception e){
            Log.i(TAG, "conBT error!");
        }
    }
}
