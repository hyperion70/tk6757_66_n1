package com.fineos.superscreenshot.longss;

import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.Arrays;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.Canvas;
import android.graphics.PixelFormat;
import android.graphics.Rect;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.view.ViewGroup.LayoutParams;
import android.view.WindowManager;
import android.webkit.WebView.FindListener;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import android.os.Handler;
import android.os.Message;
import android.R.integer;
import android.app.StatusBarManager;
/*add by huangjie for screenshot begin*/
import android.provider.Settings;
/*add by huangjie for screenshot end*/

import com.fineos.superscreenshot.floatwindow.StandOutWindow;
import com.fineos.superscreenshot.floatwindow.Window;
import com.fineos.superscreenshot.R;
import com.fineos.superscreenshot.SuperGlobalScreenshot;
import com.fineos.superscreenshot.SuperScreenShotApplication;
import com.fineos.superscreenshot.SuperScreenShotConstant;
import com.fineos.superscreenshot.SuperScreenShotTools;
import android.view.SurfaceControl;

public class LongScreenShotView extends StandOutWindow implements View.OnTouchListener{
	final static private String TAG = "LongScreenShotView";
    private StandOutLayoutParams mStandOutLayoutParams;
    private int mScreenWidth;
	private int mScreenHeight;
	private int mId;
	private TextView backButton;
	private TextView finishButton;
	private ImageView frameView;
	private TextView nextPageButton;
	private ImageView upFrameView;
	private ImageView downFrameView;
	private View view;
	private static final int TOUCH_TOP_BORDER = 1;
	private static final int TOUCH_BOTTOM_BORDER = 2;
	private static final int TOUCH_NULL_BORDER = 0;
	private static final int TOUCH_BORDER_SENSIVE = 50;
	private int touchPositon = LongScreenShotView.TOUCH_NULL_BORDER;
	private Rect mOrigalFrameRect;   
	private float touchDownY = 0;
	private boolean isInited = false;
	private Handler mHandler = new Handler();
	private Bitmap result = null;
	private StatusBarManager mStatusBarManager;
	private boolean next = false;
	private ArrayList<Bitmap> clipBitmapList;
	private long lastClickTime;
	private long maxTime = 800;
	private int safeNum = 13;
	private int pageNum = 1;
	private Toast mToast = null;
	private Handler mHandlerToast  = new Handler();
	
	//add by liweixin for [TPlink] longscreenshot don't show navigation bar
	private static final int NAVGATION_BAR_HEIGHT = 133;//FeatureOption.HQ_FHD_LONG_SCREENSHOT ? 133 : 96;
	private static final int LONGSS_FRAME_HEIGHT = 1776;//FeatureOption.HQ_FHD_LONG_SCREENSHOT ? 1776 : 1193;

	@Override
	public String getAppName() {
		// TODO Auto-generated method stub
		return LongScreenShotView.class.getSimpleName();
	}

	@Override
	public int getAppIcon() {
		// TODO Auto-generated method stub
		return android.R.drawable.ic_menu_close_clear_cancel;
	}

	@Override
	public boolean onKeyEvent(int id, Window window, KeyEvent event) {
		// TODO Auto-generated method stub
		goBack();
		return true;
	}
	
	@Override
	public void createAndAttachView(int id, FrameLayout frame) {
		disableStatusbar();
		LayoutInflater inflater = (LayoutInflater)getSystemService(LAYOUT_INFLATER_SERVICE);
		view = inflater.inflate(R.layout.long_screen_shot_view, frame, true);
		view.setOnTouchListener(this);
		backButton = (TextView)view.findViewById(R.id.long_shot_back);
		backButton.setOnTouchListener(this);
		finishButton = (TextView)view.findViewById(R.id.long_shot_save);
		finishButton.setOnTouchListener(this);
        nextPageButton = (TextView) view.findViewById(R.id.next_page);
        nextPageButton.setOnTouchListener(this);
		frameView = (ImageView)view.findViewById(R.id.frame);
		LayoutParams frameParam = (LayoutParams)frameView.getLayoutParams();
		upFrameView = (ImageView)view.findViewById(R.id.upframe);
		LayoutParams upFrameParam = (LayoutParams)upFrameView.getLayoutParams();
		downFrameView = (ImageView)view.findViewById(R.id.downframe);
        clipBitmapList = new ArrayList<Bitmap>();
        
	}

