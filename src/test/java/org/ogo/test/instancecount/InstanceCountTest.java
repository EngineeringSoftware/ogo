package org.ogo.test.instancecount;

import static org.junit.jupiter.api.Assertions.assertTrue;
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
public class InstanceCountTest {

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
    OGO.setWhiteList("ogo");
  }

  /**
   * regular
   *
   * @author 1sand0s
   * @since 1.0.0
   * @version 1.0.0
   */
  @Test
  public void instanceACountTest() throws RemoteException {
    A a = new A();
    Object[] result = query("MATCH (n:`org.ogo.test.instancecount.A`) RETURN COUNT(n)");

    assertTrue(result != null && result.length == 1 && ((Long) result[0]).longValue() == 1);
  }

  /**
   * regular
   *
   * @author 1sand0s
   * @since 1.0.0
   * @version 1.0.0
   */
  @Test
  public void instanceBCountTest() throws RemoteException {
    A a = new A();
    B b = new B();
    Object[] result = query("MATCH (n:`org.ogo.test.instancecount.B`) RETURN COUNT(n)");

    assertTrue(result != null && result.length == 1 && ((Long) result[0]).longValue() == 2);
  }

  /**
   * regular
   *
   * @author 1sand0s
   * @since 1.0.0
   * @version 1.0.0
   */
  @Test
  public void instanceEdgeCountTest() throws RemoteException {
    A a = new A();
    B b = new B();
    Object[] result = query("MATCH (n:`org.ogo.test.instancecount.A`)-[e:b]->(m) RETURN COUNT(m)");

    assertTrue(result != null && result.length == 1 && ((Long) result[0]).longValue() == 1);
  }
}
