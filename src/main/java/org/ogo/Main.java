package org.ogo;

import org.ogo.client.OGO;

public class Main {

  public static void main(String[] args) throws Exception {
    if (args.length != 1) {
      System.out.println("No query given");
      System.exit(1);
    }

    Thread.sleep(1000);
    System.out.println("abc");
    OGO.init();
    OGO.setWhiteList("java/lang/Object", "org.ogo.Main", "org/ogo/Main", "org/ogo", "org.ogo");
    OGO.clearDatabase = false;
    OGO.forceGC = true;
    OGO.followRoot = true;
    // OGO.whitelist = true;
    OGO.setExcludeFromBlackList("java/lang/Object");
    System.out.println("def");

    // public static Object[] query(Object root, String cQuery, Object... objects)
    Object obj = new Main();
    Object[] results = OGO.query(obj, "match (n) return count (n)");
    System.out.println(results);
    if (results != null) {
      System.out.println(results.length);
      for (Object o : results) {
        System.out.println(o);
      }
    }

    // String query = args[0];
    // System.out.println(query);
  }
}
