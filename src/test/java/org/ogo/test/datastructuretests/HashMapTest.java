package org.ogo.test.datastructuretests;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.ogo.client.OGO.queryBool;

import java.rmi.RemoteException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.concurrent.ThreadLocalRandom;
import java.util.stream.Collectors;
import org.junit.jupiter.api.RepeatedTest;

public class HashMapTest extends DataStructureTest {
    private HashMap<Integer, Integer> map1;

    @RepeatedTest(1)
    public void checkHashMap() throws RemoteException, InterruptedException {
        ThreadLocalRandom randGen = ThreadLocalRandom.current();
        int size = randGen.nextInt(10, 500);
        ArrayList<Integer> temp = ((randGen.ints(size, 0, 100000)).boxed())
                .collect(Collectors.toCollection(ArrayList::new));
        ArrayList<Integer> temp2 = ((randGen.ints(size, 0, 100000)).boxed())
                .collect(Collectors.toCollection(ArrayList::new));
        map1 = new HashMap<>();
        for (int j = 0; j < temp.size(); j++) {
            map1.put(temp.get(j), temp2.get(j));
        }
        Integer item = randGen.nextInt(100, 100000);
        boolean var1 = queryBool(this,
                "MATCH ({$1})-[:table]->(m)-[*]->()-[:key]->(n {value:" + item + "}) RETURN COUNT(n) <> 0", map1);
        boolean var2 = OgoHashMap.containsKey(map1, item, true);
        assertEquals(var1, var2);
    }
}
