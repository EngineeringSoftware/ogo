package org.ogo.test.queryJavaDataStructures;

import static org.ogo.client.OGO.queryBool;
import static org.ogo.client.OGO.query;

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
          "MATCH ({$1})-[:elementData]->(m)-[*]->(n {value:" + item.intValue() + "}) RETURN True",
          vector);
    }
  }

  public static int indexOf(Vector vector, Integer item, boolean defaultMode)
      throws RemoteException {
    if (defaultMode) {
      return vector.indexOf(item);
    } else {
      Object result[] =
          query(
              "MATCH ({$1})-[:elementData]->(m)-[r]->(n {value:"
                  + item.intValue()
                  + "}) RETURN type(r)",
              vector);
      if (result != null && result.length > 0) {
        return Integer.parseInt((String) result[0]);
      } else {
        return -1;
      }
    }
  }
}
