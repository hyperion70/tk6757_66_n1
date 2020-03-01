package com.fineos.superscreenshot.funscreenshot;

import java.util.ArrayList;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.PorterDuff;
import android.graphics.PorterDuffXfermode;
import android.graphics.RectF;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import com.fineos.superscreenshot.floatwindow.*;
import java.util.List;
import java.util.Iterator;
import android.graphics.Region;
import android.graphics.PathMeasure;
import android.graphics.DashPathEffect;
import android.content.res.Configuration;
import android.widget.Toast;
import com.fineos.superscreenshot.R;


public class ClearView extends View implements Runnable{
	
	private final static String TAG="ClearView";
	private int mScreenWidth;
	private int mScrreenHeight;
	private Paint mPaint;
	private Path mPath;
	private Path bihePath;
	private Paint mClearPaint;
	private Bitmap mBitmap;
	private Bitmap mSevenBitmap;
	private Bitmap mBlackBitmap;
	
    private float mX, mY;  
    private static final float TOUCH_TOLERANCE = 4;
	private int mDrawType = -1;
    private ArrayList<ClearView.Point> mList = new ArrayList<ClearView.Point>();
	private Boolean samePosition=false;
	private RectF  mRect= new RectF();
	private TouchEventListener mListenerCallback;
	private List<Path> mSavePathList;
	private List<RectF> mSaveRectList;
	private int mStartIndex,mEndIndex;
	boolean isAttached =false;
	int start = 5;
	int step =5;
	boolean result = false;
	private Context mContext;
	public ClearView(Context context) {
		this(context,null);
	}
	
	
	
	public ClearView(Context context, AttributeSet attrs, int defStyleAttr) {
		super(context, attrs, defStyleAttr);
		mContext = context;
		setBackgroundColor(Color.argb(0, 255, 255, 255));
		setScreenWH();
		Bitmap bm=createBitmapFromARGB(0x7f000000, mScreenWidth, mScrreenHeight);
		mSevenBitmap=createBitmapFromARGB(0xb3000000, mScreenWidth, mScrreenHeight);
		setCoverBitmap(bm);
		mSavePathList = new ArrayList<Path>();
		mSaveRectList = new ArrayList<RectF>();
		// TODO Auto-generated constructor stub
	}



	public ClearView(Context context, AttributeSet attrs) {
		this(context, attrs,-1);
	}



	private void setScreenWH(){
		DisplayMetrics dm= new DisplayMetrics();
		dm=getResources().getDisplayMetrics();
		mScreenWidth=dm.widthPixels;
		mScrreenHeight=dm.heightPixels;
		//mScreenWidth = mScreenWidth>mScrreenHeight? mScreenWidth:mScrreenHeight;
		//mScrreenHeight = mScreenWidth;
	}
	
	private Bitmap createBitmapFromARGB(int colorARGB,int width,int height){
			int[] argbs=new int[width*height];
			for(int i=0;i<argbs.length;i++){
				argbs[i]=colorARGB;
			}
			return Bitmap.createBitmap(argbs, width, height, Config.ARGB_8888);		
	}
	
