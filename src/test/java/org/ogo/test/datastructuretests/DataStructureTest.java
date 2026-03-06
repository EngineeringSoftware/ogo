package org.ogo.test.datastructuretests;

import java.io.IOException;
import java.rmi.NotBoundException;
import org.junit.jupiter.api.BeforeAll;
import org.ogo.client.OGO;

public abstract class DataStructureTest {

    /**
     * @since 0.1.0
     */
    @BeforeAll
    public static void initQueryEngine() throws InterruptedException, IOException, NotBoundException {
        OGO.init();
        OGO.setWhiteList("ogo/test", "java/util");
        OGO.clearDatabase = true;
        OGO.forceGC = true;
        OGO.followRoot = true;
        OGO.whitelist = true;
    }
}
