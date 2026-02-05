package org.ogo.test.datastructuretests;

import static org.junit.jupiter.api.Assertions.*;
import static org.ogo.client.OGO.queryBool;

import edu.uci.ics.jung.graph.DirectedSparseGraph;
import edu.uci.ics.jung.graph.SparseGraph;
import edu.uci.ics.jung.graph.UndirectedSparseGraph;
import java.io.IOException;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.Vector;
import java.util.concurrent.ThreadLocalRandom;
import java.util.stream.Collectors;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.RepeatedTest;
import org.junit.jupiter.api.Test;
import org.ogo.client.OGO;

/**
 * @author 1sand0s
 */
public class DataStructureTest {

  LinkedList<Integer> list1;
  ArrayList<Integer> list2;
  ArrayDeque<Integer> dque1;
  Vector<Integer> vector1;
  HashMap<Integer, Integer> map1;
  SparseGraph<Integer, Integer> jungSparseGraph;
  DirectedSparseGraph<Integer, Integer> jungdSparseGraph;
  UndirectedSparseGraph<Integer, Integer> jungudSparseGraph;

  /**
   * regular
   *
   * @author 1sand0s
   * @since 1.0.0
   */
  @BeforeAll
  public static void initQueryEngine() throws InterruptedException, IOException, NotBoundException {
    Thread.sleep(5000);
    OGO.init();
    OGO.setWhiteList(
        "ogo/test",
        "java/util"); // OGO.setExcludeFromBlackList("java/lang/Byte","java/lang/Short","java/lang/Integer","java/lang/Long","java/lang/Float","java/lang/Double","java/lang/Character","java/lang/String","java/lang/Boolean","List","Map","Set","java/lang/Class","Array","LinkedList","Collection","Deque","Vector");
    //		OGO.setBlackList("jdk/","javax/","com/sun","sun/","Exception","maven","junit");
    OGO.clearDatabase = true;
    // OGO.printCSV=true;
    OGO.forceGC = true;
    OGO.followRoot = true;
    OGO.whitelist = true;
  }

  /**
   * regular
   *
   * @author 1sand0s
   * @since 1.0.0
   */
  @RepeatedTest(1)
  public void checkArrayDeque() throws RemoteException, InterruptedException {
    Thread.sleep(200);
    ThreadLocalRandom randGen = ThreadLocalRandom.current();
    int size = randGen.nextInt(10, 1000);
    dque1 =
        ((randGen.ints(size, 0, 100000)).boxed()).collect(Collectors.toCollection(ArrayDeque::new));
    Integer item = randGen.nextInt(100, 100000);
    boolean var1 =
        queryBool(
            this,
            "MATCH ({$1})-[:elements]->(m)-[*]->(n {value:" + item + "}) RETURN COUNT(n) <> 0",
            dque1);
    // boolean var1 = OgoArrayDeque.contains(dque1, item , false, this);
    boolean var2 = OgoArrayDeque.contains(dque1, item, true, this);
    assertEquals(var1, var2);
  }

  @Test
  public void checkArrayDeque2() throws RemoteException {
    dque1 = new ArrayDeque<>();
    dque1.add(1);
    boolean var1 = OgoArrayDeque.contains(dque1, 0, false, this);
    boolean var2 = OgoArrayDeque.contains(dque1, 0, true, this);

    assertFalse(var1);
    assertEquals(var1, var2);
    assertFalse(var2);
  }

  @Test
  public void checkArrayDeque3() throws RemoteException {
    dque1 = new ArrayDeque<>();
    dque1.add(1);
    // boolean var1 = OgoArrayDeque.contains(dque1, 0, false, this);
    // boolean var2 = OgoArrayDeque.contains(dque1, 0, true, this);

    boolean var3 = OgoArrayDeque.contains(dque1, 1, false, this);
    boolean var4 = OgoArrayDeque.contains(dque1, 1, true, this);

    // assertTrue(!var1);
    assertTrue(var3);
    // assertTrue(var1 == var2);
    assertEquals(var3, var4);
  }

