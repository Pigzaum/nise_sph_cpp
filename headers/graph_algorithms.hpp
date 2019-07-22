/*
 * File: graph_algorithms.hpp
 * Author: Guilherme O. Chagas
 *
 * @brief Header of some graph algorithms.
 * 
 * (I'm sorry for my bad english xD)
 *
 * Created on August 27, 2018, 20:58 PM
 * 
 * References:
 * [1] J. Hopcroft and R. Tarjan. Algorithm 447: efficient algorithms for graph 
 * manipulation, Communications of the ACM, 16(6) (1973) 372-378.
 */

#ifndef UTILS_GRAPH_ALGORITHMS_HPP
#define UTILS_GRAPH_ALGORITHMS_HPP


#include "./graph.hpp"


namespace utils
{
namespace graph_algorithms
{
    /**
    * @brief Breadth First Search (BFS) algorithm. Performs the BFS in the 
    * input graph stating from vertex v and identify the connected component 
    * that v belongs to. It uses the "component_id" parameter to "label" all 
    * vertices reached from v. These vertices labels are stored in the 
    * "vertices_label" vector. All vertices with same label label at the 
    * "vertices_label" vector belong to the same connected component.
    * @param const Graph &g: input graph.
    * @param unsigned int v: starting vertex.
    * @param unsigned int: current connected component label.
    * @param std::vector<unsigned int> &: connected component label of each 
    * vertex. 
    */
    void bfs(const Graph &g, unsigned int starting_v, unsigned int component_id,
        std::vector<unsigned int> &vertices_label);

    /**
    * @brief Find the biconnected components of the input graph using the 
    * recursive Hopcroft-Tarjan algorithm [1].
    * @param const Graph &g: input graph.
    * @return std::vector<std::vector<Graph::edge>> : edges that compose each of
    *  the biconnected components of the input graph.
    */
    std::vector<std::vector<Graph::edge>> biconnected_components(
        const Graph &g);

    /**
    * @brief Identify the connected components of the input graph g using the 
    * BFS algorithm. The connected components are represented by a vector of 
    * vectors where each one of these vectors is composed by vertices indices 
    * that belong to the ith-connected component of g.
    * @param const Graph &g: input graph.
    * @return std::vector<std::vector<unsigned int>>: vector of vectors of 
    * vertices indices. Each one of these vectors of vertices indices represent 
    * one connected component.
    */
    std::vector<std::vector<unsigned int>> connected_components(const Graph &g);

    /**
    * @brief Depth First Search.
    * @param const Graph &: graph.
    * @param unsigned int: starting vertex.
    * @return std::vector<bool>: flags vector of visited vertices.
    */
    std::vector<bool> dfs(const Graph &g, unsigned int v);

    /**
    * @brief 
    * @param const Graph &: graph.
    * @param const unsigned int: starting vertex.
    * @param const unsigned int:.
    * @return bool:.
    */
    bool is_reachable(const Graph &g, const unsigned int starting_v, 
        const unsigned int target_v);

} // graph_algorithms
} // utils

#endif /* UTILS_GRAPH_ALGORITHMS_HPP */