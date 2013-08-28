/**
 * 
 */
package org.airhispania.fscom2ts.ui;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.airhispania.fscom2ts.Com1Runner;
import org.airhispania.fscom2ts.FSCom;
import org.airhispania.fscom2ts.FrequencyUpdater;
import org.airhispania.fscom2ts.configuration.ChannelMap;
import org.airhispania.fscom2ts.configuration.Configuration;
import org.airhispania.fscom2ts.configuration.TSConnection;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.wb.swt.SWTResourceManager;

import com.flightsim.fsuipc.FSAircraft;
import com.flightsim.fsuipc.fsuipc_wrapper;

/**
 * @author Jose Manuel García Valladolid - josemanuelgv@gmail.com
 * 
 */
public class ApplicationWindow {

	/**
	 * 
	 */
	private static final String CONFIG_PROPERTIES = "config.properties";
	/**
	 * 
	 */
	private static final String TSCONNECTIONS_CSV = "tsconnections.csv";

	protected Shell shlXplaneRadioFor;
	private Label freqText;
	private Label depText;
	private Label statusLabel;
	private Button connectButton;

	private Configuration cfg;
	private List<ChannelMap> channels;

	private Runnable com1_runner;

	/**
	 * Launch the application.
	 * 
	 * @param args
	 */
	public static void main(String[] args) {
		try {

			ApplicationWindow window = new ApplicationWindow();
			window.open();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	/**
	 * @throws Exception
	 * 
	 */
	public ApplicationWindow() throws Exception {
		super();

	}

	/**
	 * Open the window.
	 */
	public void open() {

		Display display = Display.getDefault();
		createContents();
		shlXplaneRadioFor.open();
		shlXplaneRadioFor.layout();
		while (!shlXplaneRadioFor.isDisposed()) {
			if (!display.readAndDispatch()) {
				display.sleep();
			}
		}

	}

	/**
	 * Create contents of the window.
	 */
	protected void createContents() {
		shlXplaneRadioFor = new Shell(SWT.CLOSE | SWT.TITLE | SWT.MIN | SWT.MAX);
		shlXplaneRadioFor.setSize(476, 171);
		shlXplaneRadioFor.setText("COM Radio for TeamSpeak");
		shlXplaneRadioFor.setLayout(new GridLayout(2, false));

		Menu menu = new Menu(shlXplaneRadioFor, SWT.BAR);
		shlXplaneRadioFor.setMenuBar(menu);

		MenuItem mntmNewSubmenu_1 = new MenuItem(menu, SWT.CASCADE);
		mntmNewSubmenu_1.setText("Configuration");

		Menu menu_2 = new Menu(mntmNewSubmenu_1);
		mntmNewSubmenu_1.setMenu(menu_2);

		MenuItem mntmTeamspeakAcount = new MenuItem(menu_2, SWT.NONE);
		mntmTeamspeakAcount.setText("TS Connections");

		MenuItem mntmChannelMap = new MenuItem(menu_2, SWT.NONE);
		mntmChannelMap.setText("Channel Map");

		MenuItem mntmSettings = new MenuItem(menu_2, SWT.NONE);
		mntmSettings.setText("Settings");

		new MenuItem(menu_2, SWT.SEPARATOR);

		MenuItem mntmExit = new MenuItem(menu_2, SWT.NONE);
		mntmExit.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				shutdown();
				shlXplaneRadioFor.close();
			}
		});
		mntmExit.setText("Exit");

		MenuItem mntmNewSubmenu = new MenuItem(menu, SWT.CASCADE);
		mntmNewSubmenu.setText("Help");

		Menu menu_1 = new Menu(mntmNewSubmenu);
		mntmNewSubmenu.setMenu(menu_1);

		MenuItem mntmHowTo = new MenuItem(menu_1, SWT.NONE);
		mntmHowTo.setText("How to");

