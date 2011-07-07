package com.tikal.android.media.rx;

/**
 * 
 * @author Miguel París Díaz
 * 
 */
public class MediaRx {
	public static native int startVideoRx(String sdp_str, VideoRx videoPlayer);
	public static native int stopVideoRx();
	
	public static native int startAudioRx(String sdp_str, AudioRx audioPlayer);
	public static native int stopAudioRx();
	
	static {
		System.loadLibrary("android-media");
	}
	
}
