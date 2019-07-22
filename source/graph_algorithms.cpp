/*
 * File: graph_algorithms.cpp
 * Author: Guilherme O. Chagas
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on August 27, 2018, 21:11 PM
 * 
 * References:
 * [1] J. Hopcroft and R. Tarjan. Algorithm 447: efficient algorithms for graph 
 * manipulation, Communications of the ACM, 16(6) (1973) 372-378.
 */

#include <algorithm>
#include <iostream>
#include <queue>
#include <stack>
#include "../headers/graph_algorithms.hpp"


/////////////////////////////// Helper functions ///////////////////////////////

namespace
{

/**
* @brief Checks if the two input edges are equal considering a undirected 
* graph, i.e., (v,u) = (u,v).
* @param const Graph::edge &: first edge.
* @param const Graph::edge &: second edge.
* @return bool: return true if e1 = e2. False otherwise.
*/
bool equal_edges(const Graph::edge &e1, const Graph::edge &e2)
{
    return (e1.first == e2.first && e1.second == e2.second) || 
        (e1.first == e2.second && e1.second == e2.first);
}

/**
* @brief Used by recursive_hopcroft_tarjan algorithm to get the edges of the 
* current biconnected component. The edges are popped from stack until the edge 
* (v,u) is reached which is also popped. These edges compose the biconnected 
* component.
* @param const unsigned int: index of vertex v.
* @param const unsigned int: index of vertex u.
* @param std::stack<Graph::edge> &: stack of edges.
* @return std::vector<Graph::edge>: vector of edges that belong to the 
* biconnected component.
*/
std::vector<Graph::edge> get_bcomponent_edges(const unsigned int v, 
    const unsigned int u, std::stack<Graph::edge> &stack)
{
    std::vector<Graph::edge> edges;
    while (!equal_edges(stack.top(), std::make_pair(v, u)))
    {
        edges.push_back(stack.top());
        stack.pop();
    }
    edges.push_back(stack.top());
    stack.pop();

    return edges;
}

/**
 * @brief Recursive Hopcroft-Tarjan algorithm [1] to find biconnected components
 * of the input graph. This algorithm applies the recursive Depth-First-Search 
 * approach to determine the articulation points of the graph. 
 * ATTENTION: this recursive function eventually can cause a stack overflow 
 * depending on the graph size. So, use "ulimit -s [new stack size in kb]".
 * @param const Graph: input graph.
 * @param const unsigned: current vertex index of the DFS execution.
 * @param unsigned int &: current discovered time.
 * @param std::vector<bool> &: vector of flags of visited vertices.
 * @param std::vector<unsigned int> &: vertices' discover time. 
 * @param std::vector<unsigned int> &: vertices' low child discover time.
 * @param std::vector<unsigned int> &: vertices' parent in subtree.
 * @param std::stack<Graph::edge> &: stack of edges use to identify the 
 * biconnected components.
 * @param std::vector<std::vector<Graph::edge>> &: edges that compose each of 
 * the biconnected components of the input graph.
*/
void recursive_hopcroft_tarjan(const Graph &g, const unsigned int v,
    unsigned int &disc_time, std::vector<bool> &visited, 
    std::vector<unsigned int> &discovered, std::vector<unsigned int> &low, 
    std::vector<unsigned int> &parent, std::stack<Graph::edge> &stack, 
    std::vector<std::vector<Graph::edge>> &bcomponents)
{
    visited[v] = true;
    discovered[v] = low[v] = disc_time++;
    unsigned int nb_of_child = 0;
    auto adj_list = g.adj_list_of_vtx(v);
    for (auto it = adj_list.first; it != adj_list.second; ++it)
    {
        if (!visited[*it])
        {
            ++nb_of_child;
            parent[*it] = v;
            stack.push(std::make_pair(v, *it));
            recursive_hopcroft_tarjan(g, *it, disc_time, visited, discovered, 
                low, parent, stack, bcomponents);
            low[v] = std::min(low[v], low[*it]);
            if ((parent[v] == g.get_nb_vertices() && nb_of_child > 1) ||
                (parent[v] != g.get_nb_vertices() && low[*it] >= discovered[v]))
            {
                bcomponents.push_back(get_bcomponent_edges(v, *it, stack));
            }
        }
        else if (parent[v] != *it && discovered[*it] < discovered[v])
        {
            low[v] = std::min(low[v], discovered[*it]);
            stack.push(std::make_pair(v, *it));
        }
    }
}

} // anonymous namespace

