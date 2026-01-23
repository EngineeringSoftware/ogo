#pragma once

#include <fstream>
#include <stdbool.h>
#include <vector>

namespace UTIL {
namespace AGENT {
/**
 * @def STRINGIFY_MACRO_
 * @brief Adds quoatation to macro value
 *
 */
#define STRINGIFY_MACRO_(x) #x

/**
 * @fn      int searchTag(int, int, long, long*, int*)
 * @brief   Binary Search for faster indexing of instanceInfo*
 *          with given tag
 *
 * @author  1sand0s
 * @param   l           low index of the array to be searched
 * @param   h           high index of the array to be searched
 * @param   value       value (tag of the instance) to search for
 * @param   tags        array to search
 * @param   index       index of the instance with the queried
 *                      tag if present
 * @return              true if found
 */
bool searchTag(int l, int h, const long value, const std::vector<long> &tags,
               int *index);

/**
 * @fn void mergeSort(int, int, long*, long*)
 * @brief
 *
 * @param i
 * @param j
 * @param a
 * @param aux
 */
void mergeSort(int i, int j, std::vector<long> &a, std::vector<long> &aux);
} // namespace AGENT
} // namespace UTIL
