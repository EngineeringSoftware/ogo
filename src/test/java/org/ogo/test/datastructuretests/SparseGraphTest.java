package org.ogo.test.datastructuretests;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.ogo.client.OGO.queryBool;

import edu.uci.ics.jung.graph.SparseGraph;
import java.rmi.RemoteException;
import java.util.ArrayList;
import java.util.concurrent.ThreadLocalRandom;
import java.util.stream.Collectors;
import org.junit.jupiter.api.RepeatedTest;

public class SparseGraphTest extends DataStructureTest {
    private SparseGraph<Integer, Integer> jungSparseGraph;

    @RepeatedTest(1)
    public void checkSparseGraph() throws RemoteException, InterruptedException {
        ThreadLocalRandom randGen = ThreadLocalRandom.current();
        int size = randGen.nextInt(10, 500);
        ArrayList<Integer> temp = ((randGen.ints(size, 0, 100000)).boxed())
                .collect(Collectors.toCollection(ArrayList::new));
        ArrayList<Integer> temp2 = ((randGen.ints(size, 0, 100000)).boxed())
                .collect(Collectors.toCollection(ArrayList::new));
        jungSparseGraph = new SparseGraph<>();
        for (int j = 0; j < temp.size(); j++) {
            jungSparseGraph.addVertex(temp.get(j));
            jungSparseGraph.addVertex(temp2.get(j));
            jungSparseGraph.addEdge(j, temp.get(j), temp2.get(j));
        }
        Integer item = randGen.nextInt(100, 100000);
        boolean var1 = queryBool(this,
                "MATCH ({$1})-[:undirected_edges]->()-[:table]->()-[:`" + item + "`]->(n) RETURN n IS NOT NULL",
                jungSparseGraph);
        boolean var2 = jungSparseGraph.containsEdge(item);
        assertEquals(var1, var2);
    }
}
