package org.ogo.test.queryJavaDataStructures;

import static org.ogo.client.OGO.queryBool;
import static org.ogo.client.OGO.query;

import java.rmi.RemoteException;
import java.util.ArrayList;

/**
 * @author 1sand0s
 */
public abstract class OgoArrayList {

  public static boolean contains(ArrayList list, Integer item, boolean defaultMode)
      throws RemoteException {
    if (defaultMode) {
      return list.contains(item);
    } else {
      return queryBool(
          "MATCH ({$1})-[:elementData]->(m)-[*]->(n {value:" + item.intValue() + "}) RETURN True",
          list);
    }
  }

  public static int indexOf(ArrayList list, Integer item, boolean defaultMode)
      throws RemoteException {
    if (defaultMode) {
      return list.indexOf(item);
    } else {
      Object result[] =
          query(
              "MATCH ({$1})-[:elementData]->(m)-[r]->(n {value:"
                  + item.intValue()
                  + "}) RETURN type(r)",
              list);
      if (result != null && result.length > 0) return Integer.parseInt((String) result[0]);
      else return -1;
    }
  }
}
