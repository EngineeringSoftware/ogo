package org.ogo.test.queryjavadatastructures.structurelibrariestests.googleguava;

import static org.ogo.client.OGO.query;

import com.google.common.collect.TreeMultiset;
import java.io.IOException;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;
import org.ogo.client.OGO;

public class TreeMultisetCypherTest {

  TreeMultiset<Integer> set1;

  /**
   * @version 1.0.0
   * @since 1.0.0
   */
  @BeforeAll
  public static void initQueryEngine()
      throws RemoteException, InterruptedException, IOException, NotBoundException {
    Thread.sleep(5000);
    OGO.init();
    OGO.setWhiteList("ogo/test", "TreeMultiset");
  }

  @Test
  public void testTreeMultiset() throws RemoteException {

    set1 = TreeMultiset.create();
    //        for(int i = 0; i < 100; i++){
    //            set1.add(i, 1+(i%3));
    //        }

    set1.add(2, 11);
    set1.add(1, 1);
    set1.add(4, 1);
    set1.add(5, 2);
    set1.add(3, 7);

    OGO.inMemory = false;

    long out2 = ((Long) sizeTreeMultiset(true)).longValue();
    //        System.out.println("Output: " + out2);
    //        System.out.println("Expected: " + set1.size());
    assert (out2 == set1.size());
  }

  private Object sizeTreeMultiset(boolean isCypher) throws RemoteException {
    if (isCypher) {
      Object[] q =
          query("MATCH ()-[:header]->()-[:succ*]->(n) " + "RETURN SUM(n.elemCount)-1 as size");
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
