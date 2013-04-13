/**
 * 
 */
package org.airhispania.xplane2rc.transformer;

import java.io.File;

/**
 * @author Jose Manuel García Valladolid - josemanuelgv@gmail.com
 * 
 */
public class Apt2rcDiff implements IApt2rcTransformerListener {

	/**
	 * @param args
	 * @throws Exception
	 */
	public static void main(String[] args) throws Exception {

		if (args.length < 2)
			throw new Exception(
					"apt.dat and r4.csv file locations are needed as an arguments");

		// Testing code
		String aptFileLocation = args[0];
		String r4FileLocation = args[1];
		String icaoFilter = args[2];

		Apt2rcTransformer tr = new Apt2rcTransformer(new File(aptFileLocation),
				new File(r4FileLocation), new File(""));
		tr.registerListener(new Apt2rcDiff());
		tr.diff(icaoFilter);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * org.airhispania.xplane2rc.transformer.IApt2rcTransformerListener#doMessage
	 * (java.lang.String)
	 */
	public void doMessage(String m) {
		System.out.println(m);

	}

}
