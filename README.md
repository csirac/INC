Program to compute various clustering coefficients

'mygraph.hpp' wrapper to igraph library, which must be installed on your machine to compile.

'main.cpp' computes a few different clustering coefficients and prints to command line

To compile, use the included Makefile (requires the Igraph C library)

'*.el' sample graphs in edge list format which can be used for testing

Usage: main 'input graph filename' 
       or main 'graph description'

       Graph descriptions:
       ER<n>:<p> -- generates an Erdos-renyi random graph with n nodes with edge probability p (p <= 1)
       ER<n>:<m> -- generates an Erdos-renyi random graph with n nodes with m edges chosen uniformly (m > 1)
       WS<n>:<p> -- generates a Watts-Strogatz small-world network with <n> nodes and edge rewiring probability <p>
       BA<n>:<m> -- generates a Barabasi-Albert scale-free graph with n nodes, adding m edges in each step.

Clustering coefficient definitions:
ECC
	0 -- For each pair of vertices (u,v), c(u,v) is computed using 
	     | N(u) \cap N(v) | / | N(u) \cup N(v) |, (1)
	     where N(.) is the set of neighbors of . in the graph,
	     including . itself.

	     Then c(u,v) is averaged over all pairs.

	     More discussion is provided in "ref/summary_ecc0.pdf".
	1 -- m / (n choose 2), where n = number of nodes, m = number of edges
	2 -- Slight modification of the definition in ECC0
	3 -- Eq. (1) averaged over only extant edges in the graph
	4 -- ECC3 / ncc^2, where ncc is the number  of connected components of the graph.
	5 -- Modification of eq (1), s.t. {u,v} is ignored and 0.0 is returned
	     if edge (u,v) does not exist. So no partial triangles are counted.
	     Only averaged over extant edges.