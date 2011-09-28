package com.kurento.kas.media.tx;

import com.kurento.kas.media.profiles.AudioProfile;

/**
 * 
 * @author mparis
 * 
 */
public class AudioInfoTx {

	private AudioProfile audioProfile;
	public int frameSize;
	private int payloadType;
	private String out;

	public AudioProfile getAudioProfile() {
		return audioProfile;
	}

	public void setAudioProfile(AudioProfile audioProfile) {
		this.audioProfile = audioProfile;
	}

	public int getFrameSize() {
		return frameSize;
	}

	public void setFrameSize(int frameSize) {
		this.frameSize = frameSize;
	}

	public int getPayloadType() {
		return payloadType;
	}

	public void setPayloadType(int payloadType) {
		this.payloadType = payloadType;
	}

	public String getOut() {
		return out;
	}

	public void setOut(String out) {
		this.out = out;
	}

	public AudioInfoTx(AudioProfile audioProfile) {
		this.audioProfile = audioProfile;
	}

}