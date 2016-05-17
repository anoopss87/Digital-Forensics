/********************************************************************
 *
 * File         : inodetime.h
 * Author       : Naveen Raj Selvaraj
 * Date         : April 5, 2016
 * Description  : This file contains the methods to calculate
 *                time elapsed between two points during execution.
 *
 *******************************************************************/
struct timeval gEndTime;
struct timeval gStartTime;
static inline void START_TIME()
{
    memset(&gStartTime, 0, sizeof(gStartTime));
    gettimeofday(&gStartTime, NULL);
}

static inline void END_TIME()
{
    memset(&gEndTime, 0, sizeof(gEndTime));
    gettimeofday(&gEndTime, NULL);
}

static inline double ELAPSED_TIME()
{
    double diff;

    diff = (gEndTime.tv_sec - gStartTime.tv_sec) * 1000.0;      // sec to ms
    diff += (gEndTime.tv_usec - gStartTime.tv_usec) / 1000.0;   // us to ms

    /* return the time difference in milliseconds */
    return diff;
}
