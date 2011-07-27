package com.tikal.android.media;

import java.util.ArrayList;

import com.tikal.android.media.exception.CodecNotSupportedException;

/**
 * 
 * @author Miguel París Díaz
 * 
 */
public enum VideoCodecType {
	H264(0, new String[]{ "H264", "h264" }),
	MPEG4(1, new String[]{ "MPEG4", "mpeg4", "MP4V-ES", "mp4v-es" }),
	H263(2, new String[]{ "H263", "h263", "H263-1998", "h263-1998", "H263-2000", "h263-2000" });
	
	private int codecID;
	private ArrayList<String> arrayCodecStrings;
	
	//private int[] supportedBitrates
	//private int[] supportedFramerates

	private VideoCodecType(int codecID, String[] codecStrings) {
		this.codecID = codecID;
		arrayCodecStrings = new ArrayList<String>();
		for (String s : codecStrings)
			arrayCodecStrings.add(s);
	}

	public int getCodecID() {
		return codecID;
	}

	public ArrayList<String> getArrayCodecStrings() {
		return arrayCodecStrings;
	}
	
	public static int getCodecIdFromName(String codecName) throws CodecNotSupportedException {
		if (H264.arrayCodecStrings.contains(codecName))
			return H264.codecID;
		else if (MPEG4.arrayCodecStrings.contains(codecName))
			return MPEG4.codecID;
		else if (H263.arrayCodecStrings.contains(codecName))
			return H263.codecID;

		throw new CodecNotSupportedException("Codec not supported");
	}
	
	public static VideoCodecType getCodecTypeFromName(String codecName) throws CodecNotSupportedException {
		if (H264.arrayCodecStrings.contains(codecName))
			return H264;
		else if (MPEG4.arrayCodecStrings.contains(codecName))
			return MPEG4;
		else if (H263.arrayCodecStrings.contains(codecName))
			return H263;

		throw new CodecNotSupportedException("Codec not supported");
	}
	
}
