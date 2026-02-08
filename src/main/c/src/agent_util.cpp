#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "agent_util.hpp"

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
/*bool
searchTag(int l,
          int h,
          const long value,
          const long* tags,
          int* index) {
  *index = (int) (ceil((l + h) / 2.0));
  if (l > h){
    return false; // Tag not found, return false
  }
  else {
    if (value == tags[*index]){
      return true; // Tag found at index mid
    }
    else if (value > tags[*index]){
      return searchTag((*index) + 1,
                       h,
                       value,
                       tags,
                       index);
    }
    else{
      return searchTag(l,
                       (*index) - 1,
                       value,
                       tags,
                       index);
    }
  }
}*/

bool UTIL::AGENT::searchTag(int l, int h, const long value,
                            const std::vector<long> &tags, int *index) {

  while (l <= h) {
    *index = (int)(ceil((l + h) / 2.0));

    if (value == tags[*index]) {
      return true; /* Tag found at index mid */
    } else if (value > tags[*index]) {
      l = (*index) + 1;
    } else {
      h = (*index) - 1;
    }
  }
  return false;
}

/**
 * @fn void mergeSort(int, int, int*, int*)
 * @brief
 *
 * @param i
 * @param j
 * @param a
 * @param aux
 */
void UTIL::AGENT::mergeSort(int i, int j, std::vector<long> &a,
                            std::vector<long> &aux) {
  if (j <= i) {
    return;
  }
  int mid = (i + j) / 2;
  mergeSort(i, mid, a, aux);
  mergeSort(mid + 1, j, a, aux);

  int pointer_left = i;
  int pointer_right = mid + 1;
  int k;

  for (k = i; k <= j; k++) {
    if (pointer_left == mid + 1) {
      aux[k] = a[pointer_right];
      pointer_right++;
    } else if (pointer_right == j + 1) {
      aux[k] = a[pointer_left];
      pointer_left++;
    } else if (a[pointer_left] < a[pointer_right]) {
      aux[k] = a[pointer_left];
      pointer_left++;
    } else {
      aux[k] = a[pointer_right];
      pointer_right++;
    }
  }

  for (k = i; k <= j; k++) {
    a[k] = aux[k];
  }
}
