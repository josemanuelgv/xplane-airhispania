/**
 * 
 */
package org.airhispania.aptviewer.provider;

import java.util.ArrayList;
import java.util.List;

import org.airhispania.xplane2rc.apt.model.LandAirport;
import org.airhispania.xplane2rc.apt.model.LandRunway;
import org.airhispania.xplane2rc.apt.model.LandRunwayEnd;
import org.airhispania.xplane2rc.model.Scenery;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.Viewer;

/**
 * @author Jose Manuel García Valladolid - josemanuelgv@gmail.com
 * 
 */
public class SceneryContentProvider implements ITreeContentProvider {

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * org.eclipse.jface.viewers.IStructuredContentProvider#getElements(java
	 * .lang.Object)
	 */
	public Object[] getElements(Object arg0) {
		if (arg0 instanceof List) {

			// Discard sceneries with no airports
			List filteredList = new ArrayList();
			for (Object o : ((List) arg0)) {
				if (o instanceof org.airhispania.xplane2rc.model.Scenery) {
					if (((Scenery) o).getAirports().size() > 0)
						filteredList.add(o);
				}
			}
			return filteredList.toArray();
		} else
			return null;

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.eclipse.jface.viewers.IContentProvider#dispose()
	 */
	public void dispose() {
		// TODO Auto-generated method stub

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * org.eclipse.jface.viewers.IContentProvider#inputChanged(org.eclipse.jface
	 * .viewers.Viewer, java.lang.Object, java.lang.Object)
	 */
	public void inputChanged(Viewer arg0, Object arg1, Object arg2) {
		// TODO Auto-generated method stub

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * org.eclipse.jface.viewers.ITreeContentProvider#getChildren(java.lang.
	 * Object)
	 */
	public Object[] getChildren(Object arg0) {
		if (arg0 instanceof Scenery) {
			return ((Scenery) arg0).getAirports().values().toArray();
		} else

		if (arg0 instanceof LandAirport) {
			List<LandRunwayEnd> ends = new ArrayList<LandRunwayEnd>();

			for (LandRunway r : ((LandAirport) arg0).getRunways()) {
				ends.addAll(r.getEnds());
			}
			return ends.toArray();
		}

		return new Object[0];
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * org.eclipse.jface.viewers.ITreeContentProvider#getParent(java.lang.Object
	 * )
	 */
	public Object getParent(Object arg0) {
		// TODO Auto-generated method stub
		return null;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * org.eclipse.jface.viewers.ITreeContentProvider#hasChildren(java.lang.
	 * Object)
	 */
	public boolean hasChildren(Object arg0) {
		if (arg0 instanceof List) {
			return ((List) arg0).size() > 0;
		} else if (arg0 instanceof Scenery) {
			return ((Scenery) arg0).getAirports().size() > 0;
		} else

		if (arg0 instanceof LandAirport) {
			return ((LandAirport) arg0).getRunways().size() > 0;
		}

		return false;
	}

}
