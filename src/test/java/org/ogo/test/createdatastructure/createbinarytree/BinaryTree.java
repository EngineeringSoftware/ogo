package org.ogo.test.createdatastructure.createbinarytree;

/** */
public class BinaryTree {
  Node root;

  BinaryTree() {
    root = null;
  }

  public Node createTree(int[] arr, int l, int h) {
    Node node = null;
    if (l > h || h < 0 || l == arr.length) return null;

    if (l == h) {
      node = new Node();
      node.value = arr[l];
      node.left = null;
      node.right = null;
    } else {
      node = new Node();
      int mid = (int) Math.floor((l + h) / 2.0);

      node.value = arr[mid];
      node.left = createTree(arr, l, mid - 1);
      node.right = createTree(arr, mid + 1, h);
    }
    return node;
  }
}
