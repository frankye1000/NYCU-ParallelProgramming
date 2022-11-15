#include "bfs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstddef>
#include <omp.h>

#include "../common/CycleTimer.h"
#include "../common/graph.h"

#define ROOT_NODE_ID 0
#define NOT_VISITED_MARKER -1
#define ZERO_MARKER 0
#define THRESHOLD 10000000

void vertex_set_clear(vertex_set *list)
{
    list->count = 0;
}

void vertex_set_init(vertex_set *list, int count)
{
    list->max_vertices = count;
    list->vertices = (int *)malloc(sizeof(int) * list->max_vertices);
    vertex_set_clear(list);
}


void top_down_step(Graph g, vertex_set *frontier, vertex_set *new_frontier, int *distances)
{
    int local_count;
    #pragma omp parallel private(local_count)
    {
        local_count = 0;
        // 我這邊創一個local的queue，讓每個thread填自己的queue
        int* local_frontier = (int*)malloc(sizeof(int) * (g->num_nodes));  
        #pragma omp for 
        for (int i=0; i<frontier->count; i++) {         // 把所有在queue裡的點都拿出來拜訪一遍      
            int node       = frontier->vertices[i];     // node: 點的id
            int start_edge = g->outgoing_starts[node];
            int end_edge   = (node == g->num_nodes-1) ? g->num_edges : g->outgoing_starts[node+1];

            // attempt to add all neighbors to the new frontier
            for (int neighbor=start_edge; neighbor<end_edge; neighbor++) { // 拜訪這個點的鄰居們
                int outgoing = g->outgoing_edges[neighbor];

                if( distances[outgoing] == NOT_VISITED_MARKER) {  // 這鄰居沒拜訪過
                    local_frontier[local_count] = outgoing;       // 就加入local queue
                    local_count++;                                // 且local count+1
                    distances[outgoing] = distances[node] + 1;    // 上一個點的距離+1，就是這個點和root的距離
                }
            }
        }

        #pragma omp critical                    
        {   // 把每一個local queue複製進new_frontier queue裡面，並依據local count更新new_frontier->count數量
            memcpy(new_frontier->vertices+new_frontier->count, local_frontier, local_count*sizeof(int));
            new_frontier->count += local_count;
        }
    }
    
}


void bfs_top_down(Graph graph, solution *sol)
{
    vertex_set list1;
    vertex_set list2;
    vertex_set_init(&list1, graph->num_nodes);
    vertex_set_init(&list2, graph->num_nodes);

    vertex_set *frontier     = &list1;
    vertex_set *new_frontier = &list2;

    // initialize all nodes to NOT_VISITED
    for (int i = 0; i < graph->num_nodes; i++) sol->distances[i] = NOT_VISITED_MARKER;
    // set the root distance with 0
    sol->distances[ROOT_NODE_ID] = 0;

    // setup frontier with the root node
    frontier->vertices[frontier->count++] = ROOT_NODE_ID;

    while (frontier->count != 0)
    {

#ifdef VERBOSE
        double start_time = CycleTimer::currentSeconds();
#endif

        vertex_set_clear(new_frontier);

        top_down_step(graph, frontier, new_frontier, sol->distances);

#ifdef VERBOSE
        double end_time = CycleTimer::currentSeconds();
        printf("frontier=%-10d %.4f sec\n", frontier->count, end_time - start_time);
#endif

        // swap pointers
        vertex_set *tmp = frontier;
        frontier = new_frontier;
        new_frontier = tmp;
    }
}


void bottom_up_step(Graph g, vertex_set* frontier, int *distances, int frontier_id)
{
    int local_count = 0;

    #pragma omp parallel
    {
	    #pragma omp for reduction(+:local_count)
	    for (int i=0; i < g->num_nodes; i++) // 圖上所有點
        {
    	    if (frontier->vertices[i] == ZERO_MARKER)  // 隨機圖上一點沒被拜訪
            {
    	        int start_edge = g->incoming_starts[i];
    	        int end_edge   = (i == g->num_nodes-1) ? g->num_edges : g->incoming_starts[i+1];

    	        for (int neighbor = start_edge; neighbor < end_edge; neighbor++) 
                {
		            int incoming = g->incoming_edges[neighbor];

		            if (frontier->vertices[incoming] == frontier_id)
                    {
			            distances[i]          = distances[incoming] + 1;
			            frontier->vertices[i] = frontier_id + 1;
    	    	      	local_count++;
    	    	      	break;
		            }
    	        }
    	    }
    	}
    }
    frontier->count += local_count;
}


