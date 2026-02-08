package org.ogo.test.createdatastructure.createbinarytree;

import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.ogo.client.OGO.query;
import static org.ogo.client.OGO.queryBool;

import java.io.IOException;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Disabled;
import org.junit.jupiter.api.Test;
import org.ogo.client.OGO;
import org.ogo.util.Profile;

/** */
@Disabled
public class BinaryTreeTest {

  /**
   * regular
   *
   * @since 1.0.0
   * @version 1.0.0
   */
  @BeforeAll
  public static void initQueryEngine()
      throws RemoteException, InterruptedException, IOException, NotBoundException {
    Thread.sleep(4000);
    OGO.init();
    OGO.setWhiteList("ogo/test");
  }

  /**
   * regular
   *
   * @since 1.0.0
   * @version 1.0.0
   */
  @Profile
  @Test
  public void addTwoNodes() throws RemoteException {
    String nodeClass = "`org.ogo.test.createDataStructure.createBinaryTree.Node`";
    OGO.inMemory = true;
    Object[] objects =
        query(
            "CREATE (a:"
                + nodeClass
                + " {value: 10}), (b:"
                + nodeClass
                + " {value: 20}), (c:"
                + nodeClass
                + " {value: 30}), (d:"
                + nodeClass
                + " {value: 40}), (e:"
                + nodeClass
                + " {value: 50}), (f:"
                + nodeClass
                + " {value: 60}) MERGE (f)<-[:right]-(e)<-[:right]-(c)-[:left]->(a)-[:right]->(b)"
                + " MERGE (e)-[:left]->(d) RETURN c");
    OGO.inMemory = false;
    Object[] result =
        query(
            "MATCH (n:"
                + nodeClass
                + " {value: 30})-[*2]->(m:"
                + nodeClass
                + " {value:40}) RETURN m.value");

    assertTrue(result.length == 1 && result[0] instanceof Integer && (Integer) result[0] == 40);
  }

  /**
   * regular
   *
   * @since 1.0.0
   * @version 1.0.0
   */
  @Profile
  @Test
  public void testInvariant() throws RemoteException {
    int[] arr = new int[] {1, 2, 3, 4, 5, 6, 7, 8, 9};
    BinaryTree tree = new BinaryTree();
    Node root = tree.createTree(arr, 0, arr.length);
    String nodeClass = "`org.ogo.test.createDataStructure.createBinaryTree.Node`";
    OGO.inMemory = false;
    assertTrue(
        queryBool(
            root,
            "MATCH (a)<-[:left]-(b:"
                + nodeClass
                + ") MATCH (c:"
                + nodeClass
                + ")-[:right]->(d) WITH COLLECT(a.value<b.value AND d.value>c.value) AS m RETURN"
                + " ALL(n in m WHERE n=true)"));
  }
}
