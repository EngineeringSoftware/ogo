package org.ogo.test.classStructureTests.singleClassMultipleConnectedObjects;

/**
 * @author 1sand0s
 */
public class A {
  B b;
  C c;
  int var;
  String var2;
  int[] var3;

  /** class constructor */
  A() {
    b = new B();
    c = new C();
    var = 10;
    var2 = "BACON";
    var3 = new int[] {1, 1, 2, 3, 5, 8};
  }
}
