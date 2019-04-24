package medford.root.clock;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothSocket;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.SeekBar;
import medford.root.clock.bluetooth.BluetoothThread;
import medford.root.clock.color.Gradient;
import medford.root.clock.color.Solid;

public class MainActivity extends AppCompatActivity {
    SeekBar red, green, blue, startRed, startGreen, startBlue, endRed, endGreen, endBlue;
    EditText mode;
    public BluetoothThread bluetooth;
    BluetoothSocket socket;
    Utils utils;
    Solid s;
    Gradient gradient;
    String BT_MAC_ADDRESS =  "00:06:66:7D:80:1A";
    int r, g, b, sr, sg, sb, er, eg, eb;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mode =             findViewById(R.id.mode);
        red =               findViewById(R.id.red);
        green =           findViewById(R.id.green);
        blue =             findViewById(R.id.blue);
        startRed =     findViewById(R.id.startRed);
        startGreen = findViewById(R.id.startGreen);
        startBlue =   findViewById(R.id.startBlue);
        endRed =         findViewById(R.id.endRed);
        endGreen =     findViewById(R.id.endGreen);
        endBlue =       findViewById(R.id.endBlue);
        utils =            new Utils(this);

        BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        socket = BluetoothThread.connect(bluetoothAdapter.getRemoteDevice(BT_MAC_ADDRESS));
        /*for(int i = 0; i < 5; i++){
            if(!BluetoothThread.isConnected){
                BluetoothThread.disconnect();
                socket = BluetoothThread.connect(bluetoothAdapter.getRemoteDevice(BT_MAC_ADDRESS));
            } else {
                break;
            }
        }*/

        bluetooth = new BluetoothThread(socket, utils);
        bluetooth.start();
        s = new Solid(bluetooth);
        gradient = new Gradient(bluetooth);

        red.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                r = progress;
            }
            public void onStartTrackingTouch(SeekBar seekBar) {}
            public void onStopTrackingTouch(SeekBar seekBar) {
                s.sendRed(r);
            }
        });

        green.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                g = progress;
            }
            public void onStartTrackingTouch(SeekBar seekBar) {}
            public void onStopTrackingTouch(SeekBar seekBar) {
                s.sendGreen(g);
            }
        });

        blue.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                b = progress;
            }
            public void onStartTrackingTouch(SeekBar seekBar) {}
            public void onStopTrackingTouch(SeekBar seekBar) {
                s.sendBlue(b);
            }
        });

        startRed.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                sr = progress;
            }
            public void onStartTrackingTouch(SeekBar seekBar) {}
            public void onStopTrackingTouch(SeekBar seekBar) {
                gradient.setStartRed(sr).sendStart();
            }
        });

        startGreen.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                sg = progress;
            }
            public void onStartTrackingTouch(SeekBar seekBar) {}
            public void onStopTrackingTouch(SeekBar seekBar) {
                gradient.setStartGreen(sg).sendStart();
            }
        });

        startBlue.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                sb = progress;
            }
            public void onStartTrackingTouch(SeekBar seekBar) {}
            public void onStopTrackingTouch(SeekBar seekBar) {
                gradient.setStartBlue(sb).sendStart();
            }
        });

        endRed.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                er = progress;
        }
            public void onStartTrackingTouch(SeekBar seekBar) {}
            public void onStopTrackingTouch(SeekBar seekBar) {
                gradient.setEndRed(er).sendEnd();
            }
        });

        endGreen.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                eg = progress;
            }
            public void onStartTrackingTouch(SeekBar seekBar) {}
            public void onStopTrackingTouch(SeekBar seekBar) {
                gradient.setEndGreen(eg).sendEnd();
            }
        });

        endBlue.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                eb = progress;
            }
            public void onStartTrackingTouch(SeekBar seekBar) {}
            public void onStopTrackingTouch(SeekBar seekBar) {
                gradient.setEndBlue(eb).sendEnd();
            }
        });
    }

    public void setMode(View v){
        String modeType = utils.getValue(mode);
        bluetooth.write("m:" + modeType + "/");
    }

    public void setSolidColors(View v){
        DialogUtils d = new DialogUtils(this, s);
        LayoutInflater inflater = (LayoutInflater) getSystemService(LAYOUT_INFLATER_SERVICE);
        View solid = inflater.inflate(R.layout.solid, (ViewGroup) findViewById(R.id.root_layout));
        d.dialog(solid);
    }
}