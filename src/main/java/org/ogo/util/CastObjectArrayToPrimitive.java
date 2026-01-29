package org.ogo.util;

import org.neo4j.graphdb.Node;

/**
 * @author 1sand0
 */
public class CastObjectArrayToPrimitive {

  /**
   * regular
   *
   * @author 1sand0s
   * @param propertyValue String Array corresponding to property values
   * @return Boolean[] returns the cast Boolean Array
   * @since 1.0.0
   */
  public static Boolean[] castToBoolean(String[] propertyValue) {
    Boolean[] obj2 = new Boolean[propertyValue.length];
    for (int j = 0; j < propertyValue.length; j++) {
      obj2[j] = Boolean.parseBoolean(propertyValue[j]);
    }
    return obj2;
  }

  /**
   * regular
   *
   * @author 1sand0s
   * @param propertyValue String Array corresponding to property values
   * @return Byte[] returns the cast Byte Array
   * @since 1.0.0
   */
  public static Byte[] castToByte(String[] propertyValue) {
    Byte[] obj2 = new Byte[propertyValue.length];
    for (int j = 0; j < propertyValue.length; j++) {
      obj2[j] = Byte.parseByte(propertyValue[j]);
    }
    return obj2;
  }

  /**
   * regular
   *
   * @author 1sand0s
   * @param propertyValue String Array corresponding to property values
   * @return Short[] returns the cast Short Array
   * @since 1.0.0
   */
  public static Short[] castToShort(String[] propertyValue) {
    Short[] obj2 = new Short[propertyValue.length];
    for (int j = 0; j < propertyValue.length; j++) {
      obj2[j] = Short.parseShort(propertyValue[j]);
    }
    return obj2;
  }

  /**
   * regular
   *
   * @author 1sand0s
   * @param propertyValue String Array corresponding to property values
   * @return Integer[] returns the cast Integer Array
   * @since 1.0.0
   */
  public static Integer[] castToInteger(String[] propertyValue) {
    Integer[] obj2 = new Integer[propertyValue.length];
    for (int j = 0; j < propertyValue.length; j++) {
      obj2[j] = Integer.parseInt(propertyValue[j]);
    }
    return obj2;
  }

  /**
   * regular
   *
   * @author 1sand0s
   * @param propertyValue String Array corresponding to property values
   * @return Long[] returns the cast Long Array
   * @since 1.0.0
   */
  public static Long[] castToLong(String[] propertyValue) {
    Long[] obj2 = new Long[propertyValue.length];
    for (int j = 0; j < propertyValue.length; j++) {
      obj2[j] = Long.parseLong(propertyValue[j]);
    }
    return obj2;
  }

  /**
   * regular
   *
   * @author 1sand0s
   * @param propertyValue String Array corresponding to property values
   * @return Float[] returns the cast Float Array
   * @since 1.0.0
   */
  public static Float[] castToFloat(String[] propertyValue) {
    Float[] obj2 = new Float[propertyValue.length];
    for (int j = 0; j < propertyValue.length; j++) {
      obj2[j] = Float.parseFloat(propertyValue[j]);
    }
    return obj2;
  }

  /**
   * regular
   *
   * @author 1sand0s
   * @param propertyValue String Array corresponding to property values
   * @return Double[] returns the cast Double Array
   * @since 1.0.0
   */
  public static Double[] castToDouble(String[] propertyValue) {
    Double[] obj2 = new Double[propertyValue.length];
    for (int j = 0; j < propertyValue.length; j++) {
      obj2[j] = Double.parseDouble(propertyValue[j]);
    }
    return obj2;
  }

  /**
   * regular
   *
   * @author 1sand0s
   * @param propertyValue String Array corresponding to property values
   * @return Character[] returns the cast Character Array
   * @since 1.0.0
   */
  public static Character[] castToCharacter(String[] propertyValue) {
    Character[] obj2 = new Character[propertyValue.length];
    for (int j = 0; j < propertyValue.length; j++) {
      obj2[j] = propertyValue[j].charAt(0);
    }
    return obj2;
  }

