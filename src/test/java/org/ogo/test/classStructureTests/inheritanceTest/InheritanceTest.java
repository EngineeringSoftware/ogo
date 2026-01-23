package org.ogo.test.classStructureTests.inheritanceTest;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.ogo.client.OGO.query;

import java.io.IOException;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;
import org.ogo.client.OGO;

/**
 * @author 1sand0s
 */
public class InheritanceTest {

  A a;

  /**
   * regular
   *
   * @author 1sand0s
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
    OGO.setBlackList(
        "jdk", "java/lang/Security", "sun", "java/util/logging", "Exception", "maven", "junit");
  }

  /**
   * regular
   *
   * @author 1sand0s
   * @since 1.0.0
   * @version 1.0.0
   */
  @Test
  public void getFieldaOfAtest() throws RemoteException {
    a = new A();
    Object result[] = query("MATCH (n {$1}) RETURN n.a", a);
    assertEquals(result.length, 1, "Expected result size must be 1");
    assertEquals(result[0], 10, "Expected result value must be 10");
  }

  /**
   * regular
   *
   * @author 1sand0s
   * @since 1.0.0
   * @version 1.0.0
   */
  @Test
  public void getFieldbOfAtest() throws RemoteException {
    a = new A();
    Object result[] = query("MATCH (n {$1}) RETURN n.b", a);
    assertEquals(result.length, 1, "Expected result size must be 1");
    assertEquals(result[0], 20, "Expected result value must be 20");
  }
}
