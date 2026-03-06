package org.ogo.test.datastructuretests;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.ogo.client.OGO.queryBool;

import java.rmi.RemoteException;
import java.util.HashSet;
import java.util.concurrent.ThreadLocalRandom;
import java.util.stream.Collectors;
import org.junit.jupiter.api.Test;

public class HashSetTest extends DataStructureTest {

    @Test
    public void checkHashSet() throws RemoteException, InterruptedException {
        ThreadLocalRandom randGen = ThreadLocalRandom.current();
        int size = randGen.nextInt(10, 500);
        HashSet<Integer> set = ((randGen.ints(size, 0, 100000)).boxed()).collect(Collectors.toCollection(HashSet::new));
        Integer item = randGen.nextInt(100000, 200000);
        boolean var1 = queryBool(this, "MATCH ({$1})-[:map]->(m)-[:table]->(t)-[*]->()-[:value]->(n {value:" + item
                + "}) RETURN COUNT(n) <> 0", set);
        boolean var2 = OgoHashSet.contains(set, item, true);
        assertEquals(var1, var2);
    }
}
