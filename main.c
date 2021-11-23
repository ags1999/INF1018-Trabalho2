#include "compilar.h"

int main(int argc, char *argv[])
{
    uc *codigo = (uc *)malloc(1024 * sizeof(uc));

    if (argc < 2)
    {
        exit(1);
    }

    const char *nomeInput = argv[1];

    FILE *input = fopen(nomeInput, "r");
    compilaSimples(input, &codigo);

    //FILE* output = NULL;

    return 0;
}
//gcc -Wall main.c compilaSimples.c -g -o compilador -Wa,--execstack