/*
 * File: graph.hpp
 * Author: Guilherme O. Chagas
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on June 30, 2017, 02:42 PM
 */

#ifndef MY_GRAPH_HPP
#define MY_GRAPH_HPP


#include <limits>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>


class Graph
{
public:

    /**
    * @brief .
    */
    struct edge
    {
        //
        unsigned int first;

        //
        unsigned int second;

        //
        edge() {}

        //
        edge(const std::pair<unsigned int, unsigned int> &e) :
            first(e.first),
            second(e.second) {}

        /**
         * @brief Overloaded << ostream operator. Prints both edges endpoint 
         * vertices as ostream standart.
         * @param std::ostream&: standart.
         * @param const edge&: edge that the endpoints will be printed.
         * @return std::ostream&: standart.
        */
        friend std::ostream& operator<<(std::ostream& out, const edge &e);
    };

    /**
    * @brief .
    */
    enum data_structure {adj_list, adj_mtx, adj_list_and_mtx, none};

    /**
    * Disabled assignment operator.
    */
    Graph& operator=(const Graph &other) = delete;

    /**
    * Default constructor.
    */
    Graph();

    /**
    * Default copy constructor.
    */
    Graph(const Graph &other) = default;

    /**
    * Default move constructor.
    */
    Graph(Graph &&other) = default;

    /**
    * Default move assignment operator.
    */
    Graph& operator=(Graph &&other) = default;

    /**
    * @brief Constructor: .
    * @param const unsigned int: .
    * @param enum: .
    * @param bool: .
    */
    Graph(const unsigned int nb_of_vertices, 
        const data_structure ds = data_structure::adj_list_and_mtx,
        const bool digraph = false);

    /**
    * Constructor: generates a graph from an input file.
    * @param const std::string &: graph input file path.
    * @param const data_structure ds: data structure option = adj_list_and_mtx.
    * @param const bool: whether is a digraph = false.
    */
    Graph(const std::string &file_path, 
        const data_structure ds = data_structure::adj_list_and_mtx,
        const bool digraph = false);

    /**
    * Destructor.
    */
    ~Graph() = default;

    /**
    * @brief.
    */
    static unsigned int invalid_vertex_id();

    /**
    * @brief Add the edge e (e.first, e.second) to this graph in the data(s) 
    * structure(s) according to "m_data_structure". If the graph is undirected, 
    * the edge (e.second, e.first) is added as well. Also, if this edge is 
    * successfully added, i.e., (e.first, e.second) was not previously added, 
    * this method return true. Otherwise, return false. In addition, the number 
    * of edges variable (m_nb_edges) is incremented.
    * @param const edge &(std::pair<unsigned int>): Graph::edge to be added.
    * @return bool: true if edge e does not exists and it was added, false if it
    * already exists and it was not added.
    */
    bool add_edge(const edge &e);

    /**
    * @brief Add the edge (v, u) to this graph in the data(s) structure(s) 
    * according to "m_data_structure". If the graph is undirected, the edge 
    * (u, v) is added as well. Also, if this edge is successfully added, i.e., 
    * (v, u) was not previously added, this method return true. Otherwise, return
    * false. In addition, the number of edges variable (m_nb_edges) is 
    * incremented.
    * @param const unsigned int: source vertex index.
    * @param const unsigned int: target vertex index.
    * @return bool: true if edge (v, u) does not exists and it was added, false 
    * if it already exists and it was not added.
    */
    bool add_edge(const unsigned int v, const unsigned int u);

    /**
    * @brief Add a edge using l1 and l2 labels. The edge (id1, id2) to this 
    * graph in the data(s) structure(s) according to "m_data_structure", where 
    * id1 and id2 are the ids of l1 and l2, respective. If the graph is 
    * undirected, the edge (id1, id2) is added as well. Also, if this edge is 
    * successfully added, i.e., (id1, id2) was not previously added, this method
    * return true. Otherwise, return false. In addition, the number of edges 
    * variable (m_nb_edges) is incremented.
    * @param const std::string &: source vertex label.
    * @param const std::string &: target vertex label.
    * @return bool: true if edge (id1, id2) does not exists and it was added, 
    * false if it already exists and it was not added.
    */
    bool add_edge(const std::string &l1, const std::string &l2);

    /**
    * @brief .
    * @param const std::string &: label.
    * @return bool: .
    */
    bool add_vtx(const std::string &label = "");

    /**
    * @brief Return the begin and end const iterators of vertex v adjacency list.
    * @param unsigned int: vertex index.
    * @return std::pair<begin, end>: a pair composed by the begin and end const 
    * iterators of the std::list adjacency list ot vertex v.
    */
    std::pair<std::vector<unsigned int>::const_iterator, 
        std::vector<unsigned int>::const_iterator> 
        adj_list_of_vtx(unsigned int v) const;

    /**
    * @brief Return the begin and end const iterators of vertex adjacency list.
    * @param const std::string&: vertex label.
    * @return std::pair<begin, end>: a pair composed by the begin and end const 
    * iterators of the std::list adjacency list of vertex v with the input 
    * label.
    */
    std::pair<std::vector<unsigned int>::const_iterator, 
        std::vector<unsigned int>::const_iterator> 
        adj_list_of_vtx(const std::string &label) const;

    /**
    * @brief get a const reference to the graph adjacency matrix.
    * @return const std::vector<std::vector<bool> >&: const reference to the 
    * graph adjacency matrix.
    */
    const std::vector<std::vector<bool> >& get_adj_mtx() const;

    /**
    * @brief Get the graph vertices average degree.
    * @return double: average degree of vertices of this graph.
    */
    double get_avg_degree() const;

