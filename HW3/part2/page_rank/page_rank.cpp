#include "page_rank.h"

#include <stdlib.h>
#include <cmath>
#include <omp.h>
#include <utility>

#include "../common/CycleTimer.h"
#include "../common/graph.h"

// pageRank --
//
// g:           graph to process (see common/graph.h)
// solution:    array of per-vertex vertex scores (length of array is num_nodes(g))
// damping:     page-rank algorithm's damping parameter
// convergence: page-rank algorithm's convergence threshold
//
// refrence: http://jpndbs.lib.ntu.edu.tw/DB/PageRank.pdf、https://zh.wikipedia.org/wiki/PageRan

void pageRank(Graph g, double *solution, double damping, double convergence)
{
  int numNodes = num_nodes(g);
  double equal_prob = 1.0 / numNodes;
  double *pre_solution;
  pre_solution = (double*)malloc(sizeof(double) * numNodes);

  #pragma omp parallel for
  for (int i = 0; i < numNodes; ++i) solution[i] = equal_prob;

  bool converged = false;
  while (!converged){
      double total_pr_diff = 0.0;
      double no_outgoing_pr = 0.0;
      memcpy(pre_solution, solution, numNodes * sizeof(double));

      #pragma omp parallel for reduction(+:no_outgoing_pr)
      for (int j = 0; j < numNodes; j++) no_outgoing_pr += (outgoing_size(g, j) == 0) ? damping*pre_solution[j]/numNodes : 0;

      #pragma omp parallel for reduction(+:total_pr_diff)
      for (int i = 0; i < numNodes; i++)  // ex: pr(c)
      {  
        const Vertex* start = incoming_begin(g, i);
        const Vertex* end   = incoming_end(g, i);
        double pr_i = 0.0;
        
        // #pragma omp parallel for reduction(+:pr_i) 這邊不加parallel比較快!!
        // ex: pr(c) = pr(a)/2 + pr(b)/1 +...
        for (const Vertex* v = start; v != end; v++) pr_i += pre_solution[*v] / (double)outgoing_size(g, *v);
        pr_i = (damping * pr_i) + (1.0 - damping) / numNodes;
        pr_i += no_outgoing_pr;
		    solution[i] = pr_i;
		    total_pr_diff += fabs(pr_i - pre_solution[i]);
      }
      converged = total_pr_diff < convergence;
  }
  delete pre_solution;
}