		MenuItem mntmAbout = new MenuItem(menu_1, SWT.NONE);
		mntmAbout.setText("About");
		mntmAbout.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				MessageDialog
						.openInformation(
								Display.getCurrent().getActiveShell(),
								"About fscom2ts",
								"FSCom2TS 0.1.0, an utility for automatic tunning of radio frecuency dependencies for FSUIPC based flight simulators.\n\n"
										+ "This program was born in order to help XPlane users for AirHispania virtual airline. Visit https://code.google.com/p/xplane-airhispania/ for more information. ");
			}
		});

		connectButton = new Button(shlXplaneRadioFor, SWT.TOGGLE | SWT.CENTER);
		connectButton.setImage(SWTResourceManager.getImage(
				ApplicationWindow.class, "/icons/progress/ani/1.png"));
		GridData gd_connectButton = new GridData(SWT.LEFT, SWT.CENTER, false,
				false, 1, 1);
		gd_connectButton.widthHint = 111;
		connectButton.setLayoutData(gd_connectButton);
		connectButton.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				if (com1_runner == null) {
					try {
						startup();
					} catch (Exception e1) {
						Status status = new Status(IStatus.ERROR, "FSCom2TS",
								0, "Status Error Message", null);

						// Display the dialog
						ErrorDialog.openError(Display.getCurrent()
								.getActiveShell(), "JFace Error", e1
								.getMessage(), status);
					}

				} else {
					shutdown();

				}
			}
		});
		connectButton.setText("Connect");

		Button btnNewButton = new Button(shlXplaneRadioFor, SWT.NONE);
		btnNewButton.setImage(SWTResourceManager.getImage(
				ApplicationWindow.class,
				"/javax/swing/plaf/metal/icons/ocean/close.gif"));
		btnNewButton.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				shutdown();
				shlXplaneRadioFor.close();
			}
		});
		GridData gd_btnNewButton = new GridData(SWT.RIGHT, SWT.CENTER, false,
				false, 1, 1);
		gd_btnNewButton.widthHint = 61;
		btnNewButton.setLayoutData(gd_btnNewButton);
		btnNewButton.setText("Exit");

		freqText = new Label(shlXplaneRadioFor, SWT.BORDER | SWT.CENTER);
		freqText.setBackground(SWTResourceManager.getColor(SWT.COLOR_BLACK));
		freqText.setForeground(SWTResourceManager.getColor(SWT.COLOR_WHITE));
		freqText.setText("000.00");
		freqText.setFont(SWTResourceManager.getFont("LCD", 40, SWT.BOLD));
		freqText.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false, 1,
				1));

		depText = new Label(shlXplaneRadioFor, SWT.BORDER | SWT.CENTER);
		depText.setForeground(SWTResourceManager.getColor(SWT.COLOR_YELLOW));
		depText.setBackground(SWTResourceManager.getColor(SWT.COLOR_BLACK));
		depText.setText("       ");
		depText.setFont(SWTResourceManager.getFont("LCD", 37, SWT.NORMAL));
		depText.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false, 1,
				1));

		statusLabel = new Label(shlXplaneRadioFor, SWT.BORDER | SWT.LEFT);
		statusLabel.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true,
				false, 2, 1));
		statusLabel.setBackground(SWTResourceManager.getColor(SWT.COLOR_BLACK));
		statusLabel.setForeground(SWTResourceManager.getColor(SWT.COLOR_WHITE));
		statusLabel.setText("Status:");

	}

	protected void startup() throws Exception {

		int ret = fsuipc_wrapper.Open(fsuipc_wrapper.SIM_ANY);

		if (ret == 0) {
			statusLabel.setText("Status: ERROR, Flight simulator not found");
			statusLabel.setForeground(SWTResourceManager
					.getColor(SWT.COLOR_RED));
		} else {

			statusLabel.setText("Status: parsing configuration ...");
			statusLabel.setForeground(SWTResourceManager
					.getColor(SWT.COLOR_WHITE));

			// Load configuration
			File configFile = new File(CONFIG_PROPERTIES);
			cfg = Configuration.parse(configFile);

			String[] freqFiles = cfg.getFrequencyFiles().split(",");
			File tsConnections = new File(TSCONNECTIONS_CSV);
			Map<Integer, TSConnection> tsconns = TSConnection
					.parse(tsConnections);
			if (tsconns.size() < 1)
				throw new Exception(
						"No existen conexiones a TeamSpeak definidas en el fichero "
								+ TSCONNECTIONS_CSV);

			channels = new ArrayList<ChannelMap>();
			for (String f : freqFiles) {
				channels.addAll(ChannelMap.parse(new File(f), tsconns.get(0)));
			}

			// Canal de Unicom
			ChannelMap unicom = new ChannelMap();
			unicom.setChannel(cfg.getUnicomChannel());
			unicom.setDependency(cfg.getUnicomChannel());
			unicom.setFrequency(cfg.getUnicomFreq());
			unicom.setTsConnection(tsconns.get(0));

			FrequencyUpdater updater = new FrequencyUpdater(freqText, depText,
					statusLabel, channels, unicom, cfg);
			com1_runner = new Com1Runner(new FSCom(), new FSAircraft(), cfg,
					updater);

			connectButton.setText("Disconnect");
			connectButton.setSelection(true);

			statusLabel.setText("Status: Flight simulator connected and "
					+ channels.size() + " frequencies loaded.");
		}
	}

	protected void shutdown() {
		if (com1_runner != null) {
			((Com1Runner) com1_runner).terminate();
			com1_runner = null;
		}
		fsuipc_wrapper.Close();
		connectButton.setText("Connect");
		freqText.setText("000.00");
		depText.setText("       ");
		statusLabel.setBackground(SWTResourceManager.getColor(SWT.COLOR_BLACK));
	}

}
