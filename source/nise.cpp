/*
 * File: nise.hpp
 * Author: Guilherme O. Chagas
 *
 * @brief Implementation of the Whang, Gleich and Dhillon [1] algorithm:
 * Neighborhood-Inflated Seed Expansion (NISE).
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on August 21, 2018, 09:39 PM
 */


#include <algorithm>
#include <queue>
#include <iostream>
#include <forward_list>
#include <fstream>
#include <numeric>
#include <sys/resource.h>
#include <set>
#include <thread>
#include "../headers/nise.hpp"
#include "../headers/graph_algorithms.hpp"


/////////////////////////////// Helper functions ///////////////////////////////

namespace
{

/**
 * @brief Graph size (number of vertices) 50000 the increase the stack size.
*/
const unsigned int graph_size_threshold = 50000; // magic number

/**
 * @brief
*/
double conductance(const unsigned int g_nb_edges, 
    const unsigned int vol, const unsigned int boundary_edges)
{
    assert(vol <= 2 * g_nb_edges);

    unsigned int compl_vol = 2 * g_nb_edges - vol;
    // prevent division by 0 see LNCS 3418 - Network Analysis
    if (compl_vol == 0 || vol == 0)
    {
        return 1;
    }

    return static_cast<double>(boundary_edges) / std::min(vol, compl_vol);
}

/**
* @brief (see [1,3]).
* @param const unsigned int: vertex index to add in queue.
* @param std::queue<unsigned int> &: queue that holds vertices indices.
* @param std::set<unsigned int> &: auxiliary data structure to track uniqueness.
*/
void add_in_epsilon_approx_queue(const unsigned int v, 
    std::queue<unsigned int> &q, std::set<unsigned int> &s)
{
    if (s.find(v) == s.end())
    {
        q.push(v);
        s.insert(v);
    }
}

/**
* @brief Given a set of vertices sorted in the deacreasing 
* probability-per-degree order this method selects a set of vertices of minimum
* conductance (see [1,3]). This method is also known as degree-normalized
* version of the sweep technique [3].
* @param const Graph&: original graph.
* @param const Graph&: biconnected core graph.
* @param const std::vector<unsigned int>&: vertices indices of the original 
* graph that belong to the biconnected core.
* @param const std::vector<unsigned int>&: graph vertices indices sorted by 
* probability-per-degree order.
* @return Cluster: set (cluster) of vertices with the minimum conductance 
* value. As described in [3] this cluster is formed by the first p vertices 
* in the "vertices_sppd" that, together, compose a cluster with minimum 
* conductance. In other words, the sweep set with minimum conductance.
*/
Cluster clst_min_conductance(
    const Graph &original_g, 
    const Graph &bcore_g,
    const std::vector<unsigned int> &bcore_v,
    const std::vector<unsigned int> &vertices_sppd)
{
    Cluster clst(original_g); // cluster constructed from original graph
    std::set<unsigned int> sweep_set;
    unsigned int vol = 0;
    unsigned int boundary_edges = 0;
    double min_cond = std::numeric_limits<double>::infinity();
    for (unsigned int i = 0; i < vertices_sppd.size(); ++i)
    {
        // compute the number of boundary edges
        sweep_set.insert(bcore_v[vertices_sppd[i]]); // insert original v label
        auto adj_list = bcore_g.adj_list_of_vtx(vertices_sppd[i]);
        for (auto it_adj = adj_list.first; it_adj != adj_list.second; ++it_adj)
        {
            if (sweep_set.find(bcore_v[*it_adj]) != sweep_set.end())
            {
                --boundary_edges;
            }
            else
            {
                ++boundary_edges;
            }
        }
        vol += bcore_g.get_vtx_degree(vertices_sppd[i]);
        // compute conductances
        double cond = conductance(bcore_g.get_nb_edges(), vol, boundary_edges);
        if (cond <= min_cond)
        {
            min_cond = cond;
            // save set of vertices (clst) of minimum conductance
            for (unsigned int j = clst.size(); j <= i; ++j)
            {
                // insert vertices from the original graph
                clst.insert(bcore_v[vertices_sppd[j]]);
            }
        }
    }

    return clst;
}

/**
* @brief (see [1]).
*/
std::vector<unsigned int> find_biconnected_core_vertices(const Graph &g)
{
    // connected components of the graph
    auto ccomponents = utils::graph_algorithms::connected_components(g);

    // the biconnected core is the largest connected component (see [1])
    return *std::max_element(ccomponents.begin(), ccomponents.end(), 
        [](std::vector<unsigned int> &cc1, std::vector<unsigned int> &cc2)
        {
            return cc1.size() < cc2.size();
        });
}

/**
* @brief (see [1]).
* @param:.
* @param:.
* @param:.
* @return:.
*/
Graph generate_biconnected_core_graph(
    const Graph &original_graph,
    const std::vector<unsigned int> &bcore_vertices, 
    const std::vector<bool> &contained_in_bcore)
{
    std::cout << "\t\t\tgenerating biconnected core...\n";
    // mapping the vertex label in the original graph to label in the new graph
    std::unordered_map<unsigned int, unsigned int> bcore_map;
    for (unsigned int i = 0; i < bcore_vertices.size(); ++i)
    {
        bcore_map[bcore_vertices[i]] = i;
    }

    Graph bcore_graph(bcore_vertices.size(), Graph::data_structure::adj_list);

    for (unsigned int i = 0; i < bcore_graph.get_nb_vertices(); ++i)
    {
        auto adj_list = original_graph.adj_list_of_vtx(bcore_vertices[i]);
        for (auto it = adj_list.first; it != adj_list.second; ++it)
        {
            if (contained_in_bcore[*it])
            {
                bcore_graph.add_edge(i, bcore_map.at(*it));
            }
        }
    }

    return bcore_graph;
}

/**
* @brief not my code.
*/
void increase_stack_size()
{
    const rlim_t k_stack_size = 32 * 1024 * 1024; // min stack size = 16 MB
    struct rlimit rl;

    int result = getrlimit(RLIMIT_STACK, &rl);
    if (result == 0)
    {
        if (rl.rlim_cur < k_stack_size)
        {
            std::cout << "Changing stack size...\n";
            rl.rlim_cur = k_stack_size;
            result = setrlimit(RLIMIT_STACK, &rl);
            if (result != 0)
            {
                std::cout << "setrlimit returned result = " << result << "\n";
            }
        }
    }
}

/**
* @brief Get a vertex independent set of the biconnected core graph with same 
* degree as the ith vertex of the "decreasing degree" order. An independent set 
* is a set of vertices in a graph, no two of which are adjacent. In this method
* the independent set is found iterating through the "decreasing_degree" vector. 
* As described in [1], vertices belonging to this independet set must be not 
* previously marked as visited.
* @param const Graph &: biconnected core graph.
* @param const std::vector<unsigned int> &: vertices decreasing degree order. 
* Vertices that were previous visited are labeled with an invalid label.
* @param const std::vector<bool> &: vertices marked vector. If the ith-vertex was
* previously visited, then marked[i] will be true. False, otherwise.
* @param unsigned int: vertex index in the decreasing degree order in which the 
* search for the independent set will start.
* @return std::vector<unsigned int>: vector of vertices that compose the 
* independent set.
*/
std::vector<unsigned int> independent_set_of(const Graph &bcore_g, 
    const std::vector<unsigned int> &decreasing_degree, 
    const std::vector<bool> &marked, unsigned int i)
{
    const unsigned int degree = bcore_g.get_vtx_degree(decreasing_degree[i]);
    std::vector<unsigned int> ind_set; // independent set

    while (bcore_g.get_vtx_degree(decreasing_degree[i]) == degree)
    {
        bool adjacent = false;
        for (auto it = ind_set.begin(); it != ind_set.end(); ++it)
        {
            if (bcore_g.has_edge(decreasing_degree[i], *it))
            {
                adjacent = true; // must not be adjacent to be inserted
                break;
            }
        }

        if (!adjacent)
        {
            ind_set.push_back(decreasing_degree[i]);
        }
        
        ++i; // next vertex index

        while (i < decreasing_degree.size() && marked[decreasing_degree[i]])
        {
            ++i; // ignore already visited vertices
        }
        if (i >= decreasing_degree.size())
        {
            break; // all vertices visited
        }
    }

    return ind_set;
}

/**
* @brief .
* @param:.
* @param:.
* @param:.
*/
void mark_vertex_neighborhood(const unsigned int v, const Graph &bcore_g, 
    std::vector<bool> &marked)
{
    // mark vertex and its neighbors
    marked[v] = true; // mark vertex
    auto adj_list = bcore_g.adj_list_of_vtx(v);
    for (auto it = adj_list.first; it != adj_list.second; ++it)
    {
        marked[*it] = true;
    }
}


/**
* @brief Sort vertices indices in decreasing probability-per-degree (PPD) order. 
* This fuction returns a vector of vertices indices sorted in decreasing 
* probability-per-degree. See [1,3] for details.
* @param:.
* @param:.
*/
std::vector<unsigned int> sort_vertices_in_decreasing_ppd(
    const std::unordered_map<unsigned int, double> &x, const Graph &bcore_g)
{
    std::vector<unsigned int> decreasing_ppd(x.size()); // vertices indices
    unsigned int i = 0;
    for (auto &e : x) // populate vector with vertices indices
    {
        decreasing_ppd[i++] = e.first;
    }
    std::sort(decreasing_ppd.begin(), decreasing_ppd.end(), // sort
        [&](const unsigned int v1, const unsigned int v2)
        {
            return (x.at(v1) / static_cast<double>(bcore_g.get_vtx_degree(v1))) 
                > (x.at(v2) / static_cast<double>(bcore_g.get_vtx_degree(v2)));
        });
    return decreasing_ppd;
}

} // anonymous namespace

