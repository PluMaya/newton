//
// Created by crl on 15/02/2026.
//

#include "parsers/single_valued_heuristic_parser.h"

#include <fstream>
#include <iostream>
#include <boost/algorithm/string.hpp>


Heuristic SVHParser::parse_heuristic(const std::string& heuristic_path) {
    std::ifstream heuristic_infile(heuristic_path);

    if (!heuristic_infile) {
        std::cerr << "Error opening file: " << heuristic_path << std::endl;
        exit(1);
    }

    std::vector<std::vector<float>> heuristic;
    std::string heuristic_line;

    while (std::getline(heuristic_infile, heuristic_line)) {

        if (heuristic_line.empty())
            continue;

        std::vector<std::string> decomposed_heuristic_line;
        split(decomposed_heuristic_line, heuristic_line, boost::is_any_of("\t"));

        size_t state = std::stoul(decomposed_heuristic_line[0]);
        float h1 = std::stof(decomposed_heuristic_line[1]);
        float h2 = std::stof(decomposed_heuristic_line[2]);

        // Resize if needed
        if (state >= heuristic.size()) {
            heuristic.resize(state + 1);
        }

        heuristic[state] = {h1, h2};
    }

    return [heuristic](size_t index) -> std::vector<float> {
        return heuristic[index];
    };
}
