package com.tikal.android.media.profiles;

import com.tikal.android.media.AudioCodec;

public enum AudioProfile {
	LOW_QUALITY(AudioCodec.CODEC_ID_AMR, AudioCodec.AMR_BIT_RATE, AudioCodec.AMR_SAMPLE_RATE, "amr bit_rate=12200"),
	HIGH_QUALITY(AudioCodec.CODEC_ID_MP2, AudioCodec.MP2_BIT_RATE, AudioCodec.MP2_SAMPLE_RATE, "mp2 bit_rate=64000");

	private int codecID;
	private int bitRate;
	private int sampleRate;
	private String description;

	private AudioProfile(int codecID, int bitRate, int sampleRate, String description) {
		this.codecID = codecID;
		this.bitRate = bitRate;
		this.sampleRate = sampleRate;
		this.description = description;
	}
}
