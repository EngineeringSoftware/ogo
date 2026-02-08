package org.ogo.test.classstructuretests.inheritancetest;

public class A extends B implements D {
  int a;
  int b;
  E f;

  /** class constructor */
  A() {
    a = 10;
    b = 20;
    f = new F();
  }
}
