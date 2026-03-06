#pragma once

#include <ctime>
namespace util {
namespace TIMER {
/**
 * @struct timeInfo
 * @brief  struct for storing method execution time
 *         of different stages of object graph construction
 *         Use this for profiling execution times
 *

 */
typedef struct TimeInfo {
  clock_t start;        /* start time */
  clock_t end;          /* end time   */
  float total_duration; /* duration   */
} TimeInfo;

/**
 * @def TIMER_START_
 * @brief Macro for starting clock while profiling
 *
 */
#define TIMER_START_(x) (x).start = clock();

/**
 * @def TIMER_STOP_
 * @brief Macro for stopping clock while profiling
 *
 */
#define TIMER_END_(x) (x).end = clock();

/**
 * @def TIMER_ACCUMULATE_
 * @brief Macro for accumulating clock while profiling
 *        (If method calls to be profiled are inside loops)
 *
 */
#define TIMER_ACCUMULATE_(x)                                                   \
  (x).end = clock();                                                           \
  (x).total_duration += ((x).end - (x).start) * 1e3 / CLOCKS_PER_SEC;

/**
 * @def DURATION_
 * @brief Computes the time taken
 *
 */
#define DURATION_(x) ((float)(x.end - x.start) * 1e3 / CLOCKS_PER_SEC)
} // namespace TIMER
} // namespace util
