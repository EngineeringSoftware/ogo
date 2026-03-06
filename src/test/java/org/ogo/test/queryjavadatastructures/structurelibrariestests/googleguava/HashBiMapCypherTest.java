package org.ogo.test.queryjavadatastructures.structurelibrariestests.googleguava;

import static org.ogo.client.OGO.query;

import com.google.common.collect.HashBiMap;
import java.io.IOException;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Disabled;
import org.junit.jupiter.api.Test;
import org.ogo.client.OGO;

@Disabled
public class HashBiMapCypherTest {

    HashBiMap<Integer, Integer> map1;

    /**
     * @version 0.1.0
     *
     * @since 0.1.0
     */
    @BeforeAll
    public static void initQueryEngine() throws RemoteException, InterruptedException, IOException, NotBoundException {
        Thread.sleep(5000);
        OGO.init();
        OGO.setWhiteList("ogo/test", "HashBiMap");
    }

    @Test
    public void testHashBiMapCypher() throws RemoteException {

        map1 = HashBiMap.create();
        for (int i = 0; i < 10; i++) {
            map1.put(10 * i + 1, i + 1);
        }

        OGO.inMemory = false;

        // query("MATCH
        // (n:`org.ogo.test.queryJavaDataStructures.QueryJavaDataStructures_Test`)-[m*]->(r) RETURN
        // True");
        // assert(true);

        long out1 = ((Long) sizeHashBiMap(true)).longValue();
        System.out.println("Output: " + out1);
        System.out.println("Expected: " + map1.size());
        assert (out1 == map1.size());
    }

    private Object sizeHashBiMap(boolean isCypher) throws RemoteException {
        if (isCypher) {
            // Object[] q = query("MATCH
            // ()-[:firstInKeyInsertionOrder]->()-[:nextInKeyInsertionOrder*]-(n) " +
            // "RETURN COUNT(DISTINCT n) as size");
            Object[] q = query("MATCH ()-[*]->()-[n:value]->() " + "RETURN COUNT(DISTINCT n) as size");
            return q[0];
        } /*
           * else{ return (Object) this.size(); }
           */
        return null;
    }

    /*
     * private Object valueAtIndexLL(int idx) throws RemoteException { Object [] q = query("MATCH
     * (n:`org.ogo.test.queryJavaDataStructures.QueryJavaDataStructures_Test`)-[m]->(i:`java.util.Linke> return q[0]; }
     */
}
