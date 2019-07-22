/*
 * File: cluster.hpp
 * Author: Guilherme O. Chagas
 *
 * @brief Header of cluster class used in the Lancichinetti, Fortunato and
 * Kertesz [1] algorithm.
 *
 * TODO: used this class in all project
 * 
 * (I'm sorry for my bad english xD)
 *
 * Created on July 23, 2018, 03:03 PM
 * 
 * References:
 * [1] A. Lancichinetti, S. Fortunato and J. Kertesz. Detecting the overlapping 
 * and hierarchical community structure in complex networks, New Journal of 
 * Physics 11 (2009) 033015: 18p.
 */

#ifndef CLUSTER_HPP
#define CLUSTER_HPP


#include <cassert>
#include <unordered_set>
#include "./graph.hpp"


/**
* @brief TODO: consider to put it somewhere else.
*/
class Cluster
{

public:

    /**
     * Disabled default constructor.
    */
    Cluster() = delete;

    /**
     * Constructor.
     * @param Graph: input graph reference.
    */
    Cluster(const Graph &g);

    /**
     * @brief Default destructor.
    */
    ~Cluster() = default;

    /**
     * @brief Overloaded << ostream operator. Prints all elements as ostream 
     * standart.
     * @param std::ostream&: standart.
     * @param const Cluster&: cluster that the elements will be printed.
     * @return std::ostream&: standart.
    */
    friend std::ostream& operator<<(std::ostream& out, const Cluster &clst);

    /**
     * @brief Overloaded equality operator.
     * @param const Cluster &: left hand side cluster.
     * @param const Cluster &: right hand side cluster.
     * @return bool: true if equal, false otherwise.
    */
    friend bool operator==(const Cluster &lhs, const Cluster &rhs);

    /**
     * @brief Overloaded inequality operator.
     * @param const Cluster &: left hand side cluster.
     * @param const Cluster &: right hand side cluster.
     * @return bool: true if equal, false otherwise.
    */
    friend bool operator!=(const Cluster &lhs, const Cluster &rhs);

    /**
     * @brief 
     * @param
     * @return Cluster: new cluster.
    */
    friend Cluster operator+(const Cluster &lhs, const Cluster &rhs);

    /**
     * @brief Assign to cluster the content of the initializer list. Each list 
     * element is the label of a vertex that belongs to the cluster.
     * @param const std::initializer_list<unsigned int>: vertices labels to be 
     * inserted in cluster.
     * @return Cluster&: cluster reference.
    */
    Cluster& operator=(const std::initializer_list<unsigned int> &vertices);

    /**
     * @brief 
     * @param
     * @return Cluster&: cluster reference.
    */
    Cluster& operator+=(const Cluster &rhs);

    /**
     * @brief Get the begin iterator of the unordered set that holds cluster's 
     * vertices.
     * @return std::unordered_set<unsigned int>::iterator: begin iterator. 
    */
    std::unordered_set<unsigned int>::iterator begin();
    // std::vector<unsigned int>::iterator begin();

    /**
     * @brief Get the begin const iterator of the unordered set that holds 
     * cluster's vertices.
     * @return std::unordered_set<unsigned int>::iterator: begin const iterator.
    */
    std::unordered_set<unsigned int>::const_iterator begin() const;
    // std::vector<unsigned int>::const_iterator begin() const;

    /**
     * @brief Get the end iterator of the unordered set that holds cluster's 
     * vertices.
     * @return std::unordered_set<unsigned int>::iterator: end iterator.
    */
    std::unordered_set<unsigned int>::iterator end();
    // std::vector<unsigned int>::iterator end();

    /**
     * @brief Get the end const iterator of the unordered set that holds 
     * cluster's vertices.
     * @return std::unordered_set<unsigned int>::iterator: end const iterator.
    */
    std::unordered_set<unsigned int>::const_iterator end() const;
    // std::vector<unsigned int>::const_iterator end() const;

    /**
     * @brief Check if vertex v is contained in this cluster.
     * @param unsigned int: .
     * @return bool: true if it is contained, false otherwise.
    */
    bool contains(const unsigned int v) const;

    /**
     * @brief Check if the cluster is empty. The empty() method of the 
     * std::unordered_set "m_vertices_set" is called.
     * @return bool: true if cluster is empty, false otherwise.
    */
    bool empty() const;

    /**
     * @brief Get the begin and the end const iterators of the vector that holds
     * the vertices that are not contained in the cluster and have at least one 
     * adjacent vertex that is inside the cluster.
     * @return std::pair: pair of the begin and the end const iterators.
    */
    std::pair<
        std::unordered_set<unsigned int>::const_iterator,
        std::unordered_set<unsigned int>::const_iterator
        >
    get_external_adj_vertices() const;

