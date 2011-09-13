package com.kurento.kas.media.tx;

/**
 * <p>
 * Provides static methods that map native media functions.
 * 
 * </p>
 * 
 * @author Miguel París Díaz
 * 
 */
public class MediaTx {

	// VIDEO
	private static native int initVideo(String outfile, int width, int height,
			int frame_rate, int bit_rate, int gop_size, int codecId,
			int payload_type, String presetFile);

	public static int initVideo(VideoInfoTx videoInfoTx) {
		return initVideo(videoInfoTx.getOut(), videoInfoTx.getVideoProfile()
				.getWidth(), videoInfoTx.getVideoProfile().getHeight(),
				videoInfoTx.getVideoProfile().getFrameRate(), videoInfoTx
						.getVideoProfile().getGopSize(), videoInfoTx
						.getVideoProfile().getBitRate(), videoInfoTx
						.getVideoProfile().getVideoCodecType().getCodecID(),
				videoInfoTx.getPayloadType(), "");
	}

	public static native int putVideoFrame(byte[] frame, int width, int height);

	public static native int finishVideo();

	// AUDIO
	/**
	 * Returns the frame size used to encode
	 */
	private static native int initAudio(String outfile, int codec_id,
			int sample_rate, int bit_rate, int payload_type);

	public static int initAudio(AudioInfoTx audioInfoTx) {
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
