package org.ogo.test.datastructuretests;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.ogo.client.OGO.queryBool;

import java.rmi.RemoteException;
import java.util.ArrayList;
import java.util.concurrent.ThreadLocalRandom;
import java.util.stream.Collectors;
import org.junit.jupiter.api.RepeatedTest;

public class ArrayListTest extends DataStructureTest {
    private ArrayList<Integer> list2;

    @RepeatedTest(1)
    public void checkArrayList() throws RemoteException, InterruptedException {
        ThreadLocalRandom randGen = ThreadLocalRandom.current();
        int size = 10;
        list2 = ((randGen.ints(size, 0, 100000)).boxed()).collect(Collectors.toCollection(ArrayList::new));
        Integer item = randGen.nextInt(100, 100000);
        boolean var1 = queryBool(this,
                "MATCH ({$1})-[:elementData]->(m)-[*]->(n {value:" + item + "}) RETURN COUNT(n) <> 0", list2);
        boolean var2 = OgoArrayList.contains(list2, item, true, this);
        assertEquals(var1, var2);
    }
}
