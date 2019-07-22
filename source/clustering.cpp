////////////////////////////////////////////////////////////////////////////////
/*
 * File: clusters_wrapper.cpp
 * Author: Guilherme O. Chagas
 *
 * @brief .
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on June 4, 2019, 02:59 PM
 * 
 * TODO: handle exceptions
 */
////////////////////////////////////////////////////////////////////////////////

#include "../headers/clustering.hpp"
#include <iostream>
#include <fstream>
#include <sstream>


Clustering::Clustering(const unsigned int nb_vertices) :
    m_id_upper_bound(0),
    m_v_clst(nb_vertices, std::unordered_set<unsigned int>())
{}


Clustering::Clustering(const Graph &g, const std::string &path) :
    m_id_upper_bound(0),
    m_v_clst(g.get_nb_vertices(), std::unordered_set<unsigned int>())
{
    std::ifstream f(path);
    std::istringstream content(std::string((std::istreambuf_iterator<char>(f)),
        (std::istreambuf_iterator<char>())));
    std::string line;
    while (std::getline(content, line))
    {
        unsigned int c_id = insert(Cluster(g));
        std::stringstream ss(line);
        std::string l; // vertex label
        while (ss >> l)
        {
            // return invalid id if graph vertices has no label
            unsigned int v_id = g.get_vtx_id(l);
            // if the graph vertices has no labels then convert l to integer
            if (v_id == Graph::invalid_vertex_id())
            {
                v_id = static_cast<unsigned int>(std::stoi(l));
            }
            insert_v_in_clst(v_id, c_id);
        }
    }
    f.close();
}


const Cluster& Clustering::operator[](const unsigned int id) const
{
    assert(id < m_id_upper_bound);
    return m_id_clst_map.at(id);
}


std::unordered_map<unsigned int, Cluster>::const_iterator 
    Clustering::begin() const
{
    return m_id_clst_map.cbegin();
}


std::unordered_map<unsigned int, Cluster>::const_iterator 
    Clustering::end() const
{
    return m_id_clst_map.cend();
}


bool Clustering::are_all_vertices_clustered() const
{
    for (unsigned int i = 0; i < m_v_clst.size(); ++i)
    {
        if (m_v_clst[i].size() == 0)
        {
            return false;
        }
    }
    return true;
}


const Cluster& Clustering::get_cluster(const unsigned int id)
{
    assert(id < m_id_upper_bound);
    return m_id_clst_map.at(id);
}


unsigned int Clustering::get_inter_size(const unsigned int c1_id,
    const unsigned int c2_id) const
{
    if (m_id_ovlp_clsts_map.at(c1_id).find(c2_id) ==
        m_id_ovlp_clsts_map.at(c1_id).end())
    {
        return 0;
    }

    if (m_id_ovlp_clsts_map.at(c1_id).at(c2_id) !=
        m_id_ovlp_clsts_map.at(c2_id).at(c1_id))
    {
        std::cerr << "Clustering::get_inter_size: different size!\n";
        exit(EXIT_FAILURE);
    }

    return m_id_ovlp_clsts_map.at(c1_id).at(c2_id);
}


unsigned int Clustering::get_nb_ovlp_clsts(const unsigned int c_id) const
{
    assert(c_id < m_id_upper_bound);
    return m_id_ovlp_clsts_map.at(c_id).size();
}


std::pair<
    std::unordered_map<unsigned int, unsigned int>::const_iterator,
    std::unordered_map<unsigned int, unsigned int>::const_iterator
    >
Clustering::get_ovlp_clsts(const unsigned int c_id)
{
    assert(c_id < m_id_upper_bound);
    return std::make_pair(m_id_ovlp_clsts_map.at(c_id).begin(),
        m_id_ovlp_clsts_map.at(c_id).end());
}


std::pair<
    std::unordered_set<unsigned int>::const_iterator,
    std::unordered_set<unsigned int>::const_iterator
    > 
Clustering::get_v_belonging(const unsigned int v) const
{
    assert(v < m_v_clst.size());
    return std::make_pair(m_v_clst[v].begin(), m_v_clst[v].end());
}


unsigned int Clustering::get_v_belonging_size(const unsigned int v) const
{
    assert(v < m_v_clst.size());
    return m_v_clst[v].size();
}


unsigned int Clustering::insert(const Cluster &c)
{
    unsigned int current_id = m_id_upper_bound++;
    
    m_id_clst_map.insert(std::make_pair(current_id, c));
    m_id_ovlp_clsts_map.insert({current_id,
        std::unordered_map<unsigned int, unsigned int>()});

    for (auto v : c)
    {
        insert_v_belonging(v, current_id);
    }

    return current_id;
}


unsigned int Clustering::insert(const Cluster &&c)
{
    unsigned int current_id = m_id_upper_bound++;
    m_id_clst_map.emplace(std::make_pair(current_id, c));
    m_id_ovlp_clsts_map.insert({current_id,
        std::unordered_map<unsigned int, unsigned int>()});

    for (auto v : c)
    {
        insert_v_belonging(v, current_id);
    }

    return current_id;
}


bool Clustering::insert_without_repetition(const Cluster &c)
{
    for (const auto &id_c : m_id_clst_map)
    {
        if (id_c.second == c)
        {
            return false;
        }
    }

    insert(c);
    return true;
}


bool Clustering::insert_v_in_clst(const unsigned int v, 
    const unsigned int c_id)
{
    assert(v < m_v_clst.size() && c_id < m_id_upper_bound);

    bool inserted = m_id_clst_map.at(c_id).insert(v);
    if (inserted)
    {
        for (auto oc_id : m_v_clst[v]) // update overlapping clst "lists"
        {
            insert_ovlp_clst(c_id, oc_id);
        }
        m_v_clst[v].insert(c_id);
    }

    return inserted;
}


