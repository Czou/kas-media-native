package com.tikal.android.mscontrol;

import java.net.InetAddress;
import java.util.ArrayList;

import com.tikal.android.media.AudioCodecType;
import com.tikal.android.media.VideoCodecType;
import com.tikal.android.mscontrol.networkconnection.ConnectionType;
import com.tikal.mscontrol.Configuration;
import com.tikal.mscontrol.MediaSession;

public class MediaSessionConfig implements Configuration<MediaSession> {

	private ArrayList<AudioCodecType> audioCodecs;
	private ArrayList<VideoCodecType> videoCodecs;

	private InetAddress localAddress;
	private ConnectionType connectionType;

	public MediaSessionConfig(ArrayList<AudioCodecType> audioCodecs,
			ArrayList<VideoCodecType> videoCodecs, InetAddress localAddress,
			ConnectionType connectionType) {
		this.audioCodecs = audioCodecs;
		this.videoCodecs = videoCodecs;
		this.connectionType = connectionType;
		this.localAddress = localAddress;
	}

}
