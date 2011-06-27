package com.tikal.android.media;

public class MediaPortManager {

	public static native int takeAudioLocalPort();
	public static native int takeVideoLocalPort();
	
	static {
		System.loadLibrary("android-media");
	}
	
}
