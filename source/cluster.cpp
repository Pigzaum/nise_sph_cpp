/*
 * File: cluster.cpp
 * Author: Guilherme O. Chagas
 *
 * @brief Implementation of Cluster class.
 * 
 * (I'm sorry for my bad english xD)
 *
 * Created on July 23, 2018, 03:11 PM
 * 
 * References:
 * [1] A. Lancichinetti, S. Fortunato and J. Kertesz. Detecting the overlapping 
 * and hierarchical community structure in complex networks, New Journal of 
 * Physics 11 (2009) 033015: 18p.
 */


#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include "../headers/cluster.hpp"


///////////////////////// non-member friend functions //////////////////////////


std::ostream& operator<<(std::ostream& out, const Cluster &clst)
{
    out << "{";
    std::string sep = "";
    for (auto v : clst.m_vertices_set)
    // for (auto v : clst.m_vertices_vector)
    {
        out << sep << v;
        sep = ", ";
    }
    out << "}";
    return out;
}


bool operator==(const Cluster &lhs, const Cluster &rhs)
{
    if (lhs.m_vertices_set.size() != rhs.m_vertices_set.size())
    {
        return false;
    }

    for (auto v : lhs.m_vertices_set)
    {
        if (!rhs.contains(v))
        {
            return false;
        }
    }

    return true;
}


bool operator!=(const Cluster &lhs, const Cluster &rhs)
{
    return !(lhs == rhs);
}


Cluster operator+(const Cluster &lhs, const Cluster &rhs)
{
    assert(&lhs.m_graph == &rhs.m_graph); // must be clusters of same graph
    Cluster c(lhs.m_graph);
    c += lhs;
    c += rhs;
    return c;
}


////////////////////////////////////////////////////////////////////////////////


Cluster::Cluster(const Graph &g) : 
    m_graph(g),
    m_updated_degrees(false),
    m_internal_degree(0),
    m_external_degree(0)
{}


Cluster& Cluster::operator=(const std::initializer_list<unsigned int> &vertices)
{
    std::for_each(std::begin(vertices), std::end(vertices), 
        std::bind(std::mem_fun(&Cluster::insert), this, std::placeholders::_1));
    return *this;
}


Cluster& Cluster::operator+=(const Cluster &rhs)
{
    assert(&this->m_graph == &rhs.m_graph); // must be clusters of same graph
    std::for_each(std::begin(rhs), std::end(rhs), 
        std::bind(std::mem_fn(&Cluster::insert), this, std::placeholders::_1));
    return *this;
}


std::unordered_set<unsigned int>::iterator Cluster::begin()
{
    return m_vertices_set.begin();
}


// std::vector<unsigned int>::iterator Cluster::begin()
// {
//     return m_vertices_vector.begin();
// }


std::unordered_set<unsigned int>::const_iterator Cluster::begin() const
{
    return m_vertices_set.cbegin();
}


// std::vector<unsigned int>::const_iterator Cluster::begin() const
// {
//     return m_vertices_vector.cbegin();
// }


std::unordered_set<unsigned int>::iterator Cluster::end()
{
    return m_vertices_set.end();
}


std::unordered_set<unsigned int>::const_iterator Cluster::end() const
{
    return m_vertices_set.cend();
}



bool Cluster::contains(const unsigned int v) const
{
    assert(v < m_graph.get_nb_vertices());
    return m_vertices_set.find(v) != m_vertices_set.end();
}


bool Cluster::empty() const
{
    return m_vertices_set.empty();
}


std::pair<
    std::unordered_set<unsigned int>::const_iterator, 
    std::unordered_set<unsigned int>::const_iterator
    > 
    Cluster::get_external_adj_vertices() const
{
    return std::make_pair(m_ext_adj_vertices.cbegin(), 
        m_ext_adj_vertices.cend());
}


unsigned int Cluster::get_external_degree() const
{
    return m_external_degree;
}


unsigned int Cluster::get_internal_degree() const
{
    return m_internal_degree;
}


unsigned int Cluster::get_volume() const
{
    return m_internal_degree + m_external_degree;
}


bool Cluster::insert(const unsigned int v)
{
    assert(v < m_graph.get_nb_vertices());

    if (m_vertices_set.insert(v).second)
    {
        update_degrees(v, vertex_operation::insertion);
        return true;
    }
    
    return false;
}


