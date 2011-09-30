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

package com.kurento.kas.media.tx;

import com.kurento.kas.media.profiles.VideoProfile;

/**
 * 
 * @author mparis
 * 
 */
public class VideoInfoTx {

	private VideoProfile videoProfile;
	private int payloadType;
	private String out;

	public VideoProfile getVideoProfile() {
		return videoProfile;
	}

	public void setVideoProfile(VideoProfile videoProfile) {
		this.videoProfile = videoProfile;
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

	public VideoInfoTx(VideoProfile videoProfile) {
		this.videoProfile = videoProfile;
	}

}
