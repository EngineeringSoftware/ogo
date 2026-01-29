package org.ogo.client;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.nio.charset.StandardCharsets;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.security.MessageDigest;
import java.text.SimpleDateFormat;
import java.util.*;
import org.ogo.bridge.AbstractGraphQueryInterface;
import org.ogo.util.FileHelper;
import org.ogo.util.OGOProperties;
import org.ogo.util.Profile;

/**
 * @author 1sand0s
 */
public abstract class OGO {

  static long sTime;
  static long eTime;
  static File profileFile;
  static ArrayList<String> profileData;
  static String profileMethod;
  private static String CsvPath = ".";
  private static GraphTriggerException graphException;
  public static boolean printCSV = false;
  public static boolean printWhiteList = false;
  public static boolean printBlackList = false;
  public static boolean clearDatabase = true;
  public static boolean printProfileMethodInfo = false;
  public static boolean inMemory = false;
  public static boolean followRoot = true;
  public static boolean forceGC = true;
  public static boolean whitelist = true;

  /**
   * Instances of Classes belonging to these packages will be definitely included in the object
   * graph
   */
  protected static ArrayList<String> whiteList = new ArrayList<>();

  /** Instances of Classes belonging to these packages will be excluded from being blacklisted */
  protected static ArrayList<String> excludeBlackListing = new ArrayList<>();

  /**
   * Instances of Classes belonging to these packages will be definitely excluded from the object
   * graph
   */
  protected static ArrayList<String> blackList = new ArrayList<>();

  private static AbstractGraphQueryInterface graphInterface = null;

  /**
   * @brief API call for executing Cypher query
   * @author 1sand0s
   * @param cQuery String in Cypher to query the database
   * @since 1.0.0
   */
  public static void init() throws IOException, NotBoundException {
    inMemory = OGOProperties.getInMemory();
    clearDatabase = OGOProperties.getClearDatabase();
    if (graphInterface == null && !inMemory) {
      int rmiPort = OGOProperties.getRmiPort();
      Registry registry = LocateRegistry.getRegistry(rmiPort);
      graphInterface = (AbstractGraphQueryInterface) registry.lookup("Neo4JGraphQueryEngine");
    }
  }

  /**
   * @brief Gets the class name to use for the dynamically generated class.
   * @author 1sand0s
   * @param className user given class name in CREATE query
   * @param fieldDescriptors field descriptors of user specified fields
   * @since 1.0.0
   */
  private static String getDynamicClassName(String className, String[] fieldDescriptors)
      throws Exception {
    MessageDigest md = MessageDigest.getInstance("SHA-256");
    StringBuilder inputString = new StringBuilder(className);
    for (String fieldDescriptor : fieldDescriptors) {
      inputString.append(fieldDescriptor);
    }
    md.update(inputString.toString().getBytes(StandardCharsets.UTF_8));
    byte[] byteArray = md.digest();
    StringBuilder hexString = new StringBuilder(2 * byteArray.length);
    for (byte b : byteArray) {
      String hex = String.format("%02x", b);
      hexString.append(hex);
    }
    String hexadecimalString = hexString.toString();
    return className + hexadecimalString;
  }

  /**
   * @brief API call for executing Cypher query
   * @author 1sand0s
   * @param cQuery String in Cypher to query the database
   * @since 1.0.0
   */
  public static void setPath(String CsvPath) throws RemoteException {
    OGO.CsvPath = CsvPath;
    graphInterface.setPath(CsvPath);
  }

  /**
   * @param cQuery the Cypher query string
   * @return If the query evaluated to true or false
   * @throws RemoteException
   */
  public static boolean queryBool(String cQuery) throws RemoteException {
    Object[] result = query(cQuery);
    return result.length > 0 && result[0] instanceof Boolean && (Boolean) result[0];
  }

  /**
   * @param cQuery
   * @param objects
   * @return
   * @throws RemoteException
   */
  public static boolean queryBool(String cQuery, Object... objects) throws RemoteException {
    Object[] result = query(cQuery, objects);
    return result.length > 0 && result[0] instanceof Boolean && (Boolean) result[0];
  }

