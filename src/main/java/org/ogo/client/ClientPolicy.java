package org.ogo.client;

import java.security.AllPermission;
import java.security.CodeSource;
import java.security.PermissionCollection;
import java.security.Permissions;
import java.security.Policy;

public class ClientPolicy extends Policy {
  private int port;

  ClientPolicy() {
    port = 1099;
  }

  ClientPolicy(int port) {
    this.port = port;
  }

  @Override
  public PermissionCollection getPermissions(CodeSource codesource) {
    Permissions perms = new Permissions();
    perms.add(new AllPermission());
    //        perms.add(new SocketPermission("localhost:" + this.port + "-", "connect,resolve"));
    return (perms);
  }

  @Override
  public void refresh() {}
}
