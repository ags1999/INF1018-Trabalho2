#include "compilar.h"

unsigned int iAt = 0; // instrução atual

/*
v1 = -32(%rbp)
v2 = -24(%rbp)
v3 = -16(%rbp)
v4 = -8(%rbp)
*/

int linhaRet[50];
int lAt = 0;

void inicio(uc ** codigo)
{
    
    (*codigo)[0] = 0x55;   //push  %rbp
    (*codigo)[1] = 0x48;   //mov   %rsp,%rbp
    (*codigo)[2] = 0x89;
    (*codigo)[3] = 0xe5;
    (*codigo)[4] = 0x48;   //sub   $0x20,%rbp
    (*codigo)[5] = 0x83;   
    (*codigo)[6] = 0xec;
    (*codigo)[7] = 0x20;
    iAt += 8;
    linhaRet[0] = 0;
    linhaRet[1] = 1;
    linhaRet[2] = 4;
    lAt += 3;
}



static int varpc1(uc* linha, uc** codigo)
{
    //mov varpc1, %r10d
    int constante = 0;
    int tmLinha = 0;
    switch (linha[0])
    {
        case 'v':
            (*codigo)[iAt] = 0x44; 
            (*codigo)[iAt + 1] = 0x8b;
            (*codigo)[iAt + 2] = 0x55;
            switch (linha[1])
            {
                case 1:
            
                    (*codigo)[iAt + 3] = 0xe0; //mov    -0x20(%rbp),%r10d
                    break;
                case 2:
                    (*codigo)[iAt + 3] = 0xe8;//mov    -0x18(%rbp),%r10d
                    break;
        
                case 3:
                    (*codigo)[iAt + 3] = 0xf0; //mov    -0x10(%rbp),%r10d
                    break;
        
                case 4:
                    (*codigo)[iAt + 3] = 0xf8;  //mov    -0x8(%rbp),%r10d
                    break;

                default:
                    exit(1);
        }
            lAt += 
            tmLinha += 3;
            iAt += 4;
            break;

        case 'p':
            (*codigo)[iAt] = 0x41;
            (*codigo)[iAt + 1] = 0x89;
            if (linha[1] == 1)
            {
                (*codigo)[iAt + 2] = 0xfa;  //mov    %edi,%r10d
            }
            else
            {
                (*codigo)[iAt + 2] = 0xf2;  //mov    %esi,%r10d
            }
            iAt += 3;
            tmLinha += 3; 
            break;

        case '$':

            while(linha[tmLinha] != ' ')
            //for (int i = 0; i < 4; i++)
            {
                constante *= 10;
                constante += linha[tmLinha+1] - 48;
                tmLinha++;
            }
                
            (*codigo)[iAt] = 0x41;     //41 ba xx xx xx xx    	mov    $Constante,%r10d
            (*codigo)[iAt + 1] = 0xba;
            iAt += 2;
            for(int i = 0; i < 4; i++)
            {
                (*codigo)[iAt] = (uc)constante;
                constante >>= 8;
                iAt++;
            }

            break;

    default:
        exit(1);
        break;
    }

    return tmLinha;
}

static void varpc2(uc* linha, uc** codigo)
{
    //mov varpc2, %r11d
    int constante = 0;
    int tmLinha = 0;
    switch (linha[0])
    {
        case 'v':
            (*codigo)[iAt] = 0x44; 
            (*codigo)[iAt + 1] = 0x8b;
            (*codigo)[iAt + 2] = 0x5d;
            switch (linha[1])
            {
                case '1':
            
                    (*codigo)[iAt + 3] = 0xe0; //mov    -0x20(%rbp),%r11d
                    break;
                case '2':
                    (*codigo)[iAt + 3] = 0xe8;//mov    -0x18(%rbp),%r11d
                    break;
        
                case '3':
                    (*codigo)[iAt + 3] = 0xf0; //mov    -0x10(%rbp),%r11d
                    break;
        
                case '4':
                    (*codigo)[iAt + 3] = 0xf8;  //mov    -0x8(%rbp),%r11d
                    break;

                default:
                    exit(1);
        }
        
            iAt += 4;
            break;

        case 'p':
            (*codigo)[iAt] = 0x41;
            (*codigo)[iAt + 1] = 0x89;
            if (linha[1] == '1')
            {
                (*codigo)[iAt + 2] = 0xfb;  //mov    %edi,%r11d
            }
            else
            {
                (*codigo)[iAt + 2] = 0xf3;  //mov    %esi,%r11d
            }
            iAt += 3;
            break;

        case '$':

            while(linha[tmLinha] != ' ')
            {
                constante *= 10;
                constante += linha[tmLinha+1] - 48;
                tmLinha++;
            }
                
            (*codigo)[iAt] = 0x41;     //mov    $Constante,%r10d
            (*codigo)[iAt + 1] = 0xbb;
            iAt += 2;
            for(int i = 0; i < 4; i++)
            {
                (*codigo)[iAt] = (uc)constante;
                constante >>= 8;
                iAt++;
            }
            linha += 8;
            break;

    default:
        exit(1);
        break;
    }

    return;
}