bool Cluster::is_an_external_adj_vtx(const unsigned int v) const
{
    assert(v < m_graph.get_nb_vertices());
    return m_ext_adj_vertices.find(v) != m_ext_adj_vertices.end();
}


bool Cluster::remove(const unsigned int v)
{
    assert(v < m_graph.get_nb_vertices());

    if (contains(v))
    {
        m_vertices_set.erase(v);
        update_degrees(v, vertex_operation::removal);
        return true;
    }
    
    return false;
}


std::unordered_set<unsigned int>::const_iterator
    Cluster::remove(const std::unordered_set<unsigned int>::const_iterator it)
{
    if (contains(*it))
    {
        auto itr = m_vertices_set.erase(it);
        update_degrees(*it, vertex_operation::removal);
        return itr;
    }

    return m_vertices_set.end();
}


unsigned int Cluster::size() const
{
    return m_vertices_set.size();
}


void Cluster::compute_degrees()
{
    m_internal_degree = 0;
    m_external_degree = 0;

    for (auto v : m_vertices_set)
    // for (auto v : m_vertices_vector)
    {
        auto adj_list = m_graph.adj_list_of_vtx(v);

        for (auto it = adj_list.first; it != adj_list.second; ++it)
        {
            if (contains(*it))
            {
                ++m_internal_degree;
            }
            else
            {
                ++m_external_degree;
            }
        }
    }

    m_updated_degrees = true;
}


void Cluster::update_adjacency(const unsigned int v, const vertex_operation op)
{
    switch (op)
    {
        default:
        {
            std::cerr << "Cluster::update_adjacency: invalid vtx operation!\n";
            exit(EXIT_FAILURE);
        }
        case vertex_operation::insertion :
        {
            m_ext_adj_vertices.insert(v);
            break;
        }
        case vertex_operation::removal :
        {
            if (m_ext_adj_vertices.find(v) != m_ext_adj_vertices.end())
            {
                m_ext_adj_vertices.erase(v);
            }
            break;
        }
    }
}


void Cluster::update_degrees(const unsigned int v, const vertex_operation op)
{
    unsigned int e = static_cast<unsigned int>(op); // insetion = 0, removal = 1
    auto adj_list = m_graph.adj_list_of_vtx(v);
    bool vtx_has_internal_adj = false;
    for (auto it_v = adj_list.first; it_v != adj_list.second; ++it_v)
    {
        const unsigned int u = *it_v;
        if (contains(u))
        {
            m_external_degree += 1 * std::pow(-1, e + 1);
            // each internal edge is considered twice
            m_internal_degree += 2 * std::pow(-1, e);
            vtx_has_internal_adj = true;
        }
        else
        {
            m_external_degree += 1 * std::pow(-1, e);
            if (op == vertex_operation::removal)
            {
                /* for v removal, it is necessary to check whether u has another
                adjacency inside the cluster. If it has, then u won't be removed
                from 'm_ext_adj_vertices' container. If it has not, then u will
                be removed from 'm_ext_adj_vertices' container. */
                bool has_another_adj = false;
                auto adj_l_u = m_graph.adj_list_of_vtx(u);
                for (auto it_u = adj_l_u.first; it_u != adj_l_u.second; ++it_u)
                {
                    if (contains(*it_u))
                    {
                        has_another_adj = true;
                        break;
                    }
                }
                if (!has_another_adj)
                {
                    update_adjacency(u, op);
                }
            }
            else
            {
                /* for insertion no need to check u adjacency because the
                'm_ext_adj_vertices' container won't keep repeated vertices */
                update_adjacency(u, op);
            }
        }
    }

    /* if v has at least one adjacency with a vertex inside cluster, then it is
    necessary to update the 'm_ext_adj_vertices' container. If v was removed
    from cluster, then it will be inserted in 'm_ext_adj_vertices'. If v was
    inserted in cluster, then it will removed from 'm_ext_adj_vertices'. */
    if (vtx_has_internal_adj)
    {
        update_adjacency(v, (op == vertex_operation::removal ? // invert
            vertex_operation::insertion : vertex_operation::removal));
    }
}