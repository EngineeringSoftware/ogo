package org.ogo.client.processor;

import com.sun.source.tree.BinaryTree;
import com.sun.source.tree.ExpressionTree;
import com.sun.source.tree.IdentifierTree;
import com.sun.source.tree.LiteralTree;
import com.sun.source.tree.MemberSelectTree;
import com.sun.source.tree.MethodInvocationTree;
import com.sun.source.tree.ParenthesizedTree;
import com.sun.source.tree.Tree;
import com.sun.source.util.TreePath;
import com.sun.source.util.Trees;
import javax.lang.model.element.Element;
import javax.lang.model.element.ExecutableElement;
import javax.lang.model.element.TypeElement;
import javax.lang.model.element.VariableElement;

final class QueryStringResolver {

    private final Trees trees;
    private final QueryNormalizer normalizer;

    QueryStringResolver(Trees trees, QueryNormalizer normalizer) {
        this.trees = trees;
        this.normalizer = normalizer;
    }

    String resolveConstantString(ExpressionTree expr, TreePath currentPath) {
        if (expr == null) {
            return null;
        }
        return switch (expr.getKind()) {
        case STRING_LITERAL -> (String) ((LiteralTree) expr).getValue();
        case IDENTIFIER -> constantValueFromElement((IdentifierTree) expr, currentPath);
        case MEMBER_SELECT -> constantValueFromElement((MemberSelectTree) expr, currentPath);
        case PARENTHESIZED -> resolveConstantString(((ParenthesizedTree) expr).getExpression(), currentPath);
        case PLUS -> resolvePlusExpression((BinaryTree) expr, currentPath);
        case METHOD_INVOCATION -> resolveFormatInvocation((MethodInvocationTree) expr, currentPath);
        default -> null;
        };
    }

    String resolveConcatenationFallback(ExpressionTree expr) {
        if (expr == null) {
            return null;
        }

        return switch (expr.getKind()) {
        case NULL_LITERAL -> "null";
        default -> QueryProcessorConstants.CONCAT_PLACEHOLDER;
        };
    }

    private String resolveFormatInvocation(MethodInvocationTree invocation, TreePath currentPath) {
        TreePath invocationPath = TreePath.getPath(currentPath, invocation);
        if (invocationPath == null) {
            return null;
        }

        Element element = trees.getElement(invocationPath);
        if (!(element instanceof ExecutableElement method)) {
            return null;
        }

        Element enclosing = method.getEnclosingElement();
        if (!(enclosing instanceof TypeElement owner)) {
            return null;
        }

        if (!QueryProcessorConstants.STRING_FQCN.equals(owner.getQualifiedName().toString())
                || !QueryProcessorConstants.FORMAT_METHOD.equals(method.getSimpleName().toString())) {
            return null;
        }

        int formatArgIndex = findFormatStringParamIndex(method);
        if (formatArgIndex < 0 || formatArgIndex >= invocation.getArguments().size()) {
            return null;
        }

        String format = resolveConstantString(invocation.getArguments().get(formatArgIndex), invocationPath);
        if (format == null) {
            return null;
        }

        return normalizer.normalizeFormatStringForSyntaxCheck(format);
    }

    private String resolvePlusExpression(BinaryTree expr, TreePath currentPath) {
        String left = resolveConstantString(expr.getLeftOperand(), currentPath);
        String right = resolveConstantString(expr.getRightOperand(), currentPath);

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

    private int findFormatStringParamIndex(ExecutableElement method) {
        var params = method.getParameters();
        if (params.isEmpty()) {
            return -1;
        }

        String firstType = params.get(0).asType().toString();
        if (QueryProcessorConstants.STRING_FQCN.equals(firstType)) {
            return 0;
        }
        if (params.size() > 1 && "java.util.Locale".equals(firstType)
                && QueryProcessorConstants.STRING_FQCN.equals(params.get(1).asType().toString())) {
            return 1;
        }
        return -1;
    }

    private String constantValueFromElement(Tree tree, TreePath currentPath) {
        TreePath treePath = TreePath.getPath(currentPath, tree);
        if (treePath == null) {
            return null;
        }
        Element element = trees.getElement(treePath);
        if (!(element instanceof VariableElement variable)) {
            return null;
        }
        Object value = variable.getConstantValue();
        if (value instanceof String constant) {
            return constant;
        }
        return null;
    }
}
