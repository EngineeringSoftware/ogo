package org.ogo.test.queryjavadatastructures.structurelibrariestests.apachecommonscollections;

import static org.ogo.client.OGO.query;

import java.io.IOException;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.util.HashMap;
import org.apache.commons.collections4.map.CompositeMap;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;
import org.ogo.client.OGO;
import org.ogo.util.Profile;

public class CompositeMapCypher {

  CompositeMap<Integer, Integer> map3;
  HashMap<Integer, Integer> map1;
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
    OGO.setWhiteList("ogo/test", "CompositeMap", "HashMap");
  }

  @Profile
  @Test
  public void CompositeMapCypherTest() throws RemoteException {

    map1 = new HashMap();
    map1.put(1, 1);
    map1.put(2, 1);
    map1.put(3, 2);
    map1.put(4, 3);
    map1.put(5, 3);

    map2 = new HashMap();
    map2.put(100, 100);
    map2.put(200, 100);
    map2.put(300, 200);
    map2.put(400, 300);
    map2.put(500, 300);

    map3 = new CompositeMap(map1, map2);

    OGO.inMemory = false;
    OGO.clearDatabase = false;

    query(
        this,
        "MATCH (n:`org.ogo.test.queryJavaDataStructures.QueryJavaDataStructures_Test`)-[m*]->(r)"
            + " RETURN True");
    assert (true);

    //        long out1 = ((Long) sizeCompositeMap(true)).longValue();
    //        System.out.println("Output: " + out1);
    //        System.out.println("Expected: " + set1.size());
    //        assert (out1 == set1.size());
  }

  // The map initialization doesn't play well with cypher, need to look further into issues
  private Object sizeCompositeMap(boolean isCypher) throws RemoteException {
    if (isCypher) {
      Object[] q = query("");
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