  /**
   * regular
   *
   * @author 1sand0s
   * @since 1.0.0
   */
  @RepeatedTest(1)
  public void checkArrayList() throws RemoteException, InterruptedException {
    Thread.sleep(200);
    ThreadLocalRandom randGen = ThreadLocalRandom.current();
    int size = 10; // randGen.nextInt(10, 10);
    list2 =
        ((randGen.ints(size, 0, 100000)).boxed()).collect(Collectors.toCollection(ArrayList::new));
    Integer item = randGen.nextInt(100, 100000);
    boolean var1 =
        queryBool(
            this,
            "MATCH ({$1})-[:elementData]->(m)-[*]->(n {value:" + item + "}) RETURN COUNT(n) <> 0",
            list2);

    // boolean var1 = OgoArrayList.contains(list2, item , false, this);
    boolean var2 = OgoArrayList.contains(list2, item, true, this);
    assertEquals(var1, var2);
  }

  /**
   * regular
   *
   * @author 1sand0s
   * @since 1.0.0
   */
  @RepeatedTest(1)
  public void checkVector() throws RemoteException, InterruptedException {
    Thread.sleep(200);
    ThreadLocalRandom randGen = ThreadLocalRandom.current();
    int size = randGen.nextInt(10, 500);
    vector1 =
        ((randGen.ints(size, 0, 100000)).boxed()).collect(Collectors.toCollection(Vector::new));
    Integer item = randGen.nextInt(100, 100000);
    boolean var1 =
        queryBool(
            this,
            "MATCH ({$1})-[:elementData]->(m)-[*]->(n {value:" + item + "}) RETURN COUNT(n) <> 0",
            vector1);
    // boolean var1 = OgoLinkedList.contains(list1, item , false, this);
    boolean var2 = OgoVector.contains(vector1, item, true);
    assertEquals(var1, var2);
  }

  /**
   * regular
   *
   * @author 1sand0s
   * @since 1.0.0
   */
  @RepeatedTest(1)
  public void checkLinkedList() throws RemoteException, InterruptedException {
    Thread.sleep(200);
    ThreadLocalRandom randGen = ThreadLocalRandom.current();
    int size = 5;
    list1 =
        ((randGen.ints(size, 0, 100000)).boxed()).collect(Collectors.toCollection(LinkedList::new));
    Integer item = randGen.nextInt(100, 100000);
    boolean var1 =
        queryBool(
            this,
            "MATCH ({$1})-[:first]->(m)-[*]->(n {value:" + item + "}) RETURN COUNT(n) <> 0",
            list1);
    // boolean var1 = OgoLinkedList.contains(list1, item , false, this);
    boolean var2 = OgoLinkedList.contains(list1, item, true, this);
    assertEquals(var1, var2);
  }

  /**
   * regular
   *
   * @author 1sand0s
   * @since 1.0.0
   */
  @RepeatedTest(1)
  public void checkHashMap() throws RemoteException, InterruptedException {
    Thread.sleep(200);
    ThreadLocalRandom randGen = ThreadLocalRandom.current();
    int size = randGen.nextInt(10, 500);
    ArrayList<Integer> temp =
        ((randGen.ints(size, 0, 100000)).boxed()).collect(Collectors.toCollection(ArrayList::new));
    ArrayList<Integer> temp2 =
        ((randGen.ints(size, 0, 100000)).boxed()).collect(Collectors.toCollection(ArrayList::new));
    map1 = new HashMap<>();
    for (int j = 0; j < temp.size(); j++) {
      map1.put(temp.get(j), temp2.get(j));
    }
    Integer item = randGen.nextInt(100, 100000);
    boolean var1 =
        queryBool(
            this,
            "MATCH ({$1})-[:table]->(m)-[*]->()-[:key]->(n {value:"
                + item
                + "}) RETURN COUNT(n) <> 0",
            map1);
    // boolean var1 = OgoLinkedList.contains(list1, item , false, this);
    boolean var2 = OgoHashMap.containsKey(map1, item, true);
    assertEquals(var1, var2);
  }

  @Test
  public void checkHashSet() throws RemoteException, InterruptedException {
    Thread.sleep(200);
    ThreadLocalRandom randGen = ThreadLocalRandom.current();
    int size = randGen.nextInt(10, 500);
    HashSet<Integer> set =
        ((randGen.ints(size, 0, 100000)).boxed()).collect(Collectors.toCollection(HashSet::new));
    Integer item = randGen.nextInt(100000, 200000);
    boolean var1 =
        queryBool(
            this,
            "MATCH ({$1})-[:map]->(m)-[:table]->(t)-[*]->()-[:value]->(n {value:"
                + item
                + "}) RETURN COUNT(n) <> 0",
            set);
    boolean var2 = OgoHashSet.contains(set, item, true);
    assertEquals(var1, var2);
  }

