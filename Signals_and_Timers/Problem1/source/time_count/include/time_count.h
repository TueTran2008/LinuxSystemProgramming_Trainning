#ifndef __TIME_COUNT_H__
#define __TIME_COUNT_H__


/*!
* @brief Start the counter
*/
void time_count_begin(void);
/*!
* @brief Stop the counter
*/
void time_count_stop(void);
/*!
* @brief Calculate time between call from start to top
*
* @return time between start and stop
*/
double time_count_print(void);
#endif