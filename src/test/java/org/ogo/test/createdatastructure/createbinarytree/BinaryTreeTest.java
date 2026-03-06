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
import org.junit.jupiter.api.Disabled;
import org.junit.jupiter.api.Test;
import org.ogo.client.OGO;
import org.ogo.util.Profile;

// @Disabled
public class BinaryTreeTest {

    /**
     * @since 0.1.0
     *
     * @version 0.1.0
     */
    String nodeClass = "`" + Node.class.getName() + "`";

    @BeforeAll
    public static void initQueryEngine() throws RemoteException, InterruptedException, IOException, NotBoundException {
        Thread.sleep(4000);
        OGO.inMemory = false;
        OGO.init();
        OGO.setWhiteList("ogo/test");
    }

    /**
     * @since 0.1.0
     *
     * @version 0.1.0
     */
    @Disabled
    @Profile
    @Test
    public void addTwoNodes() throws RemoteException {
        OGO.inMemory = true;
        Object[] objects = query(String.format(
                "CREATE (a:%s {value: 10}), (b:%s {value: 20}), (c:%s {value: 30}), (d:%s {value: 40}), (e:%s {value: 50}), (f:%s {value: 60}) MERGE (f)<-[:right]-(e)<-[:right]-(c)-[:left]->(a)-[:right]->(b) MERGE (e)-[:left]->(d) RETURN c",
                nodeClass, nodeClass, nodeClass, nodeClass, nodeClass, nodeClass));
        OGO.inMemory = false;
        Object[] result = query(
                String.format("MATCH (n:%s {value: 30})-[*2]->(m:%s {value:40}) RETURN m.value", nodeClass, nodeClass));

        assertTrue(result.length == 1 && result[0] instanceof Integer && (Integer) result[0] == 40);
    }

    /**
     * @since 0.1.0
     *
     * @version 0.1.0
     */
    @Disabled
    @Profile
    @Test
    public void testInvariant() throws RemoteException {
        int[] arr = new int[] { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        BinaryTree tree = new BinaryTree();
        Node root = tree.createTree(arr, 0, arr.length);
        OGO.inMemory = false;
        assertTrue(queryBool(root, String.format(
                "MATCH (a)<-[:left]-(b:%s) MATCH (c:%s)-[:right]->(d) WITH COLLECT(a.value<b.value AND d.value>c.value) AS m RETURN ALL(n in m WHERE n=true)",
                nodeClass, nodeClass)));
    }

    @Test
    public void checkContains() throws RemoteException {
        int[] arr = new int[] { 1, 2, 3, 4, 5, 6, 7 };
        BinaryTree tree = new BinaryTree();
        Node root = tree.createTree(arr, 0, arr.length);
        // String nodeClass = "`org.ogo.test.createdatastructure.createbinarytree.Node`";
        assertTrue(queryBool(root, String.format("MATCH (n:%s {value: 4}) RETURN TRUE", nodeClass)));
    }

    @Test
    public void checkSize() throws RemoteException {
        int[] arr = new int[] { 1, 2, 3, 4, 5, 6, 7 };
        BinaryTree tree = new BinaryTree();
        Node root = tree.createTree(arr, 0, arr.length);
        // String nodeClass = "`org.ogo.test.createdatastructure.createbinarytree.Node`";
        assertEquals(7L, queryLong(root, String.format("MATCH (n:%s) RETURN COUNT(n)", nodeClass)));
    }

    @Test
    public void checkLeafCount() throws RemoteException {
        // Leaves in {1,2,3,4,5,6,7} tree are: 1, 3, 5, 7 → 4 leaves
        int[] arr = new int[] { 1, 2, 3, 4, 5, 6, 7 };
        BinaryTree tree = new BinaryTree();
        Node root = tree.createTree(arr, 0, arr.length);
        // String nodeClass = "`org.ogo.test.createdatastructure.createbinarytree.Node`";
        assertEquals(4L, queryLong(root, String
                .format("MATCH (n:%s) WHERE NOT (n)-[:left]->() AND NOT (n)-[:right]->() RETURN COUNT(n)", nodeClass)));
    }

    @Test
    public void checkBSTInvariant() throws RemoteException {
        // Every left child must be smaller than its parent, every right child must be larger
        int[] arr = new int[] { 1, 2, 3, 4, 5, 6, 7 };
        BinaryTree tree = new BinaryTree();
        Node root = tree.createTree(arr, 0, arr.length);
        // String nodeClass = "`org.ogo.test.createdatastructure.createbinarytree.Node`";
        assertTrue(queryBool(root, String.format(
                "MATCH (a:%s)<-[:left]-(b:%s) MATCH (c:%s)-[:right]->(d:%s) WITH COLLECT(a.value<b.value AND d.value>c.value) AS m RETURN ALL(n in m WHERE n=true)",
                nodeClass, nodeClass, nodeClass, nodeClass)));
    }

    @Test
    public void checkParent() throws RemoteException {
        // Parent of node with value 2 should be the root node with value 4
        int[] arr = new int[] { 1, 2, 3, 4, 5, 6, 7 };
        BinaryTree tree = new BinaryTree();
        Node root = tree.createTree(arr, 0, arr.length);
        // String nodeClass = "`org.ogo.test.createdatastructure.createbinarytree.Node`";
        Object[] result = query(root, String.format(
                "MATCH (parent:%s)-[:left|right]->(child:%s {value: 2}) RETURN parent.value", nodeClass, nodeClass));
        assertEquals(1, result.length, "Expected exactly one parent");
        assertEquals(4, result[0], "Parent of node 2 should be node 4");
    }

    @Test
    public void checkDepth() throws RemoteException {
        // Shortest path from root (value=4) to leaf (value=1) should be 2 hops: 4->2->1
        int[] arr = new int[] { 1, 2, 3, 4, 5, 6, 7 };
        BinaryTree tree = new BinaryTree();
        Node root = tree.createTree(arr, 0, arr.length);
        // String nodeClass = "`org.ogo.test.createdatastructure.createbinarytree.Node`";
        assertEquals(2L,
                queryLong(root,
                        String.format("MATCH p=shortestPath((r:%s {value: 4})-[*]->(n:%s {value: 1})) RETURN length(p)",
                                nodeClass, nodeClass)));
    }
}
