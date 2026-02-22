//
// Created by crl on 21/02/2026.
//


#include "multivalued_heuristic/post_process.h"

#include <fstream>

MultiValuedHeuristic
PostProcess::operator()(const MultiValuedHeuristic& mvh,
                        const EPS& eps,
                        const std::string& output_file)
{
    MultiValuedHeuristic processed_mvh(mvh.size());

    const float e0 = 1.0f + eps[0];
    const float e1 = 1.0f + eps[1];

    for (size_t i = 0; i < mvh.size(); ++i) {
        if (mvh[i].empty())
            continue;

        auto& out = processed_mvh[i];
        out.reserve(mvh[i].size());

        out.push_back(mvh[i][0]);

        for (size_t j = 1; j < mvh[i].size(); ++j) {

            auto& last = out.back();
            const auto& cur = mvh[i][j];

            if (last[0] <= e0 * cur[0] &&
                last[1] <= e1 * cur[1])
            {
                last[1] = cur[1];
            }
            else {
                out.push_back(cur);
            }
        }

        if (out.size() > 1) {
            std::vector<std::vector<float>> stair;
            stair.reserve(out.size() - 1);

            for (size_t j = 0; j + 1 < out.size(); ++j) {
                stair.push_back({ out[j][0], out[j+1][1] });
            }

            out.swap(stair);
        }
    }

    std::ofstream PlotOutput(output_file);

    for (size_t i = 0; i < processed_mvh.size(); ++i)
        for (const auto& p : processed_mvh[i])
            PlotOutput << i << "\t"
                       << p[0] << "\t"
                       << p[1] << "\n";

    return processed_mvh;
}