	@Override
	public void onDestroy() {	
		super.onDestroy();
              enableStatusbar();
		
	}

	
	@Override
	public StandOutLayoutParams getParams(int id, Window window) {
		int tempScreenWidth;
		mId = id;
        mScreenWidth = ((WindowManager) getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay().getWidth();
		mScreenHeight = ((WindowManager) getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay().getHeight();
        if(mScreenWidth > mScreenHeight)
        	tempScreenWidth = mScreenHeight;
        else
        	tempScreenWidth = mScreenWidth;
        mStandOutLayoutParams = new StandOutLayoutParams(id, mScreenWidth, mScreenHeight,0, 0);
        mStandOutLayoutParams.flags += WindowManager.LayoutParams.FLAG_FULLSCREEN
         		| WindowManager.LayoutParams.FLAG_HARDWARE_ACCELERATED
         		| WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN
         		| WindowManager.LayoutParams.FLAG_SHOW_WHEN_LOCKED;
        mStandOutLayoutParams.screenOrientation = ActivityInfo.SCREEN_ORIENTATION_SENSOR_PORTRAIT;
        return mStandOutLayoutParams;
	}
	
    @Override    
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        Window window = getWindow(mId);
        if(window == null)
        	return;
        mScreenWidth = ((WindowManager) getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay().getWidth();
        if (mStandOutLayoutParams.x < mScreenWidth / 3)
            mStandOutLayoutParams.x = 0;
        else 
            mStandOutLayoutParams.x = mScreenWidth - mStandOutLayoutParams.width;
		window.changeDisplaySize();
        getParams(mId, window);
        updateViewLayout(mId, mStandOutLayoutParams);
    }
    
    @Override
    public boolean onTouch(View v, MotionEvent event) {
    	// TODO Auto-generated method stub
    	int action = event.getAction();
    	int id = v.getId();
    	switch (id) {
		case R.id.next_page:			
			onTouchNextPage(action, event);
			break;
		case R.id.long_shot_save:
			pageNum = 1;
			onTouchLongshotsave(action, event);
			break;
		case R.id.long_shot_back:
			pageNum = 1;
			goBack();
			break;
		default:
			onTouchFrameview(action, event);
			break;
		}
    	return true;
    }
    
    private void goBack(){
	    Intent brocIntent = new Intent(SuperScreenShotConstant.BroadCast.LONG_SCREEN_SHOT_DISMISS);
	    sendBroadcast(brocIntent);
	    enableStatusbar();
    }
    
    private void onTouchLongshotsave(int action, MotionEvent event){
    	switch (action) {
		case MotionEvent.ACTION_DOWN:
			hideIcons(false);
			break;
		case MotionEvent.ACTION_UP:
			combineImage();
			enableStatusbar();
			//showIcons(false);  //add by liukai3 for HQ01000108 to avoid quickly click
			break;
		default:
			break;
		}
    }
    
    private void onTouchFrameview(int action,MotionEvent event){
    	switch (action) {
		case MotionEvent.ACTION_DOWN:
			mOrigalFrameRect = new Rect();
			frameView.getGlobalVisibleRect(mOrigalFrameRect);  
			isInited = true;
			touchPositon = isBorderTouched(event.getY());
			touchDownY = event.getY();
//			hideIcons(true);
			break;
		case MotionEvent.ACTION_MOVE:
			float moveStep = event.getY() - touchDownY;
			updateFrameView(moveStep);
			break;
		case MotionEvent.ACTION_UP:
//			showIcons(true);
			break;
		default:
			break;
		}
    }
    
    private void hideIcons(boolean frameenable){
    	if(!frameenable)
    		frameView.setVisibility(View.GONE);
		backButton.setVisibility(View.GONE);
		nextPageButton.setVisibility(View.GONE);
		finishButton.setVisibility(View.GONE);
    }
    
    private void showIcons(boolean frameenable){
    	if(!frameenable)
    		frameView.setVisibility(View.VISIBLE);
		backButton.setVisibility(View.VISIBLE);
		nextPageButton.setVisibility(View.VISIBLE);
		finishButton.setVisibility(View.VISIBLE);
    }
    
    private void onTouchNextPage(int action,MotionEvent event){
    	switch (action) {
		case MotionEvent.ACTION_DOWN:
			hideIcons(false);
			break;
		case MotionEvent.ACTION_UP:
			if(pageNum <= safeNum){
				doScreenShot();// the next button will do ScreenShot then go to next page!!
			}else{
				mHandler.removeCallbacks(toastRunable);
				if(mToast != null)
					mToast.setText(R.string.page_too_long_warning);
				else
					mToast = Toast.makeText(getApplicationContext(), R.string.page_too_long_warning, Toast.LENGTH_SHORT);
				mToast.show();
				mHandler.postDelayed((toastRunable), 1000);
			}
			Settings.System.putInt(getContentResolver(),Settings.System.SCREEN_SHOT_NEXT_PAGE, next?1:0);  //add for waiting for huangjie
			next = !next;
			pageNum++;
			showIcons(false);
			break;
		default:
			break;
		}
    }
    
    private void updateFrameView(float moveStep){
    	int cap = mOrigalFrameRect.bottom + (int)moveStep - mOrigalFrameRect.top;
    	switch (touchPositon) {
		case LongScreenShotView.TOUCH_TOP_BORDER:
			if(((mOrigalFrameRect.bottom - mOrigalFrameRect.top - (int)moveStep) > 70 && (mOrigalFrameRect.top + (int)moveStep > 50))){
				frameView.layout(mOrigalFrameRect.left, mOrigalFrameRect.top + (int)moveStep, mOrigalFrameRect.right, mOrigalFrameRect.bottom);
				upFrameView.layout(mOrigalFrameRect.left, 0 , mOrigalFrameRect.right, mOrigalFrameRect.top + (int)moveStep);
				downFrameView.layout(mOrigalFrameRect.left, mOrigalFrameRect.bottom, mOrigalFrameRect.right, mScreenHeight);
			}
			break;
		case LongScreenShotView.TOUCH_BOTTOM_BORDER:
			Log.v(TAG,"TOUCH_BOTTOM_BORDER mOrigalFrameRect.height:" + (mOrigalFrameRect.bottom + (int)moveStep - mOrigalFrameRect.top));
			if((mOrigalFrameRect.bottom + (int)moveStep - mOrigalFrameRect.top) > 70){
				//modified by liweixin for HQ01688124 begin
				if(mOrigalFrameRect.bottom + (int)moveStep >= LONGSS_FRAME_HEIGHT) {
				   frameView.layout(mOrigalFrameRect.left, mOrigalFrameRect.top, mOrigalFrameRect.right, LONGSS_FRAME_HEIGHT);
				} else{
				   frameView.layout(mOrigalFrameRect.left, mOrigalFrameRect.top, mOrigalFrameRect.right, mOrigalFrameRect.bottom + (int)moveStep);
				}
				//modified by liweixin for HQ01688124 end
				upFrameView.layout(mOrigalFrameRect.left, 0, mOrigalFrameRect.right, mOrigalFrameRect.top);
				downFrameView.layout(mOrigalFrameRect.left, mOrigalFrameRect.bottom + (int)moveStep, mOrigalFrameRect.right, mScreenHeight);
			}
			break;
		default:
			break;
		}
    }
    
    
    private int isBorderTouched(float y){
		Rect globalRect = new Rect();
		frameView.getGlobalVisibleRect(globalRect);    	
    	int result = LongScreenShotView.TOUCH_NULL_BORDER;
    	if(Math.abs(y - globalRect.top) < LongScreenShotView.TOUCH_BORDER_SENSIVE)
    		return LongScreenShotView.TOUCH_TOP_BORDER;
    	if(Math.abs(y - globalRect.bottom) < LongScreenShotView.TOUCH_BORDER_SENSIVE){
    		return LongScreenShotView.TOUCH_BOTTOM_BORDER;
    	}
    	return LongScreenShotView.TOUCH_NULL_BORDER; 
    }
    	
	//add by zhangchunzhao 
	//shot screen and clip screen into needed shape
	private void doScreenShot(){
		new Thread(new Runnable() {
			@Override
			public void run() {
				Log.d(TAG,"longshot---doScreenShot");			
				synchronized (this) {
					Bitmap mMap = SurfaceControl.screenshot(mScreenWidth, mScreenHeight);
					Rect rect = new Rect();
					frameView.getGlobalVisibleRect(rect);
					int[] location = new int[4];
					frameView.getLocationOnScreen(location);
					//modified by liweixin for [TPlink] longscreenshot don't show navigation bar
					Bitmap newMap = (rect.height() - NAVGATION_BAR_HEIGHT <= LONGSS_FRAME_HEIGHT/2)
? Bitmap.createBitmap(mMap, location[0], location[1], rect.width(), rect.height() - NAVGATION_BAR_HEIGHT/4)
: Bitmap.createBitmap(mMap, location[0], location[1], rect.width(), rect.height() - NAVGATION_BAR_HEIGHT);
					int height = 0;
					float ratio = 0;
					if(clipBitmapList.size() >= 1){
						height = getCompareHeight(clipBitmapList.get(0), newMap);
						ratio = getCompareRatio(clipBitmapList.get(0), newMap);
					}
					Log.d(TAG, "admas doScreenShot ratio = " + ratio +  " ,newMap.getHeight() = " + newMap.getHeight() + ",height = " + height + ",clipBitmapList.size() = " + clipBitmapList.size());
					if((newMap.getHeight() - height <= 0) || (ratio > 90.0)){
						mMap.recycle();				
						if(isToCombineImage)
							startImageCombileHandle();							
						return;
					}
					if(height > 0){
						Bitmap clipedBm = Bitmap.createBitmap(newMap, 0, height, newMap.getWidth(), newMap.getHeight() - height);
						newMap.recycle();
						newMap = Bitmap.createBitmap(clipedBm);
					}
					if(clipBitmapList.size() >=1 ){
						height =  getBottomCompareHeight(clipBitmapList.get(clipBitmapList.size() -1), newMap);
					}
					Log.d(TAG, "admas111 newMap.getHeight() = " + newMap.getHeight() + ",height = " + height);
					if(height > 0 && (newMap.getHeight() - height > 0)){
						Bitmap clipedBm = Bitmap.createBitmap(newMap, 0, height, newMap.getWidth(), newMap.getHeight() - height);
						newMap.recycle();
						newMap = Bitmap.createBitmap(clipedBm);					
					}

					clipBitmapList.add(newMap);
					mMap.recycle();				
					if(isToCombineImage)
						startImageCombileHandle();					
				}
			}  
		}).start();
	}
	
	private int getCompareHeight(Bitmap first, Bitmap second){
		int height = 0;
		int[] pixels_first = new int[first.getWidth()*first.getHeight()];
		int[] pixels_second = new int[second.getWidth()*second.getHeight()];
		first.getPixels(pixels_first, 0, first.getWidth(), 0, 0, first.getWidth(), first.getHeight());
		second.getPixels(pixels_second, 0, second.getWidth(), 0, 0, second.getWidth(), second.getHeight());
		for(int i=0; i<pixels_first.length; i++){
			int cur_first = pixels_first[i];
			if(pixels_second.length>=(i+1)){
				int cur_second = pixels_second[i];
				if(cur_second != cur_first)
					break;
			}
			if(i%first.getWidth() == 0){
				height ++;
//				Log.d(TAG, "admas:getCompareHeight height ++  = " + height);
			}
		}
//		Log.d(TAG, "admas:getCompareHeight height = " + height);
		return height;
	}  
	
	private float getCompareRatio(Bitmap first, Bitmap second){
		int k = 0;
		int[] pixels_first = new int[first.getWidth()*first.getHeight()];
		int[] pixels_second = new int[second.getWidth()*second.getHeight()];
		first.getPixels(pixels_first, 0, first.getWidth(), 0, 0, first.getWidth(), first.getHeight());
		second.getPixels(pixels_second, 0, second.getWidth(), 0, 0, second.getWidth(), second.getHeight());
		for(int i=0; i<pixels_first.length; i++){
			int cur_first = pixels_first[i];
			if(pixels_second.length>=(i+1)){
				int cur_second = pixels_second[i];
				if(cur_second == cur_first)
					k++;
			}
		}
		return k*100/pixels_first.length;
	}  
	
	
	private int getBottomCompareHeight(Bitmap first, Bitmap second){
		int height = first.getHeight();
		int[] pixels_first = new int[first.getWidth()*first.getHeight()];
		int[] pixels_second = new int[second.getWidth()*second.getHeight()];		
		boolean isBlanked = true;
		first.getPixels(pixels_first, 0, first.getWidth(), 0, 0, first.getWidth(), first.getHeight());
		second.getPixels(pixels_second, 0, second.getWidth(), 0, 0, second.getWidth(), second.getHeight());		
		int startMatchLine = 0;
		for(int i = 0; i < pixels_second.length;i++){
			if(pixels_second[0] != pixels_second[i]){
				startMatchLine = i/second.getWidth();
				break;
			}
		}
		startMatchLine += 1;
//		Log.d(TAG, "admas：getBottomCompareHeight：startMatchLine = " + startMatchLine + ",first.getHeight() = " + first.getHeight() + ",second.getHeight() = " + second.getHeight());
/*		int line1 = 1113;
		int line2 = 28;
		for(int i = 0; i<first.getWidth();i++){
			Log.d("a", "admasindex = " + i + " ,pixels_first =: " + pixels_first[line1*first.getWidth() + i] + ", " + pixels_first[(line1 + 1)*first.getWidth() + i] + "," + pixels_first[(line1 + 2)*first.getWidth() + i] +  ", " + ",pixels_second = " + pixels_second[line2*second.getWidth() + i]);
		}

*/
		for(int i = 0; i<first.getHeight(); i ++){
			boolean isMateched = true;
			for(int j = 0;j < first.getWidth(); j++){
				if(pixels_first[j+i*first.getWidth()] != pixels_second[startMatchLine*second.getWidth() + j]){
					isMateched = false;
					break;
				}
			}
//			Log.d(TAG, "admas getBottomCompareHeight i = " + i + ",isMateched = " + isMateched);
			if(isMateched){
				height = i;
				Log.d(TAG, "admas getBottomCompareHeight i = " + i);
			}
		}
		return first.getHeight() - height + startMatchLine;
	}
		
	//add by zhangchunzhao
	//combile temp images into a long image ,and delete temp images
	private boolean isToCombineImage = false;
	private void combineImage(){
		android.util.Log.e("liukai3","clipBitmapList's size is:"+clipBitmapList.size());
/*		if(clipBitmapList.size()==0){
			android.util.Log.e("liukai3","clipBitmapList's size is 0");
			doScreenShot();
		}
*/		
		isToCombineImage = true;
		if(mToast != null){
			mToast.cancel();
			mToast = null;
		}
		doScreenShot();  
	}
	
	private void startImageCombileHandle(){
		isToCombineImage = false;
		mHandler.postDelayed(combineImageRun, 100);
		mHandler.postDelayed(startAnimRun, 100);
	}
	
	private Runnable combineImageRun = new Runnable() {
		@Override
		public void run() {
			int maxlength = 0;
			for(Bitmap bitmap : clipBitmapList) {
				maxlength += bitmap.getHeight();
			}
			try {
				result = Bitmap.createBitmap(mScreenWidth, maxlength, Config.ARGB_8888);
			} catch (Exception e) {
				// TODO: handle exception
				result = null;
			}
			int drawtop = 0;
			Canvas canvas = null;
			if(result != null)
				canvas = new Canvas(result);
			for (Bitmap bitmap : clipBitmapList) {
				canvas.drawBitmap(bitmap, 0, drawtop, null);
				drawtop += bitmap.getHeight();
			}
//			SuperScreenShotTools mSuperScreenShotTools = new SuperScreenShotTools();
//			mSuperScreenShotTools.saveImage(getApplicationContext(),result,1);
		}
	};
	
	
	private Runnable toastRunable = new Runnable() {
		
		@Override
		public void run() {
			// TODO Auto-generated method stub
			if(mToast != null){
				mToast.cancel();
				mToast = null;
			}
		}
	};
	
	private Runnable startAnimRun = new Runnable() {
		@Override
		public void run() {
			if(result!=null){
				final SuperGlobalScreenshot screenshot = new SuperGlobalScreenshot(getApplicationContext());
				screenshot.showScreenshotAnim(result,new Runnable() {  
	            @Override  
	            public void run() {    
	            }  
				}, true, true , true);
				Intent intent = new Intent();
				intent.setAction(SuperScreenShotConstant.BroadCast.LONG_SCREEN_SHOT_DISMISS);
				getApplicationContext().sendBroadcast(intent);
		}
	  }
	};
	
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
        
        mStatusBarManager.setIconVisibility("clock", false);
        mStatusBarManager.disable(flags);
	}
	
	public void enableStatusbar(){
        if (mStatusBarManager == null) {
            mStatusBarManager = (StatusBarManager)getSystemService(Context.STATUS_BAR_SERVICE);
        }
        int flags = StatusBarManager.DISABLE_NONE;
        mStatusBarManager.disable(flags);
	}

	public boolean isFastDoubleClick() {
        long time = System.currentTimeMillis();
        long timeD = time - lastClickTime;
        if (timeD > maxTime) {
        	lastClickTime = time;
        	return true;
        }
        	lastClickTime = time;
        	return false;
    }
}
