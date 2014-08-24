package org.airhispania.xplane2rc.util;

import java.io.File;
import java.io.FilenameFilter;
import java.util.List;

public class FilesFilter {

	public static File[] finder(String dirName, final String suffix) {
		File dir = new File(dirName);

		return dir.listFiles(new FilenameFilter() {
			public boolean accept(File dir, String filename) {
				return filename.endsWith(suffix);
			}
		});

	}

	public static void walk(String path, final String pattern, List<File> files) {

		File root = new File(path);
		File[] list = root.listFiles();

		for (File f : list) {
			if (f.isDirectory()) {
				walk(f.getAbsolutePath(), pattern, files);
			} else {
				if (f.getAbsolutePath().matches(pattern)) {
					files.add(f);
				}
			}
		}
	}

}