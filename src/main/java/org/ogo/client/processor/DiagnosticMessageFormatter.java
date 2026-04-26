package org.ogo.client.processor;

import org.ogo.cypher.CypherSyntaxValidator;

final class DiagnosticMessageFormatter {

    private record LineColumn(int line, int column) {
    }

    String renderErrorWithOriginalPosition(String methodName, QueryNormalizer.NormalizedQuery normalizedQuery,
            CypherSyntaxValidator.SyntaxError error, int originalOffset) {
        if (normalizedQuery.original().isEmpty()) {
            return "Invalid Cypher syntax in OGO." + methodName + " call: " + error.message();
        }

        int safeOriginalOffset = Math.min(Math.max(0, originalOffset), normalizedQuery.original().length() - 1);
        LineColumn originalPos = indexToLineColumn(normalizedQuery.original(), safeOriginalOffset);
        String snippet = buildSnippet(normalizedQuery.original(), safeOriginalOffset);
        return "Invalid Cypher syntax in OGO." + methodName + " call at query line " + originalPos.line() + ", column "
                + (originalPos.column() + 1) + " (offset " + safeOriginalOffset + "): " + error.message() + " | "
                + snippet;
    }

    private String buildSnippet(String query, int offset) {
        int lineStart = offset;
        while (lineStart > 0 && query.charAt(lineStart - 1) != '\n') {
            lineStart--;
        }
        int lineEnd = offset;
        while (lineEnd < query.length() && query.charAt(lineEnd) != '\n') {
            lineEnd++;
        }

        String lineText = query.substring(lineStart, lineEnd);
        int caret = Math.max(0, Math.min(offset - lineStart, Math.max(0, lineText.length() - 1)));
        StringBuilder pointer = new StringBuilder();
        for (int i = 0; i < caret; i++) {
            char c = lineText.charAt(i);
            pointer.append(c == '\t' ? '\t' : ' ');
        }
        pointer.append('^');
        return lineText + "\\n" + pointer;
    }

    private LineColumn indexToLineColumn(String text, int index) {
        int safeIndex = Math.max(0, Math.min(index, Math.max(0, text.length() - 1)));
        int line = 1;
        int lineStart = 0;
        for (int i = 0; i < safeIndex; i++) {
            if (text.charAt(i) == '\n') {
                line++;
                lineStart = i + 1;
            }
        }
        return new LineColumn(line, safeIndex - lineStart);
    }
}