    /**
    * @brief Get the maximum degree of a vertex of this graph.
    * @return unsigned int: maximum degree of a vertex of this graph.
    */
    unsigned int get_max_degree() const;

    /**
    * @brief .
    * @return
    */
    unsigned int get_nb_edges() const;

    /**
    * @brief
    * @return int: number of vertices in graph.
    */
    unsigned int get_nb_vertices() const;

    /**
    * @brief Get the degree (number of adjacencies) of the vertex
    * vtx_idx.
    * @return unsigned int: degree (number of adjacencies) of the
    * vertex vtx_idx.
    */
    unsigned int get_vtx_degree(const unsigned int vtx_idx) const;

    /**
    * @brief Get the degree (number of adjacencies) of the vertex
    * associated to label.
    * @param const std::string &: label (unique) of vertex.
    * @return unsigned int: degree (number of adjacencies) of the
    * vertex with label.
    */
    unsigned int get_vtx_degree(const std::string &label) const;

    /**
    * @brief TODO.
    * @param const unsigned int:.
    * @return bool:.
    */
    unsigned int get_vtx_id(const std::string &l) const;

    /**
    * @brief TODO.
    * @param const unsigned int:.
    * @return bool:.
    */
    std::string get_vtx_label(const unsigned int v) const;

    /**
    * @brief Return true if edge (v_src, v_dest) exists, i.e., vertex v_dest is 
    * adjacent to vertex v_src. Return false otherwise.
    * @param const unsigned int: source vertex index.
    * @param const unsigned int: destination vertex index.
    * @return bool: true if (v_src, v_dest) exists, false otherwise.
    */
    bool has_edge(const unsigned int v_src, const unsigned int v_dest) const;

    /**
    * @brief TODO.
    * @return std::string: TODO.
    */
    bool remove_edge(const edge &e);

    /**
    * @brief TODO.
    * @param const unsigned int: TODO.
    * @param const unsigned int: TODO.
    * @return const unsigned int: TODO.
    */
    bool remove_edge(const unsigned int v, const unsigned int u);

    /**
    * @brief TODO.
    * @return std::string: TODO.
    */
    bool remove_vertex(const unsigned int v);

    /**
    * @brief Set the number of vertices of graph to the "nb_vertices" input 
    * parameter. When this is done, the data structures are reset and the 
    * previous adjacencies are erased.
    * @param const unsigned int: number of vertices.
    */
    void set_nb_vertices(const unsigned int nb_vertices);

    /**
     * @brief Set label l to vertex v even v was already labeled
     * @param const unsigned int: vertex id.
     * @param const std::string&: label l.
     */
    void set_vtx_label(const unsigned int v, const std::string &l);

    /**
    * @brief If vertex v has not a label, then set the label l to the vertex v
    * (id). The label l is assigned to the vth position of the m_vertices_labels
    * vector. In addition, the pair (l, v) is inserted to the unordered_map
    * "m_label_id_map". This method was created, mainly, to initialize the graph
    * from a SLN, where are utilized labels instead of id in the input file.
    * @param const unsigned int: vertex id v.
    * @param const unsigned int: new vertex v label.
    * @return bool: true if the pair (l, v) was inserted in m_label_id_map and
    * false if vertex v was already labeled.
    */
    bool try_set_vtx_label(const unsigned int v, const std::string &l);

    /**
    * @brief Save the graph in a csv file.
    * @param std::string: output csv file path.
    */
    void to_csv_file(const std::string &output_file) const;

private:

    /**
    * @brief Invalid vertex id (a constant that is equal to the largest unsigned
    * int).
    */
    const static unsigned int s_invalid_vertex_id =
        std::numeric_limits<unsigned int>::infinity();

    /**
    * @brief
    */
    bool m_digraph;

    /**
    * @brief
    */
    unsigned int m_nb_vertices;

    /**
    * @brief.
    */
    unsigned int m_nb_edges;

    /**
    * @brief enumeration
    */
    data_structure m_data_structure;

    /**
    * @brief File path of the graph if it was gerated from an instance file. If 
    * not then it is equal to the default std::string value ("").
    */
    std::string m_instance_path;

    /**
    * @brief
    */
    std::vector<std::vector<bool> > m_adj_mtx;

    /**
    * @brief
    */
    std::vector<std::vector<unsigned int> > m_adj_list;

    /**
    * @brief
    */
    std::vector<std::string> m_vertices_labels;

    /**
    * @brief Label-vertex_id mapping.
    */
    std::unordered_map<std::string, unsigned int> m_label_id_map;

    /**
    * @brief TODO: adapt the project to use this matrix.
    */
    std::vector<std::vector<double> > m_weight_mtx;

    /**
    * @brief Add the edge (v,u) in this graph data structure according to 
    * "m_data_structure".
    * @param const unsigned int: vertex v.
    * @param const unsigned int: vertex u.
    */
    void add_edge_in_data_structure(const unsigned int v, const unsigned int u);

    /**
    * @brief
    */
    void compute_nb_of_edges();

    /**
    * @brief
    */
    void create_adj_list();

    /**
    * @brief
    * @param :.
    */
    void create_data_structure_from_file(const std::string &path);

    /**
    * @brief
    */
    void init();

    /**
    * @brief Remove the edge (v,u) from this graph data structure according to 
    * "m_data_structure".
    * @param const unsigned int: vertex v.
    * @param const unsigned int: vertex u.
    */
    void remove_edge_from_data_structure(const unsigned int v, 
        const unsigned int u);
};


#endif /* MY_GRAPH_HPP */
