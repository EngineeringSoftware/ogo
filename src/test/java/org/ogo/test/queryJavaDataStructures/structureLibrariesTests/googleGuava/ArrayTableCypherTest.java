package org.ogo.test.queryJavaDataStructures.structureLibrariesTests.googleGuava;

import static org.ogo.client.OGO.query;

import com.google.common.collect.ArrayTable;
import com.google.common.collect.Lists;
import java.io.IOException;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.util.List;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.Disabled;
import org.ogo.client.OGO;

@Disabled
public class ArrayTableCypherTest {

  ArrayTable<Integer, Integer, Integer> table1;
  ArrayTable<Integer, Integer, Integer> table2;
  List<Integer> helplist1;
  List<Integer> helplist2;

  List<Integer> helplist3;
  List<Integer> helplist4;
  List<Integer> helplist5;

  /**
   * regular
   *
   * @author 1sand0s
   * @version 1.0.0
   * @since 1.0.0
   */
  @BeforeAll
  public static void initQueryEngine()
      throws RemoteException, InterruptedException, IOException, NotBoundException {
    Thread.sleep(5000);
    OGO.inMemory = false;
    OGO.init();
    OGO.setWhiteList("ogo/test", "ArrayTable", "java/lang/Integer");
  }

  @Test
  public void testArrayTableCypher() throws RemoteException {
    // Square table test
    //        helplist1 = Lists.newArrayList(111, 222);
    //        helplist2 = Lists.newArrayList(100, 200);
    //        table1 = ArrayTable.create(helplist1, helplist2);
    //        table1.put(111, 100, 1);
    //        table1.put(111, 200, 2);
    //        table1.put(222, 100, 3);
    // table1.put(222, 200, 4);

    //        long out1 = ((Long) sizeArrayTable(true)).longValue();
    //        System.out.println(out1);
    //        assert (out1 == table1.size());

    // Asymmetrical Test
    helplist3 = Lists.newArrayList(111, 222, 333);
    helplist4 = Lists.newArrayList(100, 200);
    table2 = ArrayTable.create(helplist3, helplist4);

    OGO.clearDatabase = false;
    OGO.inMemory = false;

    long out2 = ((Long) sizeArrayTable(true)).longValue();
    System.out.println(out2 + "  " + table2.size());
    assert (out2 == table2.size());
  }

  private Object sizeArrayTable(boolean isCypher) throws RemoteException {
    if (isCypher) {
      Object[] q =
          query(
              "MATCH ()<-[c]-()<-[]-()<-[:columnList]-()-[:rowList]->()-[]->()-[r]->() "
                  + "WHERE type(r) <> \"instanceof\" AND type(c) <> \"instanceof\" "
                  + "RETURN COUNT(DISTINCT r) * COUNT(DISTINCT c) AS size");
      return q[0];
    }
    return null;
  }

  /*private Object valueAtIndexLL(int idx) throws RemoteException {
      Object [] q = query("MATCH (n:`org.ogo.test.queryJavaDataStructures.QueryJavaDataStructures_Test`)-[m]->(i:`java.util.Linke>
      return q[0];
  }*/
}
