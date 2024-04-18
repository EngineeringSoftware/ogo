## Coming Soon! ##
1. OGO Code base
2. ICSE 2024 OGO evaluation datasets

## Examples ##

1. Searching an `ArrayList` for a given element.
   ```java
   ArrayList<Long> lst = new ArrayList<Long>();
   lst.add(10L);
   lst.add(20L);
   lst.add(30L);
   query("MATCH ({$1})-[:elementData]->()-[]->({value : 20}) RETURN TRUE");
   ```
   In this example, we use imperative code to instantiate and add elements to
   an `ArrayList` and then use `Cypher` query through `OGO` to query `lst` to
   check if it contains element 20.
     
## Citation ##
If you use OGO in your research, please cite our ICSE'24 paper.
The paper can be found [here](https://users.ece.utexas.edu/~gligoric/papers/ThimmaiahETAL24OGO.pdf).

```bibtex
@inproceedings{ThimmaiahETAL24OGO,
  author    = {Thimmaiah, Aditya and Lampropoulos, Leonidas and Rossbach, Christopher and Gligoric, Milos},
  title     = {Object Graph Programming},
  year      = {2024},
  doi       = {10.1145/3597503.3623319},
  booktitle = {Proceedings of the IEEE/ACM 46th International Conference on Software Engineering},
  pages     = {1--13},
}
```
