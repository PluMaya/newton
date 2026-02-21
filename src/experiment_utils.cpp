//
// Created by crl on 28/09/2024.
//

#include "experiment_utils.h"

#include "multivalued_heuristic/backward_search.h"
#include "multivalued_heuristic/colored_forward_search.h"
#include "solvers/apex.h"
#include "solvers/boa.h"
#include "solvers/shortest_path_heuristic_computer.h"

#include <fstream>
#include <iostream>

#include "multivalued_heuristic/boa_backward_search.h"
#include "multivalued_heuristic/closure.h"
#include "multivalued_heuristic/post_process.h"


void ExperimentUtils::run_boa_star(const AdjacencyMatrix& adjecency_matrix,
                                   const size_t& source, const size_t& target,
                                   const Heuristic& heuristic, const clock_t heuristic_runtime,
                                   const std::string& logging_file) {
    auto boa = BOAStar(adjecency_matrix);
    SolutionSet solutions;
    boa(source, target, heuristic, solutions, 30, logging_file);
    std::cout << "BOA"
        << "\t" << source << "\t" << target << "\t" << solutions.size()
        << "\t" << boa.num_expansion << "\t" << boa.num_generation << "\t"
        << boa.runtime << "\t" << heuristic_runtime << std::endl;
}

void ExperimentUtils::run_apex(const AdjacencyMatrix& adjecency_matrix,
                               const size_t& source, const size_t& target,
                               const EPS& eps, const Heuristic& heuristic, const long heuristic_duration,
                               const std::string& logging_file) {
    auto apex = ApexSearch(adjecency_matrix, eps);
    SolutionSet solutions;
    apex(source, target, heuristic, solutions, 300);
    std::cout << "Apex"
        << "\t" << source << "\t" << target << "\t" << solutions.size()
        << "\t" << apex.num_expansion << "\t" << apex.num_generation << "\t"
        << apex.runtime << "\t" << heuristic_duration << std::endl;
}

void ExperimentUtils::run_backward_search(
    const AdjacencyMatrix& adjecency_matrix, const size_t& source,
    const size_t& target, const EPS& eps, const Heuristic& source_to_target,
    const Heuristic& target_to_source,
    const std::string& logging_file) {
    auto backward_search = BackwardSearch(adjecency_matrix, eps);
    MultiValuedHeuristic mvh =
        backward_search(source, target, source_to_target, target_to_source);
    std::cout << "BS"
        << "\t" << source << "\t" << target << "\t" << mvh[target].size()
        << "\t" << backward_search.num_expansion << "\t"
        << backward_search.num_generation << "\t" << backward_search.runtime
        << std::endl;
}

void ExperimentUtils::run_boa_backward_search(const AdjacencyMatrix& adjecency_matrix,
    const size_t& target) {
    auto boa_backward_search = BOABackwardSearch(adjecency_matrix);
    MultiValuedHeuristic mvh = boa_backward_search(target);
    std::cout << "BOA-BS"
        << "\t" << target << "\t" << target << "\t" << mvh[target].size()
        << "\t" << boa_backward_search.num_expansion << "\t"
        << boa_backward_search.num_generation << "\t" << boa_backward_search.runtime
        << std::endl;
}

void ExperimentUtils::run_forward_search(
    const AdjacencyMatrix& adjecency_matrix, const size_t& source,
    const size_t& target, const EPS& eps, const MultiValuedHeuristic& mvh,
    const std::string& logging_file) {
    auto forward_search = ColoredForwardSearch(adjecency_matrix, eps);
    SolutionSet solutions;
    forward_search(source, target, mvh, solutions, logging_file);
    std::cout << "FS"
        << "\t" << source << "\t" << target << "\t" << solutions.size()
        << "\t" << forward_search.num_expansion << "\t"
        << forward_search.num_generation << "\t" << forward_search.runtime / CLOCKS_PER_SEC
        << std::endl;
}

void ExperimentUtils::single_run(AdjacencyMatrix& adjecency_matrix,
                                 size_t source, size_t target,
                                 const std::string& algorithm, const EPS& eps,
                                 std::vector<int> multi_sources,
                                 const Heuristic& svh_heuristic, const MultiValuedHeuristic& mvh,
                                 const std::string& logging_file) {
    if (multi_sources.empty()) {
        multi_sources.push_back(source);
    }
    if (algorithm == "BOA") {
        auto heuristic_duration = 0L;
        if (svh_heuristic == nullptr) {
            auto heuristic_start_time = std::clock();

            Heuristic new_svh_heuristic =
                ShortestPathHeuristicComputer::compute_ideal_point_heuristic(
                    target, adjecency_matrix);
            heuristic_duration = static_cast<long>(std::clock() - heuristic_start_time);
            for (auto multi_source : multi_sources) {
                run_boa_star(adjecency_matrix, multi_source, target, new_svh_heuristic, heuristic_duration,
                    logging_file);
            }
        } else {
            for (auto multi_source : multi_sources) {
                run_boa_star(adjecency_matrix, multi_source, target, svh_heuristic, heuristic_duration, logging_file);
            }
        }

    }
    else if (algorithm == "APEX") {
        auto heuristic_duration = 0L;

        if (svh_heuristic == nullptr) {
            auto heuristic_start_time = std::clock();

            Heuristic new_svh_heuristic =
                ShortestPathHeuristicComputer::compute_ideal_point_heuristic(
                    target, adjecency_matrix);
            heuristic_duration = static_cast<long>(std::clock() - heuristic_start_time);
            for (auto multi_source : multi_sources) {
                run_apex(adjecency_matrix, multi_source, target, eps, new_svh_heuristic, heuristic_duration, logging_file);
            }
        } else {
            for (auto multi_source : multi_sources) {
                run_apex(adjecency_matrix, multi_source, target, eps, svh_heuristic, heuristic_duration, logging_file);
            }
        }

    }
    else if ( algorithm == "BOA-MVH" ) {
        run_boa_backward_search(adjecency_matrix, target);
    }
    else if (algorithm == "OLDBS") {
        Heuristic blind_heuristic = [](size_t vertex) -> std::vector<float> {
            return {0, 0};
        };
        run_backward_search(adjecency_matrix, source, target, eps, blind_heuristic,
                            blind_heuristic, logging_file);

    }
    else if (algorithm == "FS") {
        if (multi_sources.empty()) {
            multi_sources.push_back(source);
        }
        for (auto multi_source : multi_sources) {
            run_forward_search(adjecency_matrix, multi_source, target, eps, mvh, logging_file);
        }
    } else if (algorithm == "EPS-MVH") {
        auto post_process = PostProcess();
        post_process(mvh, target, eps);
        std::cout << "Post-processing computed." << std::endl;

    } else if (algorithm == "Closure") {
        auto closure = Closure(adjecency_matrix);
        MultiValuedHeuristic new_mvh = closure(mvh, target, eps[0], eps[1]);
        std::cout << "Closure computed." << std::endl;
    }
    else {
        std::cout << "Unknown algorithm: " << algorithm << std::endl;
    }
}
