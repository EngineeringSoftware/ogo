package org.ogo.server;

import static org.neo4j.configuration.GraphDatabaseSettings.DEFAULT_DATABASE_NAME;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.rmi.AlreadyBoundException;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;
import java.util.*;
import java.util.Map.Entry;
import org.neo4j.dbms.api.DatabaseManagementService;
import org.neo4j.dbms.api.DatabaseManagementServiceBuilder;
import org.neo4j.graphdb.GraphDatabaseService;
import org.neo4j.graphdb.Label;
import org.neo4j.graphdb.Node;
import org.neo4j.graphdb.QueryExecutionException;
import org.neo4j.graphdb.Relationship;
import org.neo4j.graphdb.RelationshipType;
import org.neo4j.graphdb.Result;
import org.neo4j.graphdb.Transaction;
import org.neo4j.io.fs.FileUtils;
import org.ogo.bridge.AbstractGraphQueryInterface;
import org.ogo.util.CastObjectArrayToPrimitive;
import org.ogo.util.FileHelper;
import org.ogo.util.OGOProperties;

/**
 * @author 1sand0s
 */
public class Neo4JGraphQueryEngine extends AbstractGraphQueryEngine
    implements AbstractGraphQueryInterface {

  /** Handle to the Neo4J database */
  private GraphDatabaseService graphDb;

  /** Used to shutdown and clear the database */
  private DatabaseManagementService managementService;

  /**
   * Use HashMap to create relationships manually between nodes (Faster than Cypher Query approach
   * of CREATE RELATION)
   */
  private HashMap<Long, Long> nodeMap;

  /**
   * Relationship type between nodes
   *
   * <p>To maintain consistency, I have named relationship types based on JVMTI heap reference kind
   *
   * <p>See
   * https://docs.oracle.com/en/java/javase/11/docs/specs/jvmti.html#JVMTI_HEAP_REFERENCE_FIELD
   */
  protected enum REFERENCE_KIND implements RelationshipType {
    JVMTI_HEAP_REFERENCE_CLASS, // 1
    JVMTI_HEAP_REFERENCE_FIELD, // 2
    JVMTI_HEAP_REFERENCE_ARRAY_ELEMENT, // 3
    JVMTI_HEAP_REFERENCE_CLASS_LOADER, // 4
    JVMTI_HEAP_REFERENCE_SIGNERS, // 5
    JVMTI_HEAP_REFERENCE_PROTECTION_DOMAIN, // 6
    JVMTI_HEAP_REFERENCE_INTERFACE, // 7
    JVMTI_HEAP_REFERENCE_STATIC_FIELD, // 8
    JVMTI_HEAP_REFERENCE_CONSTANT_POOL, // 9
    JVMTI_HEAP_REFERENCE_SUPERCLASS // 10
  }

  /** Use an array for quicker assigning of relationships */
  protected final REFERENCE_KIND relationArray[] = {
    REFERENCE_KIND.JVMTI_HEAP_REFERENCE_CLASS,
    REFERENCE_KIND.JVMTI_HEAP_REFERENCE_FIELD,
    REFERENCE_KIND.JVMTI_HEAP_REFERENCE_ARRAY_ELEMENT,
    REFERENCE_KIND.JVMTI_HEAP_REFERENCE_CLASS_LOADER,
    REFERENCE_KIND.JVMTI_HEAP_REFERENCE_SIGNERS,
    REFERENCE_KIND.JVMTI_HEAP_REFERENCE_PROTECTION_DOMAIN,
    REFERENCE_KIND.JVMTI_HEAP_REFERENCE_INTERFACE,
    REFERENCE_KIND.JVMTI_HEAP_REFERENCE_STATIC_FIELD,
    REFERENCE_KIND.JVMTI_HEAP_REFERENCE_CONSTANT_POOL,
    REFERENCE_KIND.JVMTI_HEAP_REFERENCE_SUPERCLASS
  };

  /** class constructor */
  public Neo4JGraphQueryEngine() throws RemoteException {}

  /**
   * @brief Setup the database and register the shutdown hook
   * @author 1sand0s
   * @since 1.0.0
   * @version 1.0.0
   */
  @Override
  public void setup() throws RemoteException {
    try {
      /* Directory to create Neo4J database in */
      File file = new File("target/databases/ObjectDB");

      /* Delete any existing Neo4J databases in the specified directory */
      FileUtils.deleteDirectory(file.toPath());

      /* Create Neo4J database */
      managementService = new DatabaseManagementServiceBuilder(file.toPath()).build();
      graphDb = managementService.database(DEFAULT_DATABASE_NAME);
      registerShutdownHook(managementService);

      /*
       * Create nodeMap for manually creating relationships later (see importCsv())
       */
      nodeMap = new HashMap<Long, Long>();
    } catch (IOException e) {
      System.out.println("Failed to find directory for database creation");
    }
  }

  /**
   * @brief Reads files named '[TAG]_Neo4JRelations.csv' containing tags of instances and primitives
   *     owned by instance with tag [TAG]
   * @author 1sand0s
   * @param file [TAG]_Neo4JRelations.csv
   * @param refNodes Tags of owned instances and primitives contained inside the file
   * @param relationType type of relation interms of JVMTI_HEAP_REFERENCE_KIND
   * @param relationName name the edges/relations using appropriate field names
   * @since 1.0.0
   * @version 1.0.0
   */
  public void readRelationFile(
      File file,
      ArrayList<Long> refNodes,
      ArrayList<REFERENCE_KIND> relationType,
      ArrayList<String> relationName) {
    Scanner scan;
    try {
      scan = new Scanner(file);
      while (scan.hasNext()) {

        /* We use ',' as the separator to separate line read into tokens */
        StringTokenizer tokens = new StringTokenizer(scan.next(), ",");

        /* The number of tokens for each line read must be 2, else throw
         * IOException
         * + TODO : Create custom Exception */
        if (tokens.countTokens() != 3) throw new IOException();

        /* First column in [*TAG*].Neo4JRelations.csv is the tag of referee object */
        refNodes.add(Long.parseLong(tokens.nextToken()));

        /* Second column in [*TAG*].Neo4JRelations.csv is the reference kind between the referrer
         * and referee object */
        relationType.add(relationArray[Integer.parseInt(tokens.nextToken()) - 1]);

        /* Third column in [*TAG*].Neo4JRelations.csv is the name of the field corresponding to
         * the referee object as declared in the Class of the referrer object */
        relationName.add(tokens.nextToken());
      }
    } catch (FileNotFoundException e) {
      System.out.println("Error : File " + file.getName() + " not found in path " + file.getPath());
      System.out.println("Unable to create relation described in " + file.getName());
      System.out.println(e.getMessage());
    } catch (IOException e) {
      System.out.println("Error : File " + file.getName() + " has invalid content");
      System.out.println("Unable to create relation described in " + file.getName());
    }
  }

  /**
   * @brief Import object graph created by native agent into Neo4J Use CREATE instead of MERGE (see
   *     https://neo4j.com/blog/bulk-data-import-neo4j-3-0/)
   * @author 1sand0s
   * @since 1.0.0
   * @version 1.0.0
   */
  @Override
  public void createNodes() throws RemoteException {
    /* For storing *.csv files exported from native agent */
    File files[];

    /*
     * Get all *.csv files in current directory describing instances and primitives
     */
    files = FileHelper.findFilesWithExtension(CsvPath, "java.csv");

    /* Create nodes */
    for (int j = 0; j < files.length; j++) {

      /* Construct Cypher query to create nodes from Csv files */
      String cQuery =
          "LOAD CSV WITH HEADERS FROM 'file://"
              + files[j].getAbsolutePath()
              + "' As row\n"
              + "CREATE (n {signature:row.Signature, tag:toInteger(row.Tag) ,"
              + " hash:toInteger(row.HashCode)})\n"
              + "RETURN"
              + " ID(n)"
              + " , n.tag"
              + " , n.signature"
              + " , n;"; // Return ID and Tag to store in HashMap
      // for creating relations later
      try (Transaction tx = graphDb.beginTx();
          Result result = tx.execute(cQuery)) {
        while (result.hasNext()) {

          /* Number of rows depends on the numebr of lines in CSV file */
          Map<String, Object> row = result.next();

          /* Store the returned node ID and Tag in HashMap to create relations later on */
          nodeMap.put((Long) row.get("n.tag"), (Long) row.get("ID(n)"));

          /* Node labels cannot be set during CSV Load cyher query so this workaround */
          Node node = ((Node) row.get("n"));
          node.addLabel(Label.label((String) row.get("n.signature")));

          ArrayList<String> propertyName = new ArrayList<String>(0);
          ArrayList<String[]> propertyValue = new ArrayList<String[]>(0);
          ArrayList<String> propertyType = new ArrayList<String>(0);

          /* Get Properties if [*TAG*]_Neo4JProperties.csv file exists */
          String propertiesFile =
              String.valueOf((Long) row.get("n.tag")) + "_" + "Neo4JProperties.csv";
          getPropertiesFromFile(CsvPath, propertiesFile, propertyName, propertyValue, propertyType);

          /* Add properties to the node if any */
          for (int k = 0; k < propertyName.size(); k++) {

            node =
                CastObjectArrayToPrimitive.castToNeo4JPropertyType(
                    propertyName.get(k), propertyValue.get(k), propertyType.get(k), node);
            /*
            		if(propertyValue.get(k).length == 1)
            		node.setProperty(propertyName.get(k), propertyValue.get(k)[0]);
            		else
            		node.setProperty(propertyName.get(k), propertyValue.get(k));
            */
          }
        }

        /* Commit progress to database . Periodic commits reduces heap usage */
        tx.commit();
      } catch (QueryExecutionException e) {
        System.out.println("Error : Creation of node failed " + e.getMessage());
        System.out.println("Unable to execute Cypher Query for CSV file " + files[j].getName());
      } catch (NullPointerException e) {
        e.getMessage();
      }
    }
  }

  /**
   * @brief Create relations described in [*REFERRER TAG*]_Neo4JRelations.csv
   * @author 1sand0s
   * @since 1.0.0
   * @version 1.0.0
   */
  @Override
  public void createRelations() throws RemoteException {
    /* For storing *.csv files exported from native agent */
    File files[];

    /*
     * Get all [*TAG*]_Neo4JRelation.csv files in current directory describing
     * relations between instances and between instances and primitives
     */
    files = FileHelper.findFilesWithExtension(CsvPath, "Neo4JRelations.csv");

    /* Create relations */
    for (int j = 0; j < files.length; j++) {
      try (Transaction tx = graphDb.beginTx()) {
        ArrayList<Long> refereeTags = new ArrayList<Long>();
        ArrayList<REFERENCE_KIND> relationType = new ArrayList<REFERENCE_KIND>();
        ArrayList<String> relationName = new ArrayList<String>();

        /* Get referrer tag from [*REFERRER TAG*]_Neo4JRelations.csv file name */
        Long referrerTag =
            Long.parseLong((files[j].getName()).substring(0, (files[j].getName()).indexOf("_")));

        /* Read [*REFERRER TAG*]_Neo4JRelations.csv file to get 'referee tags', reference kinds and reference names*/
        readRelationFile(files[j], refereeTags, relationType, relationName);

        /* Get 'referrer node ID', tag present in [*REFERRER TAG*]_NeoJRelations.csv file name */
        Long referrerNodeID = nodeMap.get(referrerTag);
        if (referrerNodeID != null) {

          /* Get 'referrer node' using the ID */
          Node referrerNode = tx.getNodeById(referrerNodeID);

          /* Iterate through referee nodes creating relations */
          for (int k = 0; k < refereeTags.size(); k++) {

            /* Get 'referee node ID', tag present inside [*REFERRER TAG*]_Neo4JRelations.csv file */
            Long refereeNodeID = nodeMap.get(refereeTags.get(k));
            if (refereeNodeID != null) {

              //                            /* Create relation between 'referrer node' and 'referee
              // node' */
              //                            Relationship relation =
              // referrerNode.createRelationshipTo(tx.getNodeById(refereeNodeID),
              //                                    relationType.get(k));

              /* Create relation between 'referrer node' and 'referee node' with name of the
               * field corresponding to the referee instance as declared in the class of
               * referrer instance */
              Relationship relation =
                  referrerNode.createRelationshipTo(
                      tx.getNodeById(refereeNodeID),
                      RelationshipType.withName(relationName.get(k)));

              /* Assign reference type
               * See : https://docs.oracle.com/en/java/javase/11/docs/specs/jvmti.html#JVMTI_HEAP_REFERENCE_FIELD
               */
              relation.setProperty("Type", relationType.get(k).ordinal());
            }
          }

          /* Commit created relation to database */
          tx.commit();
        }
      } catch (QueryExecutionException e) {
        System.out.println("Error : Creation of relationship failed");
        System.out.println(
            "Unable to create relation for instances in CSV file " + files[j].getName());
      } catch (NullPointerException e) {
        e.getMessage();
      }
    }
  }

  /**
   * @brief Executes the Cypher query contained in the string cQuery
   * @author 1sand0s
   * @param cQuery String in Cypher to query the database
   * @return boolean returns true if queried pattern matches any existing pattern in the database
   * @since 1.0.0
   * @version 1.0.0
   */
  @Override
  public ArrayList<Object> query(String cQuery) throws RemoteException {

    ArrayList<Object> res = new ArrayList<Object>();

    /* Execute the query */
    try (Transaction tx = graphDb.beginTx();
        Result result = tx.execute(cQuery)) {
      while (result.hasNext()) {

        /* For Debugging */
        Map<String, Object> row = result.next();
        for (Entry<String, Object> column : row.entrySet()) {
          // System.out.println(column.getKey() + ": " + column.getValue() + "; ");
          res.add(column.getValue());
        }
      }
    } catch (QueryExecutionException e) {
      System.out.println("Error : Cyher Query failed " + e.getMessage());
      System.out.println("Unable to execute query " + cQuery);
    } catch (NullPointerException e) {
      e.printStackTrace();
    }

    /*
     * Return status fo query True if query succeeded (Cypher query must have RETURN
     * command)
     */
    return res;
  }

  /**
   * @brief Remove all nodes and relations from database before importing new object graph.
   *     (Possibly update the databse instead ?)
   * @author 1sand0s
   * @since 1.0.0
   * @version 1.0.0
   */
  @Override
  public void clearDatabase() throws RemoteException {
    try (Transaction tx = graphDb.beginTx()) {
      for (Node node : tx.getAllNodes()) {
        for (Relationship relation : node.getRelationships()) {
          relation.delete();
        }
        node.delete();
      }
      tx.commit();
    } catch (NullPointerException e) {
      e.printStackTrace();
    }

    /* Clear all tag and nodeId mapping */
    if (nodeMap != null) {
      nodeMap.clear();
    }
  }

  /**
   * @brief Shutdown database
   * @author 1sand0s
   * @since 1.0.0
   * @version 1.0.0
   */
  @Override
  public void shutDown() throws RemoteException {
    managementService.shutdown();
  }

  /**
   * @brief Add shutdown hook to the database
   * @author 1sand0s
   * @param managementService reference to the database management service
   * @since 1.0.0
   * @version 1.0.0
   */
  private void registerShutdownHook(final DatabaseManagementService managementService) {
    Runtime.getRuntime()
        .addShutdownHook(
            new Thread() {
              @Override
              public void run() {
                managementService.shutdown();
              }
            });
  }

  /**
   * @brief Sets the path from where Csv files generated by native agent are read
   * @author 1sand0s
   * @param path Path where the native agent writes the Csv files
   * @since 1.0.0
   * @version 1.0.0
   */
  @Override
  public void setPath(String path) throws RemoteException {
    CsvPath = path;
  }

  /**
   * @brief Sets the path from where Csv files generated by native agent are read
   * @author 1sand0s
   * @param path Path where the native agent writes the Csv files
   * @since 1.0.0
   * @version 1.0.0
   */
  @Override
  public String getPath() throws RemoteException {
    return CsvPath;
  }

  public static void main(String[] args)
      throws RemoteException, IOException, AlreadyBoundException {
    int rmiPort = OGOProperties.getRmiPort();
    String name = "Neo4JGraphQueryEngine";
    AbstractGraphQueryInterface engine = new Neo4JGraphQueryEngine();
    AbstractGraphQueryInterface stub =
        (AbstractGraphQueryInterface) UnicastRemoteObject.exportObject(engine, 0);
    Registry registry = LocateRegistry.createRegistry(rmiPort);
    registry.bind(name, stub);
    ((Neo4JGraphQueryEngine) engine).setup();
  }
}
