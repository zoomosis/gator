/*********************************************************************/
/*                                                                   */
/*  This Program Written by Paul Edwards, 3:711/934@fidonet.         */
/*  Released to the Public Domain                                    */
/*                                                                   */
/*********************************************************************/
/*********************************************************************/
/*                                                                   */
/*  dateToFido - convert date to fido                                */
/*                                                                   */
/*********************************************************************/

#include <stdio.h>

#include "dtfido.h"

int dateToFido(unsigned char *buf, 
               size_t max, 
               int year, 
               int month,
               int day,
               int hour,
               int minute,
               int sec)
{
    static char *monthTab[] = { "",
                                "Jan",
                                "Feb",
                                "Mar",
                                "Apr",
                                "May",
                                "Jun",
                                "Jul",
                                "Aug",
                                "Sep",
                                "Oct",
                                "Nov",
                                "Dec" };
                                
    sprintf((char *)buf, 
            "%.2d %s %.2d  %.2d:%.2d:%.2d",
            day,
            monthTab[month],
            year%100,
            hour,
            minute,
            sec);
    (void)max;        
    return (0);
}