static void expressao(uc *linha, uc** codigo)
{
    //varpc op varpc
    
    uc expr = 0;
    
    expr  = varpc1(linha, codigo);
    linha += expr + 2;
    varpc2(linha, codigo);
    switch (expr) // r10d op r11d
    {
        case '+':
            (*codigo)[iAt] = 0x45; //add    %r10d,%r11d
            (*codigo)[iAt + 1] = 0x01;
            (*codigo)[iAt + 2] = 0xd3;
            iAt += 3;

        case '-':
            (*codigo)[iAt] = 0x45; //sub   %r10d,%r11d
            (*codigo)[iAt + 1] = 0x29;
            (*codigo)[iAt + 2] = 0xd3;
            iAt += 3;

            break;
        case '*':
            (*codigo)[iAt] = 0x45; //imul   %r10d,%r11d
            (*codigo)[iAt + 1] = 0x0f;
            (*codigo)[iAt + 2] = 0xaf;
            (*codigo)[iAt + 2] = 0xda;
            iAt += 4;

            break;
        

    }
    //
}

static void atribuicao (uc tipo, uc* linha, uc** codigo)
{
    expressao(linha + 5, codigo);
    if (tipo == 'v')
    {
        (*codigo)[iAt] = 0x44;     
        (*codigo)[iAt + 1] = 0x89;
        (*codigo)[iAt + 2] = 0x55;
        switch (linha[1])
        {
            
            case '1':
                (*codigo)[iAt + 3] = 0xe0;//mov %r10d, v1
                
                break;
            case '2':
                (*codigo)[iAt + 3] = 0xe8;//mov %r10d, v2
                
                break;
            case '3':
                (*codigo)[iAt + 3] = 0xf0;//mov %r10d, v3
                
                break;
            case '4':
                (*codigo)[iAt + 3] = 0xf8;//mov %r10d, v4
                
                break;
            default:
                break;
            
        }
        iAt +=4;
    }
    else
    {
        (*codigo)[iAt] = 0x44;
        (*codigo)[iAt + 1] = 0x89;
        if (linha[1] == '1')//mov    %r10d,%edi
        {
            
            (*codigo)[iAt + 2] = 0xd7;
        }
        else//mov    %r10d, %esi
        {
            (*codigo)[iAt + 2] = 0xd6;
        }
        iAt +=3;
    }
    
}

funcp compilaSimples (FILE *f, uc **codigo)
{
    inicio(codigo);
    //int tmLinha = 0;
    int lnAtual = 0;
    int charAtual = 0;
    //int tmArq = 0;
    uc ** vetorCodigo = (uc**)malloc(50*sizeof(uc*));

    for(int i = 0; i < 50; i++)
    {
        vetorCodigo[i] = (uc*)malloc(20*sizeof(uc));
    }


    
    while(!feof(f))
    {
        vetorCodigo[lnAtual][charAtual] = fgetc(f);
        char oof = vetorCodigo[lnAtual][charAtual];
        if ((vetorCodigo[lnAtual][charAtual] == '\n'))
        {
            
            vetorCodigo[lnAtual][charAtual] = 0;
            charAtual = 0;
            lnAtual++;
            continue;
        }
        charAtual++;
        
    }
    vetorCodigo[lnAtual][charAtual - 1] = 0;
    printf("%s\n", vetorCodigo[1]);

    for (int i = 0; i <= lnAtual; i++)
    {
        switch (vetorCodigo[i][0])
        {
            case 'v':       //atribuição para variaável
                atribuicao('v', vetorCodigo[i], codigo);
                break;

            case 'p':       //atribuição para parâmetro
                atribuicao('p', vetorCodigo[i], codigo);
                break;

            case 'i':
                break;

            case 'r':
                (*codigo)[iAt] = 0xc9;
                (*codigo)[iAt + 1] = 0xc3;
                break;

            default:
                break;
        }
    }
    

    for(int i = 0; i < 50; i++)
    {
        free(vetorCodigo[i]);
    }
    free(vetorCodigo);
    return (funcp)(codigo);
}

/*
char * calculaDelta(unsigned long rip, unsigned long fp)
{
    uc delta[4];
    unsigned long dt = rip - fp;
    delta[0] = (unsigned)dt;
    delta[1] = (unsigned)(dt>>8);
    delta[2] = (unsigned)(dt>>16);
    delta[3] = (unsigned)(dt>>24);
    return delta;
}
*/