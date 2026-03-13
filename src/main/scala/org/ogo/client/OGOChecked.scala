package org.ogo.client

import cypher.Cypher

object OGOChecked:
  private def toJavaObject(value: Any): Object =
    value.asInstanceOf[AnyRef]

  private def toJavaObjects(values: Seq[Any]): Array[Object] =
    values.iterator.map(toJavaObject).toArray

  inline def queryBool(inline cQuery: String): Boolean =
    OGO.queryBool(Cypher(cQuery).text)

  inline def queryBool(inline cQuery: String, objects: Any*): Boolean =
    OGO.queryBool(Cypher(cQuery).text, toJavaObjects(objects)*)

  inline def queryBool(root: Any, inline cQuery: String): Boolean =
    OGO.queryBool(toJavaObject(root), Cypher(cQuery).text)

  inline def queryBool(root: Any, inline cQuery: String, objects: Any*): Boolean =
    OGO.queryBool(toJavaObject(root), Cypher(cQuery).text, toJavaObjects(objects)*)

  inline def queryInt(inline cQuery: String): Int =
    OGO.queryInt(Cypher(cQuery).text)

  inline def queryInt(inline cQuery: String, objects: Any*): Int =
    OGO.queryInt(Cypher(cQuery).text, toJavaObjects(objects)*)

  inline def queryInt(root: Any, inline cQuery: String): Int =
    OGO.queryInt(toJavaObject(root), Cypher(cQuery).text)

  inline def queryInt(root: Any, inline cQuery: String, objects: Any*): Int =
    OGO.queryInt(toJavaObject(root), Cypher(cQuery).text, toJavaObjects(objects)*)

  inline def queryLong(inline cQuery: String): Long =
    OGO.queryLong(Cypher(cQuery).text)

  inline def queryLong(inline cQuery: String, objects: Any*): Long =
    OGO.queryLong(Cypher(cQuery).text, toJavaObjects(objects)*)

  inline def queryLong(root: Any, inline cQuery: String): Long =
    OGO.queryLong(toJavaObject(root), Cypher(cQuery).text)

  inline def queryLong(root: Any, inline cQuery: String, objects: Any*): Long =
    OGO.queryLong(toJavaObject(root), Cypher(cQuery).text, toJavaObjects(objects)*)

  inline def query(inline cQuery: String, objects: Any*): Array[Object] =
    OGO.query(Cypher(cQuery).text, toJavaObjects(objects)*)

  inline def query(root: Any, inline cQuery: String): Array[Object] =
    OGO.query(toJavaObject(root), Cypher(cQuery).text)

  inline def query(root: Any, inline cQuery: String, objects: Any*): Array[Object] =
    OGO.query(toJavaObject(root), Cypher(cQuery).text, toJavaObjects(objects)*)
