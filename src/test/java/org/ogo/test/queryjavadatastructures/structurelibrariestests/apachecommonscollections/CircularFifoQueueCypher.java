package org.ogo.test.queryjavadatastructures.structurelibrariestests.apachecommonscollections;

import static org.ogo.client.OGO.query;

import java.io.IOException;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import org.apache.commons.collections4.queue.CircularFifoQueue;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;
import org.ogo.client.OGO;
import org.ogo.util.Profile;

public class CircularFifoQueueCypher {

  CircularFifoQueue<Integer> queue1;

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
    OGO.setWhiteList("ogo/test", "CircularFifoQueue");
  }

  @Profile
  @Test
  public void CircularFifoQueueCypherTest() throws RemoteException {

    queue1 = new CircularFifoQueue(10);
    for (int i = 0; i < 10; i++) {
      queue1.add(i);
    }

    if (!queue1.add(100)) {
      System.out.println(queue1.remove());
      queue1.add(100);
    }

    if (!queue1.add(222)) {
      System.out.println(queue1.remove());
      queue1.add(222);
    }

    OGO.inMemory = false;
    OGO.clearDatabase = false;

    //        query("MATCH
    // (n:`org.ogo.test.queryJavaDataStructures.QueryJavaDataStructures_Test`)-[m*]->(r) RETURN
    // True");
    //        assert(true);

    long out1 = ((Long) sizeCircularFifoQueue(true)).longValue();
    System.out.println("Output: " + out1);
    System.out.println("Expected: " + queue1.size());
    assert (out1 == queue1.size());
  }

  private Object sizeCircularFifoQueue(boolean isCypher) throws RemoteException {
    if (isCypher) {
      Object[] q =
          query(
              "MATCH ()-[:elements]->()-[n]->() "
                  + "WHERE type(n) <> 'instanceof' "
                  + "RETURN COUNT(n) AS size");
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
