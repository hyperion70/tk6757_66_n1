package com.fineos.superscreenshot;

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.Rect;
import android.view.View;
import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.animation.AnimatorSet;
import android.animation.ValueAnimator;
import android.animation.ValueAnimator.AnimatorUpdateListener;
import android.app.PendingIntent;
import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.ColorMatrix;
import android.graphics.ColorMatrixColorFilter;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.PixelFormat;
import android.graphics.PointF;
import android.media.MediaActionSound;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Environment;
import android.os.Process;
import android.provider.MediaStore;
import android.util.DisplayMetrics;
import android.view.Display;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceControl;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.view.animation.Interpolator;
import android.widget.ImageView;
import java.io.File;
import java.io.InputStream;
import java.io.OutputStream;
import java.text.SimpleDateFormat;
import java.util.Date;
import android.content.pm.ResolveInfo;  
import android.content.pm.ActivityInfo;  
import android.os.Parcelable;
import java.util.List;
import java.util.ArrayList;
import com.mediatek.storage.StorageManagerEx;



public class SuperScreenShotTools  {

private AsyncTask<SaveImageInBackgroundData, Void, SaveImageInBackgroundData> mSaveInBgTask;


class SaveImageInBackgroundTask extends AsyncTask<SaveImageInBackgroundData, Void,
        SaveImageInBackgroundData> {
    private static final String TAG = "SaveImageInBackgroundTask";
    private static final String SCREENSHOTS_DIR_NAME = "Screenshots";
	private static final String SCREENSHOTS_DIR_NAME_LONG = "longs";
	private static final String SCREENSHOTS_DIR_NAME_FUN = "funs";
	private static final String SCREENSHOTS_DIR_NAME_VEDIO = "videos";
    private String SCREENSHOT_FILE_NAME_TEMPLATE = "Screenshot_%s.png";
    private String SCREENSHOT_SHARE_SUBJECT_TEMPLATE = "Screenshot (%s)";
    private final File mScreenshotDir;
    private final String mImageFileName;
    private final String mImageFilePath;
    private final long mImageTime;
    private int mImageWidth;
    private int mImageHeight;
    private boolean mTickerAddSpace;
	private static final int LONG = 1;
	private static final int FUN = 0;
	private static final int VIDEO =2;
	

    SaveImageInBackgroundTask (Context context, SaveImageInBackgroundData data , int flag) {

		android.util.Log.e("liukai3","----------SaveImageInBackgroundTask----------");

		
        // Prepare all the output metadata
        mImageTime = System.currentTimeMillis();
        String imageDate = new SimpleDateFormat("yyyy-MM-dd-HH-mm-ss").format(new Date(mImageTime));
        mImageFileName = String.format(SCREENSHOT_FILE_NAME_TEMPLATE, imageDate);
        if(FUN == flag){
        	mScreenshotDir = new File(StorageManagerEx.getDefaultPath() + '/' + Environment.DIRECTORY_PICTURES,
 	               SCREENSHOTS_DIR_NAME+'/'+SCREENSHOTS_DIR_NAME_FUN);
        }else if(LONG == flag){
			mScreenshotDir = new File(StorageManagerEx.getDefaultPath() + '/' + Environment.DIRECTORY_PICTURES,
							   SCREENSHOTS_DIR_NAME+'/'+SCREENSHOTS_DIR_NAME_LONG);
		}else if(VIDEO == flag){
			mScreenshotDir = new File(StorageManagerEx.getDefaultPath() + '/' + Environment.DIRECTORY_PICTURES,
							   SCREENSHOTS_DIR_NAME+'/'+SCREENSHOTS_DIR_NAME_VEDIO);
		}else{
        	 mScreenshotDir = new File(Environment.getExternalStoragePublicDirectory(
            Environment.DIRECTORY_PICTURES), SCREENSHOTS_DIR_NAME);
        }
        
        mImageFilePath = new File(mScreenshotDir, mImageFileName).getAbsolutePath();
		android.util.Log.e("liukai3","mImageFilePath is : "+mImageFilePath);
    }

    @Override
    protected SaveImageInBackgroundData doInBackground(SaveImageInBackgroundData... params) {

	android.util.Log.e("liukai3","----------doInBackground----------");
    
        if (params.length != 1) return null;
        if (isCancelled()) {
            params[0].clearImage();
            params[0].clearContext();
            return null;
        }

        // By default, AsyncTask sets the worker thread to have background thread priority, so bump
        // it back up so that we save a little quicker.
        Process.setThreadPriority(Process.THREAD_PRIORITY_FOREGROUND);

        Context context = params[0].context;
        Bitmap image = params[0].image;
		mImageWidth = image.getWidth();
        mImageHeight = image.getHeight();
		Resources r = context.getResources();

        try {
            // Create screenshot directory if it doesn't exist
            mScreenshotDir.mkdirs();

            // media provider uses seconds for DATE_MODIFIED and DATE_ADDED, but milliseconds
            // for DATE_TAKEN
            long dateSeconds = mImageTime / 1000;

            // Save the screenshot to the MediaStore
            ContentValues values = new ContentValues();
            ContentResolver resolver = context.getContentResolver();
            values.put(MediaStore.Images.ImageColumns.DATA, mImageFilePath);
            values.put(MediaStore.Images.ImageColumns.TITLE, mImageFileName);
            values.put(MediaStore.Images.ImageColumns.DISPLAY_NAME, mImageFileName);
            values.put(MediaStore.Images.ImageColumns.DATE_TAKEN, mImageTime);
            values.put(MediaStore.Images.ImageColumns.DATE_ADDED, dateSeconds);
            values.put(MediaStore.Images.ImageColumns.DATE_MODIFIED, dateSeconds);
            values.put(MediaStore.Images.ImageColumns.MIME_TYPE, "image/png");
            values.put(MediaStore.Images.ImageColumns.WIDTH, mImageWidth);
            values.put(MediaStore.Images.ImageColumns.HEIGHT, mImageHeight);
            Uri uri = resolver.insert(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, values);

            OutputStream out = resolver.openOutputStream(uri);
            boolean bCompressOK = image.compress(Bitmap.CompressFormat.PNG, 100, out);
            out.flush();
            out.close();
            /// M: [ALPS00800619] Handle Compress Fail Case.
            if (!bCompressOK) {
                resolver.delete(uri, null, null);
                params[0].result = 1;
                return params[0];
            }

            // update file size in the database
            values.clear();

            /// M: FOR ALPS00266037 & ALPS00289039 pic taken by phone shown wrong on cumputer. @{
            InputStream inputStream = resolver.openInputStream(uri);
            int size = inputStream.available();
            inputStream.close();
            values.put(MediaStore.Images.ImageColumns.SIZE, size);

            // values.put(MediaStore.Images.ImageColumns.SIZE, new File(mImageFilePath).length());
            uri = uri.buildUpon().appendQueryParameter("notifyMtp", "1").build();
            resolver.update(uri, values, null, null);
            /// M: FOR ALPS00266037 & ALPS00289039. @}

            params[0].image = null;
            params[0].result = 0;
        } catch (Exception e) {
            // IOException/UnsupportedOperationException may be thrown if external storage is not
            // mounted
			e.printStackTrace(); 
			android.util.Log.e("liukai3","----------SaveImage is Exception----------::",e);

			
            params[0].clearImage();
            params[0].result = 1;
        }

        // Recycle the bitmap data
        if (image != null) {
            image.recycle();
        }

        return params[0];
    }

    @Override
    protected void onPostExecute(SaveImageInBackgroundData params) {
      //do nothing
    }
	
}


   public void saveImage(Context mContext , Bitmap mBitmap , int flag) {
   		android.util.Log.e("liukai3","----------saveImage----------");
        SaveImageInBackgroundData data = new SaveImageInBackgroundData();
        data.context = mContext;
        data.image = mBitmap;
        if (mSaveInBgTask != null) {
            mSaveInBgTask.cancel(false);
        }
        mSaveInBgTask = new SaveImageInBackgroundTask(mContext, data, flag).execute(data);
    }

	class SaveImageInBackgroundData {
		Context context;
		Bitmap image;
		int result;
	
		void clearImage() {
			image = null;
		}
		void clearContext() {
			context = null;
		}
	}	
}
