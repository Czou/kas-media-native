package com.tikal.android.mscontrol.networkconnection;

import java.net.InetAddress;
import java.util.ArrayList;
import java.util.List;
import java.util.Vector;

import javaxt.sdp.SdpException;

import android.util.Log;


import com.tikal.android.info.AudioConfig;
import com.tikal.android.info.ConnectionInfo;
import com.tikal.android.info.MediaConfigure;
import com.tikal.android.info.RTPInfo;
import com.tikal.android.info.VideoConfig;
import com.tikal.android.media.AudioCodecType;
import com.tikal.android.media.MediaPortManager;
import com.tikal.android.media.VideoCodecType;
import com.tikal.android.media.profiles.AudioProfile;
import com.tikal.android.media.profiles.MediaQuality;
import com.tikal.android.media.profiles.VideoProfile;
import com.tikal.android.media.rx.AudioRx;
import com.tikal.android.media.rx.MediaRx;
import com.tikal.android.media.rx.VideoRx;
import com.tikal.android.media.tx.MediaTx;
import com.tikal.android.mscontrol.join.AudioJoinableStreamImpl;
import com.tikal.android.mscontrol.join.VideoJoinableStreamImpl;
import com.tikal.media.format.MediaSpec;
import com.tikal.media.format.PayloadSpec;
import com.tikal.media.format.SessionSpec;
import com.tikal.media.format.SpecTools;
import com.tikal.mscontrol.MediaControlException;
import com.tikal.mscontrol.join.JoinableStream.StreamType;
import com.tikal.sdp.enums.MediaType;

/**
 * 
 * @author Miguel París Díaz
 * 
 */
public class NetworkConnectionImpl extends NetworkConnectionBase {

	private static final long serialVersionUID = 1L;

	public final static String LOG_TAG = "NW";

	private SessionSpec localSessionSpec;
	private SessionSpec remoteSessionSpec;

	private MediaConfigure mediaConfigure;
	private ConnectionInfo connectionInfo;

	ArrayList<AudioProfile> audioProfiles;
	ArrayList<VideoProfile> videoProfiles;

	private static int videoPort = -1;
	private static int audioPort = -1;

	private String sdpVideo = "";
	private String sdpAudio = "";

	// TODO It must be a String
	// public static final String PRESET_FILE = Environment
	// .getExternalStorageDirectory().getAbsolutePath()
	// + "/DCIM/libx264-ffpreset";

	public String getSdpVideo() {
		return sdpVideo;
	}

	public String getSdpAudio() {
		return sdpAudio;
	}

	@Override
	public void setLocalSessionSpec(SessionSpec arg0) {
		this.localSessionSpec = arg0;
	}

	@Override
	public void setRemoteSessionSpec(SessionSpec arg0) {
		this.remoteSessionSpec = arg0;
	}

	protected NetworkConnectionImpl(MediaConfigure mediaConfigure,
			ConnectionInfo connectionInfo) {
		super();

		if (mediaConfigure == null)
			return; // TODO throw exception
		if (connectionInfo == null)
			return; // TODO throw exception

		this.mediaConfigure = mediaConfigure;
		this.connectionInfo = connectionInfo;

		// Process MediaConfigure and determinate media profiles
		audioProfiles = getAudioProfiles(mediaConfigure.getAudioConfig(),
				connectionInfo);
		videoProfiles = getVideoProfiles(mediaConfigure.getVideoConfig(),
				connectionInfo);

		// TODO Only create stream if there are profiles
		this.streams[0] = new VideoJoinableStreamImpl(this, StreamType.video);
		this.streams[1] = new AudioJoinableStreamImpl(this, StreamType.audio);

		Log.d(LOG_TAG, "Take ports");
		if (videoPort == -1)
			videoPort = MediaPortManager.takeVideoLocalPort();
		if (audioPort == -1)
			audioPort = MediaPortManager.takeAudioLocalPort();
	}

