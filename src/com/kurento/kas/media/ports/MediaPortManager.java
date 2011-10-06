package com.kurento.kas.media.ports;

public class MediaPortManager {

	public static native int takeAudioLocalPort();
	public static native int takeAudioLocalPort(int audioPort);
	public static native void releaseAudioLocalPort();
	
	public static native int takeVideoLocalPort();
	public static native int takeVideoLocalPort(int videoPort);
	public static native void releaseVideoLocalPort();
	
	static {
		System.loadLibrary("android-media");
	}
	
}