  /**
   * 0
   *
   * @param root
   * @param cQuery
   * @return
   * @throws RemoteException
   */
  public static boolean queryBool(Object root, String cQuery) throws RemoteException {
    Object[] result = query(root, cQuery);
    return result.length > 0 && result[0] instanceof Boolean && (Boolean) result[0];
  }

  /**
   * 0
   *
   * @param root
   * @param cQuery
   * @param objects
   * @return
   * @throws RemoteException
   */
  public static boolean queryBool(Object root, String cQuery, Object... objects)
      throws RemoteException {
    Object[] result = query(root, cQuery, objects);
    return result.length > 0 && result[0] instanceof Boolean && (Boolean) result[0];
  }

  /**
   * 0
   *
   * @param cQuery
   * @return
   * @throws RemoteException
   */
  public static int queryInt(String cQuery) throws RemoteException {
    Object[] result = query(cQuery);
    if (result.length > 0) {
      return (Integer) result[0];
    }
    return -1;
  }

  /**
   * 0
   *
   * @param cQuery
   * @param objects
   * @return
   * @throws RemoteException
   */
  public static int queryInt(String cQuery, Object... objects) throws RemoteException {
    Object[] result = query(cQuery, objects);
    if (result.length > 0) {
      return (Integer) result[0];
    }
    return -1;
  }

  /**
   * 0
   *
   * @param root
   * @param cQuery
   * @return
   * @throws RemoteException
   */
  public static int queryInt(Object root, String cQuery) throws RemoteException {
    Object[] result = query(root, cQuery);
    if (result.length > 0) {
      return (Integer) result[0];
    }
    return -1;
  }

  /**
   * 0
   *
   * @param root
   * @param cQuery
   * @param objects
   * @return
   * @throws RemoteException
   */
  public static int queryInt(Object root, String cQuery, Object... objects) throws RemoteException {
    Object[] result = query(root, cQuery, objects);
    if (result.length > 0) {
      return (Integer) result[0];
    }
    return -1;
  }

  /**
   * 0
   *
   * @param cQuery
   * @return
   * @throws RemoteException
   */
  public static long queryLong(String cQuery) throws RemoteException {
    Object[] result = query(cQuery);
    if (result.length > 0) {
      return (Long) result[0];
    }
    return -1;
  }

  /**
   * 0
   *
   * @param cQuery
   * @param objects
   * @return
   * @throws RemoteException
   */
  public static long queryLong(String cQuery, Object... objects) throws RemoteException {
    Object[] result = query(cQuery, objects);
    if (result.length > 0) {
      return (Long) result[0];
    }
    return -1;
  }

  /**
   * 0
   *
   * @param root
   * @param cQuery
   * @return
   * @throws RemoteException
   */
  public static long queryLong(Object root, String cQuery) throws RemoteException {
    Object[] result = query(root, cQuery);
    if (result.length > 0) {
      return (Long) result[0];
    }
    return -1;
  }

  /**
   * 0
   *
   * @param root
   * @param cQuery
   * @param objects
   * @return
   * @throws RemoteException
   */
  public static long queryLong(Object root, String cQuery, Object... objects)
      throws RemoteException {
    Object[] result = query(root, cQuery, objects);
    if (result.length > 0) {
      return (Long) result[0];
    }
    return -1;
  }

  /**
   * 0
   *
   * @param root
   * @param cQuery
   * @return
   * @throws RemoteException
   */
  public static Object[] query(Object root, String cQuery) throws RemoteException {
    return query(root, cQuery, (Object) null);
  }

  /**
   * 0
   *
   * @param cQuery
   * @param objects
   * @return
   * @throws RemoteException
   */
  public static Object[] query(String cQuery, Object... objects) throws RemoteException {
    return query(null, cQuery, objects);
  }

