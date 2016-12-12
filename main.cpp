#include "mygraph.hpp"
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
  igraph_i_set_attribute_table(  &igraph_cattribute_table );

  string fname( argv[1] );

  graph G( fname );
  G.print_gml();

  cout << "GCC: " << G.compute_GCC() << endl;
  cout << "ALCC: " << G.compute_ALCC() << endl;
  //  vector< int > neis0;
  //  G.get_all_neighbors( 0, neis0 );
  //  G.print_vector( neis0, cout );

  //  cout << G.compute_local_ECC(0,1) << endl;

  cout << "ECC 0: " << G.compute_ECC('0') << endl;
  cout << "ECC 1: " << G.compute_ECC('1') << endl;
  cout << "ECC 2: " << G.compute_ECC('2') << endl;
  //  cout << G.compute_local_ECC(1,2, '0') << endl;
  //  cout << G.compute_local_ECC(1,2, '1') << endl;
  return 0;
}
