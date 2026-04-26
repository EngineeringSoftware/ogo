package org.ogo.client.processor;

import com.sun.source.tree.CompilationUnitTree;
import com.sun.source.tree.ExpressionTree;
import com.sun.source.util.Trees;
import com.sun.tools.javac.processing.JavacProcessingEnvironment;
import com.sun.tools.javac.util.Log;
import java.io.IOException;
import javax.annotation.processing.ProcessingEnvironment;
import javax.tools.JavaFileObject;

final class DiagnosticEmitter {

    private final Trees trees;
    private final Log javacLog;

    DiagnosticEmitter(ProcessingEnvironment processingEnv, Trees trees) {
        this.trees = trees;
        if (processingEnv instanceof JavacProcessingEnvironment javacProcessingEnvironment) {
            this.javacLog = Log.instance(javacProcessingEnvironment.getContext());
        } else {
            this.javacLog = null;
        }
    }

    boolean printPreciseDiagnostic(CompilationUnitTree cu, ExpressionTree diagnosticTree, int queryOffset,
            String message) {
        if (javacLog == null || cu.getSourceFile() == null) {
            return false;
        }

        long absolutePosition = resolveAbsoluteSourcePosition(cu, diagnosticTree, queryOffset);
        if (absolutePosition < 0 || absolutePosition > Integer.MAX_VALUE) {
            return false;
        }

        JavaFileObject prevSource = javacLog.useSource(cu.getSourceFile());
        try {
            int position = (int) absolutePosition;
            javacLog.error(position, "proc.messager", message);
            return true;
        } catch (Throwable ignored) {
            return false;
        } finally {
            javacLog.useSource(prevSource);
        }
    }

    private long resolveAbsoluteSourcePosition(CompilationUnitTree cu, ExpressionTree diagnosticTree, int queryOffset) {
        long start = trees.getSourcePositions().getStartPosition(cu, diagnosticTree);
        long end = trees.getSourcePositions().getEndPosition(cu, diagnosticTree);
        if (start < 0) {
            return -1;
        }
        if (queryOffset <= 0) {
            return start;
        }
        if (end <= start) {
            return start;
        }

        String source;
        try {
            source = cu.getSourceFile().getCharContent(true).toString();
        } catch (IOException ignored) {
            return start;
        }

        int from = (int) Math.max(0, Math.min(start, source.length()));
        int to = (int) Math.max(from, Math.min(end, source.length()));
        if (from >= to) {
            return start;
        }

        String token = source.substring(from, to);
        int relative = mapQueryOffsetToLiteralSourceOffset(token, queryOffset);
        return start + relative;
    }

    private int mapQueryOffsetToLiteralSourceOffset(String literalToken, int queryOffset) {
        if (literalToken.isEmpty()) {
            return 0;
        }

        if (!literalToken.startsWith("\"") || literalToken.length() < 2) {
            return Math.max(0, Math.min(queryOffset, literalToken.length() - 1));
        }

        int contentStart = 1;
        int contentEnd = literalToken.endsWith("\"") ? literalToken.length() - 1 : literalToken.length();
        int decodedIndex = 0;
        int i = contentStart;
        while (i < contentEnd) {
            if (decodedIndex >= queryOffset) {
                return i;
            }

            char c = literalToken.charAt(i);
            if (c != '\\') {
                i++;
                decodedIndex++;
                continue;
            }

            int consumed = consumeEscapeSequence(literalToken, i, contentEnd);
            i += consumed;
            decodedIndex++;
        }

        return Math.max(contentStart, contentEnd - 1);
    }

    private int consumeEscapeSequence(String literalToken, int slashIndex, int contentEnd) {
        int next = slashIndex + 1;
        if (next >= contentEnd) {
            return 1;
        }

        char marker = literalToken.charAt(next);
        if (marker == 'u') {
            int i = next;
            while (i < contentEnd && literalToken.charAt(i) == 'u') {
                i++;
            }
            if (i + 4 <= contentEnd) {
                return i + 4 - slashIndex;
            }
            return contentEnd - slashIndex;
        }

        if (marker >= '0' && marker <= '7') {
            int i = next + 1;
            int maxDigits = marker <= '3' ? 3 : 2;
            int consumedDigits = 1;
            while (i < contentEnd && consumedDigits < maxDigits) {
                char ch = literalToken.charAt(i);
                if (ch < '0' || ch > '7') {
                    break;
                }
                i++;
                consumedDigits++;
            }
            return i - slashIndex;
        }

        return 2;
    }
}
