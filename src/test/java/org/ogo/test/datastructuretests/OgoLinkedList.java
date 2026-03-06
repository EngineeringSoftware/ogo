package org.ogo.test.datastructuretests;

import static org.ogo.client.OGO.queryBool;
import static org.ogo.client.OGO.queryInt;
import static org.ogo.client.OGO.queryLong;

import java.rmi.RemoteException;
import java.util.LinkedList;

public abstract class OgoLinkedList {

    public static boolean contains(LinkedList list, Integer item, boolean defaultMode, DataStructureTest test)
            throws RemoteException {
        if (defaultMode) {
            return list.contains(item);
        } else {
            return queryBool(test, "MATCH ({$1})-[:first]->(m)-[*]->(n {value:" + item.intValue() + "}) RETURN True",
                    list);
        }
    }

    public static int indexOf(LinkedList list, Integer item, boolean defaultMode) throws RemoteException {
        if (defaultMode) {
            return list.indexOf(item);
        } else {
            return (int) queryLong(list, "MATCH ({$1})-[:first]->(m)-[*]->(n {value:" + item.intValue()
                    + "}) RETURN length(shortestpath((m)-[*]->(n))) - 1", list);
        }
    }

    public static boolean add(LinkedList list, Integer item, boolean defaultMode) throws RemoteException {
        if (defaultMode) {
            return list.add(item);
        } else {
            return queryBool(
                    "MATCH (n:`java.util.LinkedList` {$1}) MATCH (m:`java.lang.Integer` {$2}) RETURN" + " n.`add`(m)",
                    list, item);
        }
    }

    public static int size(LinkedList list, boolean defaultMode) throws RemoteException {
        if (defaultMode) {
            return list.size();
        } else {
            return queryInt("MATCH (n:`java.util.LinkedList` {$1}) RETURN n.size", list);
        }
    }
}
