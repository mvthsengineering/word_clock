package medford.root.clock.bluetooth;

import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.os.Handler;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.Method;
import java.util.UUID;

import medford.root.clock.Utils;

public class BluetoothThread extends Thread {
    private final InputStream mmInStream;
    private final OutputStream mmOutStream;
    private Handler handler;
    private static final UUID MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    private static BluetoothSocket socket;
    BluetoothDevice device;
    public static boolean isConnected;
    static Utils utils;

    public BluetoothThread(BluetoothSocket socket, Utils utils) {
        this.utils = utils;
        InputStream tmpIn = null;
        OutputStream tmpOut = null;
        try {
            tmpIn = socket.getInputStream();
            tmpOut = socket.getOutputStream();
        } catch (IOException e) {
            e.printStackTrace();
        }
        this.handler = handler;
        mmInStream = tmpIn;
        mmOutStream = tmpOut;
    }

    public void run() {
        byte[] buffer = new byte[256];
        int bytes;
        while (true) {
            /*try {
                bytes = mmInStream.read(buffer);
                handler.obtainMessage(1, bytes, -1, buffer).sendToTarget();
            } catch (IOException e) {
                break;
            }*/
        }
    }

    public void write(String message) {
        byte[] msgBuffer = message.getBytes();
        try {
            mmOutStream.write(msgBuffer);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static Handler handle(final String delimiter, final BluetoothClass bluetoothClass) {
        final StringBuilder sb = new StringBuilder();
        Handler h = new Handler() {
            public void handleMessage(android.os.Message msg) {
                switch (msg.what) {
                    case 1:
                        byte[] readBuf = (byte[]) msg.obj;
                        String strIncom = new String(readBuf, 0, msg.arg1);
                        sb.append(strIncom);
                        int endOfLineIndex = sb.indexOf(delimiter);
                        if (endOfLineIndex > 0) {
                            String sbprint = sb.substring(0, endOfLineIndex);
                            sb.delete(0, sb.length());
                            bluetoothClass.onReceived(sbprint);

                        }
                        break;
                }
            }
        };
        return h;
    }

    interface BluetoothClass {
        void onReceived(String data);
    }

    private static BluetoothSocket createSocket(BluetoothDevice device) throws IOException {
        try {
            final Method m = device.getClass().getMethod("createInsecureRfcommSocketToServiceRecord", UUID.class);
            return (BluetoothSocket) m.invoke(device, MY_UUID);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return device.createRfcommSocketToServiceRecord(MY_UUID);
    }

    public static BluetoothSocket connect(BluetoothDevice device){
        try {
            socket = createSocket(device);
        } catch (IOException e) {
            e.printStackTrace();
        }
        try {
            socket.connect();
            //utils.makeToast("Connected to device" + device.getName());
            isConnected = true;
        } catch (IOException e) {
            e.printStackTrace();
            isConnected = false;
        }
        return socket;
    }

    public static void disconnect(){
        try {
            socket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
