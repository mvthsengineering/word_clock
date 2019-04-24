package medford.root.clock.color;

import java.util.Locale;

import medford.root.clock.bluetooth.BluetoothThread;

public class Gradient {
    private int start_red, start_green, start_blue, end_red, end_green, end_blue;
    private BluetoothThread bluetooth;

    public Gradient(BluetoothThread bluetooth){
        this.bluetooth = bluetooth;
    }
    public Gradient setStartRed(int start_red){
        this.start_red = start_red;
        return this;
    }

    public Gradient setStartGreen(int start_green){
        this.start_green = start_green;
        return this;
    }

    public Gradient setStartBlue(int start_blue){
        this.start_blue = start_blue;
        return this;
    }

    public Gradient setEndRed(int end_red){
        this.end_red = end_red;
        return this;
    }

    public Gradient setEndGreen(int end_green){
        this.end_green = end_green;
        return this;
    }

    public Gradient setEndBlue(int end_blue){
        this.end_blue = end_blue;
        return this;
    }

    public void sendStart(){
        bluetooth.write(String.format(Locale.US, "s:%1$d,%2$d,%3$d/", start_red, start_green, start_blue));

    }

    public void sendEnd(){
        bluetooth.write(String.format(Locale.US, "e:%1$d,%2$d,%3$d/", end_red, end_green, end_blue));
    }
}
