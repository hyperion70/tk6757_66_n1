package com.fineos.superscreenshot.funscreenshot;

import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import com.fineos.superscreenshot.floatwindow.Utils;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.PorterDuff;
import android.graphics.PorterDuffXfermode;
import android.graphics.RectF;
import android.graphics.Bitmap.Config;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import com.fineos.superscreenshot.R;
import android.content.res.Resources;
import android.graphics.Paint.Align;
import android.content.res.Configuration;

public class RectClearView extends View {

	private final static String TAG="RectClearView";
	private int mScreenWidth;
	private int mScrreenHeight;
	private Paint mPaint;
	private Paint mClingPaint;
	private Paint mLinePaint;
	private Paint mChunkPaint;
	private Path mPath;
	private Path bihePath;
	private Bitmap mBitmap;
	private Bitmap mBlackBitmap;

	private float mX, mY;
	private static final float TOUCH_TOLERANCE = 4;
	private int mDrawType = -1;
	private ArrayList<ClearView.Point> mList = new ArrayList<ClearView.Point>();
	private Boolean samePosition = false;
	private RectF mRect = new RectF();
	private RectF mBeginRect = new RectF();
	private TouchEventListener mListenerCallback;
	private List<Path> mSavePathList;
	private int mStartIndex, mEndIndex;
	private float mRectLeft,mRectTop,mRectRight,mRectBottom;
	private Bitmap rect_background;
	private Bitmap mClingBitmap;
	private RectF mLeftTopRect,mRightTopRect,mLeftBottomRect,mRightBottomRect,mLeftMiddleRect,mRightMiddleRect,mTopMiddleRect,mBottomMiddleRect;
	private int radius = 35;
	private boolean mRectSelected = false;
	private int mMoveType = -1;
	private int mSelectedIndex = -1;
	private boolean mShowCling = true;
	private String introduceStr;
	private int mTextMarginTop = 40;
	private int mTextMarginLeft = 160;
	private int mAngleRadius = 2;
	

	public RectClearView(Context context) {
		this(context, null);
		// TODO Auto-generated constructor stub
	}

	public RectClearView(Context context, AttributeSet attrs, int defStyleAttr) {
		super(context, attrs, defStyleAttr);
		setBackgroundColor(Color.argb(0, 255, 255, 255));
		setScreenWH();
		Bitmap bm = createBitmapFromARGB(0xb3000000, mScreenWidth,
				mScrreenHeight);
		setCoverBitmap(bm);
		//rect_background = readBitMap(context,R.drawable.rect_clear_background);
		mClingBitmap = readBitMap(context,R.drawable.rect_cling_drawable);
		Resources res = context.getResources();
		introduceStr = res.getString(R.string.rect_tips_intr_text);
		// TODO Auto-generated constructor stub
	}

	public RectClearView(Context context, AttributeSet attrs) {
		this(context, attrs, -1);
		// TODO Auto-generated constructor stub
	}

