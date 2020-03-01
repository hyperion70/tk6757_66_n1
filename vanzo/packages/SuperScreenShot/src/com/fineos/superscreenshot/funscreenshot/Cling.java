package com.fineos.superscreenshot.funscreenshot;

import android.content.Context;
import android.graphics.drawable.AnimationDrawable;
import android.util.AttributeSet;
import android.view.View;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.TextView;
import com.fineos.superscreenshot.R;
import com.fineos.superscreenshot.floatwindow.Utils;
import android.os.Handler;
import android.util.Log;

public class Cling extends FrameLayout {

	ImageView mImage;
	Button button ;
	TextView mTextView,mRegionText;
	public Cling(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		// TODO Auto-generated constructor stub
	}

	public Cling(Context context, AttributeSet attrs) {
		super(context, attrs);
		// TODO Auto-generated constructor stub
	}

	public Cling(Context context) {
		super(context);
		// TODO Auto-generated constructor stub
	}
	
	
	@Override
	protected void onAttachedToWindow() {
		// TODO Auto-generated method stub
		super.onAttachedToWindow();
		 button = (Button) findViewById(R.id.button);
		 mImage = (ImageView) findViewById(R.id.image);
		 mTextView = (TextView) findViewById(R.id.tips_text);
		 mRegionText = (TextView) findViewById(R.id.region_text_view);
/*		 button.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
			SharedPreferences mClingPreferences = getSharedPreferences(PREFERENCES_NAME, Activity.MODE_PRIVATE);
			SharedPreferences.Editor editor = unReadEventPreferences.edit();
                           editor.putInt(UNREAD_EVENT_MMS,unReadCount);
                           editor.commit();
			}
		});*/
	}

	public void setType(int type){
		if(type == Utils.TYPE_LARIAT){
			mImage.setBackgroundResource(R.anim.lariat_animation_drawable);
			mTextView.setText(R.string.lariat_tips_intr_text);
			}else{
			mImage.setBackgroundResource(R.anim.scrawl_animation_drawable);
			mTextView.setText(R.string.scrawl_tips_intr_text);
		}
	}

	public void startAnimation(){
		if(mImage !=null){
			AnimationDrawable mDrawable = (AnimationDrawable) mImage.getBackground();
			int duration = 0;
			for (int i = 0; i < mDrawable.getNumberOfFrames(); i++) {
            	duration += mDrawable.getDuration(i);
			}
			Handler handler = new Handler();
			handler.postDelayed(new Runnable() {
            	public void run() {
						android.util.Log.e("lixiaosong","start Amination end duration:");
                        mRegionText.setVisibility(View.VISIBLE);                        //另外一个动画
						button.setVisibility(View.VISIBLE);  
            		}
            	}, duration);
			android.util.Log.e("lixiaosong","start Amination start duration:"+duration);
			mDrawable.setOneShot(true);
			mDrawable.start();	
		}
	}

	public void setButtonClickListener(View.OnClickListener  listener){
		if(button !=null){
			button.setOnClickListener(listener);
		}
	}


	

}
