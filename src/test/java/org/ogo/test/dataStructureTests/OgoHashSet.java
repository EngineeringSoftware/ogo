package org.ogo.test.dataStructureTests;

import static org.ogo.client.OGO.queryBool;

import java.rmi.RemoteException;
import java.util.HashSet;

public abstract class OgoHashSet {

  public static boolean contains(HashSet set, Integer item, boolean defaultMode)
      throws RemoteException {
    if (defaultMode) {
      return set.contains(item);
    } else {
      return queryBool(
          set,
          "MATCH ({$1})-[:map]->(m)-[:table]->(t)-[*]->()-[:value]->(n {value:"
              + item.intValue()
              + "}) RETURN True",
          set);
    }
  }
}
