/*
 * File: nise_parameters.cpp
 * Author: Guilherme O. Chagas
 *
 * @brief NISE-SPR [1] parameters implementation.
 * 
 * (I'm sorry for my bad english xD)
 *
 * Created on July 21, 2019, 02:45 PM
 * 
 * References:
 * [1] J. J. Whang, D. F. Gleich and I. S. Dhillon. Overlapping community
 * detection using neighborhood-inflated seed expansion, IEEE Transactions on
 * Knowledge and Data Engineering 28(5) (2016) p. 1272-1284.
 */

#include "../headers/nise_parameters.hpp"
#include <experimental/filesystem>


/////////////////////////////// Helper functions ///////////////////////////////

namespace
{

/**
 * @brief
 * @param
 * @return
 */
bool is_number(const std::string &str)
{
    try
    {
        std::stod(str);
    }
    catch(...)
    {
        return false;
    }
    return true;
}

} // anonymous namespace

////////////////////////////////////////////////////////////////////////////////


double Nise_parameters::alpha() const
{
    return m_flag_val_map.at("-a");
}


double Nise_parameters::epsilon() const
{
    return m_flag_val_map.at("-e");
}


unsigned int Nise_parameters::nb_of_seeds() const
{
    return m_nb_of_seeds;
}


std::string Nise_parameters::get_graph_path() const
{
    return m_graph_path;
}


bool Nise_parameters::set_parameters(const int argc, char** argv)
{
    if (argc < 5 || argc > 9 || (argc - 1) % 2 != 0) // argc - 1 must be even
    {
        std::cerr << "[ERROR] Wrong number of parameters.\n";
        return false;
    }

    int argv_i = 1; // argv array index (even is flag and odd is value)
    // flag "-f" is mandatory
    if (std::string(argv[argv_i]) != "-f")
    {
        std::cerr << "[ERROR] Flag -f is mandatory.\n";
        return false;
    }

    ++argv_i; // even index to get the value
    if (!set_graph_path(argv[argv_i]))
    {
        std::cerr << "[ERROR] Graph file does not exists.\n";
        return false;
    }

    ++argv_i; // odd index to get flag
    // flag "-s" is also mandatory, the remaning is optinal
    if (std::string(argv[argv_i]) != "-s")
    {
        std::cerr << "[ERROR] Flag -s is mandatory.\n";
        return false;
    }

    ++argv_i; // even index to get the value
    if (!set_nb_of_seeds(argv[argv_i]))
    {
        std::cerr << "[ERROR] Invalid number of seeds.\n";
        return false;
    }

    // remaing flags and values
    while ((argv_i += 2) < argc)
    {
        if (!set(argv[argv_i - 1], argv[argv_i]))
        {
            std::cerr << "[ERROR] Wrong parameter.\n";
            return false;
        }
    }

    return true;
}


/////////////////////////////// private methods ////////////////////////////////


bool Nise_parameters::set(const std::string &flag, const std::string &val_str)
{
    auto it = m_flag_val_map.find(flag);
    if (it == m_flag_val_map.end()) // checks if the flag is valid
    {
        return false;
    }

    if (!is_number(val_str)) // checks whether the value is a valid number
    {
        return false;
    }

    double val_d = std::stod(val_str);
    if (val_d < 0 || val_d > 1) // checks whether the value is in the range
    {
        return false;
    }

    (*it).second = val_d;

    return true;
}


bool Nise_parameters::set_nb_of_seeds(const std::string &val_str)
{
    if (!is_number(val_str)) // checks whether the value is a valid number
    {
        return false;
    }

    int val_i = std::stoi(val_str);
    if (val_i < 1) // checks whether the value is in the range
    {
        return false;
    }

    m_nb_of_seeds = static_cast<unsigned int>(val_i);

    return true;
}


bool Nise_parameters::set_graph_path(const std::string &path)
{
    if (!std::experimental::filesystem::exists(path))
    {
        return false;
    }
    m_graph_path = path;
    return true;
}