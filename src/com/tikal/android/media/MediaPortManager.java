package com.tikal.android.media;

public class MediaPortManager {

	public static native int takeAudioLocalPort();
	public static native void releaseAudioLocalPort();
	
	public static native int takeVideoLocalPort();
	public static native void releaseVideoLocalPort();
	
	static {
		System.loadLibrary("android-media");
	}
	
}
