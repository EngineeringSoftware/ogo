package org.ogo.test.createclasses;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.ogo.client.OGO.query;

import java.io.IOException;
import java.lang.reflect.*;
import java.rmi.NotBoundException;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Disabled;
import org.junit.jupiter.api.Test;
import org.ogo.client.OGO;

@Disabled
public class CreateClassWithOnlyPrimitiveFieldsTest {

    /**
     * @since 0.1.0
     */
    @BeforeAll
    public static void initQueryEngine() throws InterruptedException, IOException, NotBoundException {
        Thread.sleep(4000);
        OGO.inMemory = true;
        OGO.init();
        OGO.setWhiteList("ogo/test", "DummyClass");
    }

    @Test
    public void testCreateClassLongFields() throws Exception {
        OGO.inMemory = true;
        Object[] results = query("CREATE (a:`DummyClass` {property1 : 1, property2 : 2, property3 : 3}) RETURN a");
        assertEquals(3, results[0].getClass().getDeclaredFields().length, "Mismatch in expected number of fields");

        Field field1 = results[0].getClass().getDeclaredField("property1");
        Field field2 = results[0].getClass().getDeclaredField("property2");
        Field field3 = results[0].getClass().getDeclaredField("property3");

        assertEquals("long", field1.getAnnotatedType().getType().getTypeName(),
                "Mismatch in expected type of field 'property1'");
        assertEquals("long", field2.getAnnotatedType().getType().getTypeName(),
                "Mismatch in expected type of field 'property2'");
        assertEquals("long", field3.getAnnotatedType().getType().getTypeName(),
                "Mismatch in expected type of field 'property3'");

        assertEquals(1L, field1.get(results[0]), "Mismatch in expected value of field 'property1'");
        assertEquals(2L, field2.get(results[0]), "Mismatch in expected value of field 'property2'");
        assertEquals(3L, field3.get(results[0]), "Mismatch in expected value of field 'property3'");
    }

    @Test
    public void testCreateClassDoubleFields() throws Exception {
        OGO.inMemory = true;
        Object[] results = query(
                "CREATE (a:`DummyClass` {property1 : 1.0, property2 : 2.0, property3 : 3.0}) RETURN a");
        assertEquals(3, results[0].getClass().getDeclaredFields().length, "Mismatch in expected number of fields");

        Field field1 = results[0].getClass().getDeclaredField("property1");
        Field field2 = results[0].getClass().getDeclaredField("property2");
        Field field3 = results[0].getClass().getDeclaredField("property3");

        assertEquals("double", field1.getAnnotatedType().getType().getTypeName(),
                "Mismatch in expected type of field 'property1'");
        assertEquals("double", field2.getAnnotatedType().getType().getTypeName(),
                "Mismatch in expected type of field 'property2'");
        assertEquals("double", field3.getAnnotatedType().getType().getTypeName(),
                "Mismatch in expected type of field 'property3'");

        assertEquals(1.0, field1.get(results[0]), "Mismatch in expected value of field 'property1'");
        assertEquals(2.0, field2.get(results[0]), "Mismatch in expected value of field 'property2'");
        assertEquals(3.0, field3.get(results[0]), "Mismatch in expected value of field 'property3'");
    }

    @Test
    public void testCreateClassBooleanFields() throws Exception {
        OGO.inMemory = true;
        Object[] results = query(
                "CREATE (a:`DummyClass` {property1 : TRUE, property2 : TRUE, property3 : FALSE}) RETURN" + " a");
        assertEquals(3, results[0].getClass().getDeclaredFields().length, "Mismatch in expected number of fields");

        Field field1 = results[0].getClass().getDeclaredField("property1");
        Field field2 = results[0].getClass().getDeclaredField("property2");
        Field field3 = results[0].getClass().getDeclaredField("property3");

        assertEquals("boolean", field1.getAnnotatedType().getType().getTypeName(),
                "Mismatch in expected type of field 'property1'");
        assertEquals("boolean", field2.getAnnotatedType().getType().getTypeName(),
                "Mismatch in expected type of field 'property2'");
        assertEquals("boolean", field3.getAnnotatedType().getType().getTypeName(),
                "Mismatch in expected type of field 'property3'");

        assertEquals(true, field1.get(results[0]), "Mismatch in expected value of field 'property1'");
        assertEquals(true, field2.get(results[0]), "Mismatch in expected value of field 'property2'");
        assertEquals(false, field3.get(results[0]), "Mismatch in expected value of field 'property3'");
    }

