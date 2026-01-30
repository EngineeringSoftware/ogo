package org.ogo.test.queryjavadatastructures;

import static org.ogo.client.OGO.queryBool;

import java.rmi.RemoteException;
import java.util.HashMap;

/**
 * @author 1sand0s
 */
public abstract class OgoHashMap {

  public static boolean containsValue(HashMap map, Integer item, boolean defaultMode)
      throws RemoteException {
    if (defaultMode) {
      return map.containsValue(item);
    } else {
      return queryBool(
          "MATCH ({$1})-[:table]->(m)-[*]->()-[:value]->(n {value:"
              + item.intValue()
              + "}) RETURN True",
          map);
    }
  }

  public static boolean containsKey(HashMap map, Integer item, boolean defaultMode)
      throws RemoteException {
    if (defaultMode) {
      return map.containsKey(item);
    } else {
      return queryBool(
          "MATCH ({$1})-[:table]->(m)-[*]->()-[:key]->(n {value:"
              + item.intValue()
              + "}) RETURN True",
          map);
    }
  }
}
