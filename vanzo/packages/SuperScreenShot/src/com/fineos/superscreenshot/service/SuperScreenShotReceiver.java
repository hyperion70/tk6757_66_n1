package com.fineos.superscreenshot.service;


import com.fineos.superscreenshot.SuperScreenShotConstant;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.app.StatusBarManager;

public class SuperScreenShotReceiver extends BroadcastReceiver {
	private static final String TAG = "SuperScreenShotReceiver";
	private TelephonyManager mTelephonyManager;
	private int mInitialCallState;
        private Context mContext;
         private StatusBarManager mStatusBarManager;
	@Override
	public void onReceive(Context context, Intent intent) {
		// TODO Auto-generated method stub
                mContext = context;
                mStatusBarManager = (StatusBarManager)mContext.getSystemService(Context.STATUS_BAR_SERVICE);
		String action = intent.getAction();
		Intent service = new Intent();
		service.setClass(context, SuperScreenShotService.class);
		mTelephonyManager = (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
		mInitialCallState = mTelephonyManager.getCallState();
		boolean inCall = mInitialCallState != TelephonyManager.CALL_STATE_IDLE;
		if(inCall)
			return;
		// 广播内容
		Log.d(TAG, "admas:SuperScreenShotReceiver");
		if (SuperScreenShotConstant.BroadCast.LONG_SCREEN_SHOT_SHOW.equals(action)) {
			// 窗口开启
			service.setAction(SuperScreenShotConstant.Intent.LONG_SCREEN_SHOT_ACTION);
			service.putExtra(SuperScreenShotConstant.Service.SUPER_SCREEN_SHOT_VISIBILITY,
					true);
			context.startService(service);
                        disableStatusbar();
		} else if (SuperScreenShotConstant.BroadCast.LONG_SCREEN_SHOT_DISMISS.equals(action) || action.equals ("android.intent.action.PHONE_STATE")) {
			// 窗口关闭
			Log.d(TAG, "admas:SuperScreenShotReceiver LONG_SCREEN_SHOT_DISMISS");
			service.setAction(SuperScreenShotConstant.Intent.LONG_SCREEN_SHOT_ACTION);
			service.putExtra(SuperScreenShotConstant.Service.SUPER_SCREEN_SHOT_VISIBILITY ,
					false);
			context.startService(service);
                        enableStatusbar();
		} else if (SuperScreenShotConstant.BroadCast.HOME_SCREEN_SHOT_SHOW.equals(action)) {
			// 主窗口开启
			Log.d(TAG, "admas:SuperScreenShotReceiver HOME_SCREEN_SHOT_SHOW");
			service.setAction(SuperScreenShotConstant.Intent.HOME_SCREEN_SHOT_ACTION);
			service.putExtra(SuperScreenShotConstant.Service.SUPER_SCREEN_SHOT_VISIBILITY,
					true);
			context.startService(service);
		} else if (SuperScreenShotConstant.BroadCast.HOME_SCREEN_SHOT_DISMISS.equals(action)) {
			// 主窗口关闭
			Log.d(TAG, "admas:SuperScreenShotReceiver HOME_SCREEN_SHOT_DISMISS");
			service.setAction(SuperScreenShotConstant.Intent.HOME_SCREEN_SHOT_ACTION);
			service.putExtra(SuperScreenShotConstant.Service.SUPER_SCREEN_SHOT_VISIBILITY ,
					false);
			context.startService(service);
		}else if (SuperScreenShotConstant.BroadCast.FUNS_SCREEN_SHOT_SHOW.equals(action)) {
			// 窗口关闭
			Log.d(TAG, "admas:SuperScreenShotReceiver LONG_SCREEN_SHOT_DISMISS");
			service.setAction(SuperScreenShotConstant.Intent.FUNS_SCREEN_SHOT_ACTION);
			service.putExtra(SuperScreenShotConstant.Service.SUPER_SCREEN_SHOT_VISIBILITY ,
					true);
			context.startService(service);
		}else if (SuperScreenShotConstant.BroadCast.FUNS_SCREEN_SHOT_DISMISS.equals(action)) {
			// 主窗口关闭
			Log.d(TAG, "admas:SuperScreenShotReceiver HOME_SCREEN_SHOT_DISMISS");
			service.setAction(SuperScreenShotConstant.Intent.FUNS_SCREEN_SHOT_ACTION);
			service.putExtra(SuperScreenShotConstant.Service.SUPER_SCREEN_SHOT_VISIBILITY ,
					false);
			context.startService(service);
		}else if (SuperScreenShotConstant.BroadCast.NORMAL_SCREEN_SHOT_SHOW.equals(action)) {
			// 快速截屏窗口关闭
			Log.d(TAG, "admas:SuperScreenShotReceiver NORMAL_SCREEN_SHOT_DISMISS");
			service.setAction(SuperScreenShotConstant.Intent.NORMAL_SCREEN_SHOT_ACTION);
			service.putExtra(SuperScreenShotConstant.Service.SUPER_SCREEN_SHOT_VISIBILITY ,
					true);
			context.startService(service);
		}else if (SuperScreenShotConstant.BroadCast.NORMAL_SCREEN_SHOT_DISMISS.equals(action)) {
			// 快速截屏主窗口关闭
			Log.d(TAG, "admas:SuperScreenShotReceiver NORMAL_SCREEN_SHOT_DISMISS");
			service.setAction(SuperScreenShotConstant.Intent.NORMAL_SCREEN_SHOT_ACTION);
			service.putExtra(SuperScreenShotConstant.Service.SUPER_SCREEN_SHOT_VISIBILITY ,
					false);
			context.startService(service);
		}						
	}

        private void disableStatusbar(){
        if (mStatusBarManager == null) {
            mStatusBarManager = (StatusBarManager)mContext.getSystemService(Context.STATUS_BAR_SERVICE);
        }
        int flags = StatusBarManager.DISABLE_NONE;
        flags |= StatusBarManager.DISABLE_RECENT;
        // showing secure lockscreen; disable expanding.
        flags |= StatusBarManager.DISABLE_EXPAND;
        // showing secure lockscreen; disable ticker.
        flags |= StatusBarManager.DISABLE_NOTIFICATION_TICKER;
        
        mStatusBarManager.setIconVisibility("clock", false);
        mStatusBarManager.disable(flags);
	}
	
	private void enableStatusbar(){
        if (mStatusBarManager == null) {
            mStatusBarManager = (StatusBarManager)mContext.getSystemService(Context.STATUS_BAR_SERVICE);
        }
        int flags = StatusBarManager.DISABLE_NONE;
        mStatusBarManager.disable(flags);
	}

}