    @Test
    public void testCreateClassStringFields() throws Exception {
        OGO.inMemory = true;
        Object[] results = query("CREATE (a:`DummyClass` {property1 : 'Apple', property2 : 'Banana', property3 :"
                + " 'Orange'}) RETURN a");
        assertEquals(3, results[0].getClass().getDeclaredFields().length, "Mismatch in expected number of fields");

        Field field1 = results[0].getClass().getDeclaredField("property1");
        Field field2 = results[0].getClass().getDeclaredField("property2");
        Field field3 = results[0].getClass().getDeclaredField("property3");

        assertEquals("java.lang.String", field1.getAnnotatedType().getType().getTypeName(),
                "Mismatch in expected type of field 'property1'");
        assertEquals("java.lang.String", field2.getAnnotatedType().getType().getTypeName(),
                "Mismatch in expected type of field 'property2'");
        assertEquals("java.lang.String", field3.getAnnotatedType().getType().getTypeName(),
                "Mismatch in expected type of field 'property3'");

        assertEquals("'Apple'", field1.get(results[0]), "Mismatch in expected value of field 'property1'");
        assertEquals("'Banana'", field2.get(results[0]), "Mismatch in expected value of field 'property2'");
        assertEquals("'Orange'", field3.get(results[0]), "Mismatch in expected value of field 'property3'");
    }

    @Test
    public void testCreateClassMixedPrimitiveFields1() throws Exception {
        OGO.inMemory = true;
        Object[] results = query(
                "CREATE (a:`DummyClass` {property1 : 100, property2 : 'Banana', property3 :" + " TRUE}) RETURN a");
        assertEquals(3, results[0].getClass().getDeclaredFields().length, "Mismatch in expected number of fields");

        Field field1 = results[0].getClass().getDeclaredField("property1");
        Field field2 = results[0].getClass().getDeclaredField("property2");
        Field field3 = results[0].getClass().getDeclaredField("property3");

        assertEquals("long", field1.getAnnotatedType().getType().getTypeName(),
                "Mismatch in expected type of field 'property1'");
        assertEquals("java.lang.String", field2.getAnnotatedType().getType().getTypeName(),
                "Mismatch in expected type of field 'property2'");
        assertEquals("boolean", field3.getAnnotatedType().getType().getTypeName(),
                "Mismatch in expected type of field 'property3'");

        assertEquals(100L, field1.get(results[0]), "Mismatch in expected value of field 'property1'");
        assertEquals("'Banana'", field2.get(results[0]), "Mismatch in expected value of field 'property2'");
        assertEquals(true, field3.get(results[0]), "Mismatch in expected value of field 'property3'");
    }

    @Test
    public void testCreateClassMixedPrimitiveFields2() throws Exception {
        OGO.inMemory = true;
        Object[] results = query(
                "CREATE (a:`DummyClass` {property1 : 'Hello', property2 : 'Banana', property3 :" + " 345.6}) RETURN a");
        assertEquals(3, results[0].getClass().getDeclaredFields().length, "Mismatch in expected number of fields");

        Field field1 = results[0].getClass().getDeclaredField("property1");
        Field field2 = results[0].getClass().getDeclaredField("property2");
        Field field3 = results[0].getClass().getDeclaredField("property3");

        assertEquals("java.lang.String", field1.getAnnotatedType().getType().getTypeName(),
                "Mismatch in expected type of field 'property1'");
        assertEquals("java.lang.String", field2.getAnnotatedType().getType().getTypeName(),
                "Mismatch in expected type of field 'property2'");
        assertEquals("double", field3.getAnnotatedType().getType().getTypeName(),
                "Mismatch in expected type of field 'property3'");

        assertEquals("'Hello'", field1.get(results[0]), "Mismatch in expected value of field 'property1'");
        assertEquals("'Banana'", field2.get(results[0]), "Mismatch in expected value of field 'property2'");
        assertEquals(345.6, field3.get(results[0]), "Mismatch in expected value of field 'property3'");
    }
}