////////////////////////////////////////////////////////////////////////////////


Nise::Nise(const Graph &g, const Nise_parameters &p) :
    m_graph(g),
    m_p(p),
    m_clusters(g.get_nb_vertices())
{
    if (m_graph.get_nb_vertices() > graph_size_threshold)
    {
        increase_stack_size();
    }
}


void Nise::execute()
{
    // utils::time_mgmt tmgmt;
    // tmgmt.start();
    
    // filtering phase: find the biconnected core (bcore)
    std::vector<unsigned int> bcore_v; // vertices belonging in bcore
    std::vector<bool> contained_in_bcore; // flag vector of original graph
    std::vector<Graph::edge> bridges; // bridges edges.

    std::tie(bcore_v, contained_in_bcore, bridges) = filtering_phase(m_graph);

    // generate new a graph from vertices belonging in the biconnected core
    Graph bcore_g = 
        generate_biconnected_core_graph(m_graph, bcore_v, contained_in_bcore);

    auto seeds = seeding_phase(bcore_g);

    seed_expansion_phase(bcore_g, bcore_v, seeds);

    propagation_phase(bridges, contained_in_bcore);

    // tmgmt.end();
    // m_execution_time = tmgmt.get_total_time_in_sec();

    m_executed = true;
}


void Nise::write_clustering() const
{
    std::cout << "Writing clustering in file...\n";
    if (!m_executed)
    {
        std::cerr << "LECM algorithm hasn't been executed! I can't write any "
            "clustering! Aborting...\n";
        exit(EXIT_FAILURE);
    }

    m_clusters.write_clusters_in_file("./clustering.dat");
}


