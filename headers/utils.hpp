/*
 * File: utils.hpp
 * Author: Guilherme O. Chagas
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on July 03, 2017, 05:18 PM
 */


#ifndef UTILS_HPP
#define UTILS_HPP

#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <limits>
#include <sstream>
#include <vector>

namespace utils
{

    /**
    * @brief
    * @param
    */
    template <class C>
    void print_container(const C &c)
    {
        std::copy(c.begin(), c.end(), 
            std::ostream_iterator<typename C::value_type>(std::cout, " "));
    }

    /**
    * @brief.
    * @param
    * @param
    */
    template <class T>
    void remove_from_vector(std::vector<T> &v, const T &e)
    {
        v.erase(std::find(v.begin(), v.end(), e));
    }

} // namespace utils

#endif /* UTILS_HPP */