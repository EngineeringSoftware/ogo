package org.ogo.test.classstructuretests.singleclassmultipleconnectedobjects;

public class A {
    B b;
    C c;
    int var;
    String var2;
    int[] var3;

    A() {
        b = new B();
        c = new C();
        var = 10;
        var2 = "BACON";
        var3 = new int[] { 1, 1, 2, 3, 5, 8 };
    }
}
