package org.ogo.server;

import java.security.AllPermission;
import java.security.CodeSource;
import java.security.PermissionCollection;
import java.security.Permissions;
import java.security.Policy;

public class ServerPolicy extends Policy {
  private int port;

  ServerPolicy() {
    port = 1099;
  }

  ServerPolicy(int port) {
    this.port = port;
  }

  @Override
  public PermissionCollection getPermissions(CodeSource codesource) {
    Permissions perms = new Permissions();
    perms.add(new AllPermission()); /*
        perms.add(new SocketPermission("localhost:" + this.port + "-", "listen,accept,resolve"));
        perms.add(new RuntimePermission("setIO,shutdownHooks"));*/
    return (perms);
  }

  @Override
  public void refresh() {}
}
