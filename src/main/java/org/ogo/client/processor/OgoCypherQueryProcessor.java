package org.ogo.client.processor;

import com.sun.source.tree.BinaryTree;
import com.sun.source.tree.CompilationUnitTree;
import com.sun.source.tree.ExpressionTree;
import com.sun.source.tree.IdentifierTree;
import com.sun.source.tree.LiteralTree;
import com.sun.source.tree.MemberSelectTree;
import com.sun.source.tree.MethodInvocationTree;
import com.sun.source.tree.ParenthesizedTree;
import com.sun.source.tree.Tree;
import com.sun.source.util.TreePath;
import com.sun.source.util.TreePathScanner;
import com.sun.source.util.Trees;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javax.annotation.processing.AbstractProcessor;
import javax.annotation.processing.ProcessingEnvironment;
import javax.annotation.processing.RoundEnvironment;
import javax.annotation.processing.SupportedAnnotationTypes;
import javax.annotation.processing.SupportedSourceVersion;
import javax.lang.model.SourceVersion;
import javax.lang.model.element.Element;
import javax.lang.model.element.ExecutableElement;
import javax.lang.model.element.TypeElement;
import javax.lang.model.element.VariableElement;
import javax.tools.Diagnostic;
import org.ogo.cypher.CypherSyntaxValidator;

@SupportedAnnotationTypes("*")
@SupportedSourceVersion(SourceVersion.RELEASE_21)
public final class OgoCypherQueryProcessor extends AbstractProcessor {

    private record NormalizedQuery(String original, String normalized, int[] normalizedToOriginal) {
    }

    private record LineColumn(int line, int column) {
    }

    private static final Set<String> QUERY_METHODS = Set.of("query", "queryInt", "queryLong", "queryBool");
    private static final String OGO_FQCN = "org.ogo.client.OGO";
    private static final String STRING_FQCN = "java.lang.String";
    private static final String FORMAT_METHOD = "format";
    private static final String CONCAT_PLACEHOLDER = "ogotmp";
    private static final Pattern DOLLAR_ARG = Pattern.compile("\\$\\d+");
    private static final Pattern FORMAT_SPECIFIER = Pattern
            .compile("%(?!%)(?:\\d+\\$)?[-#+ 0,(<]*(?:\\d+)?(?:\\.\\d+)?(?:[tT])?[a-zA-Z]");

    private Trees trees;

    @Override
    public synchronized void init(ProcessingEnvironment processingEnv) {
        super.init(processingEnv);
        trees = Trees.instance(processingEnv);
    }

    @Override
    public boolean process(Set<? extends TypeElement> annotations, RoundEnvironment roundEnv) {
        for (Element root : roundEnv.getRootElements()) {
            TreePath rootPath = trees.getPath(root);
            if (rootPath == null) {
                continue;
            }
            new QueryInvocationScanner().scan(rootPath, null);
        }
        return false;
    }

    private final class QueryInvocationScanner extends TreePathScanner<Void, Void> {
        @Override
        public Void visitMethodInvocation(MethodInvocationTree node, Void unused) {
            TreePath path = getCurrentPath();
            Element element = trees.getElement(path);
            if (!(element instanceof ExecutableElement method)) {
                return super.visitMethodInvocation(node, unused);
            }

            Element enclosing = method.getEnclosingElement();
            if (!(enclosing instanceof TypeElement owner)) {
                return super.visitMethodInvocation(node, unused);
            }

            String ownerName = owner.getQualifiedName().toString();
            String methodName = method.getSimpleName().toString();
            if (!OGO_FQCN.equals(ownerName) || !QUERY_METHODS.contains(methodName)) {
                return super.visitMethodInvocation(node, unused);
            }

            int queryArgIndex = findQueryStringParamIndex(method);
            if (queryArgIndex < 0 || queryArgIndex >= node.getArguments().size()) {
                return super.visitMethodInvocation(node, unused);
            }

            ExpressionTree queryArg = node.getArguments().get(queryArgIndex);
            String query = resolveConstantString(queryArg);
            if (query == null) {
                return super.visitMethodInvocation(node, unused);
            }

            // Match OGO runtime behavior where $N is replaced with hash:<identityHashCode>.
            NormalizedQuery normalizedQuery = normalizeForSyntaxCheck(query);
            if (normalizedQuery.normalized().isBlank()) {
                return super.visitMethodInvocation(node, unused);
            }
            List<CypherSyntaxValidator.SyntaxError> errors;
            try {
                errors = CypherSyntaxValidator.checkWithLocations(normalizedQuery.normalized());
            } catch (IllegalStateException ex) {
                // Generated parser classes may be unavailable during early build/import phases.
                return super.visitMethodInvocation(node, unused);
            }
            if (!errors.isEmpty()) {
                CompilationUnitTree cu = path.getCompilationUnit();
                for (CypherSyntaxValidator.SyntaxError error : errors) {
                    int originalOffset = mapNormalizedOffset(normalizedQuery, error.offset());
                    int anchorOffset = Math.min(Math.max(0, originalOffset), Math.max(0, query.length() - 1));
                    ExpressionTree targetTree = findDiagnosticTree(queryArg, anchorOffset);
                    String message = renderErrorWithOriginalPosition(methodName, normalizedQuery, error,
                            originalOffset);
                    trees.printMessage(Diagnostic.Kind.ERROR, message, targetTree != null ? targetTree : queryArg, cu);
                }
            }

            return super.visitMethodInvocation(node, unused);
        }

