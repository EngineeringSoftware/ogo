package org.ogo.test.dataStructureTests;

import static org.ogo.client.OGO.queryBool;

import java.rmi.RemoteException;
import java.util.ArrayDeque;

/**
 * @author 1sand0s
 */
public abstract class OgoArrayDeque {

  public static boolean contains(
      ArrayDeque deque, Integer item, boolean defaultMode, DataStructureTest test)
      throws RemoteException {
    if (defaultMode) {
      return deque.contains(item);
    } else {
      return queryBool(
          test,
          "MATCH ({$1})-[:elements]->(m)-[*]->(n {value:"
              + item.intValue()
              + "}) RETURN COUNT(n) <> 0",
          deque);
    }
  }
}