	@Override
	protected void onDraw(Canvas canvas) {
		// TODO Auto-generated method stub
		canvas.drawBitmap(mBitmap, 0, 0, null);
		if(mShowCling){
		if(mClingBitmap !=null){
			canvas.drawBitmap(mClingBitmap, (mScreenWidth-mClingBitmap.getWidth())/2, (mScrreenHeight-mClingBitmap.getHeight())/2, new Paint());
			canvas.drawText(introduceStr,(mScreenWidth-mClingBitmap.getWidth())/2+mTextMarginLeft,(mScrreenHeight+mClingBitmap.getHeight())/2+mTextMarginTop,mClingPaint);
		}
		}else{
		canvas.drawRect(mRect, mPaint);
		//if(rect_background !=null){
			//canvas.drawBitmap(rect_background, null, mRect, mPaint);
			//drawLine
			canvas.drawLine(mRect.left,mRect.top,mRect.right,mRect.top,mLinePaint);
			canvas.drawLine(mRect.left,mRect.top,mRect.left,mRect.bottom,mLinePaint);
			canvas.drawLine(mRect.right,mRect.top,mRect.right,mRect.bottom,mLinePaint);
			canvas.drawLine(mRect.left,mRect.bottom,mRect.right,mRect.bottom,mLinePaint);
			//drawChunk
			if(mRect.width() < 80.0f){
				canvas.drawLine(mRect.left-mAngleRadius,mRect.top,mRect.right+mAngleRadius,mRect.top,mChunkPaint);
				canvas.drawLine(mRect.left-mAngleRadius,mRect.bottom,mRect.right+mAngleRadius,mRect.bottom,mChunkPaint);
			}else{
				canvas.drawLine(mRect.left-mAngleRadius,mRect.top,mRect.left+24,mRect.top,mChunkPaint);
				canvas.drawLine(mRect.right-24,mRect.top,mRect.right+mAngleRadius,mRect.top,mChunkPaint);
				canvas.drawLine(mRect.left-mAngleRadius,mRect.bottom,mRect.left+24,mRect.bottom,mChunkPaint);
				canvas.drawLine(mRect.right-24,mRect.bottom,mRect.right+mAngleRadius,mRect.bottom,mChunkPaint);
				canvas.drawLine((mTopMiddleRect.left+mTopMiddleRect.right)/2-13,mRect.top,(mTopMiddleRect.left+mTopMiddleRect.right)/2+13,mRect.top,mChunkPaint);
				canvas.drawLine((mBottomMiddleRect.left+mBottomMiddleRect.right)/2-13,mRect.bottom,(mBottomMiddleRect.left+mBottomMiddleRect.right)/2+13,mRect.bottom,mChunkPaint);
			}
			if(mRect.height()< 80.0f){
				canvas.drawLine(mRect.left,mRect.top-mAngleRadius,mRect.left,mRect.bottom+mAngleRadius,mChunkPaint);
				canvas.drawLine(mRect.right,mRect.top-mAngleRadius,mRect.right,mRect.bottom+mAngleRadius,mChunkPaint);
			}else{
				canvas.drawLine(mRect.left,mRect.top-mAngleRadius,mRect.left,mRect.top+24,mChunkPaint);
				canvas.drawLine(mRect.right,mRect.top-mAngleRadius,mRect.right,mRect.top+24,mChunkPaint);
				canvas.drawLine(mRect.left,mRect.bottom+mAngleRadius,mRect.left,mRect.bottom-24,mChunkPaint);
				canvas.drawLine(mRect.right,mRect.bottom+mAngleRadius,mRect.right,mRect.bottom-24,mChunkPaint);
				canvas.drawLine(mRect.left,(mLeftMiddleRect.top+mLeftMiddleRect.bottom)/2-13,mRect.left,(mLeftMiddleRect.top+mLeftMiddleRect.bottom)/2+13,mChunkPaint);
				canvas.drawLine(mRect.right,(mRightMiddleRect.top+mRightMiddleRect.bottom)/2-13,mRect.right,(mRightMiddleRect.top+mRightMiddleRect.bottom)/2+13,mChunkPaint);
			}
		//}
		}
		super.onDraw(canvas);
	}

	@Override
	protected void onConfigurationChanged(Configuration newConfig) {
		// TODO Auto-generated method stub
		android.util.Log.e("lixiaosong","RectClearView onConfigurationChanged");
		setScreenWH();
		android.util.Log.e("lixiaosong","RectClearView onConfigurationChanged mScreenWidth:"+mScreenWidth+"  mScrreenHeight:"+mScrreenHeight);
		Bitmap bm=createBitmapFromARGB(0xb3000000, mScreenWidth, mScrreenHeight);
		setCoverBitmap(bm);
		reset();
		invalidate();
		super.onConfigurationChanged(newConfig);
	}

