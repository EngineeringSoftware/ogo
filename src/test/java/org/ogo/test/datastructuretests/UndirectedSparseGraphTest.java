package org.ogo.test.datastructuretests;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.ogo.client.OGO.queryBool;

import edu.uci.ics.jung.graph.UndirectedSparseGraph;
import java.rmi.RemoteException;
import java.util.ArrayList;
import java.util.concurrent.ThreadLocalRandom;
import java.util.stream.Collectors;
import org.junit.jupiter.api.RepeatedTest;

public class UndirectedSparseGraphTest extends DataStructureTest {
    private UndirectedSparseGraph<Integer, Integer> jungudSparseGraph;

    @RepeatedTest(1)
    public void checkUndirectedSparseGraph() throws RemoteException, InterruptedException {
        Thread.sleep(200);
        ThreadLocalRandom randGen = ThreadLocalRandom.current();
        int size = randGen.nextInt(10, 500);
        ArrayList<Integer> temp = ((randGen.ints(size, 0, 100000)).boxed())
                .collect(Collectors.toCollection(ArrayList::new));
        ArrayList<Integer> temp2 = ((randGen.ints(size, 0, 100000)).boxed())
                .collect(Collectors.toCollection(ArrayList::new));
        jungudSparseGraph = new UndirectedSparseGraph<>();
        for (int j = 0; j < temp.size(); j++) {
            jungudSparseGraph.addVertex(temp.get(j));
            jungudSparseGraph.addVertex(temp2.get(j));
            jungudSparseGraph.addEdge(j, temp.get(j), temp2.get(j));
        }
        Integer item = randGen.nextInt(100, 100000);
        boolean var1 = queryBool(this,
                "MATCH ({$1})-[:edges]->()-[:table]->()-[:`" + item + "`]->(n) RETURN n IS NOT NULL",
                jungudSparseGraph);
        boolean var2 = jungudSparseGraph.containsEdge(item);
        assertEquals(var1, var2);
    }
}
