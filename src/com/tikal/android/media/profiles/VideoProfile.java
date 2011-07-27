package com.tikal.android.media.profiles;

import com.tikal.android.media.VideoCodec;

public enum VideoProfile {
	LOW_QUALITY(VideoCodec.CODEC_ID_H263, 32000, 15, 352, 288, "h263p CIF bit_rate=320000"),
	HIGH_QUALITY(VideoCodec.CODEC_ID_MPEG4, 1500000, 15, 352, 288, "mpeg4 CIF bit_rate=1500000");

	private int codecID;
	private int bitRate;
	private int frameRate;
	private int width;
	private int height;
	private String description;

	private VideoProfile(int codecID, int bitRate, int frameRate, int width,
			int height, String description) {
		this.codecID = codecID;
		this.bitRate = bitRate;
		this.frameRate = frameRate;
		this.width = width;
		this.height = height;
		this.description = description;
	}
}
