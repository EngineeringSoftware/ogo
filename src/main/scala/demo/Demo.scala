package demo

import cypher.Cypher

object Demo:
  val q =
    Cypher(
      """MATCH (n) RETURN n.a
        """
    )