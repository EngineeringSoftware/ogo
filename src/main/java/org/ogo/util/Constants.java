package org.ogo.util;

public abstract class Constants {

    /** Name of the file to which whitelist of packages will be written to */
    public static final String WhiteListName = "CyphssertWhitelist.txt";

    /** Name of the file to which backlist of packages will be written to */
    public static final String BlackListName = "CyphssertBlacklist.txt";

    /** Name of the file to which ogo maven properties will be written to */
    public static final String OGO_PROPERTIES_FILE = "ogo.properties";

    /** Name of the maven property referring to RMI port number used */
    public static final String OGO_PROPERTY_RMI_PORT = "rmiPort";

    /** Name of the maven property referring to inMemory */
    public static final String OGO_PROPERTY_IN_MEMORY = "inMemory";

    /** Name of the maven property referring to clearDatabase */
    public static final String OGO_PROPERTY_CLEAR_DATABASE = "clearDatabase";
}
