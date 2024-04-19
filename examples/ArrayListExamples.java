public boolean containsMethodExample(){
    ArrayList<Long> lst = new ArrayList<Long>();
    lst.add(10L);
    lst.add(20L);
    lst.add(30L);

    return queryBool("MATCH ({$1})-[:elementData]->()-[]->({value:20}) RETURN TRUE");
}
