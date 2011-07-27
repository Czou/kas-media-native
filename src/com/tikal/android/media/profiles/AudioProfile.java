package com.tikal.android.media.profiles;

import com.tikal.android.media.AudioCodecType;

public enum AudioProfile {
	AMR(MediaQuality.LOW, AudioCodecType.AMR, AudioCodecType.AMR
			.getSupportedBitRate(),
			AudioCodecType.AMR.getSupportedSampleRate(), "amr bit_rate=12200"),
	MP2(
			MediaQuality.HEIGH, AudioCodecType.MP2, AudioCodecType.MP2
					.getSupportedBitRate(), AudioCodecType.MP2
					.getSupportedSampleRate(), "mp2 bit_rate=64000");

	private MediaQuality mediaQuality;
	private AudioCodecType audioCodecType;
	private int bitRate;
	private int sampleRate;
	private String description;

	public MediaQuality getMediaQuality() {
		return mediaQuality;
	}

	public AudioCodecType getAudioCodecType() {
		return audioCodecType;
	}

	public int getBitRate() {
		return bitRate;
	}

	public int getSampleRate() {
		return sampleRate;
	}

	public String getDescription() {
		return description;
	}

	private AudioProfile(MediaQuality mediaQuality,
			AudioCodecType audioCodecType, int bitRate, int sampleRate,
			String description) {
		this.mediaQuality = mediaQuality;
		this.audioCodecType = audioCodecType;
		this.bitRate = bitRate;
		this.sampleRate = sampleRate;
		this.description = description;
	}
	
	public static AudioProfile getAudioProfileFromAudioCodecType(AudioCodecType audioCodecType) {
		if (AMR.audioCodecType.equals(audioCodecType))
			return AMR;
		if (MP2.audioCodecType.equals(audioCodecType))
			return MP2;

		return null;
	}
}
