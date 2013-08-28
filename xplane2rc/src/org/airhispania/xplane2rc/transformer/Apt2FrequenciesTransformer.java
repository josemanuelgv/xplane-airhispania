/**
 * 
 */
package org.airhispania.xplane2rc.transformer;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.text.NumberFormat;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

import org.airhispania.xplane2rc.apt.model.ComFrequency;
import org.airhispania.xplane2rc.apt.model.LandAirport;
import org.airhispania.xplane2rc.apt.model.LandRunwayEnd;
import org.airhispania.xplane2rc.model.Scenery;

import com.grum.geocalc.EarthCalc;

/**
 * @author Jose Manuel García Valladolid - josemanuelgv@gmail.com
 * 
 */
public class Apt2FrequenciesTransformer {

	private NumberFormat formatter;

	/**
	 * 
	 */
	public Apt2FrequenciesTransformer() {
		super();
		DecimalFormatSymbols otherSymbols = new DecimalFormatSymbols(
				Locale.ENGLISH);
		otherSymbols.setDecimalSeparator('.');

		formatter = new DecimalFormat("###.######", otherSymbols);
	}

	public void transform(List<Scenery> sceneries, File fFreq) throws Exception {

		List<String> frqProcessed = new ArrayList<String>();

		Scenery defaultScenery = null;
		List<Scenery> customSceneries = new ArrayList<Scenery>();

		// Search for default scenery
		for (Scenery s : sceneries) {
			if (s.isDefaultXplaneDatabase()) {
				defaultScenery = s;
			} else
				customSceneries.add(s);
		}

		// Iterate over customSceneries
		// for (Scenery s : customSceneries) {
		// for (String airport : s.getAirports().keySet()) {
		// LandAirport la = s.getAirports().get(airport);
		// if (la.getFrequencies() != null)
		// for (ComFrequency f : la.getFrequencies()) {
		// String freqLine = createFreqLine(f, la);
		// if ((freqLine != null)
		// && (!frqProcessed.contains(freqLine)))
		// frqProcessed.add(freqLine);
		// }
		// }
		// }

		if (defaultScenery != null) {
			// Iterate over default scenery
			for (String airport : defaultScenery.getAirports().keySet()) {
				LandAirport la = defaultScenery.getAirports().get(airport);
				if (la.getFrequencies() != null)
					for (ComFrequency f : la.getFrequencies()) {
						String freqLine = createFreqLine(f, la);
						if ((freqLine != null)
								&& (!frqProcessed.contains(freqLine)))
							frqProcessed.add(freqLine);
					}
			}
		}

		// Spain FIR Frequencies
		for (int i = 0; i < FIRFrequencies.SPAIN_FIR.length; i++) {
			frqProcessed.add(FIRFrequencies.SPAIN_FIR[i]);
		}

		BufferedWriter out = new BufferedWriter(new FileWriter(fFreq));
		for (String l : frqProcessed) {
			out.write(l + "\n");
		}
		out.close();
	}

	protected String createFreqLine(ComFrequency f, LandAirport la) {
		String freqLine = f.getFrequency() + "," + la.getIcao_code() + "_"
				+ f.getDependency();
		com.grum.geocalc.Point p = calculatePosition(la);
		if (p != null) {
			freqLine = freqLine + "," + formatter.format(p.getLatitude()) + ","
					+ formatter.format(p.getLongitude());
		} else
			return null;

		return freqLine;
	}

	protected com.grum.geocalc.Point calculatePosition(LandAirport la) {

		if (la.getRunways().size() == 0)
			return null;
		if (la.getRunwayEndCount() < 2)
			return null;
		LandRunwayEnd rwy0 = la.getRunways().get(0).getEnds().get(0);
		LandRunwayEnd rwy1 = la.getRunways().get(0).getEnds().get(1);

		// Calculamos el punto medio de la primera pista disponible como
		// posición de referencia del aeropuerto
		double bearing = EarthCalc.getBearing(rwy0.getLocation(),
				rwy1.getLocation()); // in
										// decimal
										// degrees
		double distance = EarthCalc.getDistance(rwy0.getLocation(),
				rwy1.getLocation()); // in
										// meters

		// Aplicamos el desplazamiento a rwy0.getLocation()
		com.grum.geocalc.Point middle = (com.grum.geocalc.Point) EarthCalc
				.pointRadialDistance(rwy0.getLocation(), bearing, distance / 2);

		return middle;
	}
}
