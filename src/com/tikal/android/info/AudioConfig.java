package com.tikal.android.info;

import java.util.ArrayList;

import com.tikal.android.media.AudioCodecType;

public class AudioConfig {

	private ArrayList<AudioCodecType> audioCodecs;

	public ArrayList<AudioCodecType> getAudioCodecs() {
		return audioCodecs;
	}

	public AudioConfig(ArrayList<AudioCodecType> audioCodecs) {
		this.audioCodecs = audioCodecs;
	}
}
