/**
 * 
 */
package org.airhispania.xplane2rc;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.airhispania.xplane2rc.model.Scenery;
import org.airhispania.xplane2rc.transformer.Apt2FrequenciesTransformer;
import org.airhispania.xplane2rc.transformer.Apt2rcTransformer;
import org.airhispania.xplane2rc.transformer.IApt2rcTransformerListener;
import org.airhispania.xplane2rc.util.FilesFilter;
import org.apache.log4j.Logger;

/**
 * @author Jose Manuel García Valladolid - josemanuelgv@gmail.com
 * 
 *         Clase principal que crea un fichero R4 a partir de las bases de datos
 *         APT de XPlane
 */
public class MakeR4 {

	protected static Logger logger = Logger.getLogger(MakeR4.class);

	private static final String BENDER9_R4_CSV = "\\r4.csv";
	private static final String FREQ_CSV = "\\frequencies.csv";

	private static final String VERSION = "0.3.1";

	/**
	 * @param args
	 * @throws IOException
	 */
	public static void main(String[] args) throws IOException {

		logger.info("XPlane2RC, xplane apt to Radar Contact R4 converter. Version = "
				+ VERSION);

		// String workDir = System.getProperty("user.dir");
		File currentDirectory = new File(new File(".").getAbsolutePath());
		String workDir = currentDirectory.getCanonicalPath();

		if (args != null) {
			if (args.length > 0) {
				workDir = args[0];
			}
		}

		logger.info("Working Directory = " + workDir);

		// 1. Búsqueda de bases de datos APT
		logger.info("Searching for APT databases ... ");
		List<File> temporalaptfiles = new ArrayList<File>();
		FilesFilter.walk(workDir, Scenery.APT_FILE_PATTERN, temporalaptfiles);

		List<File> aptfiles = new ArrayList<File>();
		// Descarte de APT's
		for (File f : temporalaptfiles) {
			// Eliminar los apt que esten deshabilitados por XAddonManager
			if (!f.getAbsolutePath().contains("(disabled)")
					&& !f.getAbsolutePath().contains("__MACOSX")) {
				aptfiles.add(f);
				logger.info("Found apt.dat: " + f.getAbsolutePath());
			}
		}

		if (aptfiles.size() == 0) {
			logger.info("There is not APT files. Exiting ..");
			return;
		}

		logger.info("Parsing found APT files ... ");
		List<Scenery> currentSceneries = new ArrayList<Scenery>();

		long airportsParsed = 0;
		for (File f : aptfiles) {
			Scenery s = new Scenery(f.getAbsolutePath(), f);
			try {
				airportsParsed += s.parse();
				if (s.getAirports().size() > 0)
					currentSceneries.add(s);
			} catch (Exception e) {
				logger.error("ERROR parsing apt file "
						+ s.getAptfile().getAbsolutePath() + ": "
						+ e.getMessage());
				logger.info("Process executed WITH errors. ");
				return;
			}
		}

		if (airportsParsed == 0) {
			logger.info("There is not airports in APT files. Exiting ..");
			return;
		}

		logger.info("Parsed " + airportsParsed + " airports.");

		// Default XPlane apt db
		File xplaneAptFile = null;
		List<File> customAptFiles = new ArrayList<File>();
		for (Scenery s : currentSceneries) {
			if (s.isDefaultXplaneDatabase())
				xplaneAptFile = s.getAptfile();
			else
				customAptFiles.add(s.getAptfile());
		}

		if (xplaneAptFile == null) {
			logger.warn("XPlane default apt file NULL!");
			return;
		}

		logger.info("Generating R4 file ... ");
		File rc4File = new File(currentDirectory + BENDER9_R4_CSV);
		logger.info("Target RC4 file location = " + rc4File.getAbsolutePath());

		try {

			Apt2rcTransformer transformer = new Apt2rcTransformer(
					xplaneAptFile, customAptFiles, new ArrayList<String>(),
					rc4File);
			transformer.registerListener(new IApt2rcTransformerListener() {

				public void doMessage(String m) {
					logger.debug(m);

				}

			});
			transformer.transform();
			for (String s : transformer.getStatistics()) {
				logger.info(s);
			}
		} catch (Exception e) {
			logger.error(e);
			logger.info("Process executed WITH errors. ");
			return;
		}

		logger.info("Generating Communication Frequencies file ... ");
		File freqFile = new File(currentDirectory + FREQ_CSV);
		logger.info("Target Frequencies file location = "
				+ freqFile.getAbsolutePath());
		try {
			new Apt2FrequenciesTransformer().transform(currentSceneries,
					freqFile);
			logger.info("Frequencies file generated successfully!");
		} catch (Exception e) {
			logger.error(e);
			logger.info("Process executed WITH errors. ");
			return;
		}
		logger.info("Process executed without errors. ");
	}

}
