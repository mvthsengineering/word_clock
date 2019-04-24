package medford.root.clock.color;

import medford.root.clock.bluetooth.BluetoothThread;

public class Solid {
    private BluetoothThread bluetooth;

    public Solid(BluetoothThread bluetooth) {
        this.bluetooth = bluetooth;
    }

    public void sendRed(int red) {
        bluetooth.write("r:" + red + "/");
    }

    public void sendGreen(int green) {
        bluetooth.write("g:" + green + "/");
    }

    public void sendBlue(int blue) {
        bluetooth.write("b:" + blue + "/");
    }
}
