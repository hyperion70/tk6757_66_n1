package com.fineos.superscreenshot.funscreenshot;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.ActivityInfo;
import android.graphics.PixelFormat;
import android.graphics.drawable.ColorDrawable;
import android.os.IBinder;
import android.telephony.PhoneStateListener;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.view.ActionMode;
import android.view.ActionMode.Callback;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.view.Window;
import android.view.WindowManager.LayoutParams;
import android.view.accessibility.AccessibilityEvent;

import com.android.internal.telephony.ITelephony;
//import com.android.internal.policy.PolicyManager;
import com.android.internal.policy.PhoneWindow;

import android.os.ServiceManager;
import android.provider.Settings;
import android.graphics.Bitmap;
import android.view.View;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.TextView;
import android.view.SurfaceControl;
import android.os.Handler;
import com.fineos.superscreenshot.floatwindow.*;
import android.widget.TableLayout;
import com.fineos.superscreenshot.*;
import android.content.SharedPreferences;
import android.app.Activity;
import android.graphics.Bitmap.Config;
import android.graphics.Canvas;
import android.graphics.RectF;
import android.util.DisplayMetrics;
import android.content.res.Configuration;
import android.app.StatusBarManager;

import android.os.Bundle;
import android.os.Message;

import android.view.SearchEvent;

public class ClearService extends Service implements Window.Callback,TouchEventListener{
	private static final String TAG = "ClearService";
	private final String PREFERENCES_NAME = "clings_preference";
	private final String LARIAT_STATE = "lariat_state";
	private final String SCRAWL_STATE = "scrawl_state";
	private final String RECTANGLE_STATE = "rectangle_state";
	private Window mWindow;
    protected WindowManager mWindowManager;
    private TelephonyManager mTelephonyManager;
	private StatusBarManager mStatusBarManager;
    private ITelephony mTelephonyService;
    public boolean isClearViewshow = false;
	private Button mPreviewButton,mSaveButton;
	private boolean showClear = true;
	private ClearView myClear;
	private RectClearView mRectView;
	private Cling mCling;
	private ImageView mImage;
	private TextView exitText,recText,lariatText,scrawlText,reworkText,backupText,previewText,saveText;
	private TableLayout mTabLayout;
	private int mCurruntDrawType;
	private Bitmap mBackgroundMap;
	private Bitmap mComposeMap;
	int mScreenWidth;
	int mScreenHeight;
	Bitmap mMap1 = null;
	RectF mRect = null;
	boolean isHit = false;
	private Handler mHandler = new Handler();
    Runnable saveRun = new Runnable() {
		
		@Override
		public void run() {
					Log.e(TAG,"saveRun callback ");

		}
	};
    Runnable mBackgroundMapRun = new Runnable() {
		
		@Override
		public void run() {
				Log.e(TAG,"mBackgroundMap mScreenWidth: "+mScreenWidth+"  mScreenHeight:"+mScreenHeight);
				 mBackgroundMap = SurfaceControl.screenshot(mScreenWidth, mScreenHeight);
        		if (mBackgroundMap == null) {
					Log.e(TAG,"mBackgroundMap is null ");
					}else{
        			// Optimizations
					Log.e(TAG,"save mBackgroundMap success ");
        			mBackgroundMap.setHasAlpha(false);
        			mBackgroundMap.prepareToDraw();
					mComposeMap = composeBitmap(mMap1,mRect,isHit);
					if(mComposeMap != null){
						mHandler.postDelayed(run, 100);
					}
					//SuperScreenShotTools mSuperScreenShotTools = new SuperScreenShotTools();
					//mSuperScreenShotTools.saveImage(getApplicationContext(),mMap,0);
					//SuperGlobalScreenshot screenshot = new SuperGlobalScreenshot(getApplicationContext());
					//screenshot.showScreenshotAnim(mBackgroundMap,saveRun,true,true,false);
					}	
		}
	};
    Runnable run = new Runnable() {
		
		@Override
		public void run() {
			// TODO Auto-generated method stub
        		if (mComposeMap == null) {
					Log.e(TAG,"mComposeMap is null ");
					}else{
        			// Optimizations
					Log.e(TAG,"save map success ");
					//SuperScreenShotTools mSuperScreenShotTools = new SuperScreenShotTools();
					//mSuperScreenShotTools.saveImage(getApplicationContext(),mMap,0);
					SuperGlobalScreenshot screenshot = new SuperGlobalScreenshot(getApplicationContext());
					screenshot.showScreenshotAnim(mComposeMap,null,true,true,true);
					}	
					closeClearView();
					stopSelf();	

		}
	};	

