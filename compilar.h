#include <stdio.h>
#include <stdlib.h>

typedef int (*funcp) ();
typedef unsigned char uc;
funcp compilaSimples (FILE *f, unsigned char** codigo);