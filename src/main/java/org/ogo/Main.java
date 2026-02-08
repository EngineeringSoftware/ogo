package org.ogo;

import java.util.Arrays;
import java.util.logging.Logger;
import org.ogo.client.OGO;

public class Main {

  private static final Logger logger = Logger.getLogger(Main.class.getName());

  public static void main(String[] args) throws Exception {
    if (args.length != 1) {
      logger.severe("No query given");
      System.exit(1);
    }

    Thread.sleep(1000);
    logger.info("Initializing OGO");
    OGO.init();
    OGO.setWhiteList("java/lang/Object", "org.ogo.Main", "org/ogo/Main", "org/ogo", "org.ogo");
    OGO.clearDatabase = false;
    OGO.forceGC = true;
    OGO.followRoot = true;
    // OGO.whitelist = true;
    OGO.setExcludeFromBlackList("java/lang/Object");

    // public static Object[] query(Object root, String cQuery, Object... objects)
    Object obj = new Main();
    Object[] results = OGO.query(obj, "match (n) return count (n)");
    logger.info("Results: " + Arrays.toString(results));
    if (results != null) {
      logger.info("Result count: " + results.length);
      for (Object o : results) {
        logger.info(String.valueOf(o));
      }
    }

    // String query = args[0];
    // logger.info(query);
  }
}
