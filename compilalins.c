#include "compilar.h"

//Alexandre Sanson - 1711450
//Luana Durand - 1820807

unsigned int iAt = 0; // instrução atual

/*
v1 = -32(%rbp)
v2 = -24(%rbp)
v3 = -16(%rbp)
v4 = -8(%rbp)
*/

// resolver constantes negativas

int linhaSimples[50];// total de instruçoes até a linha i+1
int lsAt = 0;

typedef struct tabelaOffset
{
    uc posicaoCodigo; //total de instruçoes até o valor do destino
    uc destino;
} Tabela;

Tabela* offset;
int indiceTabela = 0;

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
            
            tmLinha += 3;
            iAt += 4;
            break;

        case 'p':
            (*codigo)[iAt] = 0x41;
            (*codigo)[iAt + 1] = 0x89;
            if (linha[1] == '1')
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

            while(linha[tmLinha + 1] != 32)
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

            while(linha[tmLinha + 1] != 32)
            {
                constante *= 10;
                constante += linha[tmLinha+1] - 48;
                tmLinha++;
            }
                
            (*codigo)[iAt] = 0x41;     //mov    $Constante,%r11d
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
            printf("Erro varpc2\n");
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
    

    varpc2(&linha[5], codigo);
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
    linhaSimples[lsAt] = iAt;
    lsAt++;
    
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

static int tipoDesvio(uc tipo, uc valor)
{
    if(tipo == 'p')
    {
        if(valor == '1')
        {
            return 1;
        }
        else
        {
            return 2;
        }
    }
    else
    {
        switch (valor)
        {
        case '1':
            return 3;

        case '2':
            return 4;

        case '3':
            return 5;

        case '4':
            return 6;

        }
    }
    return -1;
}

static void desvio(uc* linha, uc** codigo)
{
    linhaSimples[lsAt] = iAt;
    lsAt++;

    int tipo = tipoDesvio(linha[3], linha[4]);
    uc voltar = linha[6] - 1;
    (*codigo)[iAt] = 0x83; 
    switch (tipo)
    {
    case 1: //p1
        (*codigo)[iAt + 1] = 0xff;
        (*codigo)[iAt + 2] = 0x00;
        iAt += 3;
        break;
    
    case 2: //p2
        (*codigo)[iAt + 1] = 0xfe;
        (*codigo)[iAt + 2] = 0x00;
        iAt += 3;
        break;

    case 3: //v1
        (*codigo)[iAt + 1] = 0x7d;
        (*codigo)[iAt + 2] = 0xe0;
        (*codigo)[iAt + 3] = 0x00;
        iAt += 4;
        break;

    case 4: //v2
        (*codigo)[iAt + 1] = 0x7d;
        (*codigo)[iAt + 2] = 0xe8;
        (*codigo)[iAt + 3] = 0x00;
        iAt += 4;
        break;

    case 5: //v3
        (*codigo)[iAt + 1] = 0x7d;
        (*codigo)[iAt + 2] = 0xf0;
        (*codigo)[iAt + 3] = 0x00;
        iAt += 4;
        break;

    case 6: //v4
        (*codigo)[iAt + 1] = 0x7d;
        (*codigo)[iAt + 2] = 0xf8;
        (*codigo)[iAt + 3] = 0x00;
        iAt += 4;
        break;

    default:
        printf("Erro no desvio\n");
        exit(1);
        break;
    }

    (*codigo)[iAt] = 0x75; // jne 
    (*codigo)[iAt + 1] = 0x0; 

    iAt += 2;
    
    offset[indiceTabela].posicaoCodigo = iAt - 1;
    offset[indiceTabela].destino = voltar;

}

static void printcodigo(uc codigo[])
{
    printf("%hhx\n", codigo[0]);
    printf("%hhx %hhx %hhx\n", codigo[1], codigo[2], codigo[3]);
    printf("%hhx %hhx %hhx %hhx\n", codigo[4], codigo[5], codigo[6], codigo[7]);
    for (int i = 8; i < iAt; i++)
    {
        printf("%hhx\n", codigo[i]);
        
    }
    
}

funcp compilaSimples (FILE *f, uc codigo[])
{
    inicio(&codigo);
    offset = (Tabela*)malloc(sizeof(Tabela)*50);
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


    for (int i = 0; i <= lnAtual; i++)
    {
        switch (vetorCodigo[i][0])
        {
            case 'v':       //atribuição para variaável
                atribuicao('v', vetorCodigo[i], &codigo);
                break;

            case 'p':       //atribuição para parâmetro
                atribuicao('p', vetorCodigo[i], &codigo);
                break;

            case 'i':
                desvio(vetorCodigo[i], &codigo);
                break;

            case 'r':
                //linhaSimples[lsAt] = iAt;
                codigo[iAt] = 0x8b; // mov    -0x20(%rbp),%eax
                codigo[iAt + 1] = 0x45;
                codigo[iAt + 2] = 0xe0;
                codigo[iAt + 3] = 0xc9; // leave
                codigo[iAt + 4] = 0xc3;// ret
                iAt += 5;

                break;

            default:
                break;
        }
    }
    
    for (int i = 0; i < indiceTabela; i++)
    {
        codigo[offset->posicaoCodigo] = linhaSimples[offset->destino] - iAt;
    }
    

    for(int i = 0; i < 50; i++)
    {
        free(vetorCodigo[i]);

    }
    printcodigo(codigo);
    free(vetorCodigo);
    
    return (funcp)(codigo);
}

