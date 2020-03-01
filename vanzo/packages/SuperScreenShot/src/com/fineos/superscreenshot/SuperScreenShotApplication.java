package com.fineos.superscreenshot;

import android.animation.Animator;
import android.animation.Animator.AnimatorListener;
import android.animation.AnimatorSet;
import android.animation.ObjectAnimator;
import android.app.Application;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.graphics.PixelFormat;
import android.os.IBinder;
import android.provider.Settings;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnKeyListener;
import android.view.View.OnTouchListener;
import android.view.Window;
import android.view.WindowManager;
import android.view.WindowManager.LayoutParams;
import android.view.animation.Animation;
import android.view.animation.TranslateAnimation;
import android.widget.ImageView;
import android.widget.Toast;
import android.content.Intent;
import com.fineos.superscreenshot.funscreenshot.ClearService;
import java.lang.reflect.Field;
import android.widget.TextView;
import android.view.KeyEvent;
import android.os.Bundle;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.res.Configuration;
import android.os.Bundle;
import android.util.DisplayMetrics;
import java.util.Locale;
/**
 * @ClassName:SuperScreenShotApplication
 * @Description:应用Application类，用于公共资源初始化等动作
 * @author:liukai
 * @version 1.0
 * @date:2015-1-27
 */

public class SuperScreenShotApplication extends Application  implements OnClickListener {
 
    private WindowManager wm;
    private WindowManager.LayoutParams wmlp;
    private View view;
    private TextView long_button ,rectangle_button;
    private TextView lariat_button,scrawl_button;
    private TextView home_button;
    private AnimatorSet mShowAnimatorSet, mHideAnimatorSet;
    private boolean isDisplay;
    private boolean funShowAsFullScreen= true;
    private float mTouchStartX;
    private float mTouchStartY;
    private float x;
    private float y;
    private static int statusBarHeight = 0;
    boolean initViewPlace = false;
    private boolean isShow = false;
    
    private void appInit(){
    	wm = (WindowManager) getApplicationContext().getSystemService(
				Context.WINDOW_SERVICE);
    	wmlp = new WindowManager.LayoutParams();
    	wmlp.type = LayoutParams.TYPE_PHONE;
    	// 设置图片格式，效果为背景透明
    	wmlp.format = PixelFormat.RGBA_8888;
    	// 设置浮动窗口不可聚焦
    	wmlp.flags = LayoutParams.FLAG_NOT_TOUCH_MODAL|LayoutParams.FLAG_WATCH_OUTSIDE_TOUCH;
    	// wmlp.gravity = Gravity.LEFT | Gravity.TOP;
    	wmlp.x = 0;
    	wmlp.y = 0;
		int width = dp2px(getApplicationContext(),250);
		int height = dp2px(getApplicationContext(),250);
		
    	wmlp.width = width;//500;//WindowManager.LayoutParams.WRAP_CONTENT;
    	wmlp.height = height;//WindowManager.LayoutParams.WRAP_CONTENT;
    	wmlp.windowAnimations =  R.style.windowAnimOne;              //anim 
    	view = getView();
    	initViewPlace = false;
		view.setOnTouchListener(new OnTouchListener() {
			public boolean onTouch(View v, MotionEvent event) {
				switch (event.getAction()) {
				case MotionEvent.ACTION_DOWN:
					if (!initViewPlace) {
						initViewPlace = true;
						//获取初始位置
						mTouchStartX = event.getRawX();
						mTouchStartY = event.getRawY();
						x = event.getRawX();
						y = event.getRawY(); 
						android.util.Log.e("liukai3","startX:" + mTouchStartX + "=>startY:" + mTouchStartY+"X:" + x + "=>Y:" + y);
					}else {
						//根据上次手指离开的位置与此次点击的位置进行初始位置微调
						mTouchStartX += (event.getRawX() -x);
						mTouchStartY += (event.getRawY() -y);
					}					
					break;
				case MotionEvent.ACTION_MOVE:
					// 获取相对屏幕的坐标，以屏幕左上角为原点
					x = event.getRawX();
					y = event.getRawY(); 
					updateViewPosition();
					break;
				case MotionEvent.ACTION_UP:
					break;
				case MotionEvent.ACTION_OUTSIDE:
					removeView();
					Settings.System.putInt(getApplicationContext().getContentResolver(),"super_screen_shot_is_show",0);
					break;
				}
				return true;
			}
		});

		view.setFocusableInTouchMode(true);
		view.setOnKeyListener(new OnKeyListener() {            
            @Override
            public boolean onKey(View v, int keyCode, KeyEvent event) {
                removeView();
				  Settings.System.putInt(getApplicationContext().getContentResolver(),"super_screen_shot_is_show",0);
                return false;
            }
        });

		
    }



