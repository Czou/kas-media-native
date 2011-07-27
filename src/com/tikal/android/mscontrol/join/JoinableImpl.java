package com.tikal.android.mscontrol.join;

import java.util.ArrayList;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.tikal.mscontrol.MediaControlException;
import com.tikal.mscontrol.join.Joinable;




public class JoinableImpl implements Joinable {

	private static final Log log = LogFactory.getLog(JoinableImpl.class);

	protected ArrayList<LocalConnection> connections = new ArrayList<LocalConnection>();

	@Override
	public Joinable[] getJoinees() throws MediaControlException {
		int i = 0;
		Joinable[] joinees = new Joinable[connections.size()];

		for (LocalConnection connection : connections)
			joinees[i++] = connection.getJoinable();

		return joinees;
	}

	@Override
	public Joinable[] getJoinees(Direction direction) throws MediaControlException {
		int i = 0;
		Joinable[] joinees = new Joinable[connections.size()];

		for (LocalConnection connection : connections) {
			if (connection.getDirection().equals(direction)) {
				joinees[i++] = connection.getJoinable();
			}
		}

		return joinees;
	}

	@Override
	public void join(Direction direction, Joinable other)
			throws MediaControlException {
		log.info("join: " + direction + " " + other);

		LocalConnection connection1 = new LocalConnection(direction, other);

		Direction dir2 = Direction.DUPLEX;
		if (Direction.SEND.equals(direction))
			dir2 = Direction.RECV;
		else if (Direction.RECV.equals(direction))
			dir2 = Direction.SEND;

		LocalConnection connection2 = new LocalConnection(dir2, this);

		connection1.join(connection2);

		this.connections.add(connection1);
		((JoinableImpl) other).connections.add(connection2);
	}

	@Override
	public void unjoin(Joinable other) throws MediaControlException {
		LocalConnection connection = null;
		for (LocalConnection conn : connections) {
			if (conn.getJoinable().equals(other)) {
				connection = conn;
				break;
			}
		}

		if (connection == null)
			throw new MediaControlException("No connected: " + other);
		
		((JoinableImpl)connection.getOther().getJoinable()).connections.remove(connection.getOther());
		this.connections.remove(connection);
	}

}