	@Override
	protected void onSizeChanged(int w, int h, int oldw, int oldh) {
		// TODO Auto-generated method stub
		android.util.Log.e("lixiaosong","ClearView onSizeChanged");
		setScreenWH();
		android.util.Log.e("lixiaosong","ClearView onSizeChanged mScreenWidth:"+mScreenWidth+"  mScrreenHeight:"+mScrreenHeight);
		Bitmap bm=createBitmapFromARGB(0xb3000000, mScreenWidth, mScrreenHeight);
		setCoverBitmap(bm);
		reset();
		invalidate();
		super.onSizeChanged(w, h, oldw, oldh);
	}

	private void setScreenWH() {
		DisplayMetrics dm = new DisplayMetrics();
		dm = getResources().getDisplayMetrics();
		mScreenWidth = dm.widthPixels;
		mScrreenHeight = dm.heightPixels;
		mRectLeft = mScreenWidth/4;
		mRectTop = mScrreenHeight/4;
		mRectBottom = mScrreenHeight*3/4;
		mRectRight = mScreenWidth*3/4;
		initRect();
	}

	private void initRect(){
		mRect.left = mRectLeft;
		mRect.right = mRectRight;
		mRect.top = mRectTop;
		mRect.bottom = mRectBottom;
		updateRects();
	}

	@Override
	public boolean onTouchEvent(MotionEvent event) {
		// TODO Auto-generated method stub
        float  x = event.getX();  
        float y = event.getY(); 
		Log.e(TAG,"onTouchEvent  mDrawType-->"+mDrawType);
		if(mDrawType == Utils.TYPE_UNKNOWN){
			return true;
		}
        if(mShowCling){
			mShowCling =false;
			releaseClingMap();
			mMoveType = 0;
			mSelectedIndex = 0;
			mRect.set(x,y,x+1,y+1);
		}
        switch (event.getAction()) {  
            case MotionEvent.ACTION_DOWN:
				if(mMoveType == -1){
            		mMoveType = checkPosition(x, y);
				}
            	if(mMoveType > -1){
            		mRectSelected =true;
					mBeginRect.set(mRect);
				}
				updateTabLayout(false);
                break;  
            case MotionEvent.ACTION_MOVE:  
            	if(mRectSelected){
            		updateClearRect(x,y);
					updateRects();
            	}else{
					if(mMoveType == -1){
                		mMoveType = checkPosition(x, y);
					}
                	if(mMoveType > -1){
                		mRectSelected =true;
						mBeginRect.set(mRect);            		
					}
            	}
                break;  
            case MotionEvent.ACTION_UP:
            	if(mRectSelected){
            		updateClearRect(x,y);
            	}
				mRectSelected = false;
				mMoveType = -1;
				mSelectedIndex = -1;
            	updateRects();
				updateTabLayout(true);
                break;  
        }  
        invalidate();
        return true; 
	}

	private Bitmap createBitmapFromARGB(int colorARGB, int width, int height) {
		int[] argbs = new int[width * height];
		for (int i = 0; i < argbs.length; i++) {
			argbs[i] = colorARGB;
		}
		return Bitmap.createBitmap(argbs, width, height, Config.ARGB_8888);
	}

