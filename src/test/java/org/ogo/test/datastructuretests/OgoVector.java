package org.ogo.test.datastructuretests;

import static org.ogo.client.OGO.queryBool;
import static org.ogo.client.OGO.queryInt;

import java.rmi.RemoteException;
import java.util.Vector;

/**
 * @author 1sand0s
 */
public abstract class OgoVector {

  public static boolean contains(Vector vector, Integer item, boolean defaultMode)
      throws RemoteException {
    if (defaultMode) {
      return vector.contains(item);
    } else {
      return queryBool(
          vector,
          "MATCH ({$1})-[:elementData]->(m)-[*]->(n {value:" + item.intValue() + "}) RETURN True",
          vector);
    }
  }

  public static int indexOf(Vector vector, Integer item, boolean defaultMode)
      throws RemoteException {
    if (defaultMode) {
      return vector.indexOf(item);
    } else {
      return queryInt(
          vector,
          "MATCH ({$1})-[:elementData]->(m)-[*]->(n {value:"
              + item.intValue()
              + "}) RETURN n.`$ARRAY_INDEX`",
          vector);
    }
  }
}
