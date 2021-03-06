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

struct component_map_type {
   int vid_global;
   int which_component;
   int vid_component;
};

class graph {
   igraph_t G;

  igraph_matrix_t dmat;
  bool d_comp; //whether dmat has been computed
  
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
   //assignment operator
   //might need to edit this. G should probably be destroyed
   //first (which could lead to issues if it hadn't been allocated)
   graph& operator=( const graph& H ) {
      //need to make a "deep" copy
      igraph_copy( &G, &(H.G) );
      return *this;
   }

   //constructors
   //copy constructor
   graph( const graph& H ) {
     d_comp = false;
     igraph_matrix_init( &dmat, 0, 0 );
     //need to make a "deep" copy
     igraph_copy( &G, &(H.G) );
   }

   graph( unsigned n ) {
     d_comp = false;
     igraph_matrix_init( &dmat, 0, 0 );
     igraph_empty( &G, n, false ); //for now, undirected
   }

   graph( string fname ) {
     d_comp = false;
     igraph_matrix_init( &dmat, 0, 0 );
      if (fname.substr(0, 2) == "ER") {
	 int n;
	 double p;
	 size_t pos_colon = fname.find_first_of( ':' );
	 string s_n = fname.substr(2, (pos_colon - 2));
	 string s_p = fname.substr( pos_colon + 1 );
	 p = stod( s_p );
	 n = stoi( s_n );
	 if ( p <= 1 ) {
	    igraph_erdos_renyi_game( &G, IGRAPH_ERDOS_RENYI_GNP,
				     n, p,
				     false,    //directed
				     false );  //no self loops 
	 } else {
	    igraph_erdos_renyi_game( &G, IGRAPH_ERDOS_RENYI_GNM,
				     n, p,
				     false,    //directed
				     false );  //no self loops 
	 }
         add_string_global_attribute("type", "ER");
      } else {
	 if (fname.substr(0, 2) == "WS") {
	    int n;
	    double p;
	    size_t pos_colon = fname.find_first_of( ':' );
	    string s_n = fname.substr(2, (pos_colon - 2));
	    string s_p = fname.substr( pos_colon + 1 );
	    p = stod( s_p );
	    n = stoi( s_n );
	    int nei = 6;
	    igraph_watts_strogatz_game( &G, 1,
					n, 
					nei,
					p,
					false,    //directed
					false );  //no self loops 
            add_string_global_attribute("type", "WS");
	 } else {
	    if (fname.substr(0,2) == "BA") {
	       int n;
	       int m;
	       size_t pos_colon = fname.find_first_of( ':' );
	       string s_n = fname.substr(2, (pos_colon - 2));
	       string s_m = fname.substr( pos_colon + 1 );
	       m = stoi( s_m );
	       n = stoi( s_n );
	       igraph_barabasi_game( &G, 
				     n, 
				     1,
				     m,
				     NULL,
				     true,
				     1,
				     false, //directed
				     IGRAPH_BARABASI_PSUMTREE,
				     NULL);  
               add_string_global_attribute("type", "BA");
	    } else {
	       read_edge_list_file( fname );
	    }
	 }
      }

   }

   //destructor
   //make sure we free the memory allocated for G
   ~graph() {
      igraph_destroy( &G );
      igraph_matrix_destroy( &dmat );
   }

   void add_numeric_global_attribute( string attr_name, double value ) {
      SETGAN( &G, attr_name.c_str(), value );

   }

   /**
    * Add a graph attribute that is a string (for example, graph type)
    */
   void add_string_global_attribute( string attr_name, string value ) {
      SETGAS( &G, attr_name.c_str(), value.c_str() );

   }

   int n() {
      return igraph_vcount(&G);
   }

