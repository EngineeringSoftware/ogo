package org.ogo.client.processor;

import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;

final class QueryNormalizer {

    record NormalizedQuery(String original, String normalized, int[] normalizedToOriginal) {
    }

    NormalizedQuery normalizeForSyntaxCheck(String query) {
        Matcher matcher = QueryProcessorConstants.DOLLAR_ARG.matcher(query);
        StringBuilder normalized = new StringBuilder();
        List<Integer> mapping = new ArrayList<>();
        int cursor = 0;

        while (matcher.find()) {
            appendIdentitySegment(query, cursor, matcher.start(), normalized, mapping);
            appendReplacement("hash:0", matcher.start(), normalized, mapping);
            cursor = matcher.end();
        }
        appendIdentitySegment(query, cursor, query.length(), normalized, mapping);
        return new NormalizedQuery(query, normalized.toString(), toIntArray(mapping));
    }

    int mapNormalizedOffset(NormalizedQuery normalizedQuery, int normalizedOffset) {
        if (normalizedQuery.normalizedToOriginal().length == 0) {
            return 0;
        }
        if (normalizedOffset >= normalizedQuery.normalizedToOriginal().length) {
            return normalizedQuery.original().length();
        }
        int safeOffset = Math.max(0, normalizedOffset);
        return normalizedQuery.normalizedToOriginal()[safeOffset];
    }

    String normalizeFormatStringForSyntaxCheck(String format) {
        String withTemporaryValues = QueryProcessorConstants.FORMAT_SPECIFIER.matcher(format)
                .replaceAll(QueryProcessorConstants.CONCAT_PLACEHOLDER);
        return withTemporaryValues.replace("%%", "%");
    }

    private void appendIdentitySegment(String source, int start, int end, StringBuilder target, List<Integer> map) {
        for (int i = start; i < end; i++) {
            target.append(source.charAt(i));
            map.add(i);
        }
    }

    private void appendReplacement(String replacement, int sourceAnchor, StringBuilder target, List<Integer> map) {
        target.append(replacement);
        for (int i = 0; i < replacement.length(); i++) {
            map.add(sourceAnchor);
        }
    }

    private int[] toIntArray(List<Integer> values) {
        int[] result = new int[values.size()];
        for (int i = 0; i < values.size(); i++) {
            result[i] = values.get(i);
        }
        return result;
    }
}
