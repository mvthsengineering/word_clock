package medford.root.clock;

import android.content.Context;
import android.widget.EditText;
import android.widget.SeekBar;
import android.widget.Toast;

public class Utils {
     private Context context;

        Utils(Context context){
            this.context = context;
        }

        public void makeToast(String text){
            Toast.makeText(context, text, Toast.LENGTH_SHORT).show();
        }

        String getValue(EditText et){
            return et.getText().toString();
        }
}