/////////////////////////////// private methods ////////////////////////////////


std::tuple<std::vector<unsigned int>, std::vector<bool>, 
    std::vector<Graph::edge>> Nise::filtering_phase(Graph g)
{
    std::cout << "\t\tfiltering phase...\n";

    // get all biconnected components of the original graph
    auto bcs = utils::graph_algorithms::biconnected_components(g);
    // vector of single-edge biconnected components
    std::vector<Graph::edge> single_edge_bc;
    single_edge_bc.reserve(bcs.size()); // at most all biconnected components
    // remove edges of all biconnected components of size one
    std::for_each(bcs.begin(), bcs.end(), 
        [&](const std::vector<Graph::edge> &bc)
        {
            if (bc.size() == 1)
            {
                g.remove_edge(bc.front());
                single_edge_bc.push_back(bc.front());
            }
        });

    // find vertices belonging to the biconnected core
    std::vector<unsigned int> bcore_v = find_biconnected_core_vertices(g);
    
    // flag vector to control which vertex is contained in biconnected core
    std::vector<bool> contained_in_bcore(g.get_nb_vertices(), false);
    std::for_each(bcore_v.begin(), bcore_v.end(), 
        [&](const unsigned int v)
        {
            contained_in_bcore[v] = true;
        });

    // get the bridges
    std::vector<Graph::edge> bridges;
    std::for_each(single_edge_bc.begin(), single_edge_bc.end(),
        [&](const Graph::edge &e)
        {
            if ((!contained_in_bcore[e.first] && contained_in_bcore[e.second])
                || 
                (contained_in_bcore[e.first] && !contained_in_bcore[e.second]))
            {
                bridges.push_back(e);
            }
        });

    return std::make_tuple(bcore_v, contained_in_bcore, bridges);
}


