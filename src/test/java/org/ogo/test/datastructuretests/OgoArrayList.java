package org.ogo.test.datastructuretests;

import static org.ogo.client.OGO.queryBool;
import static org.ogo.client.OGO.queryInt;

import java.rmi.RemoteException;
import java.util.ArrayList;

/**
 * @author 1sand0s
 */
public abstract class OgoArrayList {

  public static boolean contains(
      ArrayList list, Integer item, boolean defaultMode, DataStructureTest test)
      throws RemoteException {
    if (defaultMode) {
      return list.contains(item);
    } else {
      return queryBool(
          test,
          "MATCH ({$1})-[:elementData]->(m)-[*]->(n {value:" + item.intValue() + "}) RETURN True",
          list);
    }
  }

  public static int indexOf(ArrayList list, Integer item, boolean defaultMode)
      throws RemoteException {
    if (defaultMode) {
      return list.indexOf(item);
    } else {
      return queryInt(
          list,
          "MATCH ({$1})-[:elementData]->(m)-[*]->(n {value:"
              + item.intValue()
              + "}) RETURN n.`$ARRAY_INDEX`",
          list);
    }
  }
}
