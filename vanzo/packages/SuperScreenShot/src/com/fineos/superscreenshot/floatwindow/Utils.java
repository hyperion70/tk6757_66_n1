package com.fineos.superscreenshot.floatwindow;

public class Utils {

	public static final int TYPE_UNKNOWN = -1;
	public static final int TYPE_SCRAWL = 0;
	public static final int TYPE_RECT = 1;
	public static final int TYPE_LARIAT = 2;
	public static boolean isSet(int flags, int flag) {
		return (flags & flag) == flag;
	}
}
