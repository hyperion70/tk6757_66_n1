package com.fineos.superscreenshot.service;

import com.fineos.superscreenshot.R;
import com.fineos.superscreenshot.SuperGlobalScreenshot;
import com.fineos.superscreenshot.SuperScreenShotApplication;
import com.fineos.superscreenshot.SuperScreenShotConstant;
import com.fineos.superscreenshot.floatwindow.StandOutWindow;
import com.fineos.superscreenshot.longss.LongScreenShotView;
import com.fineos.superscreenshot.funscreenshot.FunScreenShotView;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.IBinder;
import android.telephony.PhoneStateListener;
import android.telephony.TelephonyManager;
import android.util.Log;

public class SuperScreenShotService extends Service {
	private static final String TAG = "SuperScreenShotService";
	private TelephonyManager mTelephonyManager;	
	@Override
	public IBinder onBind(Intent intent) {
		// TODO Auto-generated method stub
		return null;
	}
	
	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		// TODO Auto-generated method stub
        mTelephonyManager =
                (TelephonyManager) getSystemService(Context.TELEPHONY_SERVICE);
        mTelephonyManager.listen(
                mPhoneStateListener, PhoneStateListener.LISTEN_CALL_STATE);
		if (intent != null) {
			String action = intent.getAction();
			boolean status = intent.getBooleanExtra(
					SuperScreenShotConstant.Service.SUPER_SCREEN_SHOT_VISIBILITY,
					false);
			Log.d(TAG, "admas:onStartCommand:action = " + action + ",status = " + status);
			if (SuperScreenShotConstant.Intent.LONG_SCREEN_SHOT_ACTION.equals(action)) {
				makeLongScreenShotViewWindow(status);
			} else if (SuperScreenShotConstant.Intent.HOME_SCREEN_SHOT_ACTION.equals(action)){
				makeHomeScreenShotViewWindow(status);
			}else if (SuperScreenShotConstant.Intent.FUNS_SCREEN_SHOT_ACTION.equals(action)){
				makeFunsScreenShotViewWindow(status);
			}else if (SuperScreenShotConstant.Intent.NORMAL_SCREEN_SHOT_ACTION.equals(action)){
				makeNormalScreenShotViewWindow(status);
			}
		}
		return super.onStartCommand(intent, flags, startId);
	}
	
    private final PhoneStateListener mPhoneStateListener = new PhoneStateListener() {
        @Override
        public void onCallStateChanged(int state, String ignored) {
            // Stop the alert sound and speech if the call state changes.
            if (state != TelephonyManager.CALL_STATE_IDLE) {
            	makeHomeScreenShotViewWindow(false);
                stopSelf();
            }
        }
    };
	
	private void makeLongScreenShotViewWindow(boolean isShow) {
		if (isShow) {
	        StandOutWindow.show(this, LongScreenShotView.class,
	                StandOutWindow.DEFAULT_ID);
	        showNotification();
		} else {
			StandOutWindow.closeAll(this, LongScreenShotView.class);
			dismissNotification();
		}
	}	
	
	//add by liukai3 for home
	private void makeHomeScreenShotViewWindow(boolean isShow) {
		android.util.Log.e("liukai3","service makeHomeScreenShotViewWindow----");
		SuperScreenShotApplication application = (SuperScreenShotApplication) getApplicationContext();
		android.util.Log.e("liukai3","service application is----"+application);
		//SuperScreenShotApplication application = new SuperScreenShotApplication();
		if (isShow) {
			android.util.Log.e("liukai3","service addView----");
			application.removeView();
			application.addView();
		} else {
			android.util.Log.e("liukai3","service removeView----");
			application.removeView();
		}
	}

	private void makeFunsScreenShotViewWindow(boolean isShow) {
		if (isShow) {
	        StandOutWindow.show(this, FunScreenShotView.class,
	                StandOutWindow.DEFAULT_ID);
		} else {
			StandOutWindow.closeAll(this, FunScreenShotView.class);
		}
	}
	
	private void makeNormalScreenShotViewWindow(boolean isShow) {
		if (isShow) {
			final SuperGlobalScreenshot screenshot = new SuperGlobalScreenshot(this);
			screenshot.takeScreenshot( new Runnable() {  
                @Override  
                public void run() {    
                }  
            }, true, true);
		} else {
			//do nothing	
		}
	}
	
	private void showNotification(){
		NotificationManager mNotificationManager = (NotificationManager)getSystemService(Context.NOTIFICATION_SERVICE); 
		//Notification notification = new Notification(R.drawable.star_big_off,"",System.currentTimeMillis()); 
		
		Notification.Builder b = new Notification.Builder(getApplicationContext())
         .setSmallIcon(R.drawable.super_screenshot_icon_pressed)
         .setWhen(System.currentTimeMillis())
          .setAutoCancel(true);
     Notification n =
        new Notification.BigTextStyle(b)
              .build();
		
		mNotificationManager.notify(0,n);
	}
	
	@Override
	public void onDestroy() {
		// TODO Auto-generated method stub
		mTelephonyManager.listen(mPhoneStateListener, 0);
		super.onDestroy();
	}
	
	private void dismissNotification(){
		NotificationManager mNotificationManager = (NotificationManager)getSystemService(Context.NOTIFICATION_SERVICE); 
		mNotificationManager.cancel(0);
	}

}
