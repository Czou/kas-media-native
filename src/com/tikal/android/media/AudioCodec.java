package com.tikal.android.media;

import java.util.ArrayList;

import com.tikal.android.media.exception.CodecNotSupportedException;

/**
 * 
 * @author Miguel París Díaz
 * 
 */
public class AudioCodec {

	//Codecs ids
	public static final int CODEC_ID_AMR= 0;
	public static final int CODEC_ID_MP2 = 1;	//MPA
	public static final int CODEC_ID_AAC = 2;
	
	//Sample rates
	public static final int AMR_SAMPLE_RATE = 8000;
	public static final int MP2_SAMPLE_RATE = 44100;
	public static final int[] SAMPLE_RATES = { AMR_SAMPLE_RATE, MP2_SAMPLE_RATE };
	
	//Bit rates
	public static final int AMR_BIT_RATE = 12200;
	public static final int MP2_BIT_RATE = 64000;
	public static final int[] BIT_RATES = { AMR_BIT_RATE, MP2_BIT_RATE };
	
	public static final String[] CODECS_STRING = { "AMR", "MP2", "AAC" };

	public static final String[] AMR_STRINGS = { "AMR", "AMR-NB" };
	public static final String[] MP2_STRINGS = { "MP2", "MPA"};
	public static final String[] AAC_STRINGS = { "AAC"};
	
	private static AudioCodec INSTANCE = null;
	private static ArrayList<String> ARRAY_AMR_STRINGS;
	private static ArrayList<String> ARRAY_MP2_STRINGS;
	private static ArrayList<String> ARRAY_AAC_STRINGS;
	

	private AudioCodec() {
		ARRAY_AMR_STRINGS = new ArrayList<String>();
		for(String s : AMR_STRINGS)
			ARRAY_AMR_STRINGS.add(s);
		
		ARRAY_MP2_STRINGS = new ArrayList<String>();
		for(String s : MP2_STRINGS)
			ARRAY_MP2_STRINGS.add(s);
		
		ARRAY_AAC_STRINGS = new ArrayList<String>();
		for(String s : AAC_STRINGS)
			ARRAY_AAC_STRINGS.add(s);
	}

	private synchronized static void createInstance() {
		if (INSTANCE == null) {
			INSTANCE = new AudioCodec();
		}
	}

	public static AudioCodec getInstance() {
		if (INSTANCE == null)
			createInstance();
		return INSTANCE;
	}

	public Object clone() throws CloneNotSupportedException {
		throw new CloneNotSupportedException();
	}
	
	
	public int getCodecId(String codecName) throws CodecNotSupportedException {
		if (ARRAY_AMR_STRINGS.contains(codecName))
			return CODEC_ID_AMR;
		else if (ARRAY_MP2_STRINGS.contains(codecName))
			return CODEC_ID_MP2;
		else if (ARRAY_AAC_STRINGS.contains(codecName))
			return CODEC_ID_AAC;

		throw new CodecNotSupportedException("Codec not supported");
	}
	
	public int getSampleRate(int codecId) {
		return SAMPLE_RATES[codecId];
	}
	
	public int getBitRate(int codecId) {
		return BIT_RATES[codecId];
	}

}
