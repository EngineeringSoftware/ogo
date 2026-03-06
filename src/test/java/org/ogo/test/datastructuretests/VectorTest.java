package org.ogo.test.datastructuretests;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.ogo.client.OGO.queryBool;

import java.rmi.RemoteException;
import java.util.Vector;
import java.util.concurrent.ThreadLocalRandom;
import java.util.stream.Collectors;
import org.junit.jupiter.api.RepeatedTest;

public class VectorTest extends DataStructureTest {
    private Vector<Integer> vector1;

    @RepeatedTest(1)
    public void checkVector() throws RemoteException, InterruptedException {
        ThreadLocalRandom randGen = ThreadLocalRandom.current();
        int size = randGen.nextInt(10, 500);
        vector1 = ((randGen.ints(size, 0, 100000)).boxed()).collect(Collectors.toCollection(Vector::new));
        Integer item = randGen.nextInt(100, 100000);
        boolean var1 = queryBool(this,
                "MATCH ({$1})-[:elementData]->(m)-[*]->(n {value:" + item + "}) RETURN COUNT(n) <> 0", vector1);
        boolean var2 = OgoVector.contains(vector1, item, true);
        assertEquals(var1, var2);
    }
}