void Nise::propagation_phase(const std::vector<Graph::edge> &bridges,
    std::vector<bool> &contained_in_bcore)
{
    std::cout << "\t\tpropagation phase...\n";
    for (auto &e : bridges)
    {
        // biconnected core vertex
        unsigned int bcv = contained_in_bcore[e.first] ? e.first : e.second;
        // starting whisker vertex
        unsigned int swv = contained_in_bcore[e.first] ? e.second : e.first;
        // breadth-first-search from swv
        std::queue<unsigned int> q;
        q.push(swv);
        while (!q.empty())
        {
            swv = q.front();
            q.pop();

            // insert swv in all clusters that bcv belongs to
            auto clsts_id = m_clusters.get_v_belonging(bcv);
            for (auto it = clsts_id.first; it != clsts_id.second; ++it)
            {
                m_clusters.insert_v_in_clst(swv, *it);
            }

            auto adj_list = m_graph.adj_list_of_vtx(swv);
            for (auto it = adj_list.first; it != adj_list.second; ++it)
            {
                if (!contained_in_bcore[*it])
                {
                    q.push(*it);
                    contained_in_bcore[*it] = true;
                }
            }
        }
    }
}


Cluster Nise::seed_expansion_by_ppr(
    const Graph &bcore_g,
    const std::vector<unsigned int> &bcore_v,
    const unsigned int seed)
{
    std::unordered_map<unsigned int, double> x;
    std::unordered_map<unsigned int, double> r;

    // initialize x and r of vertex seed and its neighboors
    x[seed] = 0;
    r[seed] = 1.0 / static_cast<double>(bcore_g.get_vtx_degree(seed) + 1);
    auto adj_list = bcore_g.adj_list_of_vtx(seed);
    for (auto it = adj_list.first; it != adj_list.second; ++it)
    {
        x[*it] = 0;
        r[*it] = 1.0 / static_cast<double>(bcore_g.get_vtx_degree(seed) + 1);
    }

    // queue of vertices used in next loop as presented by [3]
    std::queue<unsigned int> q;
    std::set<unsigned int> s; // auxiliary data structure to track uniqueness
    for (auto it = r.begin(); it != r.end(); ++it)
    {
        if ((*it).second > bcore_g.get_vtx_degree((*it).first) * m_p.epsilon())
        {
            add_in_epsilon_approx_queue((*it).first, q, s);
        }
    }

    // compute and update x and r values
    while (!q.empty()) // while there is a v such that r[v] > deg(v) * epsilon
    {
        unsigned int v = q.front();
        x[v] += (1 - m_p.alpha()) * r[v];
        adj_list = bcore_g.adj_list_of_vtx(v);
        for (auto it_u = adj_list.first; it_u != adj_list.second; ++it_u)
        {
            r[*it_u] += (m_p.alpha() * r[v]) / (2 * bcore_g.get_vtx_degree(v));
            if (r[*it_u] > bcore_g.get_vtx_degree(*it_u) * m_p.epsilon())
            {
                add_in_epsilon_approx_queue(*it_u, q, s);
            }
        }
        r[v] = m_p.alpha() * r[v] / 2;
        if (r[v] <= bcore_g.get_vtx_degree(v) * m_p.epsilon())
        {
            q.pop();
        }
    }

    // sort vertices in decreasing probability-per-degree (PPD) order
    auto decreasing_ppd = sort_vertices_in_decreasing_ppd(x, bcore_g);

    // sweep step: pick and return the set (cluster) with minimum conductance
    return clst_min_conductance(m_graph, bcore_g, bcore_v, decreasing_ppd);
}


