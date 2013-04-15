/**
 * 
 */
package org.airhispania.xplane2rc.transformer;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.util.ArrayList;
import java.util.List;

import org.airhispania.xplane2rc.apt.model.ComFrequency;
import org.airhispania.xplane2rc.apt.model.LandAirport;
import org.airhispania.xplane2rc.model.Scenery;

/**
 * @author Jose Manuel García Valladolid - josemanuelgv@gmail.com
 * 
 */
public class Apt2FrequenciesTransformer {

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
		for (Scenery s : customSceneries) {
			for (String airport : s.getAirports().keySet()) {
				LandAirport la = s.getAirports().get(airport);
				if (la.getFrequencies() != null)
					for (ComFrequency f : la.getFrequencies()) {
						String freqLine = f.getFrequency() + "," + airport
								+ "_" + f.getDependency();
						if (!frqProcessed.contains(freqLine))
							frqProcessed.add(freqLine);
					}
			}
		}

		// Iterate over default scenery
		for (String airport : defaultScenery.getAirports().keySet()) {
			LandAirport la = defaultScenery.getAirports().get(airport);
			if (la.getFrequencies() != null)
				for (ComFrequency f : la.getFrequencies()) {
					String freqLine = f.getFrequency() + "," + airport + "_"
							+ f.getDependency();
					if (!frqProcessed.contains(freqLine))
						frqProcessed.add(freqLine);
				}
		}

		BufferedWriter out = new BufferedWriter(new FileWriter(fFreq));
		for (String l : frqProcessed) {
			out.write(l + "\n");
		}
		out.close();
	}
}
