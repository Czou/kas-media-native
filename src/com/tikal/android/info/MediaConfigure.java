package com.tikal.android.info;

import com.tikal.android.media.profiles.MediaQuality;

public class MediaConfigure {

	private AudioConfig audioConfig;
	private VideoConfig videoConfig;

	public AudioConfig getAudioConfig() {
		return audioConfig;
	}

	public VideoConfig getVideoConfig() {
		return videoConfig;
	}

	public MediaConfigure(AudioConfig audioConfig, VideoConfig videoConfig,
			MediaQuality mediaQuality) {
		this.audioConfig = audioConfig;
		this.videoConfig = videoConfig;
	}

}
