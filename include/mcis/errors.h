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

namespace mcis {

/**
 * @brief Graph operation errors
 */
enum class GraphError {
    NODE_ALREADY_EXISTS,
    NODE_NOT_FOUND,
    EDGE_ALREADY_EXISTS,
    EDGE_NOT_FOUND,
    SELF_LOOP_NOT_ALLOWED,
    FILE_OPERATION_FAILED,
    BULK_OPERATION_PARTIAL_FAILURE
};

/**
 * @brief Node operation errors
 */
enum class NodeError {
    EDGE_TO_SELF,
    EDGE_ALREADY_EXISTS,
    EDGE_NOT_FOUND,
    INVALID_NEIGHBOR
};

/**
 * @brief Algorithm execution errors
 */
enum class AlgorithmError {
    INVALID_INPUT,
    TIMEOUT,
    MEMORY_LIMIT_EXCEEDED,
    NOT_IMPLEMENTED,
    INCOMPATIBLE_GRAPHS,
    EMPTY_RESULT
};

}  // namespace mcis

#endif  // INCLUDE_MCIS_ERRORS_H_