        private int findQueryStringParamIndex(ExecutableElement method) {
            List<? extends VariableElement> params = method.getParameters();
            for (int i = 0; i < params.size(); i++) {
                String typeName = params.get(i).asType().toString();
                if ("java.lang.String".equals(typeName)) {
                    return i;
                }
            }
            return -1;
        }

        private String resolveConstantString(ExpressionTree expr) {
            if (expr == null) {
                return null;
            }
            return switch (expr.getKind()) {
            case STRING_LITERAL -> (String) ((LiteralTree) expr).getValue();
            case IDENTIFIER -> constantValueFromElement((IdentifierTree) expr);
            case MEMBER_SELECT -> constantValueFromElement((MemberSelectTree) expr);
            case PARENTHESIZED -> resolveConstantString(((ParenthesizedTree) expr).getExpression());
            case PLUS -> resolvePlusExpression((BinaryTree) expr);
            case METHOD_INVOCATION -> resolveFormatInvocation((MethodInvocationTree) expr);
            default -> null;
            };
        }

        private String resolveFormatInvocation(MethodInvocationTree invocation) {
            TreePath path = TreePath.getPath(getCurrentPath(), invocation);
            if (path == null) {
                return null;
            }

            Element element = trees.getElement(path);
            if (!(element instanceof ExecutableElement method)) {
                return null;
            }

            Element enclosing = method.getEnclosingElement();
            if (!(enclosing instanceof TypeElement owner)) {
                return null;
            }

            if (!STRING_FQCN.equals(owner.getQualifiedName().toString())
                    || !FORMAT_METHOD.equals(method.getSimpleName().toString())) {
                return null;
            }

            int formatArgIndex = findFormatStringParamIndex(method);
            if (formatArgIndex < 0 || formatArgIndex >= invocation.getArguments().size()) {
                return null;
            }

            String format = resolveConstantString(invocation.getArguments().get(formatArgIndex));
            if (format == null) {
                return null;
            }

            return normalizeFormatStringForSyntaxCheck(format);
        }

        private String resolvePlusExpression(BinaryTree expr) {
            String left = resolveConstantString(expr.getLeftOperand());
            String right = resolveConstantString(expr.getRightOperand());

            // Keep query validation active for string concatenations with runtime values.
            if (left == null) {
                left = resolveConcatenationFallback(expr.getLeftOperand());
            }
            if (right == null) {
                right = resolveConcatenationFallback(expr.getRightOperand());
            }

            if (left == null || right == null) {
                return null;
            }
            return left + right;
        }

        private String resolveConcatenationFallback(ExpressionTree expr) {
            if (expr == null) {
                return null;
            }

            return switch (expr.getKind()) {
            case NULL_LITERAL -> "null";
            default -> CONCAT_PLACEHOLDER;
            };
        }

