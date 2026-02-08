package org.ogo.test.classstructuretests.singleclassmultipleconnectedobjects;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.ogo.client.OGO.query;
import static org.ogo.client.OGO.queryBool;

import java.io.IOException;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Disabled;
import org.junit.jupiter.api.Test;
import org.ogo.client.OGO;

public class SingleClassMultipleConnectedObjectsTest {

  /**
   * @since 1.0.0
   * @version 1.0.0
   */
  @BeforeAll
  public static void initQueryEngine()
      throws RemoteException, InterruptedException, IOException, NotBoundException {
    Thread.sleep(4000);
    OGO.inMemory = false;
    OGO.init();
    OGO.setWhiteList("ogo/test");
  }

  /**
   * @since 1.0.0
   * @version 1.0.0
   */
  @Test
  public void matchATest() throws RemoteException {
    A a = new A();

    assertTrue(
        queryBool(
            a,
            "MATCH (n:`org.ogo.test.classstructuretests.singleclassmultipleconnectedobjects.A`"
                + " {$1})\n"
                + "RETURN true",
            a));
  }

  /**
   * @since 1.0.0
   * @version 1.0.0
   */
  @Test
  public void getFieldOfCTest() throws RemoteException {
    A a = new A();

    Object[] result = query(a, "MATCH (n {$1})-[e:c]->(m) RETURN m.var2", a);

    assertEquals(1, result.length, "Expected result size must be 1");
    assertEquals(25.0, ((Double) result[0]).doubleValue(), "Expected result value must be 25.0");
  }

  /**
   * @since 1.0.0
   * @version 1.0.0
   */
  @Test
  @Disabled
  public void getStaticFieldOfCTest() throws RemoteException {
    A a = new A();

    Object[] result =
        query(
            "MATCH"
                + " (n:`org.ogo.test.classStructureTests.singleClassMultipleConnectedObjects.C`)-[e:instanceof]->(m)\n"
                + "RETURN m.var1");

    assertEquals(1, result.length, "Expected result size must be 1");
    assertEquals(10, ((Integer) result[0]).intValue(), "Expected result value must be 10");
  }
}
