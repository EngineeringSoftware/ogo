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
   * Initializes the OGO framework by configuring in-memory or Neo4j database mode.
   *
   * <p>This method must be called before executing any queries. It reads configuration from
   * OGOProperties to determine whether to use the in-memory graph query engine (OGO_Mem) or connect
   * to a Neo4j database (OGO_Neo) via RMI for query execution.
   *
   * @throws IOException if RMI registry connection fails
   * @throws NotBoundException if the Neo4JGraphQueryEngine is not bound in the RMI registry
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
   * Sets the path for CSV file output when using Neo4j database mode.
   *
   * <p>In OGO_Neo mode, the object graph is serialized to CSV files at this path for batch import
   * into Neo4j. This path is also used for profile data output when profiling is enabled.
   *
   * @param CsvPath the directory path where CSV files will be written
   * @throws RemoteException if the remote graph interface is unavailable
   */
  public static void setPath(String CsvPath) throws RemoteException {
    OGO.CsvPath = CsvPath;
    graphInterface.setPath(CsvPath);
  }

  /**
   * Executes an unbounded Cypher query and returns the first result as a boolean.
   *
   * @param cQuery the Cypher query string to execute on the entire object heap
   * @return true if the query returns at least one result and the first result is a Boolean true,
   *     false otherwise
   * @throws RemoteException if the query execution fails
   */
  public static boolean queryBool(String cQuery) throws RemoteException {
    Object[] result = query(cQuery);
    return result.length > 0 && result[0] instanceof Boolean && (Boolean) result[0];
  }

  /**
   * Executes an unbounded Cypher query with positional arguments and returns the first result as a
   * boolean.
   *
   * @param cQuery the Cypher query string to execute on the entire object heap
   * @param objects variadic arguments used to expand positional arguments ($N, @N, []N) in the
   *     query string
   * @return true if the query returns at least one result and the first result is a Boolean true,
   *     false otherwise
   * @throws RemoteException if the query execution fails
   */
  public static boolean queryBool(String cQuery, Object... objects) throws RemoteException {
    Object[] result = query(cQuery, objects);
    return result.length > 0 && result[0] instanceof Boolean && (Boolean) result[0];
  }

  /**
   * Executes a bounded Cypher query and returns the first result as a boolean.
   *
   * @param root the root object that constrains query execution to the reachable subgraph
   * @param cQuery the Cypher query string to execute on the object graph
   * @return true if the query returns at least one result and the first result is a Boolean true,
   *     false otherwise
   * @throws RemoteException if the query execution fails
   */
  public static boolean queryBool(Object root, String cQuery) throws RemoteException {
    Object[] result = query(root, cQuery);
    return result.length > 0 && result[0] instanceof Boolean && (Boolean) result[0];
  }

  /**
   * Executes a bounded Cypher query with positional arguments and returns the first result as a
   * boolean.
   *
   * @param root the root object that constrains query execution to the reachable subgraph
   * @param cQuery the Cypher query string to execute on the object graph
   * @param objects variadic arguments used to expand positional arguments ($N, @N, []N) in the
   *     query string
   * @return true if the query returns at least one result and the first result is a Boolean true,
   *     false otherwise
   * @throws RemoteException if the query execution fails
   */
  public static boolean queryBool(Object root, String cQuery, Object... objects)
      throws RemoteException {
    Object[] result = query(root, cQuery, objects);
    return result.length > 0 && result[0] instanceof Boolean && (Boolean) result[0];
  }

  /**
   * Executes an unbounded Cypher query and returns the first result as an integer.
   *
   * @param cQuery the Cypher query string to execute on the entire object heap
   * @return the first result as an Integer, or -1 if no results are returned
   * @throws RemoteException if the query execution fails
   */
  public static int queryInt(String cQuery) throws RemoteException {
    Object[] result = query(cQuery);
    if (result.length > 0) {
      return (Integer) result[0];
    }
    return -1;
  }

  /**
   * Executes an unbounded Cypher query with positional arguments and returns the first result as an
   * integer.
   *
   * @param cQuery the Cypher query string to execute on the entire object heap
   * @param objects variadic arguments used to expand positional arguments ($N, @N, []N) in the
   *     query string
   * @return the first result as an Integer, or -1 if no results are returned
   * @throws RemoteException if the query execution fails
   */
  public static int queryInt(String cQuery, Object... objects) throws RemoteException {
    Object[] result = query(cQuery, objects);
    if (result.length > 0) {
      return (Integer) result[0];
    }
    return -1;
  }

  /**
   * Executes a bounded Cypher query and returns the first result as an integer.
   *
   * @param root the root object that constrains query execution to the reachable subgraph
   * @param cQuery the Cypher query string to execute on the object graph
   * @return the first result as an Integer, or -1 if no results are returned
   * @throws RemoteException if the query execution fails
   */
  public static int queryInt(Object root, String cQuery) throws RemoteException {
    Object[] result = query(root, cQuery);
    if (result.length > 0) {
      return (Integer) result[0];
    }
    return -1;
  }

  /**
   * Executes a bounded Cypher query with positional arguments and returns the first result as an
   * integer.
   *
   * @param root the root object that constrains query execution to the reachable subgraph
   * @param cQuery the Cypher query string to execute on the object graph
   * @param objects variadic arguments used to expand positional arguments ($N, @N, []N) in the
   *     query string
   * @return the first result as an Integer, or -1 if no results are returned
   * @throws RemoteException if the query execution fails
   */
  public static int queryInt(Object root, String cQuery, Object... objects) throws RemoteException {
    Object[] result = query(root, cQuery, objects);
    if (result.length > 0) {
      return (Integer) result[0];
    }
    return -1;
  }

  /**
   * Executes an unbounded Cypher query and returns the first result as a long.
   *
   * @param cQuery the Cypher query string to execute on the entire object heap
   * @return the first result as a Long, or -1 if no results are returned
   * @throws RemoteException if the query execution fails
   */
  public static long queryLong(String cQuery) throws RemoteException {
    Object[] result = query(cQuery);
    if (result.length > 0) {
      return (Long) result[0];
    }
    return -1;
  }

  /**
   * Executes an unbounded Cypher query with positional arguments and returns the first result as a
   * long.
   *
   * @param cQuery the Cypher query string to execute on the entire object heap
   * @param objects variadic arguments used to expand positional arguments ($N, @N, []N) in the
   *     query string
   * @return the first result as a Long, or -1 if no results are returned
   * @throws RemoteException if the query execution fails
   */
  public static long queryLong(String cQuery, Object... objects) throws RemoteException {
    Object[] result = query(cQuery, objects);
    if (result.length > 0) {
      return (Long) result[0];
    }
    return -1;
  }

  /**
   * Executes a bounded Cypher query and returns the first result as a long.
   *
   * @param root the root object that constrains query execution to the reachable subgraph
   * @param cQuery the Cypher query string to execute on the object graph
   * @return the first result as a Long, or -1 if no results are returned
   * @throws RemoteException if the query execution fails
   */
  public static long queryLong(Object root, String cQuery) throws RemoteException {
    Object[] result = query(root, cQuery);
    if (result.length > 0) {
      return (Long) result[0];
    }
    return -1;
  }

  /**
   * Executes a bounded Cypher query with positional arguments and returns the first result as a
   * long.
   *
   * @param root the root object that constrains query execution to the reachable subgraph
   * @param cQuery the Cypher query string to execute on the object graph
   * @param objects variadic arguments used to expand positional arguments ($N, @N, []N) in the
   *     query string
   * @return the first result as a Long, or -1 if no results are returned
   * @throws RemoteException if the query execution fails
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
   * Executes a bounded Cypher query on the object graph reachable from the specified root.
   *
   * @param root the root object that constrains query execution to only objects reachable (under
   *     transitive closure of reference fields) from this root. This enables localized queries and
   *     improves performance by focusing traversal on a subgraph of the entire object heap.
   * @param cQuery the Cypher query string to execute on the object graph. The query treats the
   *     object heap as a graph database where each object is a node (labeled with its fully
   *     qualified class name), primitive/String fields are node properties, and reference fields
   *     are relationships (labeled with the field name) between nodes.
   * @return an array of objects matching the query results
   * @throws RemoteException if the query execution fails
   */
  public static Object[] query(Object root, String cQuery) throws RemoteException {
    return query(root, cQuery, (Object) null);
  }

  /**
   * Executes an unbounded Cypher query on the entire object heap.
   *
   * <p>Unlike bounded queries, unbounded queries execute on the complete available object heap
   * without restriction to a specific subgraph. This provides access to all objects in memory but
   * with less precise semantics due to the dynamic nature of the JVM and garbage collection.
   *
   * @param cQuery the Cypher query string to execute on the object graph. The query treats the
   *     object heap as a graph database where each object is a node (labeled with its fully
   *     qualified class name), primitive/String fields are node properties, and reference fields
   *     are relationships (labeled with the field name) between nodes.Supports positional arguments
   *     that will be expanded using the provided objects array.
   * @param objects variadic arguments used to expand positional arguments in the query string:
   *     <ul>
   *       <li>{@code $N} expands to the unique identifier of {@code objects[N-1]}
   *       <li>{@code @N} expands to the fully qualified class name of {@code objects[N-1]}
   *       <li>{@code []N} unions results for each element if {@code objects[N-1]} is Iterable
   *     </ul>
   *
   * @return an array of objects matching the query results
   * @throws RemoteException if the query execution fails
   */
  public static Object[] query(String cQuery, Object... objects) throws RemoteException {
    return query(null, cQuery, objects);
  }

  /**
   * Executes a bounded Cypher query on the object graph with positional argument expansion.
   *
   * <p>This is the most flexible query method, combining both root-bounded execution and positional
   * argument expansion. The query is constrained to objects reachable from the root object, and the
   * query string can reference runtime values through positional arguments.
   *
   * <p><b>Example usage:</b>
   *
   * <pre>{@code
   * // Find all nodes reachable from bTree.root with value=1 exactly 2 hops away
   * Object[] result = query(
   *     bTree.root,
   *     "MATCH (n {$1})-[:left|right*2]->(m:@1 {value:1}) RETURN m",
   *     bTree.root, Node.class
   * );
   * }</pre>
   *
   * @param root the root object that constrains query execution to only objects reachable (under
   *     transitive closure of reference fields) from this root. This enables localized queries and
   *     improves performance by focusing traversal on a subgraph of the entire object heap. Can be
   *     a collection containing multiple roots.
   * @param cQuery the Cypher query string to execute on the object graph. The query treats the
   *     object heap as a graph database where each object is a node (labeled with its fully
   *     qualified class name), primitive/String fields are node properties, and reference fields
   *     are relationships (labeled with the field name) between nodes.
   * @param objects variadic arguments used to expand positional arguments in the query string:
   *     <ul>
   *       <li>{@code $N} expands to the unique identifier of {@code objects[N-1]}
   *       <li>{@code @N} expands to the fully qualified class name of {@code objects[N-1]}
   *       <li>{@code []N} unions results for each element if {@code objects[N-1]} is Iterable
   *     </ul>
   *
   * @return an array of objects matching the query results
   * @throws RemoteException if the query execution fails
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
   * Sets the blacklist of package names whose instances will be excluded from the object graph.
   *
   * <p>The blacklist optimization helps reduce the size of the object graph by filtering out
   * instances of classes from specified packages. This improves performance by limiting the number
   * of objects traversed and serialized during query execution. Clears any existing blacklist
   * entries before adding the new packages.
   *
   * @param packageNames package names to exclude from the object graph (e.g., "java.lang",
   *     "sun.misc")
   * @see #addBlackList(String)
   * @see #clearBlackList()
   */
  public static void setBlackList(String... packageNames) {

    /* clear existing package names from blacklist */
    clearBlackList();

    /* Add new packages in packageNames to blacklist */
    blackList.addAll(Arrays.asList(packageNames));
  }

  /**
   * Adds a single package name to the blacklist.
   *
   * <p>Instances of classes belonging to this package will be excluded from the object graph during
   * query execution.
   *
   * @param packageName the package name to add to the blacklist
   * @see #setBlackList(String...)
   */
  public static void addBlackList(String packageName) {
    blackList.add(packageName);
  }

  /**
   * Clears all entries from the blacklist.
   *
   * <p>After calling this method, no packages will be blacklisted and all instances will be
   * considered for inclusion in the object graph (subject to whitelist constraints).
   *
   * @see #setBlackList(String...)
   */
  public static void clearBlackList() {
    blackList.clear();
  }

  /**
   * Sets the whitelist of package names whose instances will be included in the object graph.
   *
   * <p>The whitelist optimization (WL) limits the object graph to only instances of classes from
   * specified packages (and objects reachable from them under transitive closure). This
   * dramatically reduces object graph size and improves query performance. According to the paper,
   * +WL+FGC provides the biggest performance reduction. Clears any existing whitelist entries
   * before adding the new packages.
   *
   * @param packageNames package names to include in the object graph (e.g., "org.myapp",
   *     "com.example")
   * @see #addWhiteList(String)
   * @see #clearWhiteList()
   */
  public static void setWhiteList(String... packageNames) {

    /* clear existing package names from whitelist */
    clearWhiteList();

    /* Add new packages in packageNames to whitelist */
    whiteList.addAll(Arrays.asList(packageNames));
  }

  /**
   * Adds a single package name to the whitelist.
   *
   * <p>Instances of classes belonging to this package will be included in the object graph during
   * query execution.
   *
   * @param packageName the package name to add to the whitelist
   * @see #setWhiteList(String...)
   */
  public static void addWhiteList(String packageName) {
    whiteList.add(packageName);
  }

  /**
   * Clears all entries from the whitelist.
   *
   * <p>After calling this method, all packages may be included in the object graph (subject to
   * blacklist constraints and other filters).
   *
   * @see #setWhiteList(String...)
   */
  public static void clearWhiteList() {
    whiteList.clear();
  }

  /**
   * Sets packages that should be excluded from automatic blacklisting.
   *
   * <p>Even if a package would normally be blacklisted by internal heuristics or other mechanisms,
   * packages in this list will be protected from blacklisting. This provides fine-grained control
   * over which packages are guaranteed to be included. Clears any existing entries before adding
   * the new packages.
   *
   * @param packageNames package names to exclude from blacklisting
   * @see #addExcludeFromBlackList(String)
   * @see #clearExcludeFromBlackList()
   */
  public static void setExcludeFromBlackList(String... packageNames) {

    /* clear existing package names from whitelist */
    clearExcludeFromBlackList();

    /* Add new packages in packageNames to whitelist */
    excludeBlackListing.addAll(Arrays.asList(packageNames));
  }

  /**
   * Adds a single package name to the exclude-from-blacklist.
   *
   * <p>This package will be protected from automatic blacklisting.
   *
   * @param packageName the package name to exclude from blacklisting
   * @see #setExcludeFromBlackList(String...)
   */
  public static void addExcludeFromBlackList(String packageName) {
    excludeBlackListing.add(packageName);
  }

  /**
   * Clears all entries from the exclude-from-blacklist.
   *
   * <p>After calling this method, no packages have special protection from blacklisting.
   *
   * @see #setExcludeFromBlackList(String...)
   */
  public static void clearExcludeFromBlackList() {
    excludeBlackListing.clear();
  }
}
