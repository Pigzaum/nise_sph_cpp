/*
 * File: graph.cpp
 * Author: Guilherme O. Chagas
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on July 03, 2017, 10:16 AM
 */


#include "../headers/graph.hpp"
#include <algorithm>
#include <cassert>
#include <fstream>
#include <map>
#include "../headers/utils.hpp"


/////////////////////////////// Helper functions ///////////////////////////////

namespace
{


/**
 * @brief Used to populate m_vertices_labels vector with default label. A 
 * default label is equal to the vertex's id.
*/
struct default_label_gen
{
public:
    default_label_gen(const unsigned int init):
        m_current(init)
    {}

    std::string operator() () 
    { 
        return std::to_string(m_current++); 
    }

private:
    unsigned int m_current;
};

/**
 * @brief .
 * @param:.
 * @param:.
 * @return:.
*/
void check_if_data_structure_is_being_used(Graph::data_structure ds)
{
    if (ds == Graph::data_structure::none)
    {
        std::cerr << "Graph: I have no data structure, you idiot! Aborting...\n";
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief .
 * @param:.
 * @param:.
 * @return:.
*/
void print_progress_bar(const unsigned int work, const unsigned int work_done)
{
    assert(work_done <= work);

    const static std::string updating_chars = {'|', '/', '-', '\\'};
    static std::string bar(78, ' ');
    static unsigned int bar_idx = 1;
    static unsigned int updating_chars_count = 0;
    static unsigned int call = 0;

    bar[0] = '[';
    bar[77] = ']';
    bool updated_bar = false;
    while (work_done >= (bar_idx * (work / 76)))
    {
        bar[bar_idx++] = '=';
        updated_bar = true;
    }

    if (!(call % 60000) || updated_bar) // magic number
    {
        char c = updating_chars[updating_chars_count % 4];
        ++updating_chars_count;
        std::cout << "\r" << std::flush << bar << " " << c;
    }

    if (work_done == work)
    {
        std::cout << "\n";
    }
    
    ++call;
}

/**
 * @brief .
 * @param:.
 * @param:.
 * @return:.
*/
void init_graph_from_file(Graph &g, const std::string &str_content)
{
    std::istringstream content(str_content);
    std::string line;
    unsigned int processed_size = 0;
    std::getline(content, line);
    processed_size += line.size() + 1;
    g.set_nb_vertices(std::stoi(line));

    while (std::getline(content, line))
    {
        std::istringstream tmp(line);
        unsigned int i, j;
        if (!(tmp >> i >> j))
        {
            std::cerr << "[ERROR] init_graph_from_file: invalid input file \n";
            exit(EXIT_FAILURE);
        }
        g.add_edge(i, j);
        processed_size += line.size() + 1;
        print_progress_bar(str_content.size(), processed_size);
    }
}

/**
 * @brief .
 * @param:.
*/
void increase_adj_mtx(std::vector<std::vector<bool>> &adj_mtx, 
    const unsigned int new_nb_vertices)
{
    assert(new_nb_vertices > adj_mtx.size());
    unsigned int diff = new_nb_vertices - adj_mtx.size();

    for (unsigned int i = 0; i < diff; ++i)
    {
        // add column
        adj_mtx.push_back(std::vector<bool>(adj_mtx.size() , false));
        for (unsigned int j = 0; j < adj_mtx.size(); ++j)
        {
            adj_mtx[j].push_back(false); // add one position to each column
        }
    }
}

} // anonymous namespace

////////////////////////////////////////////////////////////////////////////////


std::ostream& operator<<(std::ostream& out, const Graph::edge &e)
{
    return out << "(" << e.first << ", " << e.second << ")";
}


Graph::Graph() :
    m_digraph(false),
    m_nb_vertices(0),
    m_nb_edges(0),
    m_data_structure(data_structure::none),
    m_instance_path("")
{}


Graph::Graph(const unsigned int nb_of_vertices, 
    const data_structure ds /*= adj_mtx_and_list */,
    const bool digraph /*=false*/) :
    m_digraph(digraph),
    m_nb_vertices(nb_of_vertices),
    m_nb_edges(0),
    m_data_structure(ds),
    m_instance_path(""),
    m_adj_mtx( (ds == data_structure::adj_mtx) || 
        (ds == data_structure::adj_list_and_mtx) ? 
        std::vector<std::vector<bool>>(nb_of_vertices, 
            std::vector<bool>(nb_of_vertices, false)) : 
            std::vector<std::vector<bool>>(0)),
    m_adj_list((ds == data_structure::adj_list) || 
        (ds == data_structure::adj_list_and_mtx) ? nb_of_vertices : 0),
    m_vertices_labels(nb_of_vertices, "")
{
    assert(m_nb_vertices > 0);
}


Graph::Graph(const std::string &file_path, 
    const data_structure ds /*= adj_mtx_and_list */,
    const bool digraph /*= false*/) :
    m_digraph(digraph),
    m_nb_vertices(0),
    m_nb_edges(0),
    m_data_structure(ds),
    m_instance_path(file_path)
{
    create_data_structure_from_file(file_path);

    assert(m_nb_vertices > 0);

    for (auto row_it = m_adj_mtx.begin(); row_it != m_adj_mtx.end(); ++row_it)
    {
        assert((*row_it).size() > 0);
    }
}


unsigned int Graph::invalid_vertex_id()
{
    return s_invalid_vertex_id;
}


bool Graph::add_edge(const edge &e)
{
    return add_edge(e.first, e.second);
}


bool Graph::add_edge(const unsigned int v, const unsigned int u)
{
    assert(v < m_nb_vertices && u < m_nb_vertices);

    if (has_edge(v, u)) // check if edge already exists
    {
        return false;
    }

    ++m_nb_edges;

    add_edge_in_data_structure(v, u);

    if (!m_digraph) // if is undirected, then create (u, v) as well
    {
        add_edge_in_data_structure(u, v);
    }

    return true;
}


bool Graph::add_edge(const std::string &l1, const std::string &l2)
{
    return add_edge(get_vtx_id(l1), get_vtx_id(l2));
}


bool Graph::add_vtx(const std::string &label /* = "" */)
{
    if (!label.empty())
    {
        if (m_label_id_map.find(label) != m_label_id_map.end())
        {
            return false; // label already associated with a vertex
        }
        // else
        m_label_id_map[label] = m_nb_vertices;
    }

    m_vertices_labels.push_back(label);
    ++m_nb_vertices;

    switch (m_data_structure)
    {
        default:
        {
            std::cerr << "Graph::add_vtx: invalid graph data structure: " << 
                m_data_structure << "Aborting...\n";
            exit(EXIT_FAILURE);
        }
        case data_structure::adj_list :
        {
            m_adj_list.push_back(std::vector<unsigned int>());
            break;
        }
        case data_structure::adj_mtx :
        {
            increase_adj_mtx(m_adj_mtx, m_nb_vertices);
            break;
        }
        case data_structure::adj_list_and_mtx :
        {
            m_adj_list.push_back(std::vector<unsigned int>());
            increase_adj_mtx(m_adj_mtx, m_nb_vertices);
            break;
        }
    }

    return true;
}


std::pair<std::vector<unsigned int>::const_iterator, 
    std::vector<unsigned int>::const_iterator> 
    Graph::adj_list_of_vtx(unsigned int v) const
{
    check_if_data_structure_is_being_used(m_data_structure);

    if (m_data_structure == data_structure::adj_mtx)
    {
        std::cerr << "Graph: I'm using only an adjacency matrix data structure,"
            << "how I'm supposed to have an \"adjacency list\"? Aborting...\n";
        exit(EXIT_FAILURE);
    }

    return std::make_pair(m_adj_list[v].cbegin(), m_adj_list[v].cend());
}


std::pair<std::vector<unsigned int>::const_iterator, 
    std::vector<unsigned int>::const_iterator> 
    Graph::adj_list_of_vtx(const std::string &label) const
{
    return adj_list_of_vtx(get_vtx_id(label));
}


const std::vector<std::vector<bool>>& Graph::get_adj_mtx() const
{
    check_if_data_structure_is_being_used(m_data_structure);

    if (m_data_structure == data_structure::adj_list)
    {
        std::cerr << "Graph: I'm using only an adjacency list data structure, "
            << "how I'm supposed to have an adjacency matrix? Aborting...\n";
        exit(EXIT_FAILURE);
    }

    return m_adj_mtx;
}


double Graph::get_avg_degree() const
{
    unsigned int sum_of_degrees = 0;
    for (unsigned int i = 0; i < m_nb_vertices; ++i)
    {
        sum_of_degrees += get_vtx_degree(i);
    }
    return static_cast<double>(sum_of_degrees) / 
        static_cast<double>(m_nb_vertices);
}


unsigned int Graph::get_max_degree() const 
{
    unsigned int max_degree = 0;
    for (unsigned int i = 0; i < m_nb_vertices; ++i)
    {
        unsigned int v_degree = get_vtx_degree(i);
        if (v_degree > max_degree)
        {
            max_degree = v_degree;
        }
    }
    return max_degree;
}


unsigned int Graph::get_nb_edges() const
{
    return m_nb_edges;
}


unsigned int Graph::get_nb_vertices() const
{
    return m_nb_vertices;
}


unsigned int Graph::get_vtx_degree(const unsigned int v) const
{
    check_if_data_structure_is_being_used(m_data_structure);

    if (m_data_structure == data_structure::adj_mtx)
    {
        unsigned int i = 0;
        std::for_each(m_adj_mtx[v].cbegin(), m_adj_mtx[v].cend(), 
            [&](const bool adj)
            {
                if (adj)
                {
                    ++i;
                }
            });
        return i;
    }
    else
    {
        return m_adj_list[v].size();
    }
}


unsigned int Graph::get_vtx_degree(const std::string &label) const
{
    return get_vtx_degree(get_vtx_id(label));
}


unsigned int Graph::get_vtx_id(const std::string &label) const
{
    const auto it = m_label_id_map.find(label);

    if (it == m_label_id_map.end())
    {
        // invalid label (not found), then return invalid id
        return s_invalid_vertex_id;
    }

    return it->second;
}


std::string Graph::get_vtx_label(const unsigned int v) const
{
    assert(v < m_nb_vertices);
    return m_vertices_labels[v];
}


bool Graph::has_edge(const unsigned int v1, const unsigned int v2) const
{
    check_if_data_structure_is_being_used(m_data_structure);

    if (m_data_structure == data_structure::adj_list)
    {
        if (std::find(m_adj_list[v1].begin(), m_adj_list[v1].end(), v2) !=
            m_adj_list[v1].end()) // check if edge exists
        {
            return true;
        }
    }
    else if (m_adj_mtx[v1][v2])
    {
        return true;
    }

    return false;
}


bool Graph::remove_edge(const edge &e)
{
    return remove_edge(e.first, e.second);
}


bool Graph::remove_edge(const unsigned int v, const unsigned int u)
{
    assert(v < m_nb_vertices && u < m_nb_vertices);

    if (!has_edge(v, u))
    {
        return false;
    }

    --m_nb_edges;

    remove_edge_from_data_structure(v, u);

    if (!m_digraph) // if is undirected, then remove (u, v) as well
    {
        remove_edge_from_data_structure(u, v);
    }

    return true;
}


void Graph::set_nb_vertices(const unsigned int nb_vertices)
{
    if (nb_vertices == m_nb_vertices)
    {
        return;
    }

    m_nb_vertices = nb_vertices;
    m_vertices_labels.clear();
    m_vertices_labels.resize(m_nb_vertices);

    switch (m_data_structure)
    {
        default:
        {
            std::cerr << "Graph::set_nb_vertices: invalid graph data structure: " 
                << m_data_structure << "Aborting...\n";
            exit(EXIT_FAILURE);
        }
        case data_structure::adj_list :
        {
            m_adj_list.clear();
            m_adj_list.resize(m_nb_vertices, std::vector<unsigned int>());
            break;
        }
        case data_structure::adj_mtx :
        {
            m_adj_mtx.clear();
            m_adj_mtx.resize(m_nb_vertices, 
                std::vector<bool>(m_nb_vertices, false));
            break;
        }
        case data_structure::adj_list_and_mtx :
        {
            m_adj_list.clear();
            m_adj_list.resize(m_nb_vertices, std::vector<unsigned int>());
            m_adj_mtx.clear();
            m_adj_mtx.resize(m_nb_vertices, 
                std::vector<bool>(m_nb_vertices, false));
            break;
        }
    }
}


void Graph::set_vtx_label(const unsigned int v, const std::string &l)
{
    assert(v < m_nb_vertices);
    m_vertices_labels[v] = l;
    m_label_id_map.insert_or_assign(l, v);
}


bool Graph::try_set_vtx_label(const unsigned int v, const std::string &l)
{
    assert(v < m_nb_vertices);

    if (m_label_id_map.insert(std::make_pair(l, v)).second)
    {
        assert(v < m_nb_vertices);
        m_vertices_labels[v] = l;
        return true;
    }
    else 
    {
        return false;
    }
}


void Graph::to_csv_file(const std::string &output_path) const
{
    check_if_data_structure_is_being_used(m_data_structure);

    std::ofstream f;
    f.open(output_path, std::fstream::trunc);

    if (m_data_structure == data_structure::adj_mtx)
    {
        for (unsigned int i = 0; i < m_nb_vertices; ++i)
        {
            f << "\"" << i << "\"";
            for (unsigned int j = 0; j < m_nb_vertices; ++j)
            {
                if ((i != j) && m_adj_mtx[i][j])
                {
                    f << "," << "\"" << j << "\"";
                }
            }
            f << "\n";
        }
    }
    else
    {
        for (unsigned int i = 0; i < m_nb_vertices; ++i)
        {
            f << "\"" << i << "\"";
            auto adj_list = adj_list_of_vtx(i);
            for (auto it = adj_list.first; it != adj_list.second; ++it)
            {
                f << "," << "\"" << *it << "\"";
            }
            f << "\n";
        }
    }
}


void Graph::add_edge_in_data_structure(const unsigned int v, 
    const unsigned int u)
{
    switch (m_data_structure)
    {
        default:
        {
            std::cerr << "Graph::add_edge_in_data_structure: invalid graph data"
                " structure: " << m_data_structure << "Aborting...\n";
            exit(EXIT_FAILURE);
        }
        case data_structure::adj_list:
        {
            m_adj_list[v].push_back(u); // adjacencies list
            break;
        }
        case data_structure::adj_mtx:
        {
            m_adj_mtx[v][u] = true; // adjacencies matrix
            break;
        }
        case data_structure::adj_list_and_mtx:
        {
            m_adj_mtx[v][u] = true; // adjacencies matrix
            m_adj_list[v].push_back(u); // adjacencies list
            break;
        }
    }
}


void Graph::compute_nb_of_edges()
{
    m_nb_edges = 0; // reset the number of edges

    for (unsigned int i = 0; i < m_nb_vertices; ++i)
    {
        m_nb_edges += get_vtx_degree(i);
    }
    
    if (!m_digraph)
    {
        m_nb_edges /= 2;
    }
}


void Graph::create_adj_list()
{
    check_if_data_structure_is_being_used(m_data_structure);

    if (m_data_structure == data_structure::adj_list)
    {
        std::cerr << "Graph: I already have an adjacency list structure. Why "
            << "are you bothering me on creating another? Aborting...\n";
        exit(EXIT_FAILURE);
    }

    for (unsigned int i = 0; i < m_nb_vertices; ++i)
    {
        for (unsigned int j = 0; j < m_nb_vertices; ++j)
        {
            if (m_adj_mtx[i][j])
            {
                m_adj_list[i].push_back(j);
            }
        }
    }
}


void Graph::create_data_structure_from_file(const std::string &path)
{
    std::ifstream f(path);

    std::string content((std::istreambuf_iterator<char>(f)),
        (std::istreambuf_iterator<char>()));

    std::cout << "Generating graph...\n";
    init_graph_from_file(*this, content);
    f.close();
}


void Graph::init()
{
    check_if_data_structure_is_being_used(m_data_structure);

    if (m_data_structure == data_structure::adj_list)
    {
        std::cerr << "Graph: I can't initialize with the init() method because "
            << "I am not currently using an adjacency matrix as data structure "
            << "and the developer who made me is a lazy person :/. Aborting\n";
        exit(EXIT_FAILURE);
    }

    create_adj_list();
    compute_nb_of_edges();
}


void Graph::remove_edge_from_data_structure(const unsigned int v, 
        const unsigned int u)
{
    switch (m_data_structure)
    {
        default:
        {
            std::cerr << "Graph::remove_edge_from_data_structure: invalid graph"
                "data structure: " << m_data_structure << "Aborting...\n";
            exit(EXIT_FAILURE);
        }
        case data_structure::adj_list :
        {
            utils::remove_from_vector(m_adj_list[v], u);
            break;
        }
        case data_structure::adj_mtx:
        {
            m_adj_mtx[v][u] = false;
            break;
        }
        case data_structure::adj_list_and_mtx:
        {
            m_adj_mtx[v][u] = false;
            utils::remove_from_vector(m_adj_list[v], u);
            break;
        }
    }
}