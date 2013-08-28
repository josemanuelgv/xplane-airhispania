/**
 * 
 */
package org.airhispania.fscom2ts;

import java.net.URI;
import java.net.URISyntaxException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;
import java.util.Locale;

import org.airhispania.fscom2ts.configuration.ChannelMap;
import org.airhispania.fscom2ts.configuration.Configuration;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;

/**
 * @author Jose Manuel García Valladolid - josemanuelgv@gmail.com
 * 
 */
public class FrequencyUpdater implements IUpdateable {

	private Label freqControl;
	private Label depControl;
	private Label statusControl;
	private List<ChannelMap> channelMap;
	private String latestFreq = "";
	private boolean connected = false;
	private ChannelMap unicom;
	private Configuration cfg;

	final SimpleDateFormat sdf = new SimpleDateFormat("HH:mm:ss",
			Locale.getDefault());

	/**
	 * @param freqControl
	 * @param depControl
	 * @param statusControl
	 * @param channelMap
	 * @param unicom
	 * @param cfg
	 */
	public FrequencyUpdater(Label freqControl, Label depControl,
			Label statusControl, List<ChannelMap> channelMap,
			ChannelMap unicom, Configuration cfg) {
		super();
		this.freqControl = freqControl;
		this.depControl = depControl;
		this.channelMap = channelMap;
		this.statusControl = statusControl;
		this.unicom = unicom;
		this.cfg = cfg;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.flightpanels.scs.IUpdateable#update(java.lang.Object)
	 */
	public void update(String arg0, Double lat, Double lon) {

		if (latestFreq.equalsIgnoreCase(arg0))
			return;

		ChannelMap c = null;

		// Si se trata de UNICOM, conectamos directamente
		if (!unicom.getFrequency().equalsIgnoreCase(arg0)) {

			// Buscamos la frecuencia en el mapa de dependencias

			for (ChannelMap ci : channelMap) {
				if (ci.getFrequency().equalsIgnoreCase(arg0)) {
					double distancia = ci.distanciaMillas(lat, lon);
					if (distancia <= cfg.getMaxrad_nm()) {
						if (c != null) {
							// Ya se encontró una dependencia que corresponde
							// con la
							// frecuencia y está dentro del radio
							// Calculamos cual de ellas está más cerca y nos
							// quedamos
							// con esa
							if (distancia < c.distanciaMillas(lat, lon))
								c = ci;
						} else
							c = ci;
					}
				}
			}
		} else {
			c = unicom;
		}

		// Si no encontramos la frecuencia, salimos
		if (c == null) {
			final String targetFreq = arg0;
			Display.getDefault().asyncExec(new Runnable() {
				public void run() {

					statusControl.setText(sdf.format(new Date())
							+ " status: frequency " + targetFreq
							+ " not found around " + cfg.getMaxrad_nm()
							+ " nm!");
					statusControl.setForeground(statusControl.getDisplay()
							.getSystemColor(SWT.COLOR_YELLOW));

					freqControl.setText("");
					depControl.setText("");
				}
			});

			try {
				if (connected)
					disconnectTS();
				connected = false;
			} catch (URISyntaxException e) {
				connected = false;
				e.printStackTrace();
			}
			latestFreq = arg0;
			return;
		}

		try {

			final String frqstr = arg0;
			// Lanzamos la invocación al SO
			if (connectTS(c)) {

				final String chstr = c.getChannel();
				connected = true;
				latestFreq = arg0;

				Display.getDefault().asyncExec(new Runnable() {
					public void run() {

						freqControl.setText(frqstr);
						depControl.setText(chstr);

						statusControl.setText(sdf.format(new Date())
								+ " status: tuned successfully");
						statusControl.setForeground(statusControl.getDisplay()
								.getSystemColor(SWT.COLOR_GREEN));
					}
				});
			} else {
				Display.getDefault().asyncExec(new Runnable() {
					public void run() {

						statusControl.setText(sdf.format(new Date())
								+ " status: could not tune frequency " + frqstr
								+ "!");
						statusControl.setForeground(statusControl.getDisplay()
								.getSystemColor(SWT.COLOR_RED));
					}
				});
			}

		} catch (URISyntaxException e) {
			Display.getDefault().asyncExec(new Runnable() {
				public void run() {

					statusControl.setText(sdf.format(new Date())
							+ " status: failed to tune due a URI Syntax exception");
					statusControl.setForeground(statusControl.getDisplay()
							.getSystemColor(SWT.COLOR_RED));
				}
			});
			e.printStackTrace();
		}

	}

	private boolean connectTS(ChannelMap c) throws URISyntaxException {
		URI uri;
		uri = new URI("teamspeak://" + c.getTsConnection().getServer()
				+ "?loginname=" + c.getTsConnection().getLoginname()
				+ "?password=" + c.getTsConnection().getPassword()
				+ "?nickname="
				+ c.getTsConnection().getNickname().replace(" ", "_")
				+ "?channel=" + c.getChannel());
		return org.eclipse.swt.program.Program.launch(uri.toString());
	}

	private void disconnectTS() throws URISyntaxException {
		URI uri;
		uri = new URI("teamspeak://localhost");
		org.eclipse.swt.program.Program.launch(uri.toString());
	}

}
