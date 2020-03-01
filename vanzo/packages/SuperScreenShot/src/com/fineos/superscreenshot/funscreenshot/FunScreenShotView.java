package com.fineos.superscreenshot.funscreenshot;

import android.content.Context;
import android.graphics.Bitmap;
import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.WindowManager;
import android.view.ViewGroup.LayoutParams;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.TableLayout;
import android.widget.TextView;

import com.fineos.superscreenshot.SuperScreenShotTools;
import com.fineos.superscreenshot.floatwindow.StandOutWindow;
import com.fineos.superscreenshot.floatwindow.Utils;
import com.fineos.superscreenshot.floatwindow.Window;
import com.fineos.superscreenshot.floatwindow.StandOutWindow.StandOutLayoutParams;
import android.view.SurfaceControl;
import com.fineos.superscreenshot.R;
import java.lang.reflect.Field;


public class FunScreenShotView extends StandOutWindow implements TouchEventListener{

	private final static String TAG="FunScreenShotView";
	private Button mPreviewButton,mSaveButton;
	private boolean showClear = true;
	private View view;
	private ClearView myClear;
	private RectClearView mRectView;
	private ImageView mImage;
	private TextView exitText,recText,lariatText,scrawlText,reworkText,backupText,previewText,saveText;
	private TableLayout mTabLayout;
	private int mId;
    private int mScreenWidth;
	private int mScreenHeight;
	private Handler mHandler = new Handler();
    private StandOutLayoutParams mStandOutLayoutParams;
	Class<?> c = null;
	Object obj = null;
	Field field = null;
	int x = 0, sbar = 0;
	int mCurruntDrawType;
	
    Runnable run = new Runnable() {
		
		@Override
		public void run() {
			// TODO Auto-generated method stub
				Bitmap mMap = SurfaceControl.screenshot(mScreenWidth, 1280);
        		if (mMap == null) {
					Log.e(TAG,"mMap is null ");
					}else{
        			// Optimizations
					Log.e(TAG,"save map success ");
        			mMap.setHasAlpha(false);
        			mMap.prepareToDraw();
					SuperScreenShotTools mSuperScreenShotTools = new SuperScreenShotTools();
					mSuperScreenShotTools.saveImage(getApplicationContext(),mMap,0);
					//closeClearView();
					stopSelf();
					}		
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
            			//closeClearView();
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
					mCurruntDrawType = Utils.TYPE_LARIAT;
					myClear.setVisibility(View.VISIBLE);
					mRectView.setVisibility(View.GONE);
					myClear.setmDrawType(Utils.TYPE_LARIAT);
					updateLayoutContent();
					Log.e(TAG,"mClickListener lariat ");
					break;
				case R.id.scrawl:
					mCurruntDrawType = Utils.TYPE_SCRAWL;
					myClear.setVisibility(View.VISIBLE);
					mRectView.setVisibility(View.GONE);
					myClear.setmDrawType(Utils.TYPE_SCRAWL);
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
					previewText.setVisibility(View.VISIBLE);
					saveText.setVisibility(View.GONE);
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
					previewText.setVisibility(View.VISIBLE);
					saveText.setVisibility(View.GONE);
					//mTabLayout.setVisibility(View.VISIBLE);
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
					mTabLayout.setVisibility(View.GONE);
					mHandler.postDelayed(run, 100);
					Log.e(TAG,"mClickListener save ");
					break;
				default :
					break;
			}
		}
};
	
	@Override
	public String getAppName() {
		// TODO Auto-generated method stub
		return FunScreenShotView.class.getName();
	}

	@Override
	public int getAppIcon() {
		// TODO Auto-generated method stub
		return android.R.drawable.ic_menu_close_clear_cancel;
	}

	@Override
	public void createAndAttachView(int id, FrameLayout frame) {
		// TODO Auto-generated method stub
		LayoutInflater inflater = (LayoutInflater)getSystemService(LAYOUT_INFLATER_SERVICE);
		view = inflater.inflate(R.layout.another_layout, frame, true);
		//view.setOnTouchListener(this);
		mTabLayout = (TableLayout) view.findViewById(R.id.tableLayout);
		myClear =  (ClearView) view.findViewById(R.id.myclear);
		mRectView = (RectClearView) view.findViewById(R.id.myrectclear);
		myClear.setTouchEventListener(this);
		mRectView.setTouchEventListener(this);
		exitText = (TextView) view.findViewById(R.id.exit);
		recText = (TextView) view.findViewById(R.id.rectangle);
		lariatText = (TextView) view.findViewById(R.id.lariat);
		scrawlText = (TextView) view.findViewById(R.id.scrawl);
		reworkText = (TextView) view.findViewById(R.id.rework);
		backupText = (TextView) view.findViewById(R.id.backup);
		previewText = (TextView) view.findViewById(R.id.preview);
		saveText = (TextView) view.findViewById(R.id.save);
		mImage = (ImageView) view.findViewById(R.id.myimage);
		exitText.setOnClickListener(mClickListener);
		recText.setOnClickListener(mClickListener);
		lariatText.setOnClickListener(mClickListener);
		scrawlText.setOnClickListener(mClickListener);
		reworkText.setOnClickListener(mClickListener);
		backupText.setOnClickListener(mClickListener);
		previewText.setOnClickListener(mClickListener);
		saveText.setOnClickListener(mClickListener);
	}

	@Override
	public StandOutLayoutParams getParams(int id, Window window) {
		// TODO Auto-generated method stub
		int tempScreenWidth;
		mId = id;
        mScreenWidth = ((WindowManager) getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay().getWidth();
		mScreenHeight = ((WindowManager) getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay().getHeight();
        if(mScreenWidth > mScreenHeight)
        	tempScreenWidth = mScreenHeight;
        else
        	tempScreenWidth = mScreenWidth;

try {
    c = Class.forName("com.android.internal.R$dimen");
    obj = c.newInstance();
    field = c.getField("status_bar_height");
    x = Integer.parseInt(field.get(obj).toString());
    sbar = getResources().getDimensionPixelSize(x);
} catch(Exception e1) {
    android.util.Log.e("lililixiaosong","get status bar height fail");
    e1.printStackTrace();
}
		mScreenHeight -=sbar;
        mStandOutLayoutParams = new StandOutLayoutParams(id, mScreenWidth, mScreenHeight,0, 0);
		android.util.Log.e("lililixiaosong","mScreenWidth-->"+mScreenWidth+" mScreenHeight-->"+mScreenHeight+" sbar-->"+sbar);
        return mStandOutLayoutParams;
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
	
	private void updateLayoutContent(){
		recText.setVisibility(View.GONE);
		lariatText.setVisibility(View.GONE);
		scrawlText.setVisibility(View.GONE);
		reworkText.setVisibility(View.VISIBLE);
		backupText.setVisibility(View.VISIBLE);
		previewText.setVisibility(View.VISIBLE);
	}

}