  /**
   * 0
   *
   * @param root
   * @param cQuery
   * @param objects
   * @return
   * @throws RemoteException
   */
  public static Object[] query(Object root, String cQuery, Object... objects)
      throws RemoteException {

    ArrayList<Object> var = null;

    /* Replace format specifiers with hashcodes of objects
     */
    if (objects != null) {
      cQuery = FileHelper.subArgWithHashCode(cQuery, objects);
    }

    /* Instantiate the exception object used to trigger the native agent */
    graphException =
        new GraphTriggerException(
            inMemory,
            forceGC,
            followRoot,
            whitelist,
            cQuery,
            root,
            whiteList.toArray(),
            blackList.toArray(),
            excludeBlackListing.toArray());

    /*
     * Create Object graph
     * Setup Database
     * Create nodes and relations
     */
    setupGraph();

    /*
     * Execute Cypher query
     * Every Query must return a value
     * If query suceeds, value is returned and Cyphssert.assertRelation returns true
     */
    try {
      if (!inMemory) {
        sTime = System.currentTimeMillis();
        var = graphInterface.query(cQuery);
        eTime = System.currentTimeMillis();
        profileData.add("Assert_Query, " + (eTime - sTime));
      }
    } finally {

      /*
       * Clear and delete database
       * Delete all *.csv files
       */
      sTime = System.currentTimeMillis();
      cleanup();
      eTime = System.currentTimeMillis();
      profileData.add("Cleanup, " + (eTime - sTime));
    }

    if (!profileMethod.isEmpty()) writeProfileFile(profileData);
    if (inMemory) {
      return graphException.getQueryResults();
    }

    assert var != null;
    return var.toArray();
  }

  /**
   * 0
   *
   * @throws RemoteException
   */
  private static void setupGraph() throws RemoteException {
    /*
     * Read the Profile Data Generated from JVMTI invocation required to create
     * object graph
     */
    profileData = new ArrayList<>();

    sTime = System.currentTimeMillis();
    try {
      /* Dummy exception to trigger object graph construction by native agent */
      throw graphException;
    } catch (Exception | Error e) {

    }
    eTime = System.currentTimeMillis();

    readJVMTIProfile(profileData);
    profileData.add("Graph_Generation, " + (eTime - sTime));

    /* Get calling test method */
    profileMethod = getTestMethodName();

    if (!inMemory) {
      sTime = System.currentTimeMillis();
      /* Create nodes from *java.csv files */
      graphInterface.createNodes();
      eTime = System.currentTimeMillis();
      profileData.add("Creation_of_Nodes, " + (eTime - sTime));

      sTime = System.currentTimeMillis();
      /* Create relations from [*TAG*]_Neo4JRelations.csv files */
      graphInterface.createRelations();
      eTime = System.currentTimeMillis();
      profileData.add("Creation_of_Relations, " + (eTime - sTime));
    }
  }

  /**
   * 0
   *
   * @throws RemoteException
   */
  private static void cleanup() throws RemoteException {
    if (!inMemory) {
      if (clearDatabase) {
        /* Delete all nodes and relations in the database */
        graphInterface.clearDatabase();
      }
    }

    for (File f : Objects.requireNonNull(new File(CsvPath).listFiles())) {
      if (f.getName().endsWith(".csv")) {
        /* Delete all *.csv files in the current directory */
        if (!printCSV && !inMemory) {
          f.delete();
        }
      } else if (f.getName().startsWith("PROFILE_HOQ_C_")) {
        /* Delete all *PROFILE_HOQ_C_* files in the current directory */
        f.delete();
      }
    }
  }

