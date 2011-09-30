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

import com.kurento.kas.media.codecs.VideoCodecType;

public class VideoProfile {

	public static final int DEFAULT_WIDTH = 352;
	public static final int DEFAULT_HEIGHT = 288;
	public static final int DEFAULT_FRAME_RATE = 15;
	public static final int DEFAULT_GOP_SIZE = 6;

	private VideoCodecType videoCodecType;
	private int width;
	private int height;
	private int frameRate;
	private int bitRate;
	private int gopSize;
	private String description;

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

	public void setWidth(int width) {
		this.width = width;
	}

	public void setHeight(int height) {
		this.height = height;
	}

	public void setFrameRate(int frameRate) {
		this.frameRate = frameRate;
	}

	public void setBitRate(int bitRate) {
		this.bitRate = bitRate;
	}

	public void setGopSize(int gopSize) {
		this.gopSize = gopSize;
	}

	/**
	 * Contructor to asign default falues of:
	 * <ul>
	 * <li>width</li>
	 * <li>height</li>
	 * <li>frameRate</li>
	 * <li>gopSize</li>
	 * </ul>
	 * 
	 * @param videoCodecType
	 * @param bitRate
	 */
	public VideoProfile(VideoCodecType videoCodecType, int bitRate) {
		this.videoCodecType = videoCodecType;
		this.width = DEFAULT_WIDTH;
		this.height = DEFAULT_HEIGHT;
		this.frameRate = DEFAULT_FRAME_RATE;
		this.gopSize = DEFAULT_GOP_SIZE;
		this.bitRate = bitRate;
	}

	public VideoProfile(VideoCodecType videoCodecType, int width, int height,
			int frameRate, int bitRate, int gopSize, String description) {
		this.videoCodecType = videoCodecType;
		this.width = width;
		this.height = height;
		this.frameRate = frameRate;
		this.bitRate = bitRate;
		this.gopSize = gopSize;
		this.description = description;
	}
}