   int m() {
      return igraph_ecount(&G);
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

   double dist( int p, int q ) { //unweighted for now
     if (!d_comp) {
       igraph_shortest_paths_dijkstra( &G,
				       &dmat,
				       igraph_vss_all(),
				       igraph_vss_all(),
				       NULL,
				       IGRAPH_OUT );
       d_comp = true;
     }

     return MATRIX( dmat, p, q );
   }
  
   //add edge from p to q
   void add_edge( int p, int q) {
      igraph_add_edge( &G, p, q );
   }

   void remove_edge( int p, int q) {
      int eid;
      igraph_get_eid( &G,
		      &eid,
		      p,
		      q,
		      true, //directed
		      false //assign -1 to eid if no edge
		      );

      if (eid == -1) //this edge doesn't exist
	 return;

      igraph_delete_edges( &G, igraph_ess_1( eid ) );
   }

   void remove_vertex( int p ) {
     igraph_delete_vertices( &G, igraph_vss_1( p ) );
   }

  //preserves node ids
  void remove_vertex_star( int p, vector< int >& nei ) {
     //int igraph_incident(const igraph_t *graph, igraph_vector_t *eids,
     //     igraph_integer_t pnode, igraph_neimode_t mode);
    get_all_neighbors( p, nei );
    
     igraph_vector_t eids;
     igraph_vector_init( &eids, 0 );
     igraph_incident( &G, &eids, p, IGRAPH_ALL );
		      
     igraph_delete_edges( &G, igraph_ess_vector( &eids ) );
     igraph_vector_destroy( &eids );
   }

  void add_vertex_star( int p, vector< int >& nei ) {
    igraph_vector_t edges;
    igraph_vector_init( &edges, 0 );
    for (unsigned i = 0; i < nei.size(); ++i) {
      igraph_vector_push_back( &edges, p );
      igraph_vector_push_back( &edges, nei[i] );
    }

    igraph_add_edges( &G, &edges, 0 );
    igraph_vector_destroy ( &edges );
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

   void get_neighborhood( int vid, vector< int >& neis, int order ) {
      //int igraph_neighborhood(const igraph_t *graph, igraph_vector_ptr_t *res,igraph_vs_t vids, igraph_integer_t order,igraph_neimode_t mode);
      neis.clear();
      igraph_vector_ptr_t res;
      igraph_vector_ptr_init( &res, 0 );
      igraph_neighborhood( &G, &res, igraph_vss_1( vid ), order, 
			   IGRAPH_OUT );
      igraph_vector_t* v;
      v = (igraph_vector_t*) igraph_vector_ptr_e( &res, 0 );
      igraph_vector_sort( v );
      for (int j = 0; j < igraph_vector_size( v ); ++j) {
	 int nei = igraph_vector_e( v, j );
	 if (nei != vid) {
	    neis.push_back( nei );
	 }
      }

      igraph_vector_ptr_destroy_all( &res );
      
   }

   void get_all_neighbors( int vid, vector< int >& neis ) {
      neis.clear();
      igraph_vector_t ineis;
      igraph_vector_init( &ineis, 0 );
      igraph_neighbors( &G, &ineis, vid, IGRAPH_ALL );
      ivector2vector( ineis, neis );
      igraph_vector_destroy( &ineis );
   }

   int get_degree( int vid ) {
      igraph_vector_t res;
      igraph_vector_init( &res, 0 );
      igraph_degree( &G, &res, igraph_vss_1( vid ),
		     IGRAPH_ALL,
		     false );
      int ires = VECTOR( res )[ 0 ];
      igraph_vector_destroy( &res );
      return ires;
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

   //HCC -- "heterogeneous clustering coefficient"
   double compute_local_HCC( int p, int q, int l, bool length = false, bool rad_dom = false ) {
     vector< int > nei_p;
     vector< int > neigh_p;
     vector< int > nei_q;
     vector< int > neigh_q;
      
      vector< int > punq;
      vector< int > pinq;
      vector< double > pdist;
      vector< double > qdist;
      
      double dp;
      double dq;
      double numerator = 0.0;

      int n = this->n();

      if (is_edge(p,q)) {
	//	get_all_neighbors( p, nei_p );
	if (l != -1) {
	   remove_vertex_star( q, neigh_q );
	   get_neighborhood( p, nei_p, l );
	   add_vertex_star( q, neigh_q );

	   remove_vertex_star( p, neigh_p );
	   //get_all_neighbors( q, nei_q );
	   get_neighborhood( q, nei_q, l );
	   add_vertex_star( p, neigh_p );

		
	   vector_union( nei_p, nei_q, punq );
	   vector_intersection( nei_p, nei_q, pinq );

	   if ( punq.size() == 0 ) {
	      return 0.0;
	   }
	} else {
	   // l == -1
	   for (int i = 0; i < n; ++i ) {
	      if ( (i != p) && (i != q) ) {
		 pinq.push_back( i );
	      }
	   }
	}

	if (length) {
	  remove_vertex_star( q, neigh_q );
	  d_comp = false;
	  for (int i = 0; i < pinq.size(); ++i) {
	    dp = dist( p, pinq[i] );
	    pdist.push_back( dp );
	  }
	  add_vertex_star(q, neigh_q );
	  
	  d_comp = false;
	  remove_vertex_star( p, neigh_p );
	  for (int i = 0; i < pinq.size(); ++i) {
	    dq = dist( q, pinq[i] );
	    qdist.push_back( dq );
	  }
	  add_vertex_star(p, neigh_p );

	  for (int i = 0; i < pinq.size(); ++i) {
	    numerator += (1.0)/ (pdist[i] + qdist[i] - 1.0);
	  }

	} else {
	    numerator = static_cast<double> ( pinq.size() );
	 }

	if (rad_dom) {
	   //only works if l == 2
	   unsigned npaths_p = 0;
	   unsigned npaths_q = 0;
	   for (int i = 0; i < pinq.size(); ++i) {
	      if ( pdist[i] == 1 ) {
		 npaths_p += get_degree( pinq[ i ] ) - 1;
	      }

	      if ( qdist[i] == 1 ) {
		 npaths_q += get_degree( pinq[ i ] ) - 1;
	      }
	   }

	   double denominator;
	   // if (npaths_p < npaths_q)
	   //    denominator = static_cast<double>( npaths_p );
	   // else
	   //    denominator = static_cast<double>( npaths_q );
	   denominator = static_cast<double>( npaths_p ) + static_cast<double>( npaths_q );

	   if (denominator == 0.0) {
	      //	      cout << "denom: " << denominator << endl;
	      return 0.0;
	   }

	   numerator = static_cast<double> ( pinq.size() );

	   //	   cout << "num: " << numerator << endl;
	   //	   cout << "denom: " << denominator << endl;

	   return numerator  / denominator;
	      
	} else {
	   if (l == -1)
	      return numerator / static_cast< double >( n - 2 );

	   return numerator / static_cast< double >( punq.size() );
	}
      }
      else {
	 return 0.0;
      }

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
      case 5:
	 get_all_neighbors( p, nei_p );
	 get_all_neighbors( q, nei_q );

	 vector_union( nei_p, nei_q, punq );
	 vector_intersection( nei_p, nei_q, pinq );

	 if (is_edge(p,q)) {
	    if ( punq.size() == 2 ) {
	       return 0.0;
	    }

	    //punq contains both p and q, so subtract 2.
	    return static_cast<double> ( pinq.size() ) / static_cast< double >( punq.size() - 2 );
	 }
	 else {
	    return 0.0;
	 }
	 break;
      case 6:
	 get_all_neighbors( p, nei_p );
	 get_all_neighbors( q, nei_q );

	 vector_intersection( nei_p, nei_q, pinq );

	 if (is_edge(p,q)) {
	    if ( punq.size() == 2 ) {
	       return 0.0;
	    }

	    int denom = nei_p.size();
	    if (nei_q.size() < denom ) 
	       denom = nei_q.size();
	    if (denom == 0)
	       return IGRAPH_INFINITY;

	    return static_cast<double> ( pinq.size() ) / static_cast< double >( denom ) ;
	 }
	 else {
	    return 0.0;
	 }
	 break;
      }
   }
  
   //break graph into connected components (strong if directed)
   unsigned break_components( vector< graph >& result, 
			      vector< component_map_type >& translation ) {
      //perform the igraph component division
      igraph_vector_t membership;
      igraph_vector_t csize;
      int ncomps;

      igraph_vector_init( &membership, 0 );
      igraph_vector_init( &csize, 0 );

      igraph_clusters( &G, &membership, &csize, &ncomps, IGRAPH_STRONG );

      //now divide the vids by component
      vector< int > v_empty;
      vector< vector< int > > cluster_vids( ncomps, v_empty );
      int n = igraph_vcount( &G );
      component_map_type cmt;
      for (int i = 0; i < n; ++i) {
	 cmt.vid_global = i;
	 cmt.which_component = VECTOR( membership )[ i ];
         cmt.vid_component = cluster_vids[ cmt.which_component ].size(); 
	 cluster_vids[ cmt.which_component ].push_back( i );

	 translation.push_back( cmt );
      }
    
      //create the component graphs
      result.clear();
      graph* g;
      for (unsigned i = 0; i < cluster_vids.size(); ++i) {
	 g = new graph( cluster_vids[i].size() );
	 result.push_back( (*g) );
      }

      //add the edges 
      int m = igraph_ecount( &G );
      int p, q;
      int which_cc;
      int newp, newq;
      for (int i = 0; i < m; ++i) {
	 igraph_edge( &G, i, &p, &q );
	 which_cc = translation[ p ].which_component;
	 newp = translation[p].vid_component;
	 newq = translation[q].vid_component;
	 //	 cout << "newp " << newp << " newq " << newq << endl;
	 //	 cout << "result[which_cc].n " << result[which_cc].n() << endl;
	 result[which_cc].add_edge( newp, newq );
      }
   }

   double compute_HCC( int order, bool b_length = false, bool rad_dom = false ) {
      int m = this->m();
      double res = 0.0;
      int p,q;

      graph G_copy( *this );
      for (int k = 0; k < m; ++k) {
	 //	cout << "\r" << k << "                "; 
	 igraph_edge( &G, k, &p, &q );
	 //	 cout << "(" << p << "," << q << "): " << G_copy.compute_local_HCC(p,q, order, b_length, rad_dom ) << endl;
	 res += G_copy.compute_local_HCC(p,q, order, b_length, rad_dom);
	 //	 cout << "k: " << k << ", res: " << res << endl;
      }

      if (m > 0)
	 res = res / static_cast<double>( m );

      return res;
   }

   void vector_minus( vector< int >& vec, int p ) {
      vector< int > res;
      for (unsigned i = 0; i < vec.size(); ++i) {
	 if (vec[i] != p)
	    res.push_back ( vec[i] );
      }

      vec.assign( res.begin(), res.end() );
   }

   //"Edge clustering coefficient"
   double compute_ECC( unsigned type = 0) {
      double res = 0.0;
      int n = igraph_vcount( &G );
      int m = igraph_ecount( &G );
      int p,q;
      int m_clique;
      //for ECC4
      vector< graph > ccs; //connected components
      vector< component_map_type > map;
      switch( type ) {
      case 4:
	 //do ECC3 on each connected component
	 //results
	 break_components( ccs, map );
	 for (int i = 0; i < ccs.size(); ++i) {
	    res += ccs[i].compute_ECC( 3 ) / static_cast<double>( ccs.size() );
	 }

	 res = res / static_cast<double>( ccs.size() );

	 break;
      case 3:
	 for (int k = 0; k < m; ++k) {
	    igraph_edge( &G, k, &p, &q );
	    res += compute_local_ECC(p,q, 0);
	 }

	 if (m > 0)
	    res = res / static_cast<double>( m );

	 break;
      case 1:
	 m_clique = static_cast<int>( this->n() * ( this->n() - 1 ) / 2.0 ); // n choose 2
	 return static_cast<double>( this->m() ) / m_clique;
	 break;
      case 5:
	 //no partial triangles
	 for (int k = 0; k < m; ++k) {
	    igraph_edge( &G, k, &p, &q );
	    res += compute_local_ECC(p,q, 5);
	 }

	 if (m > 0)
	    res = res / static_cast<double>( m );
	 break;
      case 6:
	 //no partial triangles
	 for (int k = 0; k < m; ++k) {
	    igraph_edge( &G, k, &p, &q );
	    res += compute_local_ECC(p,q, 6);
	 }

	 if (m > 0)
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
