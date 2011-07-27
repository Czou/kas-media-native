package com.tikal.android.media;

import java.util.ArrayList;

import com.tikal.android.media.exception.CodecNotSupportedException;

/**
 * 
 * @author Miguel París Díaz
 * 
 */
public enum AudioCodecType {
	AMR(0, 8000, 12200, new String[]{ "AMR", "AMR-NB" }),
	MP2(1, 44100, 64000, new String[]{ "MP2", "MPA" }),
	AAC(2, 44100, 64000, new String[]{ "AAC" });
	
	private int codecID;
	private int supportedSampleRate;
	private int supportedBitRate;
	private ArrayList<String> arrayCodecStrings;

	private AudioCodecType(int codecID, int supportedSampleRate, int supportedBitRate, String[] codecStrings) {
		this.codecID = codecID;
		this.supportedSampleRate = supportedSampleRate;
		this.supportedBitRate = supportedBitRate;
		arrayCodecStrings = new ArrayList<String>();
		for (String s : codecStrings)
			arrayCodecStrings.add(s);
	}

	public int getCodecID() {
		return codecID;
	}

	public int getSupportedSampleRate() {
		return supportedSampleRate;
	}

	public int getSupportedBitRate() {
		return supportedBitRate;
	}

	public ArrayList<String> getArrayCodecStrings() {
		return arrayCodecStrings;
	}
	
	public static int getCodecIdFromName(String codecName) throws CodecNotSupportedException {
		if (AMR.arrayCodecStrings.contains(codecName))
			return AMR.codecID;
		else if (MP2.arrayCodecStrings.contains(codecName))
			return MP2.codecID;
		else if (AAC.arrayCodecStrings.contains(codecName))
			return AAC.codecID;

		throw new CodecNotSupportedException("Codec not supported");
	}
	
}
