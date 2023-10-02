#include <time.h>
#include <stdio.h>
static clock_t m_clock_begin;
static clock_t m_clock_endl;
static double m_time_spend;
void time_count_begin(void)
{
    m_clock_begin = clock();
}
void time_count_stop(void)
{
    m_clock_endl = clock();
}
double time_count_print(void)
{
    m_time_spend = (double)(m_clock_endl - m_clock_begin) / CLOCKS_PER_SEC;
    printf("The elapsed time is %f seconds\r\n", m_time_spend);
    return m_time_spend;
}