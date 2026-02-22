//
// Created by crl on 21/02/2026.
//

#ifndef POST_PROCESS_H
#define POST_PROCESS_H

#include "definitions.h"


class PostProcess {
public:
    PostProcess() = default;

    MultiValuedHeuristic operator()(const MultiValuedHeuristic& mvh, const EPS& eps,
    const std::string& output_file);
};

#endif //POST_PROCESS_H
