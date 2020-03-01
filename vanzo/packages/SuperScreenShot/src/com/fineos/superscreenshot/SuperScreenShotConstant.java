package com.fineos.superscreenshot;

/**
 * @ClassName:RainbowConstant
 * @Description:常量类,用于存储本应用内所有公共常量
 * @author:BingWu.Lee
 * @version 1.0
 * @date:2014-3-11
 */
 
public class SuperScreenShotConstant {

	/**
	 * @ClassName:BroadCast
	 * @Description:广播消息常量类
	 * @author:BingWu.Lee
	 * @version 1.0
	 * @date:2014-3-12
	 */
	public interface BroadCast {
		public static final String LONG_SCREEN_SHOT_SHOW = "broad.longscreenshot.show";
		public static final String LONG_SCREEN_SHOT_DISMISS = "broad.longscreenshot.dismiss";
		public static final String HOME_SCREEN_SHOT_SHOW = "broad.homescreenshot.show";
		public static final String HOME_SCREEN_SHOT_DISMISS = "broad.homescreenshot.dismiss";
		public static final String FUNS_SCREEN_SHOT_SHOW = "broad.funsscreenshot.show";
		public static final String FUNS_SCREEN_SHOT_DISMISS = "broad.funsscreenshot.dismiss";
		public static final String NORMAL_SCREEN_SHOT_SHOW = "broad.normalscreenshot.show";
		public static final String NORMAL_SCREEN_SHOT_DISMISS = "broad.normalscreenshot.dismiss";
	}
	/**
	 * @ClassName:Intent
	 * @Description:Intent常量类
	 * @author:BingWu.Lee
	 * @version 1.0
	 * @date:2014-4-10
	 */
	public interface Intent {
		public static final String LONG_SCREEN_SHOT_ACTION = "com.fineos.longss.action.window";
		public static final String HOME_SCREEN_SHOT_ACTION = "com.fineos.home.action.window";
		public static final String FUNS_SCREEN_SHOT_ACTION = "com.fineos.funs.action.window";
		public static final String NORMAL_SCREEN_SHOT_ACTION = "com.fineos.normal.action.window";
	}
	
	/**
	 * @ClassName:Service
	 * @Description:服务常量类
	 * @author:BingWu.Lee
	 * @version 1.0
	 * @date:2014-3-12
	 */
	public interface Service {
		public static final String SUPER_SCREEN_SHOT_VISIBILITY = "super.screen.shot.visibility";
	}

}
