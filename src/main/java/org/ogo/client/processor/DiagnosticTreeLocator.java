package org.ogo.client.processor;

import com.sun.source.tree.BinaryTree;
import com.sun.source.tree.ExpressionTree;
import com.sun.source.tree.ParenthesizedTree;
import com.sun.source.util.TreePath;

final class DiagnosticTreeLocator {

    private final QueryStringResolver resolver;

    DiagnosticTreeLocator(QueryStringResolver resolver) {
        this.resolver = resolver;
    }

    ExpressionTree findDiagnosticTree(ExpressionTree expression, int queryOffset, TreePath currentPath) {
        if (expression == null) {
            return null;
        }

        return switch (expression.getKind()) {
        case PARENTHESIZED -> findDiagnosticTree(((ParenthesizedTree) expression).getExpression(), queryOffset,
                currentPath);
        case PLUS -> findInPlusExpression((BinaryTree) expression, queryOffset, currentPath);
        default -> expression;
        };
    }

    private ExpressionTree findInPlusExpression(BinaryTree plusExpression, int queryOffset, TreePath currentPath) {
        ExpressionTree left = plusExpression.getLeftOperand();
        ExpressionTree right = plusExpression.getRightOperand();

        int leftLength = estimateValidationLength(left, currentPath);
        if (leftLength < 0) {
            return plusExpression;
        }

        if (queryOffset < leftLength) {
            return findDiagnosticTree(left, queryOffset, currentPath);
        }

        int rightOffset = Math.max(0, queryOffset - leftLength);
        return findDiagnosticTree(right, rightOffset, currentPath);
    }

    private int estimateValidationLength(ExpressionTree expression, TreePath currentPath) {
        if (expression == null) {
            return -1;
        }

        return switch (expression.getKind()) {
        case PARENTHESIZED -> estimateValidationLength(((ParenthesizedTree) expression).getExpression(), currentPath);
        case PLUS -> {
            BinaryTree plus = (BinaryTree) expression;
            int left = estimateValidationLength(plus.getLeftOperand(), currentPath);
            int right = estimateValidationLength(plus.getRightOperand(), currentPath);
            yield left < 0 || right < 0 ? -1 : left + right;
        }
        default -> {
            String resolved = resolver.resolveConstantString(expression, currentPath);
            if (resolved != null) {
                yield resolved.length();
            }
            String fallback = resolver.resolveConcatenationFallback(expression);
            yield fallback == null ? -1 : fallback.length();
        }
        };
    }
}
