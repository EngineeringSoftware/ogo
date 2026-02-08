package org.ogo.client;

public final class GraphTriggerException extends Exception {
  private final Boolean inMemory;
  private final Boolean followRoot;
  private final Boolean forceGC;
  private final Boolean whitelist;
  private final String cQuery;
  private Object[] queryResult;
  private final Object root;
  private final Object[] whiteLists;
  private final Object[] blackLists;
  private final Object[] excludeBlackListing;

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
      Object[] whiteLists,
      Object[] blackLists,
      Object[] excludeBlackListing) {
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
   * @param inMemory
   * @param cQuery
   */
  public GraphTriggerException(
      boolean inMemory,
      boolean forceGC,
      boolean followRoot,
      boolean whitelist,
      String cQuery,
      Object[] whiteLists,
      Object[] blackLists,
      Object[] excludeBlackListing) {
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
