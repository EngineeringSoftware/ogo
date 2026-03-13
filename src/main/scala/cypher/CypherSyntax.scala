package cypher

import org.antlr.v4.runtime.*
import org.ogodb.antlr4.{CypherLexer, CypherParser}
import scala.collection.mutable.ArrayBuffer

object CypherSyntax:

  final case class SyntaxError(line: Int, col: Int, msg: String):
    override def toString: String = s"line $line:$col $msg"

  private final class CollectingErrorListener extends BaseErrorListener:
    val errors: ArrayBuffer[SyntaxError] = ArrayBuffer.empty

    override def syntaxError(
        recognizer: Recognizer[?, ?],
        offendingSymbol: Any,
        line: Int,
        charPositionInLine: Int,
        msg: String,
        e: RecognitionException | Null
    ): Unit =
      errors += SyntaxError(line, charPositionInLine, msg)

  /** Parse `query` using the openCypher ANTLR grammar.
   *  Returns Right(()) when the query is syntactically valid,
   *  or Left(errors) with one entry per syntax error. */
  def check(query: String): Either[List[SyntaxError], Unit] =
    val input = CharStreams.fromString(query)

    val lexer = CypherLexer(input)
    lexer.removeErrorListeners()
    val lexerErrors = CollectingErrorListener()
    lexer.addErrorListener(lexerErrors)

    val tokens = CommonTokenStream(lexer)
    val parser = CypherParser(tokens)
    parser.removeErrorListeners()
    val parserErrors = CollectingErrorListener()
    parser.addErrorListener(parserErrors)

    parser.oC_Cypher()

    val allErrors = (lexerErrors.errors ++ parserErrors.errors).toList
    if allErrors.isEmpty then Right(())
    else Left(allErrors)