void bfs_bottom_up(Graph graph, solution *sol)
{
    vertex_set list1;
    vertex_set_init(&list1, graph->num_nodes);
    vertex_set *frontier = &list1;

    // initialize all nodes to NOT_VISITED
    for (int i = 0; i < graph->num_nodes; i++) sol->distances[i] = NOT_VISITED_MARKER;
    // setup frontier with the root node
    sol->distances[ROOT_NODE_ID] = 0;

    int frontier_id = 1;
    // 初始把1放進去frontier queue 
    frontier->vertices[frontier->count++] = frontier_id;

    while (frontier->count != 0){
        frontier->count = 0;

#ifdef VERBOSE
        double start_time = CycleTimer::currentSeconds();
#endif

        bottom_up_step(graph, frontier, sol->distances, frontier_id);

#ifdef VERBOSE
        double end_time = CycleTimer::currentSeconds();
        printf("frontier=%-10d %.4f sec\n", frontier->count, end_time - start_time);
#endif

	    frontier_id++;
    }

}




/* 因為使用frontier_id紀錄比較好實作hybrid方法
   所以我改寫top_down_step => frank_top_down_step 
            bfs_top_down  => frank_bfs_top_down 
*/
void frank_top_down_step(Graph g,vertex_set *frontier,int *distances,int frontier_id)
{
    int local_count = 0;
    #pragma omp parallel
    {
        #pragma omp for reduction(+:local_count)
        for (int i = 0; i < g->num_nodes; i++) // 所有的點
		{
			if (frontier->vertices[i] == frontier_id)
			{
				int start_edge = g->outgoing_starts[i];
				int end_edge   = (i == g->num_nodes-1) ? g->num_edges : g->outgoing_starts[i+1];

				// attempt to add all neighbors to the new frontier
				for (int neighbor = start_edge; neighbor < end_edge; neighbor++) 
				{
					int outgoing = g->outgoing_edges[neighbor];
					if (frontier->vertices[outgoing] == ZERO_MARKER)
					{
						local_count++;
						distances[outgoing] = distances[i] + 1;
						frontier->vertices[outgoing] = frontier_id + 1;
					}
				}
			}
        }
    }
    frontier->count = local_count;
}

void frank_bfs_top_down(Graph graph, solution *sol)
{
    vertex_set list1;
    vertex_set_init(&list1, graph->num_nodes);
    vertex_set *frontier = &list1;
    int frontier_id = 1;
    // 初始把frontier_id放進去frontier queue
    frontier->vertices[frontier->count++] = frontier_id;
    // initialize all nodes to NOT_VISITED
    for (int i = 0; i < graph->num_nodes; i++) sol->distances[i] = NOT_VISITED_MARKER;
    // setup frontier with the root node
    sol->distances[ROOT_NODE_ID] = 0;
    while (frontier->count != 0)
    {
        frontier->count = 0;
        frank_top_down_step(graph, frontier, sol->distances, frontier_id);
        // swap pointers
	    frontier_id++;
    }
}





/* 因為使用frontier_id紀錄比較好實作hybrid方法
   所以我改寫top_down_step => frank_top_down_step 
            bfs_top_down  => frank_bfs_top_down 
*/
void bfs_hybrid(Graph graph, solution *sol)
{
    vertex_set list1;
    vertex_set_init(&list1, graph->num_nodes);
    vertex_set *frontier = &list1;

    int frontier_id = 1;
     // 初始把frontier_id放進去frontier queue
    frontier->vertices[frontier->count++] = frontier_id;
    // initialize all nodes to NOT_VISITED
    for (int i = 0; i < graph->num_nodes; i++) sol->distances[i] = NOT_VISITED_MARKER;
    sol->distances[ROOT_NODE_ID] = 0;

    while (frontier->count != 0){
#ifdef VERBOSE
        double start_time = CycleTimer::currentSeconds();
#endif
    // 因為bottom_up在Queue裡面數量較多時比較快 所以這裡設THRESHOLD，選擇使用哪個方法
	if (frontier->count > THRESHOLD){  
	    frontier->count = 0;
	    bottom_up_step(graph, frontier, sol->distances, frontier_id);
	}
	else{
	    frontier->count = 0;
	    frank_top_down_step(graph, frontier, sol->distances, frontier_id);
	}

#ifdef VERBOSE
        double end_time = CycleTimer::currentSeconds();
        printf("frontier=%-10d %.4f sec\n", frontier->count, end_time - start_time);
#endif

	    frontier_id++;
    }

}