void Nise::seed_expansion_phase(const Graph &bcore_g, 
    const std::vector<unsigned int> &bcore_v, 
    const std::vector<unsigned int> &seeds)
{
    std::cout << "\t\tseeding expansion phase...\n";
    const unsigned int nb_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    threads.reserve(nb_threads);

    for (unsigned int i = 0; i < nb_threads; ++i)
    {
        threads.push_back(std::thread(&Nise::seed_expansion_thread_task, this,
            std::ref(bcore_g), std::ref(bcore_v), std::ref(seeds), i));
    }

    for (auto &t : threads)
    {
        t.join();
    }
}


void Nise::seed_expansion_thread_task(const Graph &bcore_g, 
    const std::vector<unsigned int> &bcore_v, 
    const std::vector<unsigned int> &seeds,
    const unsigned int thread_id)
{
    const unsigned int nb_threads = std::thread::hardware_concurrency();
    const unsigned int thread_offset = seeds.size() / nb_threads;
    const unsigned int rem = seeds.size() % nb_threads;
    unsigned int starting_seed = thread_id * thread_offset; 
    unsigned int final_seed = starting_seed + thread_offset;

    if (thread_id >= nb_threads - rem)
    {
        starting_seed += thread_id - (nb_threads - rem);
        final_seed += thread_id - (nb_threads - rem - 1);
    }

    for (unsigned int i = starting_seed; i < final_seed; ++i)
    {
        shared_clst_insertion(seed_expansion_by_ppr(bcore_g,bcore_v, seeds[i]));
    }
}


std::vector<unsigned int> 
    Nise::seeding_by_spread_hubs(const Graph &bcore_g) const
{
    // it needs test
    std::vector<unsigned int> decreasing_degree(bcore_g.get_nb_vertices());
    std::vector<bool> marked(bcore_g.get_nb_vertices(), false);
    std::iota(decreasing_degree.begin(), decreasing_degree.end(), 0);
    // sort vertices in decreasing degree order
    std::sort(decreasing_degree.begin(), decreasing_degree.end(), 
        [&](const unsigned int v1, const unsigned int v2)
        {
            return bcore_g.get_vtx_degree(v1) > bcore_g.get_vtx_degree(v2);
        });

    std::vector<unsigned int> seeds;
    seeds.reserve(2 * m_p.nb_of_seeds());

    unsigned int i = 0;
    while (seeds.size() < m_p.nb_of_seeds() && i < bcore_g.get_nb_vertices())
    {
        if (!marked[decreasing_degree[i]]) // unmarked (unvisited) vertex
        {
            /* get an independent set of vertices (indices in the 
             * "decreasing_degree" vector) with same degree as the ith vtx */
            auto vertices = 
                independent_set_of(bcore_g, decreasing_degree, marked, i);

            for (auto v : vertices)
            {
                seeds.push_back(v); // put vertex in seeds vector
                // mark vertex and its neighbors as visited
                mark_vertex_neighborhood(v, bcore_g, marked);
            }
        }
        ++i;
    }


    if (i >= bcore_g.get_nb_vertices() && seeds.size() < m_p.nb_of_seeds())
    {
        std::cout << "[WARNING] Nise::seeding_by_spread_hubs: it was not "
            "possible to find all seeds. Expected: " << m_p.nb_of_seeds() <<
            ". Found: " << seeds.size() << "\n";
    }

    return seeds;
}


std::vector<unsigned int> Nise::seeding_phase(const Graph &bcore_g) const
{
    std::cout << "\t\tseeding phase...\n";

    // TODO: implement graclus.

    return seeding_by_spread_hubs(bcore_g);
}


void Nise::shared_clst_insertion(const Cluster &clst)
{
    std::lock_guard<std::mutex> locker(m_mutex);
    m_clusters.insert(clst);
}