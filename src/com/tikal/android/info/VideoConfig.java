package com.tikal.android.info;

import java.util.ArrayList;

import com.tikal.android.media.VideoCodecType;

public class VideoConfig {

	private ArrayList<VideoCodecType> videoCodecs;

	public ArrayList<VideoCodecType> getVideoCodecs() {
		return videoCodecs;
	}

	public VideoConfig(ArrayList<VideoCodecType> videoCodecs) {
		this.videoCodecs = videoCodecs;
	}

}
