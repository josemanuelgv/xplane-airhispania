/**
 * 
 */
package org.airhispania.fscom2ts.configuration;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.HashMap;
import java.util.Map;

/**
 * @author Jose Manuel García Valladolid - josemanuelgv@gmail.com
 * 
 */
public class TSConnection {

	private int number;
	private String server;
	private String loginname;
	private String nickname;
	private String password;

	/**
	 * @return the number
	 */
	public int getNumber() {
		return number;
	}

	/**
	 * @param number
	 *            the number to set
	 */
	public void setNumber(int number) {
		this.number = number;
	}

	/**
	 * @return the server
	 */
	public String getServer() {
		return server;
	}

	/**
	 * @param server
	 *            the server to set
	 */
	public void setServer(String server) {
		this.server = server;
	}

	/**
	 * @return the loginname
	 */
	public String getLoginname() {
		return loginname;
	}

	/**
	 * @param loginname
	 *            the loginname to set
	 */
	public void setLoginname(String loginname) {
		this.loginname = loginname;
	}

	/**
	 * @return the nickname
	 */
	public String getNickname() {
		return nickname;
	}

	/**
	 * @param nickname
	 *            the nickname to set
	 */
	public void setNickname(String nickname) {
		this.nickname = nickname;
	}

	/**
	 * @return the password
	 */
	public String getPassword() {
		return password;
	}

	/**
	 * @param password
	 *            the password to set
	 */
	public void setPassword(String password) {
		this.password = password;
	}

	public static Map<Integer, TSConnection> parse(File f) throws IOException {

		Map<Integer, TSConnection> l = new HashMap<Integer, TSConnection>();

		FileInputStream fstream = new FileInputStream(f);

		// Get the object of DataInputStream
		DataInputStream in = new DataInputStream(fstream);
		BufferedReader br = new BufferedReader(new InputStreamReader(in));
		String strLine;
		// Read File Line By Line
		while ((strLine = br.readLine()) != null) {
			String[] data = strLine.split(",");
			TSConnection c = new TSConnection();
			if ((data.length > 0) && (data[0] != null)
					&& (data[0].length() > 0)) {
				c.setNumber(Integer.parseInt(data[0]));
			}
			if ((data.length > 1) && (data[1] != null)
					&& (data[1].length() > 0)) {
				c.setServer(data[1]);
			}
			if ((data.length > 2) && (data[2] != null)
					&& (data[2].length() > 0)) {
				c.setLoginname(data[2]);
			}
			if ((data.length > 3) && (data[3] != null)
					&& (data[3].length() > 0)) {
				c.setPassword(data[3]);
			}
			if ((data.length > 4) && (data[4] != null)
					&& (data[4].length() > 0)) {
				c.setNickname(data[4]);
			}
			l.put(c.getNumber(), c);
		}
		// Close the input stream
		in.close();

		return l;
	}
}
