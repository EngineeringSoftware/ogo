package org.ogo.test.queryjavadatastructures.structurelibrariestests.apachecommonscollections;

import static org.ogo.client.OGO.query;

import java.io.IOException;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.util.HashMap;
import org.apache.commons.collections4.bidimap.DualHashBidiMap;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;
import org.ogo.client.OGO;
import org.ogo.util.Profile;

public class DualHashBidiMapCypher {

  DualHashBidiMap<Integer, Integer> map1;

  HashMap<Integer, Integer> map2;

  /**
   * @version 1.0.0
   * @since 1.0.0
   */
  @BeforeAll
  public static void initQueryEngine()
      throws RemoteException, InterruptedException, IOException, NotBoundException {
    Thread.sleep(5000);
    OGO.init();
    OGO.setWhiteList("ogo/test", "DualHashBidiMap", "HashMap");
  }

  @Profile
  @Test
  public void DualHashBidiMapCypherTest() throws RemoteException {

    map2 = new HashMap();

    //        map2.put(1, 156);
    //        map2.put(2, 237);
    //        map2.put(3, 385);
    //        map2.put(4, 491);
    //        map2.put(5, 512);

    map1 = new DualHashBidiMap(/*map2*/ );

    map1.put(1, 156);
    map1.put(2, 237);
    map1.put(3, 385);
    map1.put(4, 491);
    map1.put(5, 512);
    map1.put(7, 8473);

    OGO.inMemory = false;
    OGO.clearDatabase = false;

    //        query("MATCH
    // (n:`org.ogo.test.queryJavaDataStructures.QueryJavaDataStructures_Test`)-[m*]->(r) RETURN
    // True");
    //        assert(true);

    long out1 = ((Long) sizeDualHashBidiMap(true)).longValue();
    System.out.println("Output: " + out1);
    System.out.println("Expected: " + map1.size());
    assert (out1 == map1.size());
  }

  private Object sizeDualHashBidiMap(boolean isCypher) throws RemoteException {
    if (isCypher) {
      Object[] q =
          query(
              "MATCH"
                  + " (:`org.apache.commons.collections4.bidimap.DualHashBidiMap`)-[*]->()-[:value]->(n)"
                  + " RETURN COUNT(DISTINCT n)/2 as size");
      return q[0];
    } /*else{
          return (Object) this.size();
      }*/
    return null;
  }

  /*private Object valueAtIndexLL(int idx) throws RemoteException {
      Object [] q = query("MATCH (n:`org.ogo.test.queryJavaDataStructures.QueryJavaDataStructures_Test`)-[m]->(i:`java.util.Linke>
      return q[0];
  }*/
}
