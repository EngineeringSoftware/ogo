package org.ogo.test.datastructuretests;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.ogo.client.OGO.queryBool;

import java.rmi.RemoteException;
import java.util.ArrayDeque;
import java.util.concurrent.ThreadLocalRandom;
import java.util.stream.Collectors;
import org.junit.jupiter.api.RepeatedTest;
import org.junit.jupiter.api.Test;

public class ArrayDequeTest extends DataStructureTest {
    private ArrayDeque<Integer> dque1;

    @RepeatedTest(1)
    public void checkArrayDeque() throws RemoteException, InterruptedException {
        ThreadLocalRandom randGen = ThreadLocalRandom.current();
        int size = randGen.nextInt(10, 1000);
        dque1 = ((randGen.ints(size, 0, 100000)).boxed()).collect(Collectors.toCollection(ArrayDeque::new));
        Integer item = randGen.nextInt(100, 100000);
        boolean var1 = queryBool(dque1,
                "MATCH ({$1})-[:elements]->(m)-[*]->(n {value:" + item + "}) RETURN COUNT(n) <> 0", dque1);
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
        boolean var3 = OgoArrayDeque.contains(dque1, 1, false, this);
        boolean var4 = OgoArrayDeque.contains(dque1, 1, true, this);

        assertTrue(var3);
        assertEquals(var3, var4);
    }
}
