package org.ogo.test.instancecount;

public class A {
  B b;
  C c;

  /** class constructor */
  A() {
    b = new B();
    c = new C();
  }
}