  /**
   * regular
   *
   * @author 1sand0s
   * @since 1.0.0
   */
  @RepeatedTest(1)
  public void checkSparseGraph() throws RemoteException, InterruptedException {
    Thread.sleep(200);
    ThreadLocalRandom randGen = ThreadLocalRandom.current();
    int size = randGen.nextInt(10, 500);
    ArrayList<Integer> temp =
        ((randGen.ints(size, 0, 100000)).boxed()).collect(Collectors.toCollection(ArrayList::new));
    ArrayList<Integer> temp2 =
        ((randGen.ints(size, 0, 100000)).boxed()).collect(Collectors.toCollection(ArrayList::new));
    jungSparseGraph = new SparseGraph<>();
    for (int j = 0; j < temp.size(); j++) {
      jungSparseGraph.addVertex(temp.get(j));
      jungSparseGraph.addVertex(temp2.get(j));
      jungSparseGraph.addEdge(j, temp.get(j), temp2.get(j));
    }
    Integer item = randGen.nextInt(100, 100000);
    boolean var1 =
        queryBool(
            this,
            "MATCH ({$1})-[:undirected_edges]->()-[:table]->()-[:`"
                + item
                + "`]->(n) RETURN n IS NOT NULL",
            jungSparseGraph);
    boolean var2 = jungSparseGraph.containsEdge(item);
    assertEquals(var1, var2);
  }

  /**
   * regular
   *
   * @author 1sand0s
   * @since 1.0.0
   */
  @RepeatedTest(1)
  public void checkDirectedSparseGraph() throws RemoteException, InterruptedException {
    Thread.sleep(200);
    ThreadLocalRandom randGen = ThreadLocalRandom.current();
    int size = randGen.nextInt(10, 500);
    ArrayList<Integer> temp =
        ((randGen.ints(size, 0, 100000)).boxed()).collect(Collectors.toCollection(ArrayList::new));
    ArrayList<Integer> temp2 =
        ((randGen.ints(size, 0, 100000)).boxed()).collect(Collectors.toCollection(ArrayList::new));
    jungdSparseGraph = new DirectedSparseGraph<>();
    for (int j = 0; j < temp.size(); j++) {
      jungdSparseGraph.addVertex(temp.get(j));
      jungdSparseGraph.addVertex(temp2.get(j));
      jungdSparseGraph.addEdge(j, temp.get(j), temp2.get(j));
    }
    Integer item = randGen.nextInt(100, 100000);
    boolean var1 =
        queryBool(
            this,
            "MATCH ({$1})-[:edges]->()-[:table]->()-[:`" + item + "`]->(n) RETURN n IS NOT NULL",
            jungdSparseGraph);
    boolean var2 = jungdSparseGraph.containsEdge(item);
    assertEquals(var1, var2);
  }

  /**
   * regular
   *
   * @author 1sand0s
   * @since 1.0.0
   */
  @RepeatedTest(1)
  public void checkUndirectedSparseGraph() throws RemoteException, InterruptedException {
    Thread.sleep(200);
    ThreadLocalRandom randGen = ThreadLocalRandom.current();
    int size = randGen.nextInt(10, 500);
    ArrayList<Integer> temp =
        ((randGen.ints(size, 0, 100000)).boxed()).collect(Collectors.toCollection(ArrayList::new));
    ArrayList<Integer> temp2 =
        ((randGen.ints(size, 0, 100000)).boxed()).collect(Collectors.toCollection(ArrayList::new));
    jungudSparseGraph = new UndirectedSparseGraph<>();
    for (int j = 0; j < temp.size(); j++) {
      jungudSparseGraph.addVertex(temp.get(j));
      jungudSparseGraph.addVertex(temp2.get(j));
      jungudSparseGraph.addEdge(j, temp.get(j), temp2.get(j));
    }
    Integer item = randGen.nextInt(100, 100000);
    boolean var1 =
        queryBool(
            this,
            "MATCH ({$1})-[:edges]->()-[:table]->()-[:`" + item + "`]->(n) RETURN n IS NOT NULL",
            jungudSparseGraph);
    boolean var2 = jungudSparseGraph.containsEdge(item);
    assertEquals(var1, var2);
  }
}
