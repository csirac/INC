#include <igraph.h>

int main() {
  
  igraph_t g;
  igraph_vector_t y;  

  /* turn on attribute handling */
  igraph_i_set_attribute_table(&igraph_cattribute_table);

  /* Create a graph, add some attributes and save it as a GraphML file */
  igraph_famous(&g, "Petersen");
  SETGAS(&g, "name", "Petersen's graph");
  SETGAN(&g, "vertices", igraph_vcount(&g));
  SETGAN(&g, "edges", igraph_ecount(&g));

  igraph_vector_init_seq(&y, 1, igraph_vcount(&g));
  SETVANV(&g, "id", &y);
  igraph_vector_destroy(&y);

  SETVAS(&g, "name", 0, "foo");
  SETVAS(&g, "name", 1, "foobar");
  
  igraph_vector_init_seq(&y, 1, igraph_ecount(&g));
  SETEANV(&g, "id", &y);
  igraph_vector_destroy(&y);

  SETEAS(&g, "name", 0, "FOO");
  SETEAS(&g, "name", 1, "FOOBAR");

  igraph_write_graph_gml(&g, stdout, 0, "");
  igraph_write_graph_graphml(&g, stdout, /*prefixattr=*/ 1);
   
  igraph_destroy(&g);
  
  return 0;

}