void Clustering::remove_clst(const unsigned int c_id)
{
    assert(c_id < m_id_upper_bound);

    if (m_id_clst_map.find(c_id) == m_id_clst_map.end())
    {
        return;
    }

    for (auto v : m_id_clst_map.at(c_id))
    {
        m_v_clst[v].erase(c_id);
    }

    for (auto oc_id : m_id_ovlp_clsts_map.at(c_id))
    {
        remove_ovlp_clst(oc_id.first, c_id); // remove c from oc ovlp list
    }

    m_id_clst_map.erase(c_id);
    m_id_ovlp_clsts_map.erase(c_id);
}


bool Clustering::remove_v_from_clst(const unsigned int v, 
    const unsigned int c_id)
{
    assert(v < m_v_clst.size() && c_id < m_id_upper_bound);

    bool removed = m_id_clst_map.at(c_id).remove(v);
    if (removed)
    {
        m_v_clst[v].erase(c_id);
        for (auto oc_id : m_v_clst[v])
        {
            --m_id_ovlp_clsts_map.at(c_id).at(oc_id);
            --m_id_ovlp_clsts_map.at(oc_id).at(c_id);
            if (m_id_ovlp_clsts_map.at(c_id).at(oc_id) == 0 && 
                m_id_ovlp_clsts_map.at(oc_id).at(c_id) == 0)
            {
                remove_ovlp_clst(c_id, oc_id);
                remove_ovlp_clst(oc_id, c_id);
            }
        }
    }

    return removed;
}


std::unordered_set<unsigned int>::const_iterator
    Clustering::remove_v_from_clst(
        const std::unordered_set<unsigned int>::const_iterator itv,
        const unsigned int c_id)
{
    assert(*itv < m_v_clst.size() && c_id < m_id_upper_bound);

    if (!m_id_clst_map.at(c_id).contains(*itv))
    {
        return m_id_clst_map.at(c_id).end();
    }

    auto itr = m_id_clst_map.at(c_id).remove(itv); // iterator to return

    m_v_clst[*itv].erase(c_id);
    for (auto oc_id : m_v_clst[*itv])
    {
        --m_id_ovlp_clsts_map.at(c_id).at(oc_id);
        --m_id_ovlp_clsts_map.at(oc_id).at(c_id);
        if (m_id_ovlp_clsts_map.at(c_id).at(oc_id) == 0 &&
            m_id_ovlp_clsts_map.at(oc_id).at(c_id) == 0)
        {
            remove_ovlp_clst(c_id, oc_id);
            remove_ovlp_clst(oc_id, c_id);
        }
    }

    return itr;
}


void Clustering::reserve(const unsigned int size)
{
    m_id_clst_map.reserve(size);
    m_id_ovlp_clsts_map.reserve(size);
}


unsigned int Clustering::size() const
{
    return m_id_clst_map.size();
}


void Clustering::write_clusters_in_file(const std::string &path) const
{
    std::ofstream f(path, std::fstream::trunc);

    for (const auto &id_c : m_id_clst_map)
    {
        const Cluster &c = id_c.second;
        std::string sep = "";
        for (auto v : c)
        {
            f << sep << v;
            sep = " ";
        }
        f << "\n";
    }

    f.close();
}


/////////////////////////////// private methods ////////////////////////////////


bool Clustering::insert_ovlp_clst(const unsigned int c_id, 
    const unsigned int oc_id)
{
    assert(c_id < m_id_upper_bound && oc_id < m_id_upper_bound);

    if (m_id_ovlp_clsts_map.at(c_id).find(oc_id) ==
        m_id_ovlp_clsts_map.at(c_id).end() &&
        m_id_ovlp_clsts_map.at(oc_id).find(c_id) ==
        m_id_ovlp_clsts_map.at(oc_id).end()) // if not already inserted
    {
        // insert in both clusters list with 1 as the number of sharing vertices
        return m_id_ovlp_clsts_map.at(c_id).insert({oc_id, 1}).second &&
            m_id_ovlp_clsts_map.at(oc_id).insert({c_id, 1}).second; // inserted
    }
    else if (m_id_ovlp_clsts_map.at(c_id).find(oc_id) !=
        m_id_ovlp_clsts_map.at(c_id).end() &&
        m_id_ovlp_clsts_map.at(oc_id).find(c_id) !=
        m_id_ovlp_clsts_map.at(oc_id).end()) // if already inserted
    {
        // increment by 1 the number of sharing vertices
        ++m_id_ovlp_clsts_map.at(c_id).at(oc_id);
        ++m_id_ovlp_clsts_map.at(oc_id).at(c_id);
        return false; // not inserted
    }
    else // error: we have some problem here TODO: implement
    {
        std::cerr << "Error: Clustering::insert_ovlp_clst\n";
        std::cin.get();
        return false;
    }
}


bool Clustering::insert_v_belonging(const unsigned int v, 
    const unsigned int c_id)
{
    assert(v < m_v_clst.size());

    for (auto oc : m_v_clst[v])
    {
        insert_ovlp_clst(oc, c_id);
    }

    return m_v_clst[v].insert(c_id).second;
}


void Clustering::remove_ovlp_clst(const unsigned int c_id,
    const unsigned int oc_id)
{
    assert(c_id < m_id_upper_bound && oc_id < m_id_upper_bound);
    m_id_ovlp_clsts_map.at(c_id).erase(oc_id);
}