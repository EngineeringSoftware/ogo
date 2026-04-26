package org.ogo.client.processor;

import com.sun.source.util.TreePath;
import com.sun.source.util.Trees;
import java.util.Set;
import javax.annotation.processing.AbstractProcessor;
import javax.annotation.processing.ProcessingEnvironment;
import javax.annotation.processing.RoundEnvironment;
import javax.annotation.processing.SupportedAnnotationTypes;
import javax.annotation.processing.SupportedSourceVersion;
import javax.lang.model.SourceVersion;
import javax.lang.model.element.Element;
import javax.lang.model.element.TypeElement;

@SupportedAnnotationTypes("*")
@SupportedSourceVersion(SourceVersion.RELEASE_21)
public final class OgoCypherQueryProcessor extends AbstractProcessor {

    private Trees trees;
    private QueryInvocationScanner scanner;

    @Override
    public synchronized void init(ProcessingEnvironment processingEnv) {
        super.init(processingEnv);
        this.trees = Trees.instance(processingEnv);

        QueryNormalizer queryNormalizer = new QueryNormalizer();
        QueryStringResolver queryStringResolver = new QueryStringResolver(trees, queryNormalizer);
        DiagnosticTreeLocator diagnosticTreeLocator = new DiagnosticTreeLocator(queryStringResolver);
        DiagnosticMessageFormatter diagnosticMessageFormatter = new DiagnosticMessageFormatter();
        DiagnosticEmitter diagnosticEmitter = new DiagnosticEmitter(processingEnv, trees);
        this.scanner = new QueryInvocationScanner(trees, queryStringResolver, queryNormalizer, diagnosticTreeLocator,
                diagnosticMessageFormatter, diagnosticEmitter);
    }

    @Override
    public boolean process(Set<? extends TypeElement> annotations, RoundEnvironment roundEnv) {
        for (Element root : roundEnv.getRootElements()) {
            TreePath rootPath = trees.getPath(root);
            if (rootPath == null) {
                continue;
            }
            scanner.scan(rootPath, null);
        }
        return false;
    }
}
