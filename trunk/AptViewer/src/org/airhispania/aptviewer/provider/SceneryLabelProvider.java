/**
 * 
 */
package org.airhispania.aptviewer.provider;

import org.airhispania.aptviewer.Application;
import org.airhispania.xplane2rc.apt.model.LandAirport;
import org.airhispania.xplane2rc.apt.model.LandRunwayEnd;
import org.airhispania.xplane2rc.model.Scenery;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.ILabelProviderListener;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Shell;

/**
 * @author Jose Manuel García Valladolid - josemanuelgv@gmail.com
 * 
 */
public class SceneryLabelProvider implements ILabelProvider {

	private Shell shell;

	/**
	 * @param shell
	 */
	public SceneryLabelProvider(Shell shell) {
		super();
		this.shell = shell;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * org.eclipse.jface.viewers.IBaseLabelProvider#addListener(org.eclipse.
	 * jface.viewers.ILabelProviderListener)
	 */
	public void addListener(ILabelProviderListener arg0) {
		// TODO Auto-generated method stub

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.eclipse.jface.viewers.IBaseLabelProvider#dispose()
	 */
	public void dispose() {
		// TODO Auto-generated method stub

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * org.eclipse.jface.viewers.IBaseLabelProvider#isLabelProperty(java.lang
	 * .Object, java.lang.String)
	 */
	public boolean isLabelProperty(Object arg0, String arg1) {
		// TODO Auto-generated method stub
		return false;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * org.eclipse.jface.viewers.IBaseLabelProvider#removeListener(org.eclipse
	 * .jface.viewers.ILabelProviderListener)
	 */
	public void removeListener(ILabelProviderListener arg0) {
		// TODO Auto-generated method stub

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.eclipse.jface.viewers.ILabelProvider#getImage(java.lang.Object)
	 */
	public Image getImage(Object arg0) {
		if (arg0 instanceof LandAirport) {
			// return new Image(
			// shell.getDisplay(),
			// Application.class
			// .getResourceAsStream("resources/airport-terminal.png"));
			return null;
		} else if (arg0 instanceof LandRunwayEnd) {
			// return new Image(
			// shell.getDisplay(),
			// Application.class
			// .getResourceAsStream("resources/airport-runway.png"));
			return null;
		}

		return new Image(shell.getDisplay(),
				Application.class.getResourceAsStream("resources/airport.png"));
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.eclipse.jface.viewers.ILabelProvider#getText(java.lang.Object)
	 */
	public String getText(Object arg0) {
		if (arg0 instanceof org.airhispania.xplane2rc.model.Scenery) {
			return ((Scenery) arg0).getTitle();
		} else

		if (arg0 instanceof LandAirport) {
			return ((LandAirport) arg0).getIcao_code() + " "
					+ ((LandAirport) arg0).getName() + ", Elev: "
					+ ((LandAirport) arg0).getElevation() + " ft";
		} else

		if (arg0 instanceof LandRunwayEnd) {
			return ((LandRunwayEnd) arg0).getNumber() + " - [Lat: "
					+ ((LandRunwayEnd) arg0).getLatitude() + ", Lon: "
					+ ((LandRunwayEnd) arg0).getLongitude() + "]";
		}

		return null;
	}

}
