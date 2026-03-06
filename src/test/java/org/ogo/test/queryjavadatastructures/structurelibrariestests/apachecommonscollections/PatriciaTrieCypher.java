package org.ogo.test.queryjavadatastructures.structurelibrariestests.apachecommonscollections;

import static org.ogo.client.OGO.query;

import java.io.IOException;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import org.apache.commons.collections4.trie.PatriciaTrie;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;
import org.ogo.client.OGO;
import org.ogo.util.Profile;

public class PatriciaTrieCypher {

    PatriciaTrie<Integer> trie1;

    /**
     * @version 0.1.0
     *
     * @since 0.1.0
     */
    @BeforeAll
    public static void initQueryEngine() throws RemoteException, InterruptedException, IOException, NotBoundException {
        Thread.sleep(5000);
        OGO.init();
        OGO.setWhiteList("ogo/test", "PatriciaTrie");
    }

    @Profile
    @Test
    public void PatriciaTrieCypherTest() throws RemoteException {

        trie1 = new PatriciaTrie();
        trie1.put("BIG", 10);
        trie1.put("BIGGER", 2);
        trie1.put("BILL", 30);
        trie1.put("GOOD", 40);
        trie1.put("GOSH", 5);

        OGO.inMemory = false;
        OGO.clearDatabase = false;

        // query("MATCH
        // (n:`org.ogo.test.queryJavaDataStructures.QueryJavaDataStructures_Test`)-[m*]->(r) RETURN
        // True");
        // assert(true);

        long out1 = ((Long) sizePatriciaTrie(true)).longValue();
        System.out.println("Output: " + out1);
        System.out.println("Expected: " + trie1.size());
        assert (out1 == trie1.size());
    }

    private Object sizePatriciaTrie(boolean isCypher) throws RemoteException {
        if (isCypher) {
            Object[] q = query(
                    "MATCH ()-[:predecessor|parent*]->()-[:value]->(n) " + "RETURN COUNT(DISTINCT n) as size");
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