	private View.OnClickListener mClickListener = new View.OnClickListener(){
		@Override
		public void onClick(View v) {
			// TODO Auto-generated method stub
			mTabLayout.setVisibility(View.GONE);
			int id = v.getId();
			switch(id){
				case R.id.tableLayout:
					Log.e(TAG,"mClickListener tableLayout ");
					break;
				case R.id.exit:
					Log.e(TAG,"mClickListener exit ");
            			closeClearView();
            			stopSelf();	
					break;
				case R.id.rectangle:
					mCurruntDrawType = Utils.TYPE_RECT;
					mRectView.setmDrawType(Utils.TYPE_RECT);
					//myClear.setmDrawType(Utils.TYPE_RECT);
					myClear.setVisibility(View.GONE);
					mRectView.setVisibility(View.VISIBLE);
					updateLayoutContent();
					Log.e(TAG,"mClickListener rectangle ");
					break;
				case R.id.lariat:
					SharedPreferences mLariatClingPreferences = getSharedPreferences(PREFERENCES_NAME, Activity.MODE_PRIVATE);
					boolean mLariatState = mLariatClingPreferences.getBoolean(LARIAT_STATE,false);
					if(mLariatState){
						mCling.setVisibility(View.GONE);
						myClear.setmDrawType(Utils.TYPE_LARIAT);
					}else{
						mCling.setType(Utils.TYPE_LARIAT);
						mCling.setVisibility(View.VISIBLE);
						mCling.setButtonClickListener(mClickListener);
						mCling.startAnimation();
					}
					mCurruntDrawType = Utils.TYPE_LARIAT;
					myClear.setVisibility(View.VISIBLE);
					mRectView.setVisibility(View.GONE);
					updateLayoutContent();
					Log.e(TAG,"mClickListener lariat ");
					break;
				case R.id.scrawl:
					SharedPreferences mScrawlClingPreferences = getSharedPreferences(PREFERENCES_NAME, Activity.MODE_PRIVATE);
					boolean mScrawlState = mScrawlClingPreferences.getBoolean(SCRAWL_STATE,false);
					if(mScrawlState){
						mCling.setVisibility(View.GONE);
						myClear.setmDrawType(Utils.TYPE_SCRAWL);
					}else{
						mCling.setType(Utils.TYPE_SCRAWL);
						mCling.setVisibility(View.VISIBLE);
						mCling.setButtonClickListener(mClickListener);
						mCling.startAnimation();
					}
					mCurruntDrawType = Utils.TYPE_SCRAWL;
					myClear.setVisibility(View.VISIBLE);
					mRectView.setVisibility(View.GONE);
					updateLayoutContent();
					Log.e(TAG,"mClickListener scrawl ");
					break;
				case R.id.rework:
					mImage.setVisibility(View.GONE);
					if(mCurruntDrawType == Utils.TYPE_RECT){
						mRectView.setVisibility(View.VISIBLE);
						mRectView.reset();
					}else{
						myClear.setVisibility(View.VISIBLE);
						myClear.reset();
					}
					previewText.setVisibility(View.GONE);
					saveText.setVisibility(View.VISIBLE);
					//mTabLayout.setVisibility(View.VISIBLE);
					Log.e(TAG,"mClickListener rework ");
					break;
				case R.id.backup:
					mImage.setVisibility(View.GONE);
					if(mCurruntDrawType == Utils.TYPE_RECT){
						mRectView.setVisibility(View.VISIBLE);
						mRectView.backup();
					}else{
						myClear.setVisibility(View.VISIBLE);
						myClear.backup();
					}
					previewText.setVisibility(View.GONE);
					saveText.setVisibility(View.VISIBLE);
					mTabLayout.setVisibility(View.VISIBLE);
					Log.e(TAG,"mClickListener backup ");
					break;
				case R.id.preview:
					Bitmap mMap = null;
					if(myClear.getVisibility() == View.VISIBLE){
						 mMap=myClear.getBitMap();
						}else{
						mMap=mRectView.getBitMap();
					}
					mImage.setVisibility(View.VISIBLE);
					mImage.setImageBitmap(mMap);
					mRectView.setVisibility(View.GONE);
					myClear.setVisibility(View.GONE);
					mTabLayout.setVisibility(View.VISIBLE);
					previewText.setVisibility(View.GONE);
					saveText.setVisibility(View.VISIBLE);
					Log.e(TAG,"mClickListener preview ");
					break;
				case R.id.save:
					if(myClear.getVisibility() == View.VISIBLE){
						 mMap1 = myClear.getBitMap();
						 mRect = myClear.getRect();
						 myClear.stopAnimation();
						isHit = true;
						}else{
						mMap1 = mRectView.getBitMap();
 						mRect = mRectView.getRect();
						isHit = false;
					}
					mTabLayout.setVisibility(View.GONE);
					myClear.setVisibility(View.GONE);
					mRectView.setVisibility(View.GONE);
					updateScreenValue();
					Log.e(TAG,"mClickListener save ");
					break;
				case R.id.button:
					Log.e(TAG,"mClickListener button ");
					myClear.setmDrawType(mCurruntDrawType);
					updateClingPreferences();
					mCling.setVisibility(View.GONE);				
					break;
				default :
					break;
			}
		}
};
	private void updateClingPreferences(){
		if(mCurruntDrawType < 0){
			return;			
			}
		String editorName = null;
		switch(mCurruntDrawType){
			case Utils.TYPE_RECT:
				editorName = RECTANGLE_STATE;
				break;
			case Utils.TYPE_SCRAWL:
				editorName = SCRAWL_STATE;
				break;
			case Utils.TYPE_LARIAT:
				editorName = LARIAT_STATE;
				break;
			default :
				break;
		}
		if(editorName != null){
					SharedPreferences unClingUpdatePreferences = getSharedPreferences(PREFERENCES_NAME, Activity.MODE_PRIVATE);
					SharedPreferences.Editor editor = unClingUpdatePreferences.edit();
					editor.putBoolean(editorName,true);
					editor.commit();
			}
	}	
    
