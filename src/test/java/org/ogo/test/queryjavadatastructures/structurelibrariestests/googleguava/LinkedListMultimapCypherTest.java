package org.ogo.test.queryjavadatastructures.structurelibrariestests.googleguava;

import static org.ogo.client.OGO.query;

import com.google.common.collect.LinkedListMultimap;
import java.io.IOException;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;
import org.ogo.client.OGO;

public class LinkedListMultimapCypherTest {

  LinkedListMultimap<Integer, Integer> map1;

  /**
   * regular
   *
   * @author 1sand0s
   * @version 1.0.0
   * @since 1.0.0
   */
  @BeforeAll
  public static void initQueryEngine()
      throws RemoteException, InterruptedException, IOException, NotBoundException {
    Thread.sleep(5000);
    OGO.init();
    OGO.setWhiteList("ogo/test", "LinkedListMultimap");
  }

  @Test
  public void testLinkedListMultimapCypher() throws RemoteException {

    map1 = LinkedListMultimap.create();
    //        for(int i = 0; i < 10000; i++){
    //            map1.put((i*13)%7, (i*473)%11);
    //        }

    OGO.inMemory = false;

    long out1 = ((Long) sizeLinkedListMultimap(true)).longValue();
    System.out.println("Output: " + out1);
    System.out.println("Expected: " + map1.size());
    assert (out1 == map1.size());
  }

  private Object sizeLinkedListMultimap(boolean isCypher) throws RemoteException {
    if (isCypher) {
      Object[] q = query("MATCH ()-[:head]->()-[:next*]-(r) " + "RETURN COUNT(DISTINCT r) as size");
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
