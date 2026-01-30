package org.ogo.test.queryjavadatastructures;

import static org.junit.jupiter.api.Assertions.assertTrue;

import java.io.IOException;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Vector;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Disabled;
import org.junit.jupiter.api.Test;
import org.ogo.client.OGO;

/**
 * @author 1sand0s
 */
@Disabled
public class QueryJavaDataStructuresTest {

  LinkedList<Integer> list1;
  ArrayList<Integer> list2;
  ArrayDeque<Integer> dque1;
  Vector<Integer> vector1;
  HashMap<Integer, Integer> map1;

  /**
   * regular
   *
   * @author 1sand0s
   * @since 1.0.0
   * @version 1.0.0
   */
  @BeforeAll
  public static void initQueryEngine()
      throws RemoteException, InterruptedException, IOException, NotBoundException {
    Thread.sleep(5000);
    OGO.init();
    OGO.setWhiteList("ogo/test", "LinkedList", "ArrayList", "ArrayDeque", "Vector", "HashMap");
  }

  /**
   * regular
   *
   * @author 1sand0s
   * @since 1.0.0
   * @version 1.0.0
   */
  @Test
  public void checkLinkedList() throws RemoteException {
    list1 = new LinkedList<Integer>();
    list1.add(new Integer(10));
    list1.add(new Integer(20));
    list1.add(new Integer(30));

    OGO.inMemory = false;
    Integer a = new Integer(20);
    Integer b = new Integer(40);

    // check contains
    assertTrue(OgoLinkedList.contains(list1, a, false) == OgoLinkedList.contains(list1, a, true));
    assertTrue(OgoLinkedList.contains(list1, b, false) == OgoLinkedList.contains(list1, b, true));

    // check indexOf
    assertTrue(OgoLinkedList.indexOf(list1, a, false) == OgoLinkedList.indexOf(list1, a, true));
    assertTrue(OgoLinkedList.indexOf(list1, b, false) == OgoLinkedList.indexOf(list1, b, true));
  }

  /**
   * regular
   *
   * @author 1sand0s
   * @since 1.0.0
   * @version 1.0.0
   */
  @Test
  public void checkArrayList() throws RemoteException {
    list2 = new ArrayList<Integer>();
    list2.add(new Integer(10));
    list2.add(new Integer(20));
    list2.add(new Integer(30));

    OGO.inMemory = false;
    Integer a = new Integer(20);
    Integer b = new Integer(40);

    // check contains
    assertTrue(OgoArrayList.contains(list2, a, false) == OgoArrayList.contains(list2, a, true));
    assertTrue(OgoArrayList.contains(list2, b, false) == OgoArrayList.contains(list2, b, true));

    // check indexOf
    assertTrue(OgoArrayList.indexOf(list2, a, false) == OgoArrayList.indexOf(list2, a, true));
    assertTrue(OgoArrayList.indexOf(list2, b, false) == OgoArrayList.indexOf(list2, b, true));
  }

  /**
   * regular
   *
   * @author 1sand0s
   * @since 1.0.0
   * @version 1.0.0
   */
  @Test
  public void checkArrayDeque() throws RemoteException {
    dque1 = new ArrayDeque<Integer>();
    dque1.add(new Integer(10));
    dque1.add(new Integer(20));
    dque1.add(new Integer(30));

    OGO.inMemory = false;
    Integer a = new Integer(20);
    Integer b = new Integer(40);

    // check contains
    assertTrue(OgoArrayDeque.contains(dque1, a, false) == OgoArrayDeque.contains(dque1, a, true));
    assertTrue(OgoArrayDeque.contains(dque1, b, false) == OgoArrayDeque.contains(dque1, b, true));
  }

  /**
   * regular
   *
   * @author 1sand0s
   * @since 1.0.0
   * @version 1.0.0
   */
  @Test
  public void checkVector() throws RemoteException {
    vector1 = new Vector<Integer>();
    vector1.add(new Integer(10));
    vector1.add(new Integer(20));
    vector1.add(new Integer(30));

    OGO.inMemory = false;
    Integer a = new Integer(20);
    Integer b = new Integer(40);

    // check contains
    assertTrue(OgoVector.contains(vector1, a, false) == OgoVector.contains(vector1, a, true));
    assertTrue(OgoVector.contains(vector1, b, false) == OgoVector.contains(vector1, b, true));

    // check indexOf
    assertTrue(OgoVector.indexOf(vector1, a, false) == OgoVector.indexOf(vector1, a, true));
    assertTrue(OgoVector.indexOf(vector1, b, false) == OgoVector.indexOf(vector1, b, true));
  }

  /**
   * regular
   *
   * @author 1sand0s
   * @since 1.0.0
   * @version 1.0.0
   */
  @Test
  public void checkHashMap() throws RemoteException {
    map1 = new HashMap<Integer, Integer>();
    map1.put(new Integer(10), new Integer(100));
    map1.put(new Integer(20), new Integer(200));
    map1.put(new Integer(30), new Integer(300));

    OGO.inMemory = false;
    Integer a = new Integer(20);
    Integer b = new Integer(200);

    // check containsKey
    assertTrue(OgoHashMap.containsKey(map1, a, false) == OgoHashMap.containsKey(map1, a, true));
    assertTrue(OgoHashMap.containsKey(map1, b, false) == OgoHashMap.containsKey(map1, b, true));

    // check containsValue
    assertTrue(OgoHashMap.containsValue(map1, a, false) == OgoHashMap.containsValue(map1, a, true));
    assertTrue(OgoHashMap.containsValue(map1, b, false) == OgoHashMap.containsValue(map1, b, true));
  }
}
