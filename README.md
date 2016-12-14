Program to compute various clustering coefficients

'mygraph.hpp' wrapper to igraph library, which must be installed on your machine to compile.

'main.cpp' computes a few different clustering coefficients and prints to command line

To compile, use the included Makefile (requires the Igraph C library)

'*.el' sample graphs in edge list format which can be used for testing

Usage: main 'input graph filename' 
       or main 'graph description'

       Graph descriptions:
       ER<n>:<p> -- generates an Erdos-renyi random graph with <n> nodes with edge probability <p>
       WS<n>:<p> -- generates a Watts-Strogatz small-world network with <n> nodes and edge rewiring probability <p>

Clustering coefficient definitions:
ECC
	0 -- For each pair of vertices (u,v), c(u,v) is computed using 
	     | N(u) \cap N(v) | / | N(u) \cup N(v) |, (1)
	     where N(.) is the set of neighbors of . in the graph,
	     including . itself.

	     Then c(u,v) is averaged over all pairs.

	     More discussion is provided in "ref/summary_ecc0.pdf".
	1 -- proportional to the number of edges in the graph (old)
	2 -- Slight modification of the definition in ECC0
	3 -- Eq. (1) averaged over only extant edges in the graph
	4 -- ECC3 / ncc^2, where ncc is the number  of connected components of the graph.