package com.tikal.android.media;

import java.util.ArrayList;

import com.tikal.android.media.exception.CodecNotSupportedException;

/**
 * 
 * @author Miguel París Díaz
 * 
 */
public class VideoCodec {

	public static final int CODEC_ID_H264 = 0;
	public static final int CODEC_ID_MPEG4 = 1;
	public static final int CODEC_ID_H263 = 2;

	public static final String[] CODECS_STRING = { "H264", "MPEG4", "H263" };

	public static final String[] H264_STRINGS = { "H264", "h264" };
	public static final String[] MPEG4_STRINGS = { "MPEG4", "mpeg4", "MP4V-ES",
			"mp4v-es" };
	public static final String[] H263_STRINGS = { "H263", "h263", "H263-1998",
			"h263-1998", "H263-2000", "h263-2000" };

	private static VideoCodec INSTANCE = null;
	private static ArrayList<String> ARRAY_H264_STRINGS;
	private static ArrayList<String> ARRAY_MPEG4_STRINGS;
	private static ArrayList<String> ARRAY_H263_STRINGS;

	private VideoCodec() {
		ARRAY_H264_STRINGS = new ArrayList<String>();
		for (String s : H264_STRINGS)
			ARRAY_H264_STRINGS.add(s);

		ARRAY_MPEG4_STRINGS = new ArrayList<String>();
		for (String s : MPEG4_STRINGS)
			ARRAY_MPEG4_STRINGS.add(s);

		ARRAY_H263_STRINGS = new ArrayList<String>();
		for (String s : H263_STRINGS)
			ARRAY_H263_STRINGS.add(s);
	}

	private synchronized static void createInstance() {
		if (INSTANCE == null) {
			INSTANCE = new VideoCodec();
		}
	}

	public static VideoCodec getInstance() {
		if (INSTANCE == null)
			createInstance();
		return INSTANCE;
	}

	public Object clone() throws CloneNotSupportedException {
		throw new CloneNotSupportedException();
	}

	public int getCodecId(String codecName) throws CodecNotSupportedException {
		if (ARRAY_H264_STRINGS.contains(codecName))
			return CODEC_ID_H264;
		else if (ARRAY_MPEG4_STRINGS.contains(codecName))
			return CODEC_ID_MPEG4;
		else if (ARRAY_H263_STRINGS.contains(codecName))
			return CODEC_ID_H263;

		throw new CodecNotSupportedException("Codec not supported");
	}

}
