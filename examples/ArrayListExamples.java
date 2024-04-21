import org.ogo.client.OGO;
import java.util.ArrayList;

import static org.ogo.client.OGO.queryBool;
import static org.ogo.client.OGO.queryInt;
 
/**
 * Example class to demonstrate the usage of OGO
 * for querying ArrayLists.
 */
public class ArrayListExamples{

    /**
     * @brief Example that demonstrates using OGO to query an
     * ArrayList to search for a given element.
     * @returns true/false
     */
    public boolean containsMethodExample() throws Exception{
        // Create an ArrayList instance and insert elements into it
        // using imperative style.
        ArrayList<Long> lst = new ArrayList<Long>();
        lst.add(10L);
        lst.add(20L);
        lst.add(30L);

        // Query the ArrayList instance to check if it contains the
        // element 20L and return True if it does and false if it doesn't.
        return queryBool("MATCH ({$1})-[:elementData]->()-[]->({value:20}) RETURN TRUE", lst);
    }
    
    /**
     * @brief Example that demonstrates using OGO to query an
     * ArrayList to find its size.
     * @returns size of the ArrayList
     */
    public int arrayListSizeExample() throws Exception{
        // Create an ArrayList instance and insert elements into it
        // using imperative style.
        ArrayList<Long> lst = new ArrayList<Long>();
        lst.add(10L);
        lst.add(20L);
        lst.add(30L);

        // Query the ArrayList instance to find its size.
        return queryInt("MATCH ({$1})-[:elementData]->()-[]->(n) RETURN COUNT(n)", lst);
    }
    
    /**
     * @brief Example that demonstrates using OGO to query an
     * ArrayList to retrieve an element at a given index.
     * @returns element at index 1
     */
    public long arrayGetElementAtExample() throws Exception{
        // Create an ArrayList instance and insert elements into it
        // using imperative style.
        ArrayList<Long> lst = new ArrayList<Long>();
        lst.add(10L);
        lst.add(20L);
        lst.add(30L);

        // Query the ArrayList instance to retrieve the element at
        // index 1.
        return queryLong("MATCH ({$1})-[:elementData]->()-[:`1`]->(n) RETURN n", lst);
    }
}
