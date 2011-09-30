/*
 * Kurento Android Media: Android Media Library based on FFmpeg.
 * Copyright (C) 2011  Tikal Technologies
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

package com.kurento.kas.media.profiles;

import com.kurento.kas.media.codecs.AudioCodecType;

public enum AudioProfile {
	AMR(AudioCodecType.AMR, AudioCodecType.AMR.getSupportedBitRate(),
			AudioCodecType.AMR.getSupportedSampleRate(), "amr bit_rate=12200"),
	MP2(AudioCodecType.MP2, AudioCodecType.MP2.getSupportedBitRate(),
			AudioCodecType.MP2.getSupportedSampleRate(), "mp2 bit_rate=64000");

	private AudioCodecType audioCodecType;
	private int bitRate;
	private int sampleRate;
	private String description;

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

	private AudioProfile(AudioCodecType audioCodecType, int bitRate,
			int sampleRate, String description) {
		this.audioCodecType = audioCodecType;
		this.bitRate = bitRate;
		this.sampleRate = sampleRate;
		this.description = description;
	}

	public static AudioProfile getAudioProfileFromAudioCodecType(
			AudioCodecType audioCodecType) {
		if (AMR.audioCodecType.equals(audioCodecType))
			return AMR;
		if (MP2.audioCodecType.equals(audioCodecType))
			return MP2;

		return null;
	}
}
