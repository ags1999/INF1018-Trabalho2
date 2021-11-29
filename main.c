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
    funcp soma = compilaSimples(input, codigo);

    printf("%d\n", soma(5, 6));

    fclose(input);

    return 0;
}
//gcc -Wall main.c compilalins.c -g -o compilador -Wa,--execstack && ./compilador teste.lins