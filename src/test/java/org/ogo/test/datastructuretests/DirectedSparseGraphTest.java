package org.ogo.test.datastructuretests;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.ogo.client.OGO.queryBool;

import edu.uci.ics.jung.graph.DirectedSparseGraph;
import java.rmi.RemoteException;
import java.util.ArrayList;
import java.util.concurrent.ThreadLocalRandom;
import java.util.stream.Collectors;
import org.junit.jupiter.api.RepeatedTest;

public class DirectedSparseGraphTest extends DataStructureTest {
    private DirectedSparseGraph<Integer, Integer> jungdSparseGraph;

    @RepeatedTest(1)
    public void checkDirectedSparseGraph() throws RemoteException, InterruptedException {
        ThreadLocalRandom randGen = ThreadLocalRandom.current();
        int size = randGen.nextInt(10, 500);
        ArrayList<Integer> temp = ((randGen.ints(size, 0, 100000)).boxed())
                .collect(Collectors.toCollection(ArrayList::new));
        ArrayList<Integer> temp2 = ((randGen.ints(size, 0, 100000)).boxed())
                .collect(Collectors.toCollection(ArrayList::new));
        jungdSparseGraph = new DirectedSparseGraph<>();
        for (int j = 0; j < temp.size(); j++) {
            jungdSparseGraph.addVertex(temp.get(j));
            jungdSparseGraph.addVertex(temp2.get(j));
            jungdSparseGraph.addEdge(j, temp.get(j), temp2.get(j));
        }
        Integer item = randGen.nextInt(100, 100000);
        boolean var1 = queryBool(this,
                "MATCH ({$1})-[:edges]->()-[:table]->()-[:`" + item + "`]->(n) RETURN n IS NOT NULL", jungdSparseGraph);
        boolean var2 = jungdSparseGraph.containsEdge(item);
        assertEquals(var1, var2);
    }
}