    /**
     * @brief Get the external degree of the cluster, i. e., the number of 
     * adjacencies of vertices from this cluster with vertices not contained in 
     * this cluster. In other words, the number of inter-cluster edges that have 
     * exactly one end-point in this cluster. 
     * @return unsigned int: external degree of this cluster.
    */
    unsigned int get_external_degree() const;

    /**
     * @brief Get the number of adjacencies between vertices belonging to this 
     * cluster. Note that this number is twice the number of intra-cluster 
     * edges. 
     * The internal degree can also be seen as the sum of vertices degrees when 
     * considering only adjacencies of vertices contained in this cluster.
     * @return unsigned int: internal degree of this cluster.
    */
    unsigned int get_internal_degree() const;

    /**
     * @brief Get the volume of the cluster, i.d., the sum of cluster's 
     * vertices' degree.
     * @return unsigned int: sum of internal and external degrees.
    */
    unsigned int get_volume() const;

    /**
     * @brief Insert the vertex v in this cluster. This vertex is inserted in 
     * the vertices set (unordered set m_vertices_set) and the flag of 
     * "m_contains" is turned true.
     * @param unsigned int: vertex index to be inserted.
     * @return bool:.
    */
    bool insert(const unsigned int v);

    /**
     * @brief Checks if the vertex passed as parameter is a external adjacent 
     * vertex of this cluster. In other words, this method return true if vertex
     * v is not contained in this cluster and it has at least one adjacency with
     * a vertex that belongs to this cluster. It returns false, otherwise.
     * @param const unsigned int: vertex.
     * @return bool: true if the vertex v is a external adjacent vertex and 
     * false otherwise.
    */
    bool is_an_external_adj_vtx(const unsigned int v) const;

    /**
     * @brief Remove the vertex v from this cluster. This vertex is removed from
     * the vertices set (unordered set m_vertices_set) and the flag of
     * "m_contains" is turned false.
     * @param const unsigned int
     * @return
     */
    bool remove(const unsigned int v);

    /**
     * @brief Remove the vertex v from this cluster. This vertex is removed from
     * the vertices set (unordered set m_vertices_set) and the flag of 
     * "m_contains" is turned false.
     * @param const std::unordered_set<unsigned int>::const_iterator: position
     * (const iterator) of the vertex to be removed.
     * @return std::unordered_set<unsigned int>::const_iterator: next iterator
     * following the removed vertex. Returns end() if the element is not found.
    */
    std::unordered_set<unsigned int>::const_iterator
        remove(const std::unordered_set<unsigned int>::const_iterator it);

    /**
     * @brief Return the size of this clusters, i. e., the number of vertices 
     * belonging to it.
     * @return unsigned int: return the number of vertices contained in this 
     * cluster.
    */
    unsigned int size() const;

private:

    /**
     * @brief enumerator used for updating internal and external degrees.
    */
    enum vertex_operation {insertion, removal};

    /**
     * @brief Graph const reference. The graph adjacency matrix and adjacency 
     * list are used through this class to compute values and metrics.
    */
    const Graph &m_graph;

    /**
     * @brief Flag to control if internal and external degrees need to be 
     * updated. Whenever a vertex is added or removed, this flag is set to 
     * false.
    */
    bool m_updated_degrees;

    /**
     * @brief Internal degree of this cluster, i. e., the sum of vertices 
     * degrees belonging to this cluster considering only adjacencies between 
     * them.
    */
    unsigned int m_internal_degree;

    /**
     * @brief External degree of this cluster, i. e., the number of 
     * inter-cluster edges that have exactly one end-point inside this cluster. 
    */
    unsigned int m_external_degree;

    /**
     * @brief Cluster's vertices unordered set.
    */
    std::unordered_set<unsigned int> m_vertices_set;

    /**
     * @brief Cluster's adjacent vertices, i.e., vertices that are not contained
     * in the cluster but are adjacent to at least one vertex contained in 
     * cluster.
    */
    std::unordered_set<unsigned int> m_ext_adj_vertices;

    /**
     * @brief Compute cluster's internal and external degrees values. The 
     * attributes m_internal_degree and m_internal_degree are updated with those
     * values. The m_updated_degrees flag is set to true.
    */
    void compute_degrees();

    /**
     * @brief Update cluster's internal and external degrees values.
     * @param
     * @param
    */
    void update_degrees(const unsigned int v, const vertex_operation op);

    /**
     * @brief Update cluster's adjacent vertices.
     * @param
     * @param
    */
    void update_adjacency(const unsigned int v, const vertex_operation op);

};

#endif /* CLUSTER_HPP */