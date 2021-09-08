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

int dateToFido(unsigned char *buf, 
               size_t max, 
               int year, 
               int month,
               int day,
               int hour,
               int minute,
               int sec);
