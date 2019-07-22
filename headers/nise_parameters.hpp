/*
 * File: nise_parameters.hpp
 * Author: Guilherme O. Chagas
 *
 * @brief NISE-SPH [1] parameters header.
 * 
 * (I'm sorry for my bad english xD)
 *
 * Created on July 21, 2019, 02:34 PM
 * 
 * References:
 * [1] J. J. Whang, D. F. Gleich and I. S. Dhillon. Overlapping community
 * detection using neighborhood-inflated seed expansion, IEEE Transactions on
 * Knowledge and Data Engineering 28(5) (2016) p. 1272-1284.
 */

#ifndef NISE_PARAMETERS_HPP
#define NISE_PARAMETERS_HPP

#include <iostream>
#include <map>
#include <string>


class Nise_parameters
{
public:

    /**
     * @brief Default constructor.
     */
    Nise_parameters() = default;

    /**
     * @brief Default destructor.
     */
    ~Nise_parameters() = default;

    /**
     * @brief Get the NISE-SPH [1] parameter alpha.
     * @return double: alpha.
     */
    double alpha() const;

    /**
     * @brief Get the NISE-SPH [1] parameter epsilon.
     * @return double: epsilon.
     */
    double epsilon() const;

    /**
     * @brief Get the NISE-SPH [1] parameter number of seeds.
     * @return unsigned int: number of seeds.
     */
    unsigned int nb_of_seeds() const;

    /**
     * @brief Get the input graph file path.
     * @return std::string: graph file path.
     */
    std::string get_graph_path() const;

    /**
     * @brief Set the parameters values from the default terminal user input.
     * @param int: number of arguments.
     * @param char**: arguments itself.
     * @return bool: true if all arguments are valid parameters and were 
     * correctly initialized. False if any input error was detected.
     */
    bool set_parameters(const int argc, char** argv);

private:

    /**
     * @brief number of seeds is graph-dependent.
     */
    unsigned int m_nb_of_seeds;

    /**
     * @brief
     */
    std::string m_graph_path;

    /**
     * @brief Mapping of flags to values.
     */
    std::map<std::string, double> m_flag_val_map =
    {
        // default values following [1]
        {"-a", 0.99},
        {"-e", 1e-4}
    };

    /**
     * @brief
     * @param
     * @param
     * @return
     */
    bool set(const std::string &flag, const std::string &val_str);

    /**
     * @brief
     * @param
     * @return
     */
    bool set_graph_path(const std::string &path);

    /**
     * @brief
     * @param
     * @return
     */
    bool set_nb_of_seeds(const std::string &val_str);
};

#endif /* NISE_PARAMETERS_HPP */