package org.ogo.cypher;

import java.util.ArrayList;
import java.util.List;
import org.antlr.v4.runtime.BaseErrorListener;
import org.antlr.v4.runtime.CharStreams;
import org.antlr.v4.runtime.CommonTokenStream;
import org.antlr.v4.runtime.Lexer;
import org.antlr.v4.runtime.Parser;
import org.antlr.v4.runtime.RecognitionException;
import org.antlr.v4.runtime.Recognizer;

public final class CypherSyntaxValidator {

    public record SyntaxError(int line, int column, int offset, String message) {
    }

    private CypherSyntaxValidator() {
    }

    public static List<String> check(String query) {
        List<SyntaxError> detailed = checkWithLocations(query);
        List<String> compact = new ArrayList<>(detailed.size());
        for (SyntaxError error : detailed) {
            compact.add("line " + error.line() + ":" + error.column() + " " + error.message());
        }
        return compact;
    }

    public static List<SyntaxError> checkWithLocations(String query) {
        CollectingErrorListener lexerErrors = new CollectingErrorListener();
        CollectingErrorListener parserErrors = new CollectingErrorListener();

        Lexer lexer = createLexer(query);
        lexer.removeErrorListeners();
        lexer.addErrorListener(lexerErrors);

        Parser parser = createParser(lexer);
        parser.removeErrorListeners();
        parser.addErrorListener(parserErrors);
        parseCypher(parser);

        List<SyntaxError> all = new ArrayList<>(lexerErrors.errors);
        all.addAll(parserErrors.errors);
        return all;
    }

    private static Lexer createLexer(String query) {
        try {
            Class<?> lexerClass = Class.forName("org.ogodb.antlr4.CypherLexer");
            return (Lexer) lexerClass.getConstructor(org.antlr.v4.runtime.CharStream.class)
                    .newInstance(CharStreams.fromString(query));
        } catch (ReflectiveOperationException e) {
            throw new IllegalStateException(
                    "Could not load generated class org.ogodb.antlr4.CypherLexer. Run 'mvn -DskipTests test-compile'.",
                    e);
        }
    }

    private static Parser createParser(Lexer lexer) {
        try {
            Class<?> parserClass = Class.forName("org.ogodb.antlr4.CypherParser");
            return (Parser) parserClass.getConstructor(org.antlr.v4.runtime.TokenStream.class)
                    .newInstance(new CommonTokenStream(lexer));
        } catch (ReflectiveOperationException e) {
            throw new IllegalStateException(
                    "Could not load generated class org.ogodb.antlr4.CypherParser. Run 'mvn -DskipTests test-compile'.",
                    e);
        }
    }

    private static void parseCypher(Parser parser) {
        try {
            parser.getClass().getMethod("oC_Cypher").invoke(parser);
        } catch (ReflectiveOperationException e) {
            throw new IllegalStateException("Could not invoke parser entry method oC_Cypher().", e);
        }
    }

    private static int toAbsoluteOffset(String source, int line, int column) {
        if (source == null || source.isEmpty()) {
            return 0;
        }

        if (line <= 1) {
            return clamp(column, 0, source.length());
        }

        int currentLine = 1;
        int lineStart = 0;
        for (int i = 0; i < source.length(); i++) {
            if (source.charAt(i) == '\n') {
                currentLine++;
                lineStart = i + 1;
                if (currentLine == line) {
                    return clamp(lineStart + column, 0, source.length());
                }
            }
        }

        return clamp(lineStart + column, 0, source.length());
    }

    private static int clamp(int value, int min, int max) {
        return Math.max(min, Math.min(max, value));
    }

    private static final class CollectingErrorListener extends BaseErrorListener {
        private final List<SyntaxError> errors = new ArrayList<>();

        @Override
        public void syntaxError(Recognizer<?, ?> recognizer, Object offendingSymbol, int line, int charPositionInLine,
                String msg, RecognitionException e) {
            String source = recognizer != null && recognizer.getInputStream() != null
                    ? recognizer.getInputStream().toString() : "";
            int absoluteOffset = toAbsoluteOffset(source, line, charPositionInLine);
            errors.add(new SyntaxError(line, charPositionInLine, absoluteOffset, msg));
        }
    }
}
