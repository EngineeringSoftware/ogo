package org.ogo.test.queryjavadatastructures.structurelibrariestests.apachecommonscollections;

import static org.ogo.client.OGO.query;

import java.io.IOException;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import org.apache.commons.collections4.list.GrowthList;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;
import org.ogo.client.OGO;
import org.ogo.util.Profile;

public class GrowthListCypher {

  GrowthList<Integer> list1;

  /**
   * regular
   *
   * @version 1.0.0
   * @since 1.0.0
   */
  @BeforeAll
  public static void initQueryEngine()
      throws RemoteException, InterruptedException, IOException, NotBoundException {
    Thread.sleep(5000);
    OGO.init();
    OGO.setWhiteList("ogo/test", "GrowthList");
  }

  @Profile
  @Test
  public void GrowthListCypherTest() throws RemoteException {

    list1 = new GrowthList();
    for (int i = 0; i < 3; i++) {
      list1.add(i * 2, i);
    }
    //        for(int i = 0; i < 223; i++){
    //            list1.add((i+2)*31-3, (i-365)*22);
    //        }

    OGO.inMemory = false;
    OGO.clearDatabase = false;

    //        query("MATCH
    // (n:`org.ogo.test.queryJavaDataStructures.QueryJavaDataStructures_Test`)-[m*]->(r) RETURN
    // True");
    //        assert(true);

    long out1 = ((Long) sizeGrowthList(true)).longValue();
    System.out.println("Output: " + out1);
    System.out.println("Expected: " + list1.size());
    assert (out1 == list1.size());
  }

  private Object sizeGrowthList(boolean isCypher) throws RemoteException {
    if (isCypher) {
      Object[] q =
          query(
              "MATCH (:`org.apache.commons.collections4.list.GrowthList`)-->()-->()-[n]->() "
                  + "WHERE type(n) <> 'instanceof' "
                  + "RETURN "
                  + "CASE "
                  + "    WHEN COUNT(n) <> 0 THEN toInteger(type(max(n)))+1 "
                  + "    ELSE 0 "
                  + "END as size");
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
