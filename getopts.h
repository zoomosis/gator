#if !defined(GETOPTS)
#define GETOPTS
#define OPTINT 1
#define OPTSTR 2
#define OPTBOOL 3
#define OPTLONG 4
#define OPTFLOAT 5
typedef struct {
    char *sw;
    int opttyp;
    void *var;
} opt_t;
int getopts(int argc, char **argv, opt_t opttable[]);
#endif
