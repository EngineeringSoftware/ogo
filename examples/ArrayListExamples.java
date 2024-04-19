import org.ogo.client.OGO;
import java.util.ArrayList;

import static org.ogo.client.OGO.queryBool;

public class ArrayListExamples{

    /**
     * @brief Example that demonstrates using OGO to search for a given element in an ArrayList
     * @returns True
     */
    public boolean containsMethodExample(){
        // Create an ArrayList instance and insert elements using imperative style
        ArrayList<Long> lst = new ArrayList<Long>();
        lst.add(10L);
        lst.add(20L);
        lst.add(30L);

        // Query the ArrayList instance to check if the element 20L exists using declarative style
        return queryBool("MATCH ({$1})-[:elementData]->()-[]->({value:20}) RETURN TRUE", lst);
    }
}
