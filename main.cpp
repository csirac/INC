#include "mygraph.hpp"
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
  igraph_i_set_attribute_table(  &igraph_cattribute_table );

  string fname( argv[1] );

  graph G( fname );

  cout << "n: " << G.n() << " m: " << G.m() << endl;
  //  int m_clique = static_cast<int>( G.n() * ( G.n() - 1 ) / 2.0 ); // n choose 2
  //  cout << "m / m_clique: " << static_cast<double>( G.m() ) / m_clique << endl;


  double cc;
  cc = G.compute_GCC();
  G.add_numeric_global_attribute( "GCC", cc );
  cout << "GCC: " << cc << endl;
  
  cc = G.compute_ALCC();
  G.add_numeric_global_attribute( "ALCC", cc );
  cout << "ALCC: " << cc << endl;

  // string sECC = "ECC";
  // for (unsigned type = 0; type <= 5; ++type) {
  //   cc = G.compute_ECC( type );
  //   string name = to_string( type );
  //   name = sECC + name;
  //   G.add_numeric_global_attribute( name, cc );
  //   cout << name << ": " << cc << endl;
  // }

  if (argc > 2) {
     G.print_gml(argv[2]);
  }

  //  cout << "HCC(0,1): " << G.compute_local_HCC(0,1,2) << endl;
  cout << "HCC(1): " << G.compute_HCC(1, false) << endl;
  cout << "HCC(1W): " << G.compute_HCC(1, true) << endl;
  cout << "HCC(2): " << G.compute_HCC(2, false) << endl;
  cout << "HCC(2W): " << G.compute_HCC(2, true) << endl;
  cout << "HCC(3): " << G.compute_HCC(3, false) << endl;
  cout << "HCC(3W): " << G.compute_HCC(3, true) << endl;

  return 0;
}
