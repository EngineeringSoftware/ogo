package cypher

import scala.quoted.*

object CypherMacro:

  def impl(queryExpr: Expr[String])(using Quotes): Expr[Query] =
    import quotes.reflect.*

    queryExpr.value match
      case Some(query) =>
        CypherSyntax.check(query) match
          case Right(_) =>
            '{ Query($queryExpr) }
          case Left(errors) =>
            val msg = errors.map(_.toString).mkString("\n")
            report.errorAndAbort(s"Cypher syntax error:\n$msg", queryExpr)

      case None =>
        report.errorAndAbort(
          "Cypher query must be a literal string for compile-time syntax checking.",
          queryExpr
        )