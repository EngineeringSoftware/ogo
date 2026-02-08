package org.ogo.util;

import java.io.IOException;
import java.util.Properties;

public abstract class OGOProperties {

  /**
   * @brief Gets the port number used for RMI between OGO client and Neo4J server
   * @return int OGO port number
   * @since 1.0.0
   */
  public static int getRmiPort() throws IOException {
    Properties p = new Properties();
    p.load(OGOProperties.class.getClassLoader().getResourceAsStream(Constants.OGO_PROPERTIES_FILE));
    return Integer.parseInt(p.getProperty(Constants.OGO_PROPERTY_RMI_PORT));
  }

  /**
   * @brief Gets the flag for inMemory or Neo4J cypher execution
   * @return boolean inMemory
   * @since 1.0.0
   */
  public static boolean getInMemory() throws IOException {
    Properties p = new Properties();
    p.load(OGOProperties.class.getClassLoader().getResourceAsStream(Constants.OGO_PROPERTIES_FILE));
    return p.getProperty(Constants.OGO_PROPERTY_IN_MEMORY).equals("true");
  }

  /**
   * @brief Gets the flag for clearing Neo4J database after query execution
   * @return boolean clearDatabase
   * @since 1.0.0
   */
  public static boolean getClearDatabase() throws IOException {
    Properties p = new Properties();
    p.load(OGOProperties.class.getClassLoader().getResourceAsStream(Constants.OGO_PROPERTIES_FILE));
    return p.getProperty(Constants.OGO_PROPERTY_CLEAR_DATABASE).equals("true");
  }
}