	private void setCoverBitmap(Bitmap bm){
		// setting paint  
		mClearPaint=new Paint();
		mClearPaint.setAlpha(0);
		mClearPaint.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.DST_IN));
		mClearPaint.setAntiAlias(true);//juchi 
		mClearPaint.setDither(true);// fang dou dong
		mClearPaint.setStyle(Paint.Style.STROKE);
		mClearPaint.setStrokeJoin(Paint.Join.ROUND);
		mClearPaint.setStrokeCap(Paint.Cap.ROUND);
		mClearPaint.setStrokeWidth(5);   
		// converting bitmap into mutable bitmap
		mBitmap = Bitmap.createBitmap(mScreenWidth, mScrreenHeight, Config.ARGB_8888);
		Canvas mCanvas = new Canvas();
		mCanvas.setBitmap(mBitmap);
		mCanvas.drawBitmap(bm, 0, 0, null);
	}

	@Override
	protected void onDraw(Canvas canvas) {
		// TODO Auto-generated method stub
		if(result){
			canvas.drawBitmap(mSevenBitmap, 0, 0, null);
		}else{
			canvas.drawBitmap(mBitmap, 0, 0, null);
		}
		if(mDrawType == Utils.TYPE_SCRAWL){
        if (mSavePathList != null && mSavePathList.size() > 0) {  
        Iterator<Path> iter = mSavePathList.iterator();  
        while (iter.hasNext()) {  
            canvas.drawPath(iter.next(), mPaint);  
        	}  
			}
		}
		if(mDrawType == Utils.TYPE_LARIAT &&result){
        if (mSavePathList != null && mSavePathList.size() > 0) {  
        Iterator<Path> iter = mSavePathList.iterator();  
        while (iter.hasNext()) {  
            canvas.drawPath(iter.next(), mClearPaint);  
        	}  
			}

		}
		if(mPath !=null){
			canvas.drawPath(mPath, mPaint);  
		}

		super.onDraw(canvas);
	}

	@Override
	protected void onConfigurationChanged(Configuration newConfig) {
		// TODO Auto-generated method stub
		android.util.Log.e("lixiaosong","ClearView onConfigurationChanged");
		setScreenWH();
		android.util.Log.e("lixiaosong","ClearView onConfigurationChanged mScreenWidth:"+mScreenWidth+"  mScrreenHeight:"+mScrreenHeight);
		Bitmap bm=createBitmapFromARGB(0x7f000000, mScreenWidth, mScrreenHeight);
		setCoverBitmap(bm);
		mSevenBitmap=createBitmapFromARGB(0xb3000000, mScreenWidth, mScrreenHeight);
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
		Bitmap bm=createBitmapFromARGB(0x7f000000, mScreenWidth, mScrreenHeight);
		setCoverBitmap(bm);
		mSevenBitmap=createBitmapFromARGB(0xb3000000, mScreenWidth, mScrreenHeight);
		reset();
		invalidate();
		super.onSizeChanged(w, h, oldw, oldh);
	}

	@Override
	public boolean onTouchEvent(MotionEvent event) {
		// TODO Auto-generated method stub
        float  x = event.getX();  
        float y = event.getY(); 
		if(mDrawType == Utils.TYPE_UNKNOWN){
			return true;
		}
          
        switch (event.getAction()) {  
            case MotionEvent.ACTION_DOWN:
				updateTabLayout(false);
				if(mDrawType == Utils.TYPE_LARIAT){
					result = false; 
					reset();
				}
				mPath = new Path(); 
                touch_start(x, y);
                invalidate();  
                break;  
            case MotionEvent.ACTION_MOVE:  
                touch_move(x, y); 
                invalidate();  
                break;  
            case MotionEvent.ACTION_UP:
                touch_up();
                invalidate();
				if(mDrawType == Utils.TYPE_LARIAT && !result){
					reset();
					makeToastTips();
					updateTabLayout(false);
				}else{
                updateTabLayout(true);  
				}
                break;  
        }  
        return true; 
	}
	
	private void makeToastTips(){
		Toast.makeText(mContext,R.string.closed_space_error_text,Toast.LENGTH_SHORT).show();
	}
	
    private void touch_start(float x, float y) {  
        mPath.reset();  
        mPath.moveTo(x, y);  
        mX = x;  
        mY = y;  
    }  
    private void touch_move(float x, float y) {  
        float dx = Math.abs(x - mX);  
        float dy = Math.abs(y - mY);  
        if (dx >= TOUCH_TOLERANCE || dy >= TOUCH_TOLERANCE) {  
            mPath.quadTo(mX, mY, (x + mX)/2, (y + mY)/2); 
            mX = x;  
            mY = y;  
        }  
    }  
    private void touch_up() {  
        mPath.lineTo(mX, mY);
		if(mDrawType == Utils.TYPE_LARIAT){
        	getPoints(mPath,mList.size());
		}
		if(mDrawType == Utils.TYPE_SCRAWL){
			mSavePathList.add(mPath);
			RectF rect = new RectF();
			mPath.computeBounds(rect, true);
			mSaveRectList.add(rect);
			mPath = null;
		}
        // kill this so we don't double draw  
    }
    
    public Bitmap getBitMap(){
		
		Bitmap bmblack=createBitmapFromARGB(0xff000000, mScreenWidth, mScrreenHeight);
		Bitmap mTestBlackBitmap = Bitmap.createBitmap(mScreenWidth, mScrreenHeight, Config.ARGB_8888);
		Canvas mBlackCanvas = new Canvas();
		mBlackCanvas.setBitmap(mTestBlackBitmap);
		mBlackCanvas.drawBitmap(bmblack, 0, 0, null);
		if(mDrawType == Utils.TYPE_SCRAWL){
			mClearPaint.setStrokeWidth(40);
		}
        if (mSavePathList != null && mSavePathList.size() > 0) {  
        Iterator<Path> iter = mSavePathList.iterator();  
        while (iter.hasNext()) {  
            mBlackCanvas.drawPath(iter.next(), mClearPaint);  
        }  
		}
		if(mPath !=null){
			mBlackCanvas.drawPath(mPath, mClearPaint);  
		}
		releaseBitmap();
    	return mTestBlackBitmap;
    }

	private void  releaseBitmap() {
		if(mBitmap !=null && (!mBitmap.isRecycled())){
			mBitmap.recycle();
		}
		if(mSevenBitmap !=null && (!mSevenBitmap.isRecycled())){
			mSevenBitmap.recycle();
		}
	}

	public RectF getRect(){
		RectF mRect = new RectF();
        if (mSaveRectList != null && mSaveRectList.size() > 0) {  
        Iterator<RectF> iter = mSaveRectList.iterator();
		boolean mFirstRect = true;  
        while (iter.hasNext()) {
			RectF rect = iter.next();
			if(mFirstRect){
				mFirstRect = false;
				mRect.set(rect);
			}else{
				mRect.set(mRect.left >rect.left? rect.left:mRect.left,mRect.top >rect.top? rect.top:mRect.top,mRect.right >rect.right? mRect.right:rect.right,mRect.bottom >rect.bottom? mRect.bottom:rect.bottom);
			}
        } 
	android.util.Log.e("lixiaosong","rect left-->"+mRect.left+" top-->"+mRect.top+" right-->"+mRect.right+"   bottom-->"+mRect.bottom);
			return mRect;
		}
		return null;	
	}

	public int getmDrawType() {
		return mDrawType;
	}



	public void setmDrawType(int mDrawType) {
		this.mDrawType = mDrawType;
		if(mDrawType == Utils.TYPE_SCRAWL){
			initScrawlPaint();
			result = true;
		}else{
			initLariatPaint();		
		}
	}

	private void initScrawlPaint(){
		mPaint=new Paint();
		mPaint.setAlpha(0);
		mPaint.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.DST_IN));
		mPaint.setAntiAlias(true);//juchi 
		mPaint.setDither(true);// fang dou dong
		mPaint.setStyle(Paint.Style.STROKE);
		mPaint.setStrokeJoin(Paint.Join.ROUND);
		mPaint.setStrokeCap(Paint.Cap.ROUND);
		mPaint.setStrokeWidth(40);
	}

	private void initLariatPaint(){
		mPaint=new Paint();
		mPaint.setAlpha(0);
		mPaint.setColor(Color.BLACK); 
		//mPaint.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.DST_IN));
		mPaint.setAntiAlias(true);//juchi 
		mPaint.setDither(true);// fang dou dong
		mPaint.setStyle(Paint.Style.STROKE);
		//mPaint.setStrokeJoin(Paint.Join.ROUND);
		DashPathEffect effects = new DashPathEffect(new float[]{20,10,20,10}, 5);
		mPaint.setPathEffect(effects);
		//mPaint.setStrokeCap(Paint.Cap.ROUND);
		mPaint.setStrokeWidth(5);
	}

	public void setTouchEventListener(TouchEventListener mListener){
		mListenerCallback  =  mListener;
	}

	private void updateTabLayout(boolean visible){
		if(mListenerCallback != null){
				mListenerCallback.updateTabLayout(visible);
		}
	}

	public void reset(){
        if (mSavePathList != null && mSavePathList.size() > 0) {  
            mSavePathList.clear(); 
			mSaveRectList.clear();
            invalidate();
        }
		mPath = null;
		isAttached =false;  
	}

	public void backup(){
        if (mSavePathList != null && mSavePathList.size() > 0) {  
			mSavePathList.remove(mSavePathList.size() - 1);
			mSaveRectList.remove(mSaveRectList.size() - 1);   
            invalidate();
        }  
		mPath = null;
	}

	private void getPoints(Path path, int pNum) {

		PathMeasure pm = new PathMeasure(path, false);
		float length = pm.getLength();
		float distance = 0f;
		float speed = length / length;
		int counter = 0;
		int mPathPointNum = 0;
		Point[] mDrawPoints = new Point[(int) length];
		float[] aCoordinates = new float[2];
		Log.e(TAG,"getPoints length:"+length);
		while ((distance < length) && (counter < (int)length)) {
			pm.getPosTan(distance, aCoordinates, null);
			mDrawPoints[mPathPointNum + counter] = new Point(aCoordinates[0], aCoordinates[1]);
			counter++;
			distance = distance + speed;
			if(sameCheck(mDrawPoints,distance,0,counter-1,speed)){
				result = true;
				break;
			}
		}
		if(result && mDrawPoints.length >0){
			Region re = new Region();
			RectF rect = new RectF();
			mPath.computeBounds(rect, true);
			re.setPath(mPath, new Region((int)rect.left, (int)rect.top, (int)rect.right, (int)rect.bottom));
			
			float startx,starty;
			startx = mDrawPoints[0].x;
			starty = mDrawPoints[0].y;
			 bihePath = new Path();
			for(int i = (int) rect.top;i < (int)rect.bottom;i++){
				 Path onePath = new Path();
				 boolean firstIn = false;
				for(int j = (int) rect.left;j < (int)rect.right;j++){
					
					if(re.contains(j, i)){
						if(!firstIn){
							onePath.moveTo(j, i);
							firstIn = true;
						}else{
							onePath.lineTo(j, i);
						}
					}else if(firstIn){
						firstIn = false;
					}
				}
				if(!onePath.isEmpty()){
					bihePath.addPath(onePath);
				}
				onePath.reset();
			}
			//bihePath = re.getBoundaryPath();
			if(!bihePath.isEmpty()){
				RectF rect1 = new RectF();
				bihePath.computeBounds(rect1, true);
				mSavePathList.add(bihePath);
				mSaveRectList.add(rect1);
				startAnimation();	
			}

			
//			for (int i = 0; i < counter-1; i++) {
//				startx = mDrawPoints[i].x;
//				starty = mDrawPoints[i].y;
//				for (int j = i; j < counter-1; j++) {
//					bihePath.quadTo(startx, starty, mDrawPoints[j].x, mDrawPoints[j].y);
//				}
//				
//			}
//			mBlackCanvas.drawPath(bihePath, mPaint);
//			for (Point point : mDrawPoints) {
//					if(point ==null){
//						Log.e("lixiaosongTest","getPoints point is null");
//						continue;
//					}
//				Log.e("lixiaosongTest","getPoints point.x:"+point.x+" point.y"+point.y);
//			}
		}
		//setPixs(0xff0000ff);
	}

	private void startAnimation(){
		isAttached = true;
		new Thread(this).start();		
	}

	public void stopAnimation(){
		isAttached =false;
	}

	private boolean sameCheck(Point[] points,float distance ,int startIndex, int endIndex,float speed) {
			if(distance <180.0f)
				return false;
			Point first = points[startIndex];
			Point end = points[endIndex];
			if(check2PointsDistance(first,end,speed)){
				mStartIndex = startIndex;
				mEndIndex = endIndex;
				return true;
			}else if((distance - speed >=180.0f) && false){
				if(sameCheck(points,distance-speed,startIndex+1,endIndex,speed)){
					return true;
				}
			}
			return false;
			
		
	}

	private boolean check2PointsDistance(Point point1,Point point2,float speed){
		if(point1 == null || point2 ==null){
			return false;
		}
		float distance = (float) Math.sqrt((point1.x-point2.x)*(point1.x-point2.x)+(point1.y-point2.y)*(point1.y-point2.y));
		if(distance < 15.0f){
			return true;
		}
		return false;
	}

	@Override
	public void run() {
		// TODO Auto-generated method stub
	     while(!Thread.currentThread().isInterrupted() && isAttached){ 
	            try{ 
					if(mPaint!=null){
					start +=step;
android.util.Log.e("lixiaosong","runnable update step start-->"+start);
		DashPathEffect effects = new DashPathEffect(new float[]{20,10,20,10}, start);
		mPaint.setPathEffect(effects);}
	                Thread.sleep(100); 
	            }catch (InterruptedException e) { 
	                // TODO: handle exception  
	                Thread.currentThread().interrupt(); 
	            } 
	            postInvalidate();  //可以直接在线程中更新界面  
	        } 	
	}

	@Override
	protected void onAttachedToWindow() {
		// TODO Auto-generated method stub
		super.onAttachedToWindow();
	}

	@Override
	protected void onDetachedFromWindow() {
		// TODO Auto-generated method stub
		isAttached = false; 
		super.onDetachedFromWindow();
	}


    
    class Point {
    	public float x;
    	public float y;
		public Point(float x2, float y2) {
			super();
			this.x = x2;
			this.y = y2;
		}
    	
    }

}
