package cypher

import scala.quoted.*

object Cypher:
  inline def apply(inline query: String): Query =
    ${ CypherMacro.impl('query) }

final case class Query(text: String)