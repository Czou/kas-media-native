package com.tikal.android.mscontrol.join;

import com.tikal.mscontrol.join.JoinableContainer;
import com.tikal.mscontrol.join.JoinableStream;

public abstract class JoinableStreamBase extends JoinableImpl implements JoinableStream {

	private com.tikal.mscontrol.join.JoinableContainer container;
	private StreamType type;
	
	protected JoinableStreamBase(JoinableContainer container, StreamType type) {
		this.container = container;
		this.type = type;
	}
	
	@Override
	public JoinableContainer getContainer() {
		return this.container;
	}

	@Override
	public StreamType getType() {
		return this.type;
	}

}
