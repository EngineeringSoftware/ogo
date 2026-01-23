package org.ogo.client;

/**
 * @author 1sand0s
 */
public final class GraphTriggerException extends Exception {
  private Boolean inMemory;
  private Boolean followRoot;
  private Boolean forceGC;
  private Boolean whitelist;
  private String cQuery;
  private Object queryResult[];
  private Object root;
  private Object whiteLists[];
  private Object blackLists[];
  private Object excludeBlackListing[];

  /**
   * 0
   *
   * @param inMemory
   * @param cQuery
   * @param root
   */
  public GraphTriggerException(
      boolean inMemory,
      boolean forceGC,
      boolean followRoot,
      boolean whitelist,
      String cQuery,
      Object root,
      Object whiteLists[],
      Object blackLists[],
      Object excludeBlackListing[]) {
    this.inMemory = inMemory;
    this.forceGC = forceGC;
    this.followRoot = followRoot;
    this.whitelist = whitelist;
    this.cQuery = cQuery;
    this.root = root;
    this.whiteLists = whiteLists;
    this.blackLists = blackLists;
    this.excludeBlackListing = excludeBlackListing;
  }

  /**
   * 0
   *
   * @param inMemory
   * @param cQuery
   */
  public GraphTriggerException(
      boolean inMemory,
      boolean forceGC,
      boolean followRoot,
      boolean whitelist,
      String cQuery,
      Object whiteLists[],
      Object blackLists[],
      Object excludeBlackListing[]) {
    this.inMemory = inMemory;
    this.forceGC = forceGC;
    this.followRoot = followRoot;
    this.whitelist = whitelist;
    this.cQuery = cQuery;
    this.root = null;
    this.whiteLists = whiteLists;
    this.blackLists = blackLists;
    this.excludeBlackListing = excludeBlackListing;
  }

  /**
   * 0
   *
   * @return
   * @throws UnsupportedOperationException
   */
  public Object[] getQueryResults() throws UnsupportedOperationException {
    if (!inMemory)
      throw new UnsupportedOperationException(
          "Error : Can only be called when 'inMemory' flag is set");
    return queryResult;
  }
}
