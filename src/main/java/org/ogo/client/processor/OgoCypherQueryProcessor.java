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
import java.util.List;
import java.util.Set;
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

    private static final Set<String> QUERY_METHODS = Set.of("query", "queryInt", "queryLong", "queryBool");
    private static final String OGO_FQCN = "org.ogo.client.OGO";
    private static final Pattern DOLLAR_ARG = Pattern.compile("\\$\\d+");

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
            String normalizedQuery = normalizeForSyntaxCheck(query);
            if (normalizedQuery.isBlank()) {
                return super.visitMethodInvocation(node, unused);
            }
            List<String> errors = CypherSyntaxValidator.check(normalizedQuery);
            if (!errors.isEmpty()) {
                String message = "Invalid Cypher syntax in OGO." + methodName + " call: " + String.join(" | ", errors);
                CompilationUnitTree cu = path.getCompilationUnit();
                trees.printMessage(Diagnostic.Kind.ERROR, message, queryArg, cu);
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
            default -> null;
            };
        }

        private String resolvePlusExpression(BinaryTree expr) {
            String left = resolveConstantString(expr.getLeftOperand());
            String right = resolveConstantString(expr.getRightOperand());
            if (left == null || right == null) {
                return null;
            }
            return left + right;
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

        private String normalizeForSyntaxCheck(String query) {
            return DOLLAR_ARG.matcher(query).replaceAll("hash:0");
        }
    }
}
