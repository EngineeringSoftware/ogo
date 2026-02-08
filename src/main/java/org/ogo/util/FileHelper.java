package org.ogo.util;

import java.io.File;
import java.io.FilenameFilter;
import java.util.logging.Level;
import java.util.logging.Logger;

public abstract class FileHelper {

  private static final Logger logger = Logger.getLogger(FileHelper.class.getName());

  /**
   * @brief Searches the directory specified by 'dirName' for files with extension specified by
   *     'endsWith'. Use this to search for *.csv files exported from native agent
   * @param dirName Directory to search files in
   * @param endsWith Extension of files to search for
   * @return File[] Array of File containing the files of specified extension
   * @since 1.0.0
   * @version 1.0.0
   */
  public static File[] findFilesWithExtension(String dirName, String endsWith) {
    File dir = new File(dirName);
    return dir.listFiles(
        new FilenameFilter() {
          public boolean accept(File dir, String filename) {
            return filename.endsWith(endsWith);
          }
        });
  }

  /**
   * @param cQuery Cypher query String
   * @param objects The objects passed as arg to replace with their hashcodes
   * @return modified cypher query string
   * @since 1.0.0
   * @version 1.0.0
   */
  public static String subArgWithHashCode(String cQuery, Object... objects) {
    for (int j = 1; j <= objects.length; j++) {
      logger.log(
          Level.FINE, "Hashcodes from Java are : " + System.identityHashCode(objects[j - 1]));
      cQuery = cQuery.replace("$" + j, "hash:" + System.identityHashCode(objects[j - 1]));
    }
    return cQuery;
  }
}