  /**
   * regular
   *
   * @author 1sand0s
   * @param propertyName name of the property
   * @param propertyValue to be cast into appropriate object
   * @param propertyType type of every element in propertyValue
   * @param node Neo4J node
   * @since 1.0.0
   */
  public static Node castToNeo4JPropertyType(
      String propertyName, String[] propertyValue, String propertyType, Node node) {
    if (propertyType.compareTo("JBOOLEAN_ARRAY") == 0) {
      node.setProperty(propertyName, castToBoolean(propertyValue));
    } else if (propertyType.compareTo("JBYTE_ARRAY") == 0) {
      node.setProperty(propertyName, castToByte(propertyValue));
    } else if (propertyType.compareTo("JCHAR_ARRAY") == 0) {
      node.setProperty(propertyName, castToCharacter(propertyValue));
    } else if (propertyType.compareTo("JSHORT_ARRAY") == 0) {
      node.setProperty(propertyName, castToShort(propertyValue));
    } else if (propertyType.compareTo("JINT_ARRAY") == 0) {
      node.setProperty(propertyName, castToInteger(propertyValue));
    } else if (propertyType.compareTo("JLONG_ARRAY") == 0) {
      node.setProperty(propertyName, castToLong(propertyValue));
    } else if (propertyType.compareTo("JFLOAT_ARRAY") == 0) {
      node.setProperty(propertyName, castToFloat(propertyValue));
    } else if (propertyType.compareTo("JDOUBLE_ARRAY") == 0) {
      node.setProperty(propertyName, castToDouble(propertyValue));
    } else if (propertyType.compareTo("JSTRING_ARRAY") == 0) {
      node.setProperty(propertyName, propertyValue);
    } else {
      if (propertyValue.length == 1) {
        if (propertyType.compareTo("JBOOLEAN") == 0) {
          node.setProperty(propertyName, Boolean.parseBoolean(propertyValue[0]));
        } else if (propertyType.compareTo("JBYTE") == 0) {
          node.setProperty(propertyName, Byte.parseByte(propertyValue[0]));
        } else if (propertyType.compareTo("JCHAR") == 0) {
          node.setProperty(propertyName, propertyValue[0].charAt(0));
        } else if (propertyType.compareTo("JSHORT") == 0) {
          node.setProperty(propertyName, Short.parseShort(propertyValue[0]));
        } else if (propertyType.compareTo("JINT") == 0) {
          node.setProperty(propertyName, Integer.parseInt(propertyValue[0]));
        } else if (propertyType.compareTo("JLONG") == 0) {
          node.setProperty(propertyName, Long.parseLong(propertyValue[0]));
        } else if (propertyType.compareTo("JFLOAT") == 0) {
          if (propertyValue[0].compareTo("inf") == 0) {
            node.setProperty(propertyName, Float.POSITIVE_INFINITY);
          } else if (propertyValue[0].compareTo("-inf") == 0) {
            node.setProperty(propertyName, Float.NEGATIVE_INFINITY);
          } else if (propertyValue[0].compareTo("nan") == 0) {
            node.setProperty(propertyName, Float.NaN);
          } else {
            node.setProperty(propertyName, Float.parseFloat(propertyValue[0]));
          }
        } else if (propertyType.compareTo("JDOUBLE") == 0) {
          if (propertyValue[0].compareTo("inf") == 0) {
            node.setProperty(propertyName, Double.POSITIVE_INFINITY);
          } else if (propertyValue[0].compareTo("-inf") == 0) {
            node.setProperty(propertyName, Double.NEGATIVE_INFINITY);
          } else if (propertyValue[0].compareTo("nan") == 0) {
            node.setProperty(propertyName, Double.NaN);
          } else {
            node.setProperty(propertyName, Double.parseDouble(propertyValue[0]));
          }
        } else if (propertyType.compareTo("JSTRING") == 0) {
          node.setProperty(propertyName, propertyValue[0]);
        }
      }
    }
    return node;
  }

  /**
   * regular
   *
   * @author 1sand0s
   * @param obj Object whose instance is to be determined
   * @return true if obj belongs to Primitive Type
   * @since 1.0.0
   */
  public static boolean checkPrimitiveType(Object obj) {
    return (obj instanceof Byte
        || obj instanceof Short
        || obj instanceof Integer
        || obj instanceof Long
        || obj instanceof Float
        || obj instanceof Double
        || obj instanceof Character
        || obj instanceof Boolean
        || obj instanceof String);
  }
}
