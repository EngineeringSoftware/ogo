package org.ogo.cypher;

import java.util.ArrayList;
import java.util.List;
import org.antlr.v4.runtime.BaseErrorListener;
import org.antlr.v4.runtime.CharStreams;
import org.antlr.v4.runtime.CommonTokenStream;
import org.antlr.v4.runtime.RecognitionException;
import org.antlr.v4.runtime.Recognizer;
import org.ogodb.antlr4.CypherLexer;
import org.ogodb.antlr4.CypherParser;

public final class CypherSyntaxValidator {

    private CypherSyntaxValidator() {
    }

    public static List<String> check(String query) {
        CollectingErrorListener lexerErrors = new CollectingErrorListener();
        CollectingErrorListener parserErrors = new CollectingErrorListener();

        CypherLexer lexer = new CypherLexer(CharStreams.fromString(query));
        lexer.removeErrorListeners();
        lexer.addErrorListener(lexerErrors);

        CypherParser parser = new CypherParser(new CommonTokenStream(lexer));
        parser.removeErrorListeners();
        parser.addErrorListener(parserErrors);
        parser.oC_Cypher();

        List<String> all = new ArrayList<>(lexerErrors.errors);
        all.addAll(parserErrors.errors);
        return all;
    }

    private static final class CollectingErrorListener extends BaseErrorListener {
        private final List<String> errors = new ArrayList<>();

        @Override
        public void syntaxError(Recognizer<?, ?> recognizer, Object offendingSymbol, int line, int charPositionInLine,
                String msg, RecognitionException e) {
            errors.add("line " + line + ":" + charPositionInLine + " " + msg);
        }
    }
}
