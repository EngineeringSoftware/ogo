package org.ogo.test.createdatastructure.createbinarytree;

public class Node {
    int value;
    Node left;
    Node right;

    Node() {
        value = 0;
        left = null;
        right = null;
    }

    Node(int val) {
        value = val;
        left = null;
        right = null;
    }

    Node(Node leftNode, Node rightNode, int val) {
        value = val;
        left = leftNode;
        right = rightNode;
    }
}