	private void setCoverBitmap(Bitmap bm) {
		// setting paint
		mPaint = new Paint();
		mPaint.setAlpha(0);
		mPaint.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.DST_IN));
		mPaint.setAntiAlias(true);// juchi
		mPaint.setDither(true);// fang dou dong
		mPaint.setStyle(Paint.Style.FILL);
		mPaint.setStrokeJoin(Paint.Join.ROUND);
		mPaint.setStrokeCap(Paint.Cap.ROUND);
		mPaint.setStrokeWidth(20);
	
		mClingPaint = new Paint();
		mClingPaint.setAntiAlias(true);
           // mTimePaint.setStyle(Style.STROKE);
		mClingPaint.setTextAlign(Align.CENTER);
		mClingPaint.setTextSize(30);
		mClingPaint.setColor(0xFFFFFFFF);
		mLinePaint = new Paint();
		mLinePaint.setAntiAlias(true);
           // mTimePaint.setStyle(Style.STROKE);
		mLinePaint.setTextAlign(Align.CENTER);
		mLinePaint.setTextSize(30);
		mLinePaint.setStrokeWidth(2);
		mLinePaint.setColor(0xFF00FF00);
		mChunkPaint = new Paint();
		mChunkPaint.setAntiAlias(true);
           // mTimePaint.setStyle(Style.STROKE);
		mChunkPaint.setTextAlign(Align.CENTER);
		mChunkPaint.setTextSize(30);
		mChunkPaint.setStrokeWidth(5);
		mChunkPaint.setColor(0xFF00FF00);
		// converting bitmap into mutable bitmap
		mBitmap = Bitmap.createBitmap(mScreenWidth, mScrreenHeight,
				Config.ARGB_8888);
		Canvas mCanvas = new Canvas();
		mCanvas.setBitmap(mBitmap);
		mCanvas.drawBitmap(bm, 0, 0, null);
	}
	
    public static Bitmap readBitMap(Context context, int resId){  
        BitmapFactory.Options opt = new BitmapFactory.Options();  
        opt.inPreferredConfig = Bitmap.Config.RGB_565;   
       opt.inPurgeable = true;  
       opt.inInputShareable = true;  
          //获取资源图片  
	InputStream is = null;
	Bitmap mBitmap = null;
	try{
        	is = context.getResources().openRawResource(resId);
	 	mBitmap = BitmapFactory.decodeStream(is,null,opt);
		if(is!=null){
			is.close();	
		}
	}catch(IOException e){
		android.util.Log.e(TAG,"readBitMap IOException");
	}
	return  mBitmap;   
   }
    
	public void setTouchEventListener(TouchEventListener mListener){
		mListenerCallback  =  mListener;
	}
	
	private void updateTabLayout(boolean visible){
		if(mListenerCallback != null){
				mListenerCallback.updateTabLayout(visible);
		}
	}


    public Bitmap getBitMap(){
		
		Bitmap bmblack=createBitmapFromARGB(0xff000000, mScreenWidth, mScrreenHeight);
		Bitmap mTestBlackBitmap = Bitmap.createBitmap(mScreenWidth, mScrreenHeight, Config.ARGB_8888);
		Canvas mBlackCanvas = new Canvas();
		mBlackCanvas.setBitmap(mTestBlackBitmap);
		mBlackCanvas.drawBitmap(bmblack, 0, 0, null);
		mBlackCanvas.drawRect(mRect, mPaint);
		//if(rect_background !=null){
		//	mBlackCanvas.drawBitmap(rect_background, null, mRect, null);
		//}
		releaseBitmap();
    	return mTestBlackBitmap;
    }

	private void releaseBitmap() {
		if(mBitmap !=null && (!mBitmap.isRecycled())){
			mBitmap.recycle();
		}
		if(rect_background !=null && (!rect_background.isRecycled())){
			rect_background.recycle();
		}
	}

	public RectF getRect(){
		return mRect;
	}


	public int getmDrawType() {
		return mDrawType;
	}



	public void setmDrawType(int mDrawType) {
		this.mDrawType = mDrawType;
	}

	public void reset(){
        initRect();
		invalidate();
	}

	public void backup(){
        //
	}
	
	private void updateRects(){
		if(mLeftTopRect == null)
			mLeftTopRect = new RectF();
		mLeftTopRect.set(mRect.left-radius, mRect.top-radius, mRect.left+radius, mRect.top+radius);
		if(mLeftBottomRect == null)
			mLeftBottomRect = new RectF();
		mLeftBottomRect.set(mRect.left-radius, mRect.bottom-radius, mRect.left+radius, mRect.bottom+radius);
		if(mRightTopRect == null)
			mRightTopRect = new RectF();
		mRightTopRect.set(mRect.right-radius, mRect.top-radius, mRect.right+radius, mRect.top+radius);
		if(mRightBottomRect == null)
			mRightBottomRect = new RectF();
		mRightBottomRect.set(mRect.right-radius, mRect.bottom-radius, mRect.right+radius, mRect.bottom+radius);
		if(mLeftMiddleRect == null)
			mLeftMiddleRect = new RectF();
		mLeftMiddleRect.set(mRect.left-radius, (mRect.top+mRect.bottom)/2-radius, mRect.left+radius, (mRect.top+mRect.bottom)/2+radius);
		if(mRightMiddleRect == null)
			mRightMiddleRect = new RectF();
		mRightMiddleRect.set(mRect.right-radius, (mRect.top+mRect.bottom)/2-radius, mRect.right+radius, (mRect.top+mRect.bottom)/2+radius);
		if(mTopMiddleRect == null)
			mTopMiddleRect = new RectF();
		mTopMiddleRect.set((mRect.left+mRect.right)/2-radius, mRect.top-radius, (mRect.left+mRect.right)/2+radius, mRect.top+radius);
		if(mBottomMiddleRect == null)
			mBottomMiddleRect = new RectF();
		mBottomMiddleRect.set((mRect.left+mRect.right)/2-radius, mRect.bottom-radius, (mRect.left+mRect.right)/2+radius, mRect.bottom+radius);
	}
	
	private int checkPosition(float x,float y){
		
		if(mLeftTopRect.contains(x, y))
			mSelectedIndex = 0;
		if(mRightTopRect.contains(x, y))
			mSelectedIndex = 1;
		if(mLeftBottomRect.contains(x, y))
			mSelectedIndex = 3;
		if(mRightBottomRect.contains(x, y))
			mSelectedIndex = 2;
		if(mSelectedIndex > -1)
			return 0;
		
		if(mLeftMiddleRect.contains(x, y))
			mSelectedIndex = 0;
		if(mRightMiddleRect.contains(x, y))
			mSelectedIndex = 1;
		if(mSelectedIndex > -1)
			return 1;
		
		if(mTopMiddleRect.contains(x, y))
			mSelectedIndex = 0;
		if(mBottomMiddleRect.contains(x, y))
			mSelectedIndex = 1;
		if(mSelectedIndex > -1)
			return 2;
		
		return -1;
	}
	
	private void updateClearRect(float x,float y){
		android.util.Log.e(TAG,"mMoveType-->"+mMoveType+" mSelectedIndex-->"+mSelectedIndex);
		switch (mMoveType) {
		case 0:
			switch (mSelectedIndex) {
			case 0:
				calculatePosition(x, y, mBeginRect.right, mBeginRect.bottom);
				break;
			case 1:
				calculatePosition(mBeginRect.left, y, x, mBeginRect.bottom);
				break;
			case 2:
				calculatePosition(mBeginRect.left, mBeginRect.top, x, y);
				break;
			case 3:
				calculatePosition(x, mBeginRect.top, mBeginRect.right, y);
				break;			
			default:
				break;
			}
			break;
		
		case 1:
			switch (mSelectedIndex) {
			case 0:
				calculatePosition(x, mBeginRect.top, mBeginRect.right, mBeginRect.bottom);
				break;
			case 1:
				calculatePosition(mBeginRect.left, mBeginRect.top, x, mBeginRect.bottom);
				break;
			default:
				break;
			}
			break;
		case 2:
			switch (mSelectedIndex) {
			case 0:
				calculatePosition(mBeginRect.left, y, mBeginRect.right, mBeginRect.bottom);
				break;
			case 1:
				calculatePosition(mBeginRect.left, mBeginRect.top, mBeginRect.right, y);
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}
	
	private void calculatePosition(float left,float top,float right,float bottom){
		mRect.set(left > right ? right:left , top > bottom ? bottom : top, left > right ? left:right , top > bottom ? top : bottom);
	}

	private void releaseClingMap(){
		if(mClingBitmap !=null && (!mClingBitmap.isRecycled())){
			mClingBitmap.recycle();
		}
	}

}
