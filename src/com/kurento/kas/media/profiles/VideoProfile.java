package com.kurento.kas.media.profiles;

import com.kurento.kas.media.VideoCodecType;

public enum VideoProfile {
	// 384000 12
	H263(MediaQuality.LOW, VideoCodecType.H263, 384000, 15, 8, 352, 288,
			"h263p CIF bit_rate=320000"),
	MPEG4(MediaQuality.HEIGH, VideoCodecType.MPEG4, 1500000, 15, 1, 352, 288,
			"mpeg4 CIF bit_rate=1500000");

	private MediaQuality mediaQuality;
	private VideoCodecType videoCodecType;
	private int width;
	private int height;
	private int frameRate;
	private int bitRate;
	private int gopSize;
	private String description;

	public MediaQuality getMediaQuality() {
		return mediaQuality;
	}

	public VideoCodecType getVideoCodecType() {
		return videoCodecType;
	}

	public int getFrameRate() {
		return frameRate;
	}

	public int getBitRate() {
		return bitRate;
	}

	public int getGopSize() {
		return gopSize;
	}

	public int getWidth() {
		return width;
	}

	public int getHeight() {
		return height;
	}

	public String getDescription() {
		return description;
	}

	private VideoProfile(MediaQuality mediaQuality,
			VideoCodecType videoCodecType, int bitRate, int frameRate,
			int gopSize, int width, int height, String description) {
		this.mediaQuality = mediaQuality;
		this.videoCodecType = videoCodecType;
		this.bitRate = bitRate;
		this.frameRate = frameRate;
		this.gopSize = gopSize;
		this.width = width;
		this.height = height;
		this.description = description;
	}

	public static VideoProfile getVideoProfileFromVideoCodecType(
			VideoCodecType videoCodecType) {
		if (H263.videoCodecType.equals(videoCodecType))
			return H263;
		if (MPEG4.videoCodecType.equals(videoCodecType))
			return MPEG4;

		return null;
	}
}
