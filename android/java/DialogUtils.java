package medford.root.clock;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.SeekBar;

import medford.root.clock.color.Solid;

import static android.content.Context.LAYOUT_INFLATER_SERVICE;

public class DialogUtils {
    private Context ctx;
    int red;
    int green;
    int blue;
    Solid s;

    public DialogUtils(Context ctx, Solid s){
        this.ctx = ctx;
        this.s = s;
    }

    public void dialog(View v) {
        Dialog yourDialog = new Dialog(ctx);
        yourDialog.setContentView(v);
        SeekBar redSeekbar = v.findViewById(R.id.red);
        SeekBar greenSeekbar = v.findViewById(R.id.green);
        SeekBar blueSeekbar = v.findViewById(R.id.blue);
        redSeekbar.setOnSeekBarChangeListener(redSeekbarListener);
        greenSeekbar.setOnSeekBarChangeListener(greenSeekbarListener);
        blueSeekbar.setOnSeekBarChangeListener(blueSeekbarListener);
    }

    SeekBar.OnSeekBarChangeListener redSeekbarListener = new SeekBar.OnSeekBarChangeListener() {
        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {
            s.sendRed(red);
        }
        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {
        }
        @Override
        public void onProgressChanged(SeekBar seekBark, int progress, boolean fromUser) {
            red = progress;
        }
    };

    SeekBar.OnSeekBarChangeListener greenSeekbarListener = new SeekBar.OnSeekBarChangeListener() {
        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {
            s.sendGreen(green);
        }
        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {
        }
        @Override
        public void onProgressChanged(SeekBar seekBark, int progress, boolean fromUser) {
            green = progress;
        }
    };

    SeekBar.OnSeekBarChangeListener blueSeekbarListener = new SeekBar.OnSeekBarChangeListener() {
        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {
            s.sendBlue(blue);
        }
        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {
        }
        @Override
        public void onProgressChanged(SeekBar seekBark, int progress, boolean fromUser) {
            blue = progress;
        }
    };
}
