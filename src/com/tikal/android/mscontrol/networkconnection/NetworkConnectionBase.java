package com.tikal.android.mscontrol.networkconnection;

import java.net.InetAddress;

import com.tikal.android.mscontrol.join.JoinableContainerImpl;
import com.tikal.media.format.SessionSpec;
import com.tikal.mscontrol.MediaControlException;
import com.tikal.mscontrol.networkconnection.NetworkConnection;
import com.tikal.mscontrol.networkconnection.SdpPortManager;


public abstract class NetworkConnectionBase extends JoinableContainerImpl implements
		NetworkConnection {
	
	protected SdpPortManager sdpPortManager;

	protected NetworkConnectionBase() {
		sdpPortManager = new SdpPortManagerImpl(this);
	}

	@Override
	public void confirm() throws MediaControlException {
		// TODO Auto-generated method stub

	}
	
	@Override
	public void release() {
		// TODO Auto-generated method stub

	}
	
	@Override
	public SdpPortManager getSdpPortManager() throws MediaControlException {
		return sdpPortManager;
	}

	/**
	 * Gets a session template copy with all medias capacities and ports
	 * assigned.
	 * 
	 * @return
	 */
	public abstract SessionSpec generateSessionSpec();

	/**
	 * Indicates own medias and ports assigned. Warning: It could has
	 * inconsistencies with template.
	 * 
	 * @param localSpec
	 */
	public abstract void setLocalSessionSpec(SessionSpec localSpec);

	/**
	 * Indicates other party medias and ports assigned.
	 * 
	 * @return
	 */
	public abstract void setRemoteSessionSpec(SessionSpec remote);

	public abstract InetAddress getLocalAddress();

}
