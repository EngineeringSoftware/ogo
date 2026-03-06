package org.ogo.test.queryjavadatastructures;

import static org.junit.jupiter.api.Assertions.assertEquals;

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

@Disabled
public class QueryJavaDataStructuresTest {

    LinkedList<Integer> list1;
    ArrayList<Integer> list2;
    ArrayDeque<Integer> dque1;
    Vector<Integer> vector1;
    HashMap<Integer, Integer> map1;

    /**
     * @since 0.1.0
     */
    @BeforeAll
    public static void initQueryEngine() throws InterruptedException, IOException, NotBoundException {
        Thread.sleep(5000);
        OGO.init();
        OGO.setWhiteList("ogo/test", "LinkedList", "ArrayList", "ArrayDeque", "Vector", "HashMap");
    }

    /**
     * @since 0.1.0
     */
    @Test
    public void checkLinkedList() throws RemoteException {
        list1 = new LinkedList<>();
        list1.add(10);
        list1.add(20);
        list1.add(30);

        OGO.inMemory = false;
        Integer a = 20;
        Integer b = 40;

        assertEquals(OgoLinkedList.contains(list1, a, false), OgoLinkedList.contains(list1, a, true));
        assertEquals(OgoLinkedList.contains(list1, b, false), OgoLinkedList.contains(list1, b, true));

        assertEquals(OgoLinkedList.indexOf(list1, a, false), OgoLinkedList.indexOf(list1, a, true));
        assertEquals(OgoLinkedList.indexOf(list1, b, false), OgoLinkedList.indexOf(list1, b, true));
    }

    /**
     * @since 0.1.0
     */
    @Test
    public void checkArrayList() throws RemoteException {
        list2 = new ArrayList<>();
        list2.add(10);
        list2.add(20);
        list2.add(30);

        OGO.inMemory = false;
        Integer a = 20;
        Integer b = 40;

        assertEquals(OgoArrayList.contains(list2, a, false), OgoArrayList.contains(list2, a, true));
        assertEquals(OgoArrayList.contains(list2, b, false), OgoArrayList.contains(list2, b, true));

        assertEquals(OgoArrayList.indexOf(list2, a, false), OgoArrayList.indexOf(list2, a, true));
        assertEquals(OgoArrayList.indexOf(list2, b, false), OgoArrayList.indexOf(list2, b, true));
    }

    /**
     * @since 0.1.0
     */
    @Test
    public void checkArrayDeque() throws RemoteException {
        dque1 = new ArrayDeque<>();
        dque1.add(10);
        dque1.add(20);
        dque1.add(30);

        OGO.inMemory = false;
        Integer a = 20;
        Integer b = 40;

        assertEquals(OgoArrayDeque.contains(dque1, a, false), OgoArrayDeque.contains(dque1, a, true));
        assertEquals(OgoArrayDeque.contains(dque1, b, false), OgoArrayDeque.contains(dque1, b, true));
    }

    /**
     * @since 0.1.0
     */
    @Test
    public void checkVector() throws RemoteException {
        vector1 = new Vector<>();
        vector1.add(10);
        vector1.add(20);
        vector1.add(30);

        OGO.inMemory = false;
        Integer a = 20;
        Integer b = 40;

        assertEquals(OgoVector.contains(vector1, a, false), OgoVector.contains(vector1, a, true));
        assertEquals(OgoVector.contains(vector1, b, false), OgoVector.contains(vector1, b, true));

        assertEquals(OgoVector.indexOf(vector1, a, false), OgoVector.indexOf(vector1, a, true));
        assertEquals(OgoVector.indexOf(vector1, b, false), OgoVector.indexOf(vector1, b, true));
    }

    /**
     * @since 0.1.0
     */
    @Test
    public void checkHashMap() throws RemoteException {
        map1 = new HashMap<>();
        map1.put(10, 100);
        map1.put(20, 200);
        map1.put(30, 300);

        OGO.inMemory = false;
        Integer a = 20;
        Integer b = 200;

        assertEquals(OgoHashMap.containsKey(map1, a, false), OgoHashMap.containsKey(map1, a, true));
        assertEquals(OgoHashMap.containsKey(map1, b, false), OgoHashMap.containsKey(map1, b, true));

        assertEquals(OgoHashMap.containsValue(map1, a, false), OgoHashMap.containsValue(map1, a, true));
        assertEquals(OgoHashMap.containsValue(map1, b, false), OgoHashMap.containsValue(map1, b, true));
    }
}
