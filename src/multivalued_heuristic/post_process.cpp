//
// Created by crl on 21/02/2026.
//


#include "multivalued_heuristic/post_process.h"

#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>


bool epsilon_dominates(const std::vector<float>& v1, const std::vector <float>& v2, const EPS& eps) {
    return v1[0] <= (1 + eps[0]) * v2[0] and v1[1] <= (1 * eps[1]) * v2[1];
}


MultiValuedHeuristic PostProcess::operator()(const MultiValuedHeuristic& mvh, size_t target,
    const EPS& eps) {
    MultiValuedHeuristic processed_mvh;
    processed_mvh.resize(mvh.size());
    for (size_t i = 0; i < mvh.size(); ++i) {
        if (mvh[i].empty()) {
            continue;
        }
        processed_mvh[i].push_back(mvh[i][0]);
        std::vector<float> last_added = processed_mvh[i][0];
        for (int j = 1; j < mvh[i].size(); j++) {
            if (epsilon_dominates(last_added, mvh[i][j], eps)) {
                processed_mvh[i][processed_mvh.size()-1][1] = mvh[i][j][1];
            } else {
                processed_mvh[i].push_back(mvh[i][j]);
                last_added = processed_mvh[i][processed_mvh[i].size()-1];
            }
        }
        if (processed_mvh[i].size() > 1) {
            std::vector<std::vector<float>> result = {};
            for (int j = 0; j < processed_mvh[i].size() - 1; j++) {
                std::vector staired_mvh = {processed_mvh[i][j][0], processed_mvh[i][j + 1][1]};
                result.push_back(staired_mvh);
            }
            processed_mvh[i] = result;
        }
    }

    std::string e1string = std::to_string(eps[0]);
    std::string e2string = std::to_string(eps[1]);
    std::vector<std::string> dceomp_temp;
    e1string = boost::split(dceomp_temp, e1string, boost::is_any_of("."))[1].substr(0, 3);
    e2string = split(dceomp_temp, e2string, boost::is_any_of("."))[1].substr(0, 3);

    std::stringstream ss;
    ss << target << "_" << e1string << "_" << e2string << "_post_process_mvh.txt";

    std::ofstream PlotOutput(ss.str());

    for (int i = 0; i < processed_mvh.size(); i++) {
        for (int j=0; j < processed_mvh[i].size(); j++) {
            PlotOutput <<  i << "\t" << processed_mvh[i][j][0] << "\t" << processed_mvh[i][j][1] << std::endl;
        }
    }

    PlotOutput.close();
    return processed_mvh;
}


