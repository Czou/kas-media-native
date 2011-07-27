package com.tikal.android.mscontrol.join;

import com.tikal.android.media.rx.AudioRx;
import com.tikal.android.media.tx.MediaTx;
import com.tikal.android.mscontrol.mediacomponent.AudioSink;
import com.tikal.mscontrol.MediaControlException;
import com.tikal.mscontrol.join.Joinable;
import com.tikal.mscontrol.join.JoinableContainer;

public class AudioJoinableStreamImpl extends JoinableStreamBase implements
		AudioSink, AudioRx {

	public AudioJoinableStreamImpl(JoinableContainer container, StreamType type) {
		super(container, type);
	}

	@Override
	public void putAudioSamples(short[] in_buffer, int in_size) {
		MediaTx.putAudioSamples(in_buffer, in_size);
	}

	@Override
	public void putAudioSamplesRx(byte[] audio, int length) {
		try {
			for (Joinable j : getJoinees(Direction.SEND))
				if (j instanceof AudioRx)
					((AudioRx) j).putAudioSamplesRx(audio, length);
			for (Joinable j : getJoinees(Direction.DUPLEX))
				if (j instanceof AudioRx)
					((AudioRx) j).putAudioSamplesRx(audio, length);
		} catch (MediaControlException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

}