	@Override
	public void confirm() throws MediaControlException {
		Log.d(LOG_TAG, "start on NCImpl");
		Log.d(LOG_TAG, "remoteSessionSpec:\n" + remoteSessionSpec);
		Log.d(LOG_TAG, "localSessionSpec:\n" + localSessionSpec);

		if (remoteSessionSpec == null)
			// throw new MediaException("SessionSpec corrupt");
			return;

		RTPInfo rtpInfo = new RTPInfo(remoteSessionSpec);

		if (!SpecTools.filterMediaByType(localSessionSpec, "video")
				.getMediaSpec().isEmpty())
			sdpVideo = SpecTools.filterMediaByType(localSessionSpec, "video")
					.toString();
		if (!SpecTools.filterMediaByType(localSessionSpec, "audio")
				.getMediaSpec().isEmpty())
			sdpAudio = SpecTools.filterMediaByType(localSessionSpec, "audio")
					.toString();

		if (!sdpVideo.equals(""))
			(new VideoRxThread()).start();
		if (!sdpAudio.equals(""))
			(new AudioRxThread()).start();

		if (videoInfo != null) {
			videoInfo.setCodecID(rtpInfo.getVideoCodecId());
			videoInfo.setOut(rtpInfo.getVideoRTPDir());
			videoInfo.setMode(VideoConfig.MODE_SEND_RTP);
			videoInfo.setPayloadType(rtpInfo.getVideoPayloadType());
			int ret = MediaTx.initVideo(videoInfo.getOut(),
					videoInfo.getWidth(), videoInfo.getHeight(), 15, 4000000,
					videoInfo.getCodecID(), videoInfo.getPayloadType(),
					PRESET_FILE);
			if (ret < 0) {
				Log.d(LOG_TAG, "Error in initVideo");
				MediaTx.finishVideo();
			}
		}
		if (audioInfo != null) {
			audioInfo.setCodecID(rtpInfo.getAudioCodecId());
			audioInfo.setOut(rtpInfo.getAudioRTPDir());
			audioInfo.setPayloadType(rtpInfo.getAudioPayloadType());
			AudioCodec ac = AudioCodec.getInstance();
			audioInfo
					.setSample_rate(ac.getSampleRate(rtpInfo.getAudioCodecId()));
			audioInfo.setBit_rate(ac.getBitRate(rtpInfo.getAudioCodecId()));
			audioInfo.setFrameSize(MediaTx.initAudio(audioInfo.getOut(),
					audioInfo.getCodecID(), audioInfo.getSample_rate(),
					audioInfo.getBit_rate(), audioInfo.getPayloadType()));

			if (audioInfo.getFrameSize() < 0) {
				MediaTx.finishAudio();
				return;
			}
		}
	}

	@Override
	public void release() {
		Log.d(LOG_TAG, "release");
		Log.d(LOG_TAG, "finishVideo");
		MediaTx.finishVideo();
		Log.d(LOG_TAG, "stopVideoRx");
		MediaRx.stopVideoRx();

		Log.d(LOG_TAG, "finishAudio");
		MediaTx.finishAudio();
		Log.d(LOG_TAG, "stopAudioRx");
		MediaRx.stopAudioRx();
		Log.d(LOG_TAG, "ALL OK");

		// MediaPortManager.releaseAudioLocalPort();
		// MediaPortManager.releaseVideoLocalPort();
	}

	private void addPayloadSpec(List<PayloadSpec> videoList, String payloadStr,
			MediaType mediaType, int port) {
		try {
			PayloadSpec payload = new PayloadSpec(payloadStr);
			payload.setMediaType(mediaType);
			payload.setPort(port);
			videoList.add(payload);
		} catch (SdpException e) {
			e.printStackTrace();
		}
	}

