package org.ogo.test.datastructuretests;

import static org.ogo.client.OGO.queryBool;

import java.rmi.RemoteException;
import java.util.ArrayDeque;

public abstract class OgoArrayDeque {

    public static boolean contains(ArrayDeque deque, Integer item, boolean defaultMode, DataStructureTest test)
            throws RemoteException {
        if (defaultMode) {
            return deque.contains(item);
        } else {
            return queryBool(deque,
                    "MATCH ({$1})-[:elements]->(m)-[*]->(n {value:" + item.intValue() + "}) RETURN COUNT(n) <> 0",
                    deque);
        }
    }
}
