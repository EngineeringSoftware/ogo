package org.ogo.test.createdatastructure.createbinarytree;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.ogo.client.OGO.query;
import static org.ogo.client.OGO.queryBool;
import static org.ogo.client.OGO.queryLong;

import java.io.IOException;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;
import org.ogo.client.OGO;

public class NewBinaryTreeTest {

  @BeforeAll
  public static void initQueryEngine()
      throws RemoteException, InterruptedException, IOException, NotBoundException {
    Thread.sleep(4000);
    OGO.inMemory = false;
    OGO.init();
    OGO.setWhiteList("ogo/test");
  }

  @Test
  public void checkContains() throws RemoteException {
    int[] arr = new int[] {1, 2, 3, 4, 5, 6, 7};
    BinaryTree tree = new BinaryTree();
    Node root = tree.createTree(arr, 0, arr.length);
    String nodeClass = "`org.ogo.test.createdatastructure.createbinarytree.Node`";
    assertTrue(queryBool(root, "MATCH (n:" + nodeClass + " {value: 4}) RETURN TRUE"));
  }

  @Test
  public void checkSize() throws RemoteException {
    int[] arr = new int[] {1, 2, 3, 4, 5, 6, 7};
    BinaryTree tree = new BinaryTree();
    Node root = tree.createTree(arr, 0, arr.length);
    String nodeClass = "`org.ogo.test.createdatastructure.createbinarytree.Node`";
    assertEquals(7L, queryLong(root, "MATCH (n:" + nodeClass + ") RETURN COUNT(n)"));
  }

  @Test
  public void checkLeafCount() throws RemoteException {
    // Leaves in {1,2,3,4,5,6,7} tree are: 1, 3, 5, 7 → 4 leaves
    int[] arr = new int[] {1, 2, 3, 4, 5, 6, 7};
    BinaryTree tree = new BinaryTree();
    Node root = tree.createTree(arr, 0, arr.length);
    String nodeClass = "`org.ogo.test.createdatastructure.createbinarytree.Node`";
    assertEquals(
        4L,
        queryLong(
            root,
            "MATCH (n:"
                + nodeClass
                + ") WHERE NOT (n)-[:left]->() AND NOT (n)-[:right]->() RETURN COUNT(n)"));
  }

  @Test
  public void checkBSTInvariant() throws RemoteException {
    // Every left child must be smaller than its parent, every right child must be larger
    int[] arr = new int[] {1, 2, 3, 4, 5, 6, 7};
    BinaryTree tree = new BinaryTree();
    Node root = tree.createTree(arr, 0, arr.length);
    String nodeClass = "`org.ogo.test.createdatastructure.createbinarytree.Node`";
    assertTrue(
        queryBool(
            root,
            "MATCH (a:"
                + nodeClass
                + ")<-[:left]-(b:"
                + nodeClass
                + ") MATCH (c:"
                + nodeClass
                + ")-[:right]->(d:"
                + nodeClass
                + ") WITH COLLECT(a.value<b.value AND d.value>c.value) AS m RETURN ALL(n in m WHERE n=true)"));
  }

  @Test
  public void checkParent() throws RemoteException {
    // Parent of node with value 2 should be the root node with value 4
    int[] arr = new int[] {1, 2, 3, 4, 5, 6, 7};
    BinaryTree tree = new BinaryTree();
    Node root = tree.createTree(arr, 0, arr.length);
    String nodeClass = "`org.ogo.test.createdatastructure.createbinarytree.Node`";
    Object[] result =
        query(
            root,
            "MATCH (parent:"
                + nodeClass
                + ")-[:left|right]->(child:"
                + nodeClass
                + " {value: 2}) RETURN parent.value");
    assertEquals(1, result.length, "Expected exactly one parent");
    assertEquals(4, result[0], "Parent of node 2 should be node 4");
  }

  @Test
  public void checkDepth() throws RemoteException {
    // Shortest path from root (value=4) to leaf (value=1) should be 2 hops: 4->2->1
    int[] arr = new int[] {1, 2, 3, 4, 5, 6, 7};
    BinaryTree tree = new BinaryTree();
    Node root = tree.createTree(arr, 0, arr.length);
    String nodeClass = "`org.ogo.test.createdatastructure.createbinarytree.Node`";
    assertEquals(
        2L,
        queryLong(
            root,
            "MATCH p=shortestPath((r:"
                + nodeClass
                + " {value: 4})-[*]->(n:"
                + nodeClass
                + " {value: 1})) RETURN length(p)"));
  }
}