/////////////////////////////////////////////////////////////////////////////////


void utils::graph_algorithms::bfs(const Graph &g, unsigned int v, 
        unsigned int component_id, std::vector<unsigned int> &vertices_label)
{
    std::queue<unsigned int> queue;
    vertices_label[v] = component_id;
    queue.push(v);

    while (!queue.empty())
    {
        auto adj_list = g.adj_list_of_vtx(queue.front());
        queue.pop();
        for (auto it = adj_list.first; it != adj_list.second; ++it)
        {
            if (vertices_label[*it] == g.get_nb_vertices()) // not visited label
            {
                queue.push(*it);
                vertices_label[*it] = component_id;
            }
        }
    }
}


std::vector<std::vector<Graph::edge>> 
    utils::graph_algorithms::biconnected_components(const Graph &g)
{
    unsigned int disc_time = 0;
    std::vector<bool> visited(g.get_nb_vertices(), false);
    std::vector<unsigned int> discovered(g.get_nb_vertices(), 0);
    // low and parent vectors are initialized with |V| which is an invalid value
    std::vector<unsigned int> low(g.get_nb_vertices(), g.get_nb_vertices());
    std::vector<unsigned int> parent(g.get_nb_vertices(), g.get_nb_vertices());
    std::stack<Graph::edge> stack;
    std::vector<std::vector<Graph::edge>> bcomponents;

    for (unsigned int i = 0; i < g.get_nb_vertices(); ++i)
    {
        if (!visited[i])
        {
            recursive_hopcroft_tarjan(g, i, disc_time, visited, discovered, low,
                parent, stack, bcomponents);
            if (!stack.empty())
            {
                bcomponents.emplace_back(std::vector<Graph::edge>());
                while (!stack.empty())
                {
                    bcomponents.back().push_back(stack.top());
                    stack.pop();
                }
            }
        }
    }

    return bcomponents;
}


std::vector<std::vector<unsigned int>> 
    utils::graph_algorithms::connected_components(const Graph &g)
{
    const unsigned int not_visited = g.get_nb_vertices(); // invalid label
    unsigned int component_id = 0;
    std::vector<unsigned int> vertices_label(g.get_nb_vertices(), not_visited);

    for (unsigned int i = 0; i < g.get_nb_vertices(); ++i)
    {
        if (vertices_label[i] == not_visited)
        {
            bfs(g, i, component_id++, vertices_label);
        }
    }
    // at the last iteration, the component_id is the nb of connected components
    std::vector<std::vector<unsigned int>> connected_components(component_id);
    // populate each connected component vector with vertices indices
    for (unsigned int i = 0; i < g.get_nb_vertices(); ++i)
    {
        connected_components[vertices_label[i]].push_back(i);
    }

    return connected_components;
}


std::vector<bool> utils::graph_algorithms::dfs(const Graph &g, 
    unsigned int v)
{
    std::vector<bool> visited(g.get_nb_vertices(), false);
    std::stack<unsigned int> stack;
    visited[v] = true;
    stack.push(v);

    while (!stack.empty())
    {
        v = stack.top();
        stack.pop();
        auto adj_list = g.adj_list_of_vtx(v);
        for (auto it = adj_list.first; it != adj_list.second; ++it)
        {
            if (!visited[*it])
            {
                visited[*it] = true;
                stack.push(*it);
            }
        }
    }

    return visited;
}


bool utils::graph_algorithms::is_reachable(const Graph &g, 
    const unsigned int starting_v, const unsigned int target_v)
{
    return dfs(g, starting_v)[target_v];
}