	 public static int dp2px(Context context, float dpValue) {
        final float scale = context.getResources().getDisplayMetrics().density;
        return (int) (dpValue * scale + 0.5f);
    }
 
    private void updateViewPosition(){
    	wmlp.x=(int)( x-mTouchStartX);
    	wmlp.y=(int) (y-mTouchStartY);
	    wm.updateViewLayout(view, wmlp);	    
	 }
             
    private View getView() {
    	if (isDisplay) {
			return view ;
		}
    	if (view == null) {
    		LayoutInflater li = (LayoutInflater) getApplicationContext().getSystemService(LAYOUT_INFLATER_SERVICE);
    		view = li.inflate(R.layout.home_float_window_service, null);
	        home_button = (TextView) view.findViewById(R.id.home_button);
//    		home_button.setOnClickListener(this);
                Locale locale = getResources().getConfiguration().locale;
                String language = locale.getLanguage();

    		lariat_button = (TextView) view.findViewById(R.id.lariat_button);
		scrawl_button = (TextView) view.findViewById(R.id.scrawl_button);
    		long_button = (TextView) view.findViewById(R.id.long_button);
    		rectangle_button = (TextView) view.findViewById(R.id.rectangle_button);

                lariat_button.setOnClickListener(this);
        	scrawl_button.setOnClickListener(this);
        	long_button.setOnClickListener(this);
    		rectangle_button.setOnClickListener(this);
    	}
    		isDisplay = true;
    		return view;    
    }
    
	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		int id = v.getId();
		switch(id){
			/*case R.id.home_button:
					removeView();
					Settings.System.putInt(getApplicationContext().getContentResolver(),"super_screen_shot_is_show",0);
					break;*/
			case R.id.rectangle_button:
					/*Intent intentNormal = new Intent();
					intentNormal.setAction(SuperScreenShotConstant.BroadCast.NORMAL_SCREEN_SHOT_SHOW);
				 	SuperScreenShotApplication.this.sendBroadcast(intentNormal);
				 	removeView();*/
				 	
					Intent intentRectangle = new Intent();
					Bundle mbundle1 = new Bundle();
    				mbundle1.putString("extra", "RECTANGULAR");
					intentRectangle.putExtras(mbundle1);
					intentRectangle.setClass(SuperScreenShotApplication.this,ClearService.class);
					startService(intentRectangle);
					removeView();
					
					break;
			case R.id.long_button:
				 	Intent intent = new Intent();
				 	intent.setAction(SuperScreenShotConstant.BroadCast.LONG_SCREEN_SHOT_SHOW);
				 	SuperScreenShotApplication.this.sendBroadcast(intent);
				 	removeView();
					break;
			case R.id.lariat_button:
					//removeView();	
				 	Intent intent1 = new Intent();
					if(funShowAsFullScreen){
						Bundle mbundle = new Bundle();
        				mbundle.putString("extra", "LASSO");
						intent1.putExtras(mbundle);
						intent1.setClass(SuperScreenShotApplication.this,ClearService.class);
						startService(intent1);
					}else{
				 	intent1.setAction(SuperScreenShotConstant.BroadCast.FUNS_SCREEN_SHOT_SHOW);
				 	SuperScreenShotApplication.this.sendBroadcast(intent1);
					}
					removeView();
					break;


			case R.id.scrawl_button:
					//removeView();
					Intent intentScrawl = new Intent();
					Bundle mbundleScrawl = new Bundle();
    				mbundleScrawl.putString("extra", "GRAFFITI");
					intentScrawl.putExtras(mbundleScrawl);
					intentScrawl.setClass(SuperScreenShotApplication.this,ClearService.class);
					startService(intentScrawl);
					removeView();
					break;

					
			default:
					break;
		}
	}
	

	
	@Override
	public void onCreate() {
		android.util.Log.e("liukai3","appliaction onCreate----");
		Settings.System.putInt(getApplicationContext().getContentResolver(),"super_screen_shot_is_show",0);
                registerReceiver();
		super.onCreate();
		appInit();
	}

        private void registerReceiver(){
            IntentFilter myIntentFilter = new IntentFilter();
            myIntentFilter.addAction(Intent.ACTION_LOCALE_CHANGED);
            this.registerReceiver(mBroadcastReceiver, myIntentFilter);
        }

        private BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver(){
            @Override
                public void onReceive(Context context, Intent intent) {
                    String action = intent.getAction();
                    if(Intent.ACTION_LOCALE_CHANGED.equals(action)){
                        Locale locale = getResources().getConfiguration().locale;
                        String language = locale.getLanguage();
                        Log.d("houcongxi","收到语言变换广播"+"----------"+language);
                        setLanguage(language);
                    }
                }
        };
        private void setLanguage(String language) {
            try {
                Configuration config = getResources().getConfiguration();
                DisplayMetrics dm = getResources().getDisplayMetrics();

                if (language.endsWith("zh")) {
                   // config.locale = Locale.SIMPLIFIED_CHINESE;
                    Log.d("houcongxi","--------中文-----------");
                    home_button.setText("魔法截屏");
                    lariat_button.setText("套索截屏");
                    scrawl_button.setText("涂鸦截屏");
                     long_button.setText("长截屏");
                    rectangle_button.setText("矩形截屏");
                } else {
                    home_button.setText("Magic screenshots");
                    lariat_button.setText("Lasso");
                    scrawl_button.setText("Graffiti");
                    long_button.setText("Long screenshots");
                    rectangle_button.setText("Rectangular");
                    //config.locale = Locale.ENGLISH;
                }
                getResources().updateConfiguration(config, dm);

                Log.d("houcongxi",config+"-------------------"+dm);
                SharedPreferences sp = getSharedPreferences("userinfo", 0);
                SharedPreferences.Editor editor=sp.edit();
                editor.putString("locale", language);
                editor.commit();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

	public void addView() {
		Settings.System.putInt(getApplicationContext().getContentResolver(),"super_screen_shot_is_show",1);
		android.util.Log.e("liukai3","appliaction addView----");
		wm.addView(view, wmlp);
		isShow = true;
	}

	public void removeView() {		
		android.util.Log.e("liukai3","appliaction removeView----");
		Settings.System.putInt(getApplicationContext().getContentResolver(),"super_screen_shot_is_show",0);
		if(view!=null && isShow){
			wm.removeView(view);
			isShow = false;
		}
	}
	  
	private int getStatusBarHeight() {
		if (statusBarHeight == 0) {
			try {
				Class c = Class.forName("com.android.internal.R$dimen");
				Object o = c.newInstance();
				Field field = c.getField("status_bar_height");
				int x = (Integer) field.get(o);
				statusBarHeight = getResources().getDimensionPixelSize(x);
				android.util.Log.e("liukai3","statusBarHeight:" + statusBarHeight);
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		return statusBarHeight;
	}
}