    private PhoneStateListener mPhoneStateListener1 = new PhoneStateListener() {
        @Override
        public void onCallStateChanged(int state, String ignored) {
        Log.d(TAG,"mPhoneStateListener1 onCallStateChanged state:" +state);
            if ((state == TelephonyManager.CALL_STATE_RINGING)){
                closeClearView();
                stopSelf();
            }
        }
    };

	@Override
	public IBinder onBind(Intent arg0) {
		// TODO Auto-generated method stub
		return null;
	}
	

	public int onStartCommand(Intent intent,int flag,int startId){
        super.onStartCommand(intent, flag, startId);
        Bundle bundle = (Bundle)intent.getExtras();
        String uri=null;
        if(bundle!=null){
        uri=bundle.getString("extra");		
        }
		  if(uri != null && uri.equals("RECTANGULAR")){
			openRectangle();
			}else if(uri != null && uri.equals("LASSO")){
				Message msg = mHandler1.obtainMessage(2);
				mHandler1.sendMessageDelayed(msg, 80);
				
			}else if(uri != null && uri.equals("GRAFFITI")){

				Message msg = mHandler1.obtainMessage(1);
				mHandler1.sendMessageDelayed(msg, 80);
			}
		
        return startId;
    }
	

	public void openGraffiti(){
		SharedPreferences mScrawlClingPreferences = getSharedPreferences(PREFERENCES_NAME, Activity.MODE_PRIVATE);
		boolean mScrawlState = mScrawlClingPreferences.getBoolean(SCRAWL_STATE,false);
		if(mScrawlState){
			mCling.setVisibility(View.GONE);
			myClear.setmDrawType(Utils.TYPE_SCRAWL);
		}else{
			mCling.setType(Utils.TYPE_SCRAWL);
			mCling.setVisibility(View.VISIBLE);
			mCling.setButtonClickListener(mClickListener);
			mCling.startAnimation();
		}
		mCurruntDrawType = Utils.TYPE_SCRAWL;
		myClear.setVisibility(View.VISIBLE);
		mRectView.setVisibility(View.GONE);
		updateLayoutContent();
		Log.e(TAG,"mClickListener scrawl ");
	}

