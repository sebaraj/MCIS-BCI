/**
 * @file errors.h
 * @author Bryan SebaRaj <bryan.sebaraj@yale.edu>
 * @version 1.0
 * @section DESCRIPTION
 *
 * Copyright (c) 2025 Bryan SebaRaj
 *
 * This software is licensed under the MIT License.
 */

#ifndef INCLUDE_MCIS_ERRORS_H_
#define INCLUDE_MCIS_ERRORS_H_

#include <iostream>

namespace mcis {

enum class NodeError {
    EDGE_ALREADY_EXISTS,
    EDGE_DOES_NOT_EXIST,
    SELF_LOOP,
};

enum class GraphError {
    NODE_ALREADY_EXISTS,
    NODE_DOES_NOT_EXIST,
    EDGE_ALREADY_EXISTS,
    EDGE_DOES_NOT_EXIST,
};

enum class AlgorithmError {
    EMPTY_GRAPH,
    INVALID_ALGORITHM,
};

inline std::ostream& operator<<(std::ostream& os, const NodeError& error) {
    switch (error) {
        case NodeError::EDGE_ALREADY_EXISTS:
            os << "NodeError: Edge already exists.";
            break;
        case NodeError::EDGE_DOES_NOT_EXIST:
            os << "NodeError: Edge does not exist.";
            break;
        case NodeError::SELF_LOOP:
            os << "NodeError: Self-loops are not allowed.";
            break;
    }
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const GraphError& error) {
    switch (error) {
        case GraphError::NODE_ALREADY_EXISTS:
            os << "GraphError: Node already exists.";
            break;
        case GraphError::NODE_DOES_NOT_EXIST:
            os << "GraphError: Node does not exist.";
            break;
        case GraphError::EDGE_ALREADY_EXISTS:
            os << "GraphError: Edge already exists.";
            break;
        case GraphError::EDGE_DOES_NOT_EXIST:
            os << "GraphError: Edge does not exist.";
            break;
    }
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const AlgorithmError& error) {
    switch (error) {
        case AlgorithmError::EMPTY_GRAPH:
            os << "AlgorithmError: The graph is empty.";
            break;
        case AlgorithmError::INVALID_ALGORITHM:
            os << "AlgorithmError: Invalid algorithm specified.";
            break;
    }
    return os;
}

}  // namespace mcis

#endif  // INCLUDE_MCIS_ERRORS_H_
