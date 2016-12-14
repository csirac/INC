//wrapper for igraph
#ifndef MYGRAPH
#define MYGRAPH
#include <igraph.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

class graph {
  igraph_t G;

  void read_edge_list_file( string fname ) {
    cout << "Reading graph from edge list '" << fname << "'..." << endl;
    ifstream ifile( fname.c_str() );
    if (!ifile.good()) {
      cout << "Could not open file." << endl;
      igraph_empty( &G, 1, false );  //for now, undirected
      return;
    }

    //vertex ids are from 0 to (n-1)
    bool is_weighted, is_directed;
    int n;
    istringstream iss;
    string sline;

    igraph_attribute_combination_t comb;
    igraph_attribute_combination(&comb, 
				 "Weight", IGRAPH_ATTRIBUTE_COMBINE_FIRST,
				 "", IGRAPH_ATTRIBUTE_COMBINE_IGNORE,
				 IGRAPH_NO_MORE_ATTRIBUTES );

    igraph_integer_t nparent, nchild;
    double weight;
    igraph_vector_t edges_to_add;
    igraph_vector_t v_weight;
    igraph_vector_init( &edges_to_add, 0 );
    igraph_vector_init( &v_weight, 0 );

    unsigned n_lines = 0;
    while( getline( ifile, sline ) ) {
      iss.clear();
      iss.str( sline );

      //skip comments
      if (sline[0] != '#') {
	if (n_lines == 0) {
	  iss >> n;

	  iss >> is_directed;
	  iss >> is_weighted;

	  cout << "n = " << n << endl;

	  if (is_weighted) {
	    cout << "Graph is weighted..." << endl;
	  } else {
	    cout << "Graph is unweighted..." << endl;
	  }

	  //create graph
	  igraph_empty( &G, n, false );  //for now, undirected
	  ++n_lines;
	} else {
	  iss >> nparent;
	  iss >> nchild;

	  if (nparent >= n || nchild >= n) {
	    cout << "Error: n is too low" << endl;
	    cout << "nparent nchild n: " << nparent << ' ' << nchild << ' ' << n << endl;
	    exit(1);
	  }

	  if (is_weighted) {
	    iss >> weight;
	    igraph_vector_push_back( &v_weight, weight );
	  }

	  igraph_vector_push_back( &edges_to_add, nparent );
	  igraph_vector_push_back( &edges_to_add, nchild );
	  ++n_lines;
	}

      }

    }

    cout << "Finished reading file." << endl;

    igraph_add_edges( &G, &edges_to_add, 0 );

    if (is_weighted) {
      SETEANV( &G, "Weight", &v_weight );
    }

    igraph_vector_destroy( &edges_to_add );
    igraph_vector_destroy( &v_weight );
    
    igraph_simplify( &G, true, true, &comb ); //make sure we don't have a multigraph

    cout << "Graph constructed." << endl;
  }

  void ivector2vector( igraph_vector_t& in, vector< int >& out ) {
    out.clear();
    for (int i = 0; i < igraph_vector_size( &in); ++i) {
      out.push_back( static_cast<int>( (VECTOR( in )[ i ]) ) );
    }
  }

  //assumes vectors are sorted
  //discards duplicates
  //returns union size
  unsigned vector_union( vector< int >& v, vector< int >& w, vector< int >& res ) {
    res.clear();

    vector< int >::iterator itv = v.begin();
    vector< int >::iterator itw = w.begin();
    
    while ( itv != v.end() || itw != w.end() ) {
      if (itv == v.end()) {
	//itw != w.end()
	res.push_back( *itw );
	++itw;
      } else {
	if (itw == w.end()) {
	  res.push_back( *itv );
	  ++itv;
	} else {
	  if ( (*itv) < (*itw) ) {
	    res.push_back( *itv );
	    ++itv;
	  } else {
	    if ( (*itv) > (*itw) ) {
	      res.push_back( *itw );
	      ++itw;
	    } else {
	      res.push_back( *itw );
	      ++itw; 
	      ++itv;
	    }
	  }
	}
      }
    }

    return res.size();

  }