	public void openLasso(){
		
		SharedPreferences mLariatClingPreferences = getSharedPreferences(PREFERENCES_NAME, Activity.MODE_PRIVATE);
		boolean mLariatState = mLariatClingPreferences.getBoolean(LARIAT_STATE,false);
		
		if(mLariatState){
			mCling.setVisibility(View.GONE);
			myClear.setmDrawType(Utils.TYPE_LARIAT);
		}else{
			mCling.setType(Utils.TYPE_LARIAT);
			mCling.setVisibility(View.VISIBLE);
			mCling.setButtonClickListener(mClickListener);
			mCling.startAnimation();
		}
		mCurruntDrawType = Utils.TYPE_LARIAT;
		myClear.setVisibility(View.VISIBLE);
		mRectView.setVisibility(View.GONE);
		updateLayoutContent();
		Log.e(TAG,"mClickListener lariat ");
	}
	public void openRectangle(){
					mCurruntDrawType = Utils.TYPE_RECT;
					mRectView.setmDrawType(Utils.TYPE_RECT);
					//myClear.setmDrawType(Utils.TYPE_RECT);
					myClear.setVisibility(View.GONE);
					mRectView.setVisibility(View.VISIBLE);
					updateLayoutContent();
		}
	
	
    @Override
	public void onCreate() {
		// TODO Auto-generated method stub
		super.onCreate();
		//mHandler.postDelayed(mBackgroundMapRun, 0);
		Log.e(TAG,"ClearService Create");
        mWindow = new PhoneWindow(this);//PolicyManager.makeNewWindow(this);
        mWindow.setCallback(this);
        mWindow.requestFeature(Window.FEATURE_NO_TITLE);
        mWindow.setBackgroundDrawable(new ColorDrawable(0x00000000));
        //mWindow.setFormat(PixelFormat.TRANSPARENT);
        Log.e(TAG, "Created and attached window: " + this);

        WindowManager.LayoutParams lp = mWindow.getAttributes();
        lp.type = WindowManager.LayoutParams.TYPE_PHONE;

        lp.flags |= (WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN
                | WindowManager.LayoutParams.FLAG_LAYOUT_INSET_DECOR
                | WindowManager.LayoutParams.FLAG_SHOW_WHEN_LOCKED
                | WindowManager.LayoutParams.FLAG_FULLSCREEN
                | WindowManager.LayoutParams.FLAG_ALT_FOCUSABLE_IM  //Add by huliang to focusable ime (general)SW00083763 2014-10-08
                | WindowManager.LayoutParams.FLAG_ALLOW_LOCK_WHILE_SCREEN_ON);
        lp.screenOrientation = ActivityInfo.SCREEN_ORIENTATION_SENSOR_PORTRAIT;
        mWindow.setAttributes(lp);
        Log.v(TAG, "Created and attached window: " + mWindow);
        mWindow.setWindowManager(null, null, "HallIdle");
        mWindowManager = mWindow.getWindowManager();
        Log.e(TAG, "HallViewMgrServiceLed onCreate ");
        layoutClockSaver();

		Intent intent1 = new Intent();
		//String StringE = intent1.getStringExtra("extra");
		
		
        // Listen for incoming calls to kill the alarm.
        mTelephonyManager = (TelephonyManager) getSystemService(Context.TELEPHONY_SERVICE);
        mTelephonyService = ITelephony.Stub.asInterface(ServiceManager.getService(Context.TELEPHONY_SERVICE));

        // Check if the device is gemini supported
        isClearViewshow = false;
        mTelephonyManager.listen(mPhoneStateListener1,
        PhoneStateListener.LISTEN_CALL_STATE);
        showClearView();

		/*if(uri != null && uri.equals("RECTANGULAR")){
			openRectangle();
		}*/
		//updateScreenValue();
	}

