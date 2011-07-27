package com.tikal.android.info;

import java.net.InetAddress;

import com.tikal.android.mscontrol.networkconnection.ConnectionType;

public class ConnectionInfo {

	private InetAddress localAddress;
	private ConnectionType connectionType;

	public InetAddress getLocalAddress() {
		return localAddress;
	}

	public ConnectionType getConnectionType() {
		return connectionType;
	}

	public ConnectionInfo(InetAddress localAddress, ConnectionType connectionType) {
		this.connectionType = connectionType;
		this.localAddress = localAddress;
	}

}