  //assumes vectors are sorted
  //discards duplicates
  //returns intersection size
  unsigned vector_intersection( vector< int >& v, vector< int >& w, vector< int >& res ) {
    res.clear();
    vector< int >::iterator itv = v.begin();
    vector< int >::iterator itw = w.begin();
    
    while ( itv != v.end() && itw != w.end() ) {
      if ( (*itv) < (*itw) ) {
	++itv;
      } else {
	if ( (*itv) > (*itw) ) {
	  ++itw;
	} else {
	  //they are equal
	  res.push_back(*itv);
	  ++itv;
	  ++itw;
	}
      }
    }

    return res.size();
  }

  unsigned vector_insert( vector< int >& v, int y ) {
    vector<int>::iterator itv = v.begin();
    while ( itv != v.end() ) {
      if ((*itv) < y)
	++itv;
      else {
	break;
      }

    } 

    if (itv != v.end()) 
      if ((*itv) == y)
	return v.size();
    v.insert ( itv, y );
    return v.size();
  }


public:
  graph( string fname ) {
    if (fname.substr(0, 2) == "ER") {
      int n;
      double p;
      size_t pos_colon = fname.find_first_of( ':' );
      string s_n = fname.substr(2, (pos_colon - 2));
      string s_p = fname.substr( pos_colon + 1 );
      p = stod( s_p );
      n = stoi( s_n );
      igraph_erdos_renyi_game( &G, IGRAPH_ERDOS_RENYI_GNP,
			       n, p,
			       false,    //directed
			       false );  //no self loops 
    } else {
      if (fname.substr(0, 2) == "WS") {
	int n;
	double p;
	size_t pos_colon = fname.find_first_of( ':' );
	string s_n = fname.substr(2, (pos_colon - 2));
	string s_p = fname.substr( pos_colon + 1 );
	p = stod( s_p );
	n = stoi( s_n );
	int nei = 3;
	igraph_watts_strogatz_game( &G, 1,
				    n, 
				    nei,
				    p,
				    false,    //directed
				    false );  //no self loops 
      } else {
	read_edge_list_file( fname );
      }
    }

  }

  void add_numeric_global_attribute( string attr_name, double value ) {
    SETGAN( &G, attr_name.c_str(), value );

  }

  /**
   * write the graph in graphml format to the standard output
   */
  void print_gml() {
    igraph_write_graph_graphml( &G, stdout, 1);
  }

  /**
   * write the graph in graphml format to a file with name filename
   */
  void print_gml(string filename_str) {
     // Need to use C
     const char* filename = filename_str.c_str();
     // open a C stream (igraph needs in C)
     FILE* filestream = fopen(filename, "w");

     igraph_write_graph_graphml( &G, filestream, 1);

     fclose( filestream );
  }

  //does an edge exist from p to q?
  bool is_edge( int p, int q ) {
    int eid;

    igraph_get_eid( &G,
		    &eid,
		    p,
		    q,
		    true, //directed
		    false //assign -1 to eid if no edge
		    );
    
    if (eid == -1)
      return false;

    return true;
  }

  void print_vector( vector< int >& v, ostream& os = std::cout) {
    for (unsigned i = 0; i < v.size(); ++i) {
      os << v[i] << ' ';
    }
    os << endl;
  }

  void get_all_neighbors( int vid, vector< int >& neis ) {
    neis.clear();
    igraph_vector_t ineis;
    igraph_vector_init( &ineis, 0 );
    igraph_neighbors( &G, &ineis, vid, IGRAPH_ALL );
    ivector2vector( ineis, neis );
    igraph_vector_destroy( &ineis );
  }

