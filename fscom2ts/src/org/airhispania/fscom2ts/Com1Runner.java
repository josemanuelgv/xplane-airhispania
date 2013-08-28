/**
 * 
 */
package org.airhispania.fscom2ts;

import org.airhispania.fscom2ts.configuration.Configuration;

import com.flightsim.fsuipc.FSAircraft;

/**
 * @author Jose Manuel García Valladolid - josemanuelgv@gmail.com
 * 
 */
public class Com1Runner implements Runnable {

	private boolean running = true;
	private FSCom receiver;
	private FSAircraft aircraft;
	private Configuration cfg;
	private FrequencyUpdater fqUpdater;
	private Thread com1_thread;

	/**
	 * @param receiver
	 * @param cfg
	 * @param fqUpdater
	 */
	public Com1Runner(FSCom receiver, FSAircraft aircraft, Configuration cfg,
			FrequencyUpdater fqUpdater) {
		super();
		this.receiver = receiver;
		this.aircraft = aircraft;
		this.cfg = cfg;
		this.fqUpdater = fqUpdater;

		com1_thread = new Thread(this);
		com1_thread.start();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Runnable#run()
	 */
	public void run() {
		while (running) {
			try {
				Thread.sleep(cfg.getUpdateFrequency());
				if (Thread.currentThread().isInterrupted()) {
					running = false;
				} else
					fqUpdater.update(receiver.FreqAsString(),
							aircraft.Latitude(), aircraft.Longitude());
			} catch (Exception e) {
				e.printStackTrace();
			}

		}

	}

	public void terminate() {
		running = false;
	}
}
