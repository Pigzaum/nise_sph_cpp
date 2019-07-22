/*
 * File: nise.hpp
 * Author: Guilherme O. Chagas
 *
 * @brief Header of the Whang, Gleich and Dhillon [1] algorithm: 
 * Neighborhood-Inflated Seed Expansion (NISE-SPH). This implementation uses the
 * Spread Hubs method for the seeding step.
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on August 21, 2018, 09:35 PM
 * 
 * References:
 * [1] J. J. Whang, D. F. Gleich and I. S. Dhillon. Overlapping community 
 * detection using neighborhood-inflated seed expansion, IEEE Transactions on 
 * Knowledge and Data Engineering 28(5) (2016) p. 1272-1284.
 * [2] I. S. Dhillon, Y. Guan and B. Kulis. Weighted graph cuts without 
 * eigenvectors a multilevel approach, IEEE Transactions on Pattern Analysis and 
 * Machine Intelligence 29(11) (2007) p. 1944-1957.
 * [3] R. Andersen, F. Chung and K. Lang. Local Graph Partitioning using 
 * PageRank Vectors. In: Proceedings of the 47th Annual IEEE Symposium on 
 * Foundations of Computer Science. Berkeley, CA, USA: IEEE, 2006.
 * 
 * TODO: needs documentation.
 */

#ifndef NISE_ALGORITHM_HPP
#define NISE_ALGORITHM_HPP


#include <list>
#include <mutex>
#include <unordered_map>
#include "../headers/clustering.hpp"
#include "../headers/nise_parameters.hpp"


class Nise
{
public:

    /**
    * @brief Disabled default constructor.
    */
    Nise() = delete;

    /**
     * Disabled copy constructor.
     */
    Nise(const Nise &other) = delete;

    /**
    * Default move constructor.
    */
    Nise(Nise &&other) = delete;

    /**
    * Disabled assignment operator.
    */
    Nise& operator=(const Nise &other) = delete;

    /**
    * Default move assignment operator.
    */
    Nise& operator=(Nise &&other) = delete;

    /**
     * @brief Constructor: .
     * @param const Graph &:..
     * @param const parameters &:..
    */
    Nise(const Graph &g, const Nise_parameters &params);

    /**
     * @brief Constructor: .
     * @param const Graph &:..
    */
    Nise(const Graph &g, const unsigned int nb_seeds, const double alpha, 
        const double epsilon);

    /**
    * @brief Default Destructor.
    */
    ~Nise() = default;

    /**
    * @brief.
    * @param:.
    * @param:.
    * @param:.
    */
    void execute();

    /**
    * @brief.
    * @param:.
    * @param:.
    * @param:.
    */
    void execute(const unsigned int nb_seeds, const double alpha, 
        const double epsilon);

    /**
    * @brief.
    * @param:.
    */
    void write_clustering() const;

private:

    /**
    * @brief Graph const reference.
    */
    const Graph &m_graph;

    /**
    * @brief .
    */
    const Nise_parameters m_p;

    /**
    * @brief .
    */
    std::mutex m_mutex;

    /**
    * @brief .
    */
    bool m_executed = false;

    /**
    * @brief .
    */
    double m_execution_time = 0;

    /**
    * @brief .
    */
    Clustering m_clusters;

    /**
    * @brief (see [1]).
    * @param Graph: graph is passed by value for safety.
    * @return
    */
    std::tuple<std::vector<unsigned int>, std::vector<bool>, 
        std::vector<Graph::edge>> filtering_phase(Graph g);

    /**
    * @brief (see [1]).
    * @param .
    */
    void propagation_phase(const std::vector<Graph::edge> &bridges, 
        std::vector<bool> &contained_in_bcore);

    /**
    * @brief Seed expansion phase by personalized PageRank vectors (see [1,3]).
    * @param const unsigned int: seed vertex in which the expansion starts.
    * @param:.
    * @param:.
    * @return Cluster: Cluster with min conductance expanded from seed vertex.
    */
    Cluster seed_expansion_by_ppr(
        const Graph &bcore_g, 
        const std::vector<unsigned int> &bcore_v,
        const unsigned int seed);

    /**
    * @brief .
    * @param .
    * @param .
    * @param .
    */
    void seed_expansion_phase(const Graph &bcore_g, 
        const std::vector<unsigned int> &bcore_v,
        const std::vector<unsigned int> &seeds);

    /**
    * @brief .
    * @param .
    * @param .
    * @param .
    */
    void seed_expansion_thread_task(const Graph &bcore_g, 
        const std::vector<unsigned int> &bcore_v,
        const std::vector<unsigned int> &seeds, 
        const unsigned int thread_id);


    /**
    * @brief Seeding by spread hubs (see [1]).
    * @param const unsigned int: the # of seeds which is the # of clusters.
    * @return std::vector<unsigned int>: vector containing the seed vertices.
    */
    std::vector<unsigned int> 
        seeding_by_spread_hubs(const Graph &bcore_g) const;

    /**
    * @brief Seeding phase (see [1]).
    * @param const Wgd_algorithm::seeding_method: enum of the chosen seeding 
    * method (seeding by GRACLUS Centers or by Spread Hubs).
    * @param const unsigned int: number of seeds to be found.
    * @return std::vector<unsigned int>: vector containing the seed vertices.
    */
    std::vector<unsigned int> seeding_phase(const Graph &bcore_g) const;

    /**
     * @brief TODO.
     * @param const Cluster&:.
    */
    void shared_clst_insertion(const Cluster &clst);
};


#endif /* NISE_ALGORITHM_HPP */