  double compute_ALCC(bool deg12 = true) {
    double res;

    if (deg12) {
      igraph_transitivity_avglocal_undirected( &G,
					       &res,
					       IGRAPH_TRANSITIVITY_ZERO );
    }
    else {
      igraph_transitivity_avglocal_undirected( &G,
					       &res,
					       IGRAPH_TRANSITIVITY_NAN );
    }
    
    return res;
  }

  double compute_local_ECC( int p, int q, unsigned type = 0 ) {
    vector< int > nei_p;
    vector< int > nei_q;

    vector< int > punq;
    vector< int > pinq;

    // for ECC1
    double res = 0.0;
    unsigned n_sides; 
    bool p_conn_q;
    int n = igraph_vcount(&G);
    switch (type) {
    case 0:
      get_all_neighbors( p, nei_p );
      get_all_neighbors( q, nei_q );

      vector_insert( nei_p, p );
      vector_insert( nei_q, q );

      vector_union( nei_p, nei_q, punq );
      vector_intersection( nei_p, nei_q, pinq );

      return static_cast<double> ( pinq.size() ) / static_cast< double >( punq.size() );
      break;
    case 1:
      //for each w, we compute a clustering score
      //based upon how much of a triangle
      //p,q,w form
      //assumes p,q distinct
      //graph undirected
      if ( is_edge(p,q) )
	p_conn_q = true;
      else
	p_conn_q = false;

      for (int w = 0; w < n; ++w) {
	if (p_conn_q)
	  n_sides = 1;
	else
	  n_sides = 0;

	if (w == p) {
	  continue;
	  //	  if (p_conn_q)
	  //	    n_sides = 3;
	  //	  else
	  //	    n_sides = 1;

	} else {
	  if (w == q) {
	    continue;
	    //	    if (p_conn_q)
	    //	      n_sides = 3;
	    //	    else
	    //	      n_sides = 1;

	  } else {
	    //w \neq p,q
	    if (is_edge(p, w))
	      ++n_sides;

	    if (is_edge(q, w))
	      ++n_sides;

	    
	  }
	}

	//nsides should be correct for this i
	//	cout << w << ": " << n_sides << ' ';
	res += static_cast<double>(n_sides) / 3.0;

      }

      res /= static_cast<double>(n - 2); //average
      //      cout << endl;
      return res;

      break;
    case 2:
      get_all_neighbors( p, nei_p );
      get_all_neighbors( q, nei_q );

      vector_union( nei_p, nei_q, punq );
      vector_intersection( nei_p, nei_q, pinq );

      if (is_edge(p,q)) {
	//punq contains both p and q, so subtract 2. But add 1 -> -1.
	return static_cast<double> ( pinq.size() + 1 ) / static_cast< double >( punq.size() - 1 );
      }
      else {
      
	return static_cast<double> ( pinq.size() ) / static_cast< double >( punq.size() + 1);
      }
	
      break;
    }
  }

  //"Edge clustering coefficient"
  double compute_ECC( unsigned type = 0) {
    double res = 0.0;
    int n = igraph_vcount( &G );
    int m = igraph_ecount( &G );
    switch( type ) {
    case 3:
      if (m == 0)
	return 0.0;

      for (int i = 0; i < n; ++i) {
	for (int j = i + 1; j < n; ++j) {
	  if (is_edge(i,j))
	    res += compute_local_ECC(i,j, 0);
	}
      }

      res = res / static_cast<double>( m );

      break;
    default:
      for (int i = 0; i < n; ++i) {
	for (int j = i + 1; j < n; ++j) {
	  res += compute_local_ECC(i,j,type);
	}
      }

      res = res / ( n * (n - 1) ) * 2;
      break;
    }
    return res;
  }

  double compute_GCC() {
    double res = 0.0;
    igraph_transitivity_undirected( &G,
				    &res,
				    IGRAPH_TRANSITIVITY_ZERO );

    return res;
  }

};

#endif