	@Override
	public void onDestroy() {
		// Stop listening for incoming calls.
		Log.e(TAG,"ClearService Destroy");
  		    mTelephonyManager.listen(mPhoneStateListener1, 0);                    
		// Just in case destroy came in before detach, let's take care of that now
		closeClearView();
		super.onDestroy();
	}
    
    @Override
	public void onConfigurationChanged(Configuration newConfig) {
		// TODO Auto-generated method stub
		android.util.Log.e("lixiaosong","onConfigurationChanged Service");
		//updateScreenValue();
		super.onConfigurationChanged(newConfig);
	}

	private void updateScreenValue(){
		DisplayMetrics metric = new DisplayMetrics();
		mWindowManager.getDefaultDisplay().getMetrics(metric);
		mScreenWidth = metric.widthPixels;
		mScreenHeight = metric.heightPixels;
		mHandler.postDelayed(mBackgroundMapRun, 100);
		android.util.Log.e("lixiaosong","updateScreenValue mScreenWidth:"+mScreenWidth+" mScreenHeight:"+mScreenHeight);
	}
	private void layoutClockSaver() {
		// TODO Auto-generated method stub
        setContentView(R.layout.another_layout);
        initViews();
	}






		private Handler mHandler1 = new Handler(){
	    @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case 1: {
						openGraffiti();
						break;
                	}
				case 2: {
						openLasso();
						break;
                	}
            	}
	    	}
		};
		
	private void initViews() {
		// TODO Auto-generated method stub
		mTabLayout = (TableLayout) findViewById(R.id.tableLayout);
		myClear =  (ClearView) findViewById(R.id.myclear);
		mCling = (Cling) findViewById(R.id.mycling);
		mRectView = (RectClearView) findViewById(R.id.myrectclear);
		myClear.setTouchEventListener(this);
		mRectView.setTouchEventListener(this);
		exitText = (TextView) findViewById(R.id.exit);
		recText = (TextView) findViewById(R.id.rectangle);
		lariatText = (TextView) findViewById(R.id.lariat);
		scrawlText = (TextView) findViewById(R.id.scrawl);
		reworkText = (TextView) findViewById(R.id.rework);
		backupText = (TextView) findViewById(R.id.backup);
		previewText = (TextView) findViewById(R.id.preview);
		saveText = (TextView) findViewById(R.id.save);
		exitText.setOnClickListener(mClickListener);
		recText.setOnClickListener(mClickListener);
		lariatText.setOnClickListener(mClickListener);
		scrawlText.setOnClickListener(mClickListener);
		reworkText.setOnClickListener(mClickListener);
		backupText.setOnClickListener(mClickListener);
		previewText.setOnClickListener(mClickListener);
		saveText.setOnClickListener(mClickListener);
		mImage = (ImageView) findViewById(R.id.myimage);
		

	}







	private void setContentView(int layoutResID) {
		// TODO Auto-generated method stub
		getWindow().setContentView(layoutResID);
	}
	
    public Window getWindow() {
        return mWindow;
    }
    
    public WindowManager getWindowManager() {
        return mWindowManager;
    }

	public View findViewById(int id) {
		return getWindow().findViewById(id);
	}





	private final void showClearView() {
        Log.v(TAG, "Attached on thread " + Thread.currentThread().getId()+"  isClearViewshow:"+isClearViewshow);
		if(isClearViewshow){
			return;
		}
        if (mWindow != null) {
            closeClearView();
        }
		disableStatusbar();
        try {
            Log.w(TAG, "adding window view"+mWindow.getDecorView());
            getWindowManager().addView(mWindow.getDecorView(), mWindow.getAttributes());
        } catch (Throwable t) {
            Log.w(TAG, "Crashed adding window view", t);
            isClearViewshow = false;
            return;
        }
        isClearViewshow = true;

    }

    private final void closeClearView() {
        isClearViewshow = false;
        if (mWindow == null) {
            // already detached!
            return;
        }
        
		enableStatusbar();
        Log.v(TAG, "closeClearView(): Removing window from window manager");
        try {
            // force our window to be removed synchronously
            mWindowManager.removeViewImmediate(mWindow.getDecorView());
        } catch (Exception t) {
            Log.w(TAG, "Crashed removing window view. Excption:" + t);
            isClearViewshow = false;
        }
    }

	private void updateLayoutContent(){
		recText.setVisibility(View.GONE);
		lariatText.setVisibility(View.GONE);
		scrawlText.setVisibility(View.GONE);
		reworkText.setVisibility(View.VISIBLE);
		if(mCurruntDrawType == Utils.TYPE_SCRAWL){
			backupText.setVisibility(View.VISIBLE);
		}
		previewText.setVisibility(View.GONE);
		saveText.setVisibility(View.VISIBLE);
	}

	private Bitmap composeBitmap(Bitmap foreground,RectF mRect,boolean useHit){
		if(mBackgroundMap == null){
			return null;
		}
    	int bgWidth = mBackgroundMap.getWidth();  
    	int bgHeight = mBackgroundMap.getHeight();  
    	int fgWidth = foreground.getWidth();  
    	int fgHeight = foreground.getHeight(); 
		Bitmap newmap = Bitmap.createBitmap(bgWidth, bgHeight, Config.ARGB_8888); 
		Canvas canvas = new Canvas(newmap);
		canvas.drawBitmap(mBackgroundMap, 0, 0, null);  
		canvas.drawBitmap(foreground,(bgWidth - fgWidth)/2,(bgHeight - fgHeight)/2, null);
		canvas.save(Canvas.ALL_SAVE_FLAG); 
    	canvas.restore();
		if(mRect !=null && true){
	android.util.Log.d(TAG,"composeBitmap  before rect left-->"+mRect.left+" top-->"+mRect.top+" right-->"+mRect.right+"   bottom-->"+mRect.bottom);
			adjustRect(mRect);
	android.util.Log.d(TAG,"composeBitmap after rect left-->"+mRect.left+" top-->"+mRect.top+" right-->"+mRect.right+"   bottom-->"+mRect.bottom);
	android.util.Log.d(TAG,"composeBitmap bgWidth="+bgWidth+" bgHeight="+bgHeight+" source bitmap:"+newmap.getWidth()+"x"+newmap.getHeight());
			if(useHit){  //modify by lixiaosong for HQ00986197,HQ01693612
                   return Bitmap.createBitmap(newmap, ((int)mRect.left-20)>0 ?((int)mRect.left-20): 0,
                           ((int)mRect.top-20) > 0?((int)mRect.top-20) : 0,
                           ((int)mRect.right+40)>bgWidth ? ((int)mRect.width()):((int)mRect.width()+40),
                           ((int)mRect.bottom+40)>bgHeight ? (int)mRect.height():((int)mRect.height()+40));
			}else{
                   return Bitmap.createBitmap(newmap, (int)mRect.left, (int)mRect.top, (int)mRect.width(),(int)mRect.height());
			}
		}else{
    android.util.Log.e("lixiaosong","mRect is null ");
		}
    	return newmap;   
	}

	//modify for HQ01522161
	private void adjustRect(RectF rect){
		rect.set(rect.left < 0.0f ? 0.0f : rect.left,rect.top < 0.0f ? 0.0f : rect.top,rect.right > (float)mScreenWidth ? (float)mScreenWidth : rect.right,rect.bottom > (float)mScreenHeight ? (float)mScreenHeight : rect.bottom);
	}

	public void disableStatusbar(){
        if (mStatusBarManager == null) {
            mStatusBarManager = (StatusBarManager)getSystemService(Context.STATUS_BAR_SERVICE);
        }
        int flags = StatusBarManager.DISABLE_NONE;
        flags |= StatusBarManager.DISABLE_RECENT;
        // showing secure lockscreen; disable expanding.
        flags |= StatusBarManager.DISABLE_EXPAND;
        // showing secure lockscreen; disable ticker.
        flags |= StatusBarManager.DISABLE_NOTIFICATION_TICKER;
        
        mStatusBarManager.disable(flags);
	}

	public void enableStatusbar(){
        if (mStatusBarManager == null) {
            mStatusBarManager = (StatusBarManager)getSystemService(Context.STATUS_BAR_SERVICE);
        }
        int flags = StatusBarManager.DISABLE_NONE;
        mStatusBarManager.disable(flags);
	}



	@Override
	public boolean dispatchGenericMotionEvent(MotionEvent event) {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean dispatchKeyEvent(KeyEvent event) {
		// TODO Auto-generated method stub
		int action = event.getAction();
		boolean down = action == KeyEvent.ACTION_DOWN;
		if(event.getKeyCode() == KeyEvent.KEYCODE_BACK && down){
			closeClearView();
			stopSelf();	
			return true;			
		}
		return false;
	}

	@Override
	public boolean dispatchKeyShortcutEvent(KeyEvent event) {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean dispatchPopulateAccessibilityEvent(AccessibilityEvent event) {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean dispatchTouchEvent(MotionEvent event) {
		// TODO Auto-generated method stub
		if (getWindow().superDispatchTouchEvent(event)) {
            return true;
        }
		return false;
	}

	@Override
	public boolean dispatchTrackballEvent(MotionEvent event) {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public void onActionModeFinished(ActionMode mode) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onActionModeStarted(ActionMode mode) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onAttachedToWindow() {
android.util.Log.e("lixiaosong","onAttachedToWindow");
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onContentChanged() {
android.util.Log.e("lixiaosong","onContentChanged");
		// TODO Auto-generated method stub
		
	}

	@Override
	public boolean onCreatePanelMenu(int featureId, Menu menu) {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public View onCreatePanelView(int featureId) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void onDetachedFromWindow() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public boolean onMenuItemSelected(int featureId, MenuItem item) {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean onMenuOpened(int featureId, Menu menu) {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public void onPanelClosed(int featureId, Menu menu) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public boolean onPreparePanel(int featureId, View view, Menu menu) {
		// TODO Auto-generated method stub
		return false;
	}



	@Override
	public void onWindowAttributesChanged(LayoutParams attrs) {
		// TODO Auto-generated method stub
android.util.Log.e("lixiaosong","onWindowAttributesChanged");
		
	}

	@Override
	public void onWindowFocusChanged(boolean hasFocus) {
		// TODO Auto-generated method stub
android.util.Log.e("lixiaosong","onWindowFocusChanged");
		
	}

	

	@Override
	public void updateTabLayout(boolean visible) {
		// TODO Auto-generated method stub
		if(visible){
			mTabLayout.setVisibility(View.VISIBLE);
		}else{
			mTabLayout.setVisibility(View.GONE);
		}
	}

	@Override
	public int getTabLayoutVisible() {
		// TODO Auto-generated method stub
		return mTabLayout.getVisibility();
	}

	 @Override
    public ActionMode onWindowStartingActionMode(ActionMode.Callback callback) {
        return null;
    }
    
    @Override
    public ActionMode onWindowStartingActionMode(ActionMode.Callback callback, int type) {
        return null;
    }

 	@Override
    public boolean onSearchRequested(SearchEvent event) {
        return onSearchRequested();
    }
    
    @Override
    public boolean onSearchRequested() {
        return false;
    }

}