  /**
   * 0
   *
   * @param profileData
   */
  private static void writeProfileFile(ArrayList<String> profileData) {

    SimpleDateFormat formatter = new SimpleDateFormat("yyyy-MM-dd-HH-mm-ss-z");
    Date date = new Date(System.currentTimeMillis());
    /* Open file for writing whitelist of packages */
    File file = new File(CsvPath + "/PROFILE_JAVA_" + profileMethod + "_" + formatter.format(date));

    try {
      /* Create a buffered writer for writing to file */
      BufferedWriter writer =
          new BufferedWriter(new OutputStreamWriter(new FileOutputStream(file)));

      /* Write blacklist of packages to file */
      for (String profileDatum : profileData) writer.write(profileDatum + "\n");

      writer.close();
    } catch (FileNotFoundException e) {
      System.out.println(
          "Error : File " + file.getName() + " could not be opened " + file.getPath());
      System.out.println("Unable to write profile data");
      System.out.println(e.getMessage());
    } catch (IOException e) {
      System.out.println("Unable to write profile data " + file.getName());
      System.out.println(e.getMessage());
    }
  }

  /**
   * 0
   *
   * @param profileData
   */
  private static void readJVMTIProfile(ArrayList<String> profileData) {
    profileData.clear();
    for (File f : Objects.requireNonNull(new File(CsvPath).listFiles())) {
      if (f.getName().startsWith("PROFILE_HOQ_C_")) {
        Scanner scan;
        try {
          scan = new Scanner(f);
          while (scan.hasNext()) {
            profileData.add(scan.nextLine());
          }
        } catch (FileNotFoundException e) {
          System.out.println("Error : File " + f.getName() + " not found in path " + f.getPath());
          System.out.println("Unable to read JVMTI profile file");
          System.out.println(e.getMessage());
        }
      }
    }
  }

  /**
   * 0
   *
   * @return
   */
  private static String getTestMethodName() {
    String methodName = "";

    for (StackTraceElement st : Thread.currentThread().getStackTrace()) {
      try {
        if (Class.forName(st.getClassName())
            .getMethod(st.getMethodName())
            .isAnnotationPresent(Profile.class)) {
          methodName = st.getMethodName();
          break;
        }
      } catch (NoSuchMethodException e) {
        if (printProfileMethodInfo) {
          System.out.println(
              "Method with name : "
                  + st.getMethodName()
                  + " does not exist in Class : "
                  + st.getClassName());
          System.out.println(e.getMessage());
        }
      } catch (ClassNotFoundException e) {
        if (printProfileMethodInfo) {
          System.out.println("Class with name : " + st.getClassName() + " not found");
          System.out.println(e.getMessage());
        }
      }
    }

    return methodName;
  }

  /**
   * 0
   *
   * @param packageNames
   */
  public static void setBlackList(String... packageNames) {

    /* clear existing package names from blacklist */
    clearBlackList();

    /* Add new packages in packageNames to blacklist */
    blackList.addAll(Arrays.asList(packageNames));
  }

  /**
   * 0
   *
   * @param packageName
   */
  public static void addBlackList(String packageName) {
    blackList.add(packageName);
  }

  /***/
  public static void clearBlackList() {
    blackList.clear();
  }

  /**
   * 0
   *
   * @param packageNames
   */
  public static void setWhiteList(String... packageNames) {

    /* clear existing package names from whitelist */
    clearWhiteList();

    /* Add new packages in packageNames to whitelist */
    whiteList.addAll(Arrays.asList(packageNames));
  }

  /**
   * 0
   *
   * @param packageName
   */
  public static void addWhiteList(String packageName) {
    whiteList.add(packageName);
  }

  /***/
  public static void clearWhiteList() {
    whiteList.clear();
  }

  /**
   * 0
   *
   * @param packageNames
   */
  public static void setExcludeFromBlackList(String... packageNames) {

    /* clear existing package names from whitelist */
    clearExcludeFromBlackList();

    /* Add new packages in packageNames to whitelist */
    excludeBlackListing.addAll(Arrays.asList(packageNames));
  }

  /**
   * 0
   *
   * @param packageName
   */
  public static void addExcludeFromBlackList(String packageName) {
    excludeBlackListing.add(packageName);
  }

  /***/
  public static void clearExcludeFromBlackList() {
    excludeBlackListing.clear();
  }
}
