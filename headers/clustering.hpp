////////////////////////////////////////////////////////////////////////////////
/*
 * File: clustering.hpp
 * Author: Guilherme O. Chagas
 *
 * @brief Clustering class. This class is a wrapper of a set of clusters.
 * TODO: needs documentation
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on June 3, 2019, 05:45 PM
 * 
 * References:
 * [1] A. Lancichinetti, S. Fortunato and J. Kertesz. Detecting the overlapping
 * and hierarchical community structure in complex networks, New Journal of
 * Physics 11 (2009) 18pp.
 */

////////////////////////////////////////////////////////////////////////////////

#ifndef CLUSTERING_HPP
#define CLUSTERING_HPP

#include <unordered_map>
#include "./cluster.hpp"


class Clustering
{
public:

    /**
     * @brief Disabled default constructor.
    */
    Clustering() = delete;

    /**
     * @brief Contructor: creates an empty clustering. The number of vertices is
     * necessary because of the m_v_clst attribute.
     * @param const unsigned int: graph number of vertices.
    */
    explicit Clustering(const unsigned int nb_vertices);

    /**
     * @brief Constructor: creates a clustering from a file. This constructor is
     * used for create a ground-truth clustering
     * @param const Graph&: graph.
     * @param const std::string&: file path of clustering.
    */
    Clustering(const Graph &g, const std::string &file_path);

    /**
     * @brief .
    */
    ~Clustering() = default;

    /**
     * @brief .
     * @return:.
    */
    const Cluster& operator[](const unsigned int id) const;

    /**
     * @brief .
     * @return
    */
    std::unordered_map<unsigned int, Cluster>::const_iterator begin() const;

    /**
     * @brief .
     * @return
    */
    std::unordered_map<unsigned int, Cluster>::const_iterator end() const;

    /**
     * @brief .
     * @return
    */
    bool are_all_vertices_clustered() const;

    /**
     * @brief .
     * @param
     * @return
    */
    const Cluster& get_cluster(const unsigned int id);

    /**
     * @brief .
     * @param
     * @param
     * @return
    */
    unsigned int get_inter_size(
        const unsigned int c1_id, 
        const unsigned int c2_id) const;

    /**
     * @brief .
     * @param
     * @param
     * @return
    */
    unsigned int get_nb_ovlp_clsts(const unsigned int c_id) const;

    /**
     * @brief .
     * @param
     * @return
    */
    std::pair<
        std::unordered_set<unsigned int>::const_iterator,
        std::unordered_set<unsigned int>::const_iterator
        >
    get_v_belonging(const unsigned int v) const;

    /**
     * @brief .
     * @param
     * @return
    */
    unsigned int get_v_belonging_size(const unsigned int v) const;

    /**
     * @brief .
     * @param
     * @return
    */
    std::pair<
        std::unordered_map<unsigned int, unsigned int>::const_iterator,
        std::unordered_map<unsigned int, unsigned int>::const_iterator
        >
    get_ovlp_clsts(const unsigned int c_id);

    /**
     * @brief .
     * @param
     * @return unsigned int: id of the inserted cluster.
    */
    unsigned int insert(const Cluster &c);

    /**
     * @brief .
     * @param
     * @return unsigned int: id of the inserted cluster.
    */
    unsigned int insert(const Cluster &&c);

    /**
     * @brief Check if the cluster already exists before inserting.
     * @param const Cluster&: candidate cluster to be inserted.
     * @return bool: true if the cluster was inserted, false otherwise.
    */
    bool insert_without_repetition(const Cluster &c);

    /**
     * @brief .
     * @param
     * @param
     * @return
    */
    bool insert_v_in_clst(const unsigned int v, const unsigned int c_id);

    /**
     * @brief .
     * @param
    */
    void remove_clst(const unsigned int c_id);

    /**
     * @brief .
     * @param
     * @param
     * @return
    */
    bool remove_v_from_clst(const unsigned int v, const unsigned int c_id);

    /**
     * @brief .
     * @param
     * @param
     * @return
    */
    std::unordered_set<unsigned int>::const_iterator remove_v_from_clst(
        const std::unordered_set<unsigned int>::const_iterator itv,
        const unsigned int c_id);

    /**
     * @brief .
     * @param
    */
    void reserve(const unsigned int size);

    /**
     * @brief .
     * @return
    */
    unsigned int size() const;

    /**
    * @brief Write the clustering in a file. The format of the output file is 
    * the same used as input of GNMI [1] software (mutual). The file format
    * follows the example above:
    * // bof
    *       0 2 5 6
    *       1 3 4
    * // eof
    * Means that there are two clusters: in the first one there are vertices 0,
    * 2, 5 and 6 and in the second one there are vertices 1, 2 and 3. For more
    * details and the mutual software, see
    * https://sites.google.com/site/santofortunato/inthepress2.
    * @param const std::string&: ouput file path.
    */
    void write_clusters_in_file(const std::string &path) const;

private:

    /**
     * @brief Upper bound of clusters id in the map.
    */
    unsigned int m_id_upper_bound;

    /**
     * @brief clusters indices that each graph vertex is currently 
     * belonging to, i.e., the ith position of m_v_clst keeps the indices of the
     * clusters that vertex i is contained into.
    */
    std::vector<std::unordered_set<unsigned int>> m_v_clst;

    /**
     * @brief Unordered map of ids and clusters.
    */
    std::unordered_map<unsigned int, Cluster> m_id_clst_map;

    /**
     * @brief Mapping of the number of overlapping vertices that each pair of 
     * clusters share. The pair is the cluster id and the number of vertices
     * that they share.
    */
    std::unordered_map<unsigned int, 
        std::unordered_map<unsigned int, unsigned int>> m_id_ovlp_clsts_map;

    /**
     * @brief .
     * @param
     * @param
     * @return
    */
    bool insert_ovlp_clst(const unsigned int c_id, const unsigned int o_id);

    /**
     * @brief .
     * @param
     * @param
     * @return
    */
    bool insert_v_belonging(const unsigned int v, const unsigned int c_id);

    /**
     * @brief .
     * @param
     * @param
    */
    void remove_ovlp_clst(const unsigned int c_id, const unsigned int oc_id);

};

#endif /* CLUSTERING_HPP */