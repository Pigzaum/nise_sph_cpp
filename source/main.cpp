/*
 * File: main.cpp
 *
 * @brief A C++ implementation of the Neighborhood-Inflated Seed Expansion with
 * seeds by the Spread Hubs method (NISE-SPH) [1]. See the README.md file for
 * compiling and run instructions.
 * @author Guilherme Oliveira Chagas (guilherme.o.chagas[a]gmail.com)
 * @disclaimer Note that I am not a NISE-SPH author, so this NISE-SPH version
 * may has errors and/or discrepancies with the actual Whang, Gleich and Dhillon
 * [1] NISE-SPH algorithm.
 * @date This file was created on July 21, 2019, 02:07 PM.
 * @acknowledgment A special thanks to Ph.D. Luis Antonio Nogueira Lorena and
 * Ph.D. Rafael Duarte Coelho dos Santos.
 * @copyright GNU General Public License.
 *
 * References:
 * [1] J. J. Whang, D. F. Gleich and I. S. Dhillon. Overlapping community
 * detection using neighborhood-inflated seed expansion, IEEE Transactions on
 * Knowledge and Data Engineering 28(5) (2016) p. 1272-1284.
 */


#include "../headers/nise_parameters.hpp"
#include "../headers/nise.hpp"


int main(int argc, char** argv)
{
    Nise_parameters params;

    if (!params.set_parameters(argc, argv))
    {
        std::cerr << "Please, look at README.md file.\n";
        return EXIT_FAILURE;
    }

    Graph g(params.get_graph_path());
    Nise nise(g, params);
    nise.execute();
    nise.write_clustering();

    return EXIT_SUCCESS;
}