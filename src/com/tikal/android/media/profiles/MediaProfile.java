package com.tikal.android.media.profiles;

public enum MediaProfile {
	LOW_QUALITY(AudioProfile.LOW_QUALITY, VideoProfile.LOW_QUALITY),
	HIGH_QUALITY(AudioProfile.HIGH_QUALITY, VideoProfile.HIGH_QUALITY);

	private AudioProfile audioProfile;
	private VideoProfile videoProfile;

	public AudioProfile getAudioProfile() {
		return audioProfile;
	}

	public VideoProfile getVideoProfile() {
		return videoProfile;
	}

	private MediaProfile(AudioProfile audioProfile, VideoProfile videoProfile) {
		this.audioProfile = audioProfile;
		this.videoProfile = videoProfile;
	}

}
