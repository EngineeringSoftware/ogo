package org.ogo.test.queryjavadatastructures.structurelibrariestests.googleguava;

import static org.ogo.client.OGO.query;

import com.google.common.collect.MinMaxPriorityQueue;
import java.io.IOException;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Disabled;
import org.junit.jupiter.api.Test;
import org.ogo.client.OGO;

@Disabled
public class MinMaxPriorityQueueCypherTest {

  MinMaxPriorityQueue<Integer> queue1;

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
    OGO.setWhiteList("ogo/test", "MinMaxPriorityQueue");
  }

  @Test
  public void testMinMaxPriorityQueueCypher() throws RemoteException {

    queue1 = MinMaxPriorityQueue.create();
    for (int i = 0; i < 100; i++) {
      queue1.add((i + 1) * 11);
    }

    OGO.inMemory = false;

    //        query("MATCH
    // (n:`org.ogo.test.queryJavaDataStructures.QueryJavaDataStructures_Test`)-[m*]->(r) RETURN
    // True");
    //        assert(true);

    long out1 = ((Long) sizeMinMaxPriorityQueue(true)).longValue();
    System.out.println("Output: " + out1);
    System.out.println("Expected: " + queue1.size());
    assert (out1 == queue1.size());
  }

  private Object sizeMinMaxPriorityQueue(boolean isCypher) throws RemoteException {
    if (isCypher) {
      Object[] q = query("MATCH ()-[:`queue`]->()-[n]->() " + "RETURN COUNT(n) AS size");
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
