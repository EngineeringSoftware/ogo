package org.ogo.test.datastructuretests;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.ogo.client.OGO.queryBool;

import java.rmi.RemoteException;
import java.util.LinkedList;
import java.util.concurrent.ThreadLocalRandom;
import java.util.stream.Collectors;
import org.junit.jupiter.api.RepeatedTest;

public class LinkedListTest extends DataStructureTest {
    private LinkedList<Integer> list1;

    @RepeatedTest(1)
    public void checkLinkedList() throws RemoteException, InterruptedException {
        ThreadLocalRandom randGen = ThreadLocalRandom.current();
        int size = 5;
        list1 = ((randGen.ints(size, 0, 100000)).boxed()).collect(Collectors.toCollection(LinkedList::new));
        Integer item = randGen.nextInt(100, 100000);
        boolean var1 = queryBool(this, "MATCH ({$1})-[:first]->(m)-[*]->(n {value:" + item + "}) RETURN COUNT(n) <> 0",
                list1);
        boolean var2 = OgoLinkedList.contains(list1, item, true, this);
        assertEquals(var1, var2);
    }
}
