package org.ogo.client.processor;

import java.util.Set;
import java.util.regex.Pattern;

final class QueryProcessorConstants {

    static final Set<String> QUERY_METHODS = Set.of("query", "queryInt", "queryLong", "queryBool");
    static final String OGO_FQCN = "org.ogo.client.OGO";
    static final String STRING_FQCN = "java.lang.String";
    static final String FORMAT_METHOD = "format";
    static final String CONCAT_PLACEHOLDER = "ogotmp";
    static final Pattern DOLLAR_ARG = Pattern.compile("\\$\\d+");
    static final Pattern FORMAT_SPECIFIER = Pattern
            .compile("%(?!%)(?:\\d+\\$)?[-#+ 0,(<]*(?:\\d+)?(?:\\.\\d+)?(?:[tT])?[a-zA-Z]");

    private QueryProcessorConstants() {
    }
}
