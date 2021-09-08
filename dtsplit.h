/* date split */
/* Written by Benjamin Lin, Romane Takkenberg and Paul Edwards */
/* Modified by Andrew Clarke (03-Sep-95) */
/* Released to the Public Domain */

#ifndef DTSPLIT
#define DTSPLIT

#define GOOD_DATE 0 /* the date is Fido or Seadog                       */
#define UNRECOGNIZED_DATE 1 /* the date is not Fido or Seadog           */

int dt_split (const unsigned char *dt, int *year, int *month, int *day,
              int *hour, int *minute, int *sec);

#endif