	@Override
	public SessionSpec generateSessionSpec() {
		Log.d(LOG_TAG, "generateSessionSpec");
		// VIDEO
		List<PayloadSpec> videoList = new Vector<PayloadSpec>();
		if (videoInfo.getSupportedCodecsID()
				.contains(VideoCodec.CODEC_ID_MPEG4))
			addPayloadSpec(videoList, "96 MP4V-ES/90000", MediaType.VIDEO,
					videoPort);
		if (videoInfo.getSupportedCodecsID().contains(VideoCodec.CODEC_ID_H263))
			addPayloadSpec(videoList, "97 H263-1998/90000", MediaType.VIDEO,
					videoPort);
		if (videoInfo.getSupportedCodecsID().contains(VideoCodec.CODEC_ID_H264))
			addPayloadSpec(videoList, "98 H264/90000", MediaType.VIDEO,
					videoPort);

		MediaSpec videoMedia = new MediaSpec();
		videoMedia.setPayloadList(videoList);

		// AUDIO
		List<PayloadSpec> audioList = new Vector<PayloadSpec>();
		if (audioInfo.getSupportedCodecsID().contains(AudioCodec.CODEC_ID_AMR)) {
			PayloadSpec audioPayloadAMR = null;
			try {
				audioPayloadAMR = new PayloadSpec("100 AMR/8000/1");
				audioPayloadAMR.setFormatParams("octet-align=1");
				audioPayloadAMR.setMediaType(MediaType.AUDIO);
				audioPayloadAMR.setPort(audioPort);
			} catch (SdpException e) {
				e.printStackTrace();
			}
			audioList.add(audioPayloadAMR);

		}
		if (audioInfo.getSupportedCodecsID().contains(AudioCodec.CODEC_ID_MP2)) {
			PayloadSpec payloadAudioMP2 = new PayloadSpec();
			payloadAudioMP2.setMediaType(MediaType.AUDIO);
			payloadAudioMP2.setPort(audioPort);
			payloadAudioMP2.setPayload(14);
			audioList.add(payloadAudioMP2);
		}
		if (audioInfo.getSupportedCodecsID().contains(AudioCodec.CODEC_ID_AAC)) {
			PayloadSpec audioPayloadAAC = null;
			try {
				audioPayloadAAC = new PayloadSpec("101 MPEG4-GENERIC/8000/1");
				audioPayloadAAC
						.setFormatParams("profile-level-id=1;mode=AAC-hbr;sizelength=13;indexlength=3;indexdeltalength=3; config=1210");
				audioPayloadAAC.setMediaType(MediaType.AUDIO);
				audioPayloadAAC.setPort(audioPort);
			} catch (SdpException e) {
				e.printStackTrace();
			}
			audioList.add(audioPayloadAAC);
		}

		MediaSpec audioMedia = new MediaSpec();
		audioMedia.setPayloadList(audioList);

		List<MediaSpec> mediaList = new Vector<MediaSpec>();
		mediaList.add(videoMedia);
		mediaList.add(audioMedia);

		SessionSpec session = new SessionSpec();
		session.setMediaSpec(mediaList);

		session.setOriginAddress(getLocalAddress().getHostAddress().toString());
		session.setRemoteHandler("0.0.0.0");
		session.setSessionName("TestSession");

		return session;
	}

	@Override
	public InetAddress getLocalAddress() {
		return connectionInfo.getLocalAddress();
	}

	private class VideoRxThread extends Thread {
		@Override
		public void run() {
			Log.d(LOG_TAG, "startVideoRx");
			MediaRx.startVideoRx(sdpVideo, (VideoRx) streams[0]);
		}
	}

	private class AudioRxThread extends Thread {
		@Override
		public void run() {
			Log.d(LOG_TAG, "startVideoRx");
			MediaRx.startAudioRx(sdpAudio, (AudioRx) streams[1]);
		}
	}

	private ArrayList<AudioProfile> getAudioProfiles(AudioConfig audioConfig,
			ConnectionInfo connectionInfo) {
		if (audioConfig == null)
			return null;

		ArrayList<AudioProfile> audioProfiles = new ArrayList<AudioProfile>(0);
		// Discard phase
		for (AudioProfile ap : AudioProfile.values()) {
			if (MediaQuality.HEIGH.equals(ap.getMediaQuality())
					&& !ConnectionType.WIFI.equals(connectionInfo
							.getConnectionType()))
				continue;
			for (AudioCodecType act : audioConfig.getAudioCodecs()) {
				if (act.equals(ap.getAudioCodecType()))
					audioProfiles.add(ap);
			}
		}

		// Scoring phase
		// TODO

		return audioProfiles;
	}

	private ArrayList<VideoProfile> getVideoProfiles(VideoConfig videoConfig,
			ConnectionInfo connectionInfo) {
		if (videoConfig == null)
			return null;

		ArrayList<VideoProfile> videoProfiles = new ArrayList<VideoProfile>(0);
		// Discard phase
		for (VideoProfile vp : VideoProfile.values()) {
			if (MediaQuality.HEIGH.equals(vp.getMediaQuality())
					&& !ConnectionType.WIFI.equals(connectionInfo
							.getConnectionType()))
				continue;
			for (VideoCodecType vct : videoConfig.getVideoCodecs()) {
				if (vct.equals(vp.getVideoCodecType()))
					videoProfiles.add(vp);
			}
		}

		// Scoring phase
		// TODO

		return videoProfiles;
	}

}
