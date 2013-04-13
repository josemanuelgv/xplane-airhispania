package org.airhispania.aptviewer;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import org.airhispania.aptviewer.provider.SceneryContentProvider;
import org.airhispania.aptviewer.provider.SceneryLabelProvider;
import org.airhispania.xplane2rc.apt.model.LandAirport;
import org.airhispania.xplane2rc.model.Scenery;
import org.apache.log4j.Logger;
import org.eclipse.jface.action.StatusLineManager;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.jface.viewers.ViewerFilter;
import org.eclipse.jface.window.ApplicationWindow;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.DirectoryDialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.ui.forms.widgets.FormToolkit;

public class Application extends ApplicationWindow {
	/**
	 * 
	 */

	public static String APPLICATION_VERSION = "0.1.1";

	private Text ubicXPlaneText;
	private final FormToolkit formToolkit = new FormToolkit(
			Display.getDefault());
	private Text filtroICAO;
	private Tree tree;
	private Label lblStat;

	protected TreeViewer viewer;

	protected static Logger logger = Logger.getLogger(Application.class);

	protected List<File> aptFiles = new ArrayList<File>();
	protected List<Scenery> currentSceneries = new ArrayList<org.airhispania.xplane2rc.model.Scenery>();
	protected long airportsParsed = 0;

	/**
	 * Create the application window,
	 */
	public Application() {
		super(null);

		createActions();
		addCoolBar(SWT.FLAT);
		addMenuBar();
		addStatusLine();

		logger.info("Working Directory = " + System.getProperty("user.dir"));
	}

