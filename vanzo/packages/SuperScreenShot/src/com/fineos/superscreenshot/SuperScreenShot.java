package com.fineos.superscreenshot;

import com.fineos.superscreenshot.floatwindow.StandOutWindow;
import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;

/**
 * @ClassName:HomeSuperScreenShot
 * @Description:SuperScreenShot主入口
 * @author:liukai
 * @version 1.0
 * @date:2014-1-27
 */

public class SuperScreenShot extends Activity {   
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		//setContentView(R.layout.home_super_screen_shot);
		//StandOutWindow.closeAll(HomeSuperScreenShot.this, HomeScreenShotView.class);
	    //StandOutWindow.show(HomeSuperScreenShot.this, HomeScreenShotView.class,StandOutWindow.DEFAULT_ID);
		 Intent intent = new Intent();
         intent.setAction(SuperScreenShotConstant.BroadCast.HOME_SCREEN_SHOT_SHOW);
         SuperScreenShot.this.sendBroadcast(intent);
	     finish();	
	}	
	
	@Override
    protected void onStop() {
        super.onStop();
        this.onDestroy();
    }
}
