package org.ogo.client.processor;

import com.sun.source.tree.CompilationUnitTree;
import com.sun.source.tree.ExpressionTree;
import com.sun.source.tree.MethodInvocationTree;
import com.sun.source.util.TreePath;
import com.sun.source.util.TreePathScanner;
import com.sun.source.util.Trees;
import java.util.List;
import javax.lang.model.element.Element;
import javax.lang.model.element.ExecutableElement;
import javax.lang.model.element.TypeElement;
import javax.lang.model.element.VariableElement;
import javax.tools.Diagnostic;
import org.ogo.cypher.CypherSyntaxValidator;

final class QueryInvocationScanner extends TreePathScanner<Void, Void> {

    private final Trees trees;
    private final QueryStringResolver queryStringResolver;
    private final QueryNormalizer queryNormalizer;
    private final DiagnosticTreeLocator diagnosticTreeLocator;
    private final DiagnosticMessageFormatter diagnosticMessageFormatter;
    private final DiagnosticEmitter diagnosticEmitter;

    QueryInvocationScanner(Trees trees, QueryStringResolver queryStringResolver, QueryNormalizer queryNormalizer,
            DiagnosticTreeLocator diagnosticTreeLocator, DiagnosticMessageFormatter diagnosticMessageFormatter,
            DiagnosticEmitter diagnosticEmitter) {
        this.trees = trees;
        this.queryStringResolver = queryStringResolver;
        this.queryNormalizer = queryNormalizer;
        this.diagnosticTreeLocator = diagnosticTreeLocator;
        this.diagnosticMessageFormatter = diagnosticMessageFormatter;
        this.diagnosticEmitter = diagnosticEmitter;
    }

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
        if (!QueryProcessorConstants.OGO_FQCN.equals(ownerName)
                || !QueryProcessorConstants.QUERY_METHODS.contains(methodName)) {
            return super.visitMethodInvocation(node, unused);
        }

        int queryArgIndex = findQueryStringParamIndex(method);
        if (queryArgIndex < 0 || queryArgIndex >= node.getArguments().size()) {
            return super.visitMethodInvocation(node, unused);
        }

        ExpressionTree queryArg = node.getArguments().get(queryArgIndex);
        String query = queryStringResolver.resolveConstantString(queryArg, path);
        if (query == null) {
            return super.visitMethodInvocation(node, unused);
        }

        QueryNormalizer.NormalizedQuery normalizedQuery = queryNormalizer.normalizeForSyntaxCheck(query);
        if (normalizedQuery.normalized().isBlank()) {
            return super.visitMethodInvocation(node, unused);
        }

        List<CypherSyntaxValidator.SyntaxError> errors;
        try {
            errors = CypherSyntaxValidator.checkWithLocations(normalizedQuery.normalized());
        } catch (IllegalStateException ex) {
            return super.visitMethodInvocation(node, unused);
        }

        if (!errors.isEmpty()) {
            CompilationUnitTree cu = path.getCompilationUnit();
            for (CypherSyntaxValidator.SyntaxError error : errors) {
                int originalOffset = queryNormalizer.mapNormalizedOffset(normalizedQuery, error.offset());
                int anchorOffset = Math.min(Math.max(0, originalOffset), Math.max(0, query.length() - 1));
                ExpressionTree targetTree = diagnosticTreeLocator.findDiagnosticTree(queryArg, anchorOffset, path);
                String message = diagnosticMessageFormatter.renderErrorWithOriginalPosition(methodName, normalizedQuery,
                        error, originalOffset);
                ExpressionTree diagnosticTree = targetTree != null ? targetTree : queryArg;
                if (!diagnosticEmitter.printPreciseDiagnostic(cu, diagnosticTree, anchorOffset, message)) {
                    trees.printMessage(Diagnostic.Kind.ERROR, message, diagnosticTree, cu);
                }
            }
        }

        return super.visitMethodInvocation(node, unused);
    }

    private int findQueryStringParamIndex(ExecutableElement method) {
        List<? extends VariableElement> params = method.getParameters();
        for (int i = 0; i < params.size(); i++) {
            String typeName = params.get(i).asType().toString();
            if (QueryProcessorConstants.STRING_FQCN.equals(typeName)) {
                return i;
            }
        }
        return -1;
    }
}