	/**
	 * Create contents of the application window.
	 * 
	 * @param parent
	 */
	@Override
	protected Control createContents(Composite parent) {

		// Image bg_Image = new Image(super.getShell().getDisplay(),
		// Application.class.getResourceAsStream("resources/xbender.png"));
		// super.getShell().setBackgroundImage(bg_Image);
		// super.getShell().setBackgroundMode(SWT.INHERIT_FORCE);

		Composite container = new Composite(parent, SWT.NONE);
		container.setLayout(new GridLayout(5, false));
		{
			Label lblUbicacinXplane = new Label(container, SWT.NONE);
			lblUbicacinXplane.setText("Ubicaci\u00F3n XPlane:");
		}
		{
			ubicXPlaneText = new Text(container, SWT.BORDER);
			ubicXPlaneText.setEditable(false);
			ubicXPlaneText.setLayoutData(new GridData(SWT.FILL, SWT.CENTER,
					true, false, 3, 1));
		}
		{
			Button btnSelButton = new Button(container, SWT.NONE);
			btnSelButton.addSelectionListener(new SelectionAdapter() {
				@Override
				public void widgetSelected(SelectionEvent e) {

					DirectoryDialog dlg = new DirectoryDialog(getShell());

					// Set the initial filter path according
					// to anything they've selected or typed in
					// dlg.setFilterPath(text.getText());

					// Change the title bar text
					dlg.setText("Instalacion de XPlane");

					// Customizable message displayed in the dialog
					dlg.setMessage("Seleccione la carpeta de instalacion de XPlane");

					// Calling open() will open and run the dialog.
					// It will return the selected directory, or
					// null if user cancels
					String dir = dlg.open();
					if (dir != null) {
						// Set the text box to the new selection
						ubicXPlaneText.setText(dir);

						getShell().setCursor(
								new Cursor(getShell().getDisplay(),
										SWT.CURSOR_WAIT));

						searchAptDatabases();

						parseAirports();

						getShell().setCursor(
								new Cursor(getShell().getDisplay(),
										SWT.CURSOR_ARROW));
					}

				}
			});
			btnSelButton
					.setToolTipText("Pulse para seleccionar la carpeta de instalaci\u00F3n de XPlane y buscar aer\u00F3dromos");
			btnSelButton.setLayoutData(new GridData(SWT.RIGHT, SWT.CENTER,
					false, false, 1, 1));
			btnSelButton.setText("Seleccionar");
		}

		filtroICAO = new Text(container, SWT.BORDER);
		filtroICAO.setToolTipText("Filtro ICAO");
		filtroICAO.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, false,
				false, 4, 1));
		formToolkit.adapt(filtroICAO, true, true);

		Button btnNewButton_3 = new Button(container, SWT.NONE);
		btnNewButton_3.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {

				getShell().setCursor(
						new Cursor(getShell().getDisplay(), SWT.CURSOR_WAIT));

				viewer.collapseAll();
				viewer.resetFilters();
				for (ViewerFilter vf : viewer.getFilters()) {
					viewer.removeFilter(vf);
				}

				if ((currentSceneries.size() > 0)
						&& (filtroICAO.getText().length() > 0)) {
					ViewerFilter filterViewer = new ViewerFilter() {

						@Override
						public boolean select(Viewer arg0, Object arg1,
								Object arg2) {
							if (arg2 instanceof LandAirport) {
								return ((LandAirport) arg2).getIcao_code()
										.contains(filtroICAO.getText());
							}
							return true;
						}
					};

					viewer.addFilter(filterViewer);
					viewer.expandAll();
				}

				getShell().setCursor(
						new Cursor(getShell().getDisplay(), SWT.CURSOR_ARROW));
			}
		});
		btnNewButton_3.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, false,
				false, 1, 1));
		formToolkit.adapt(btnNewButton_3, true, true);
		btnNewButton_3.setText("Buscar");

		tree = new Tree(container, SWT.BORDER);
		tree.setToolTipText("Listado de aer\u00F3dromos existentes en la instalaci\u00F3n de XPlane");
		tree.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 5, 1));

		viewer = new TreeViewer(tree);
		viewer.setContentProvider(new SceneryContentProvider());
		viewer.setLabelProvider(new SceneryLabelProvider(getShell()));
		viewer.setInput(currentSceneries);

		formToolkit.adapt(tree);
		formToolkit.paintBordersFor(tree);

		lblStat = new Label(container, SWT.NONE);
		lblStat.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, false, false,
				4, 1));
		formToolkit.adapt(lblStat, true, true);

		Button btnNewButton_2 = new Button(container, SWT.NONE);
		btnNewButton_2.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, false,
				false, 1, 1));
		btnNewButton_2.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				close();
			}
		});
		formToolkit.adapt(btnNewButton_2, true, true);
		btnNewButton_2.setText("Salir");

		return container;
	}

	/**
	 * Create the actions.
	 */
	private void createActions() {
		// Create the actions
	}

	/**
	 * Create the status line manager.
	 * 
	 * @return the status line manager
	 */
	@Override
	protected StatusLineManager createStatusLineManager() {
		StatusLineManager statusLineManager = new StatusLineManager();
		return statusLineManager;
	}

	/**
	 * Launch the application.
	 * 
	 * @param args
	 */
	public static void main(String args[]) {
		try {
			Application window = new Application();
			window.setBlockOnOpen(true);
			window.open();
			Display.getCurrent().dispose();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	/**
	 * Configure the shell.
	 * 
	 * @param newShell
	 */
	@Override
	protected void configureShell(Shell newShell) {
		super.configureShell(newShell);
		newShell.setText("Visor de aeródromos para XPlane v. "
				+ APPLICATION_VERSION);
	}

	/**
	 * Return the initial size of the window.
	 */
	@Override
	protected Point getInitialSize() {
		return new Point(601, 505);
	}

	/**
	 * Search for apt.dat files in the hierarchy of the current XPlane directory
	 */
	protected void searchAptDatabases() {

		final List<File> aptfiles = new ArrayList<File>();

		final boolean[] done = new boolean[1];
		new Thread() {

			public void run() {

				try {
					Thread.sleep(500);
				} catch (Throwable th) {

				}

				getShell().getDisplay().syncExec(new Runnable() {

					public void run() {

						org.airhispania.xplane2rc.util.FilesFilter.walk(
								ubicXPlaneText.getText(),
								Scenery.APT_FILE_PATTERN, aptfiles);

					}

				});

				done[0] = true;

				// wake the user interface thread from sleep

				getShell().getDisplay().wake();

			}

		}

		.start();

		this.getStatusLineManager().setMessage(
				"Buscando bases de datos de aeródromos en XPlane ...");

		while (!done[0]) {

			if (!getShell().getDisplay().readAndDispatch())
				getShell().getDisplay().sleep();

		}

		// Remove disabled custom scenerys by XAddonManager
		for (File f : aptfiles) {
			if (!f.getAbsolutePath().contains("(disabled)")) {
				aptFiles.add(f);
				logger.info("apt.dat: " + f.getAbsolutePath());
			}
		}

	}

	protected void parseAirports() {

		this.getStatusLineManager().setMessage(
				"Encontrados " + aptFiles.size() + " ficheros apt.dat");

		final boolean[] done = new boolean[1];

		new Thread() {

			public void run() {

				try {
					Thread.sleep(500);
				} catch (Throwable th) {

				}

				getShell().getDisplay().syncExec(new Runnable() {

					public void run() {

						for (File f : aptFiles) {
							try {
								Scenery s = new Scenery(f.getAbsolutePath(), f);
								airportsParsed = airportsParsed + s.parse();
								if (s.getAirports().size() > 0)
									currentSceneries.add(s);
							} catch (Exception e) {
								logger.error(e);
							}
						}
					}

				});

				done[0] = true;

				// wake the user interface thread from sleep

				getShell().getDisplay().wake();

			}

		}

		.start();

		this.getStatusLineManager().setMessage("Buscando aeródromos ...");

		while (!done[0]) {

			if (!getShell().getDisplay().readAndDispatch())
				getShell().getDisplay().sleep();

		}

		this.getStatusLineManager().setMessage("");
		lblStat.setText("Total aer\u00F3dromos: " + airportsParsed);
		viewer.refresh();
	}

}