        private int findFormatStringParamIndex(ExecutableElement method) {
            List<? extends VariableElement> params = method.getParameters();
            if (params.isEmpty()) {
                return -1;
            }

            // String.format(String, Object...) and String.format(Locale, String, Object...)
            String firstType = params.get(0).asType().toString();
            if ("java.lang.String".equals(firstType)) {
                return 0;
            }
            if (params.size() > 1 && "java.util.Locale".equals(firstType)
                    && "java.lang.String".equals(params.get(1).asType().toString())) {
                return 1;
            }
            return -1;
        }

        private String constantValueFromElement(Tree tree) {
            Element element = trees.getElement(TreePath.getPath(getCurrentPath(), tree));
            if (!(element instanceof VariableElement variable)) {
                return null;
            }
            Object value = variable.getConstantValue();
            if (value instanceof String constant) {
                return constant;
            }
            return null;
        }

        private String renderErrorWithOriginalPosition(String methodName, NormalizedQuery normalizedQuery,
                CypherSyntaxValidator.SyntaxError error, int originalOffset) {
            if (normalizedQuery.original().isEmpty()) {
                return "Invalid Cypher syntax in OGO." + methodName + " call: " + error.message();
            }

            int safeOriginalOffset = Math.min(Math.max(0, originalOffset), normalizedQuery.original().length() - 1);
            LineColumn originalPos = indexToLineColumn(normalizedQuery.original(), safeOriginalOffset);
            String snippet = buildSnippet(normalizedQuery.original(), safeOriginalOffset);
            return "Invalid Cypher syntax in OGO." + methodName + " call at query line " + originalPos.line()
                    + ", column " + (originalPos.column() + 1) + " (offset " + safeOriginalOffset + "): "
                    + error.message() + " | " + snippet;
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

        private int mapNormalizedOffset(NormalizedQuery normalizedQuery, int normalizedOffset) {
            if (normalizedQuery.normalizedToOriginal().length == 0) {
                return 0;
            }
            if (normalizedOffset >= normalizedQuery.normalizedToOriginal().length) {
                return normalizedQuery.original().length();
            }
            int safeOffset = Math.max(0, normalizedOffset);
            return normalizedQuery.normalizedToOriginal()[safeOffset];
        }

        private ExpressionTree findDiagnosticTree(ExpressionTree expression, int queryOffset) {
            if (expression == null) {
                return null;
            }

            return switch (expression.getKind()) {
            case PARENTHESIZED -> findDiagnosticTree(((ParenthesizedTree) expression).getExpression(), queryOffset);
            case PLUS -> findInPlusExpression((BinaryTree) expression, queryOffset);
            default -> expression;
            };
        }

        private ExpressionTree findInPlusExpression(BinaryTree plusExpression, int queryOffset) {
            ExpressionTree left = plusExpression.getLeftOperand();
            ExpressionTree right = plusExpression.getRightOperand();

            int leftLength = estimateValidationLength(left);
            if (leftLength < 0) {
                return plusExpression;
            }

            if (queryOffset < leftLength) {
                return findDiagnosticTree(left, queryOffset);
            }

            int rightOffset = Math.max(0, queryOffset - leftLength);
            return findDiagnosticTree(right, rightOffset);
        }

        private int estimateValidationLength(ExpressionTree expression) {
            if (expression == null) {
                return -1;
            }

            return switch (expression.getKind()) {
            case PARENTHESIZED -> estimateValidationLength(((ParenthesizedTree) expression).getExpression());
            case PLUS -> {
                BinaryTree plus = (BinaryTree) expression;
                int left = estimateValidationLength(plus.getLeftOperand());
                int right = estimateValidationLength(plus.getRightOperand());
                yield left < 0 || right < 0 ? -1 : left + right;
            }
            default -> {
                String resolved = resolveConstantString(expression);
                if (resolved != null) {
                    yield resolved.length();
                }
                String fallback = resolveConcatenationFallback(expression);
                yield fallback == null ? -1 : fallback.length();
            }
            };
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

        private NormalizedQuery normalizeForSyntaxCheck(String query) {
            Matcher matcher = DOLLAR_ARG.matcher(query);
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

        private String normalizeFormatStringForSyntaxCheck(String format) {
            String withTemporaryValues = FORMAT_SPECIFIER.matcher(format).replaceAll(CONCAT_PLACEHOLDER);
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
}
