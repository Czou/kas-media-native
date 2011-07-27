package com.tikal.android.media.tx;

/**
 * 
 * @author Miguel París Díaz
 * 
 */
public class MediaTx {

	// VIDEO
	public static native int initVideo(String outfile, int width, int height,
			int frame_rate, int bit_rate, int codecId, int payload_type,
			String presetFile);

	public static native int putVideoFrame(byte[] frame);

	public static native int finishVideo();

	// AUDIO
	/**
	 * Returns the frame size used to encode
	 */
	private static native int initAudio(String outfile, int codec_id,
			int sample_rate, int bit_rate, int payload_type);

	public int initAudio(AudioInfoTx audioInfoTx) {
		return initAudio(audioInfoTx.getOut(), audioInfoTx.getAudioProfile()
				.getAudioCodecType().getCodecID(), audioInfoTx
				.getAudioProfile().getSampleRate(), audioInfoTx
				.getAudioProfile().getBitRate(), audioInfoTx.getPayloadType());
	}

	public static native int putAudioSamples(short[] in_buffer, int in_size);

	public static native int finishAudio();

	static {
		System.loadLibrary("android-media");
	}

}
