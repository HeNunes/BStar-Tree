#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fornecido.h"
#include "auxiliar.h"

// Mensagem de erro ao abrir arquivo
void erro_arquivo()
{
    printf("Falha no processamento do arquivo. \n");
    exit (0); // Como o arquivo não pode ser aberto, encerramos o programa
}

// Mensagem de erro para arquivo vazio
void erro_registros()
{
    printf("Registro inexistente.\n");
}

void checar_null(FILE *arq)
{
    // Verificando se o arquivo existe
    if(arq == NULL)
        erro_arquivo();
}
int len_campo(char *buffer)
{
    // Percorrendo a string até que um delimitador seja encontrado
    int len = 1;
    for(len = 1; buffer[len - 1] != DELIM_STRING && buffer[len - 1] != '\0'; len++);

    // Retornando o tamanho da string
    return len;
}
void trata_string(char *buffer, int tam)
{
    // Percorrendo a string
    for(int i = 0; i < tam; i++)
    {
        // Substituindo o primeiro lixo ou delimitador encontrado por '\0'
        if(buffer[i] == LIXO || buffer[i] == DELIM_STRING)
        {
            buffer[i] = '\0';
        }
    }
}
void ler_campo(FILE *arq_csv, char *buffer)
{
    char c = fgetc(arq_csv);
    int i = 0;

    // Enquanto não for encontrado um delimitador (virgula, quebra de linha ou EOF)
    while(c != '\n' && c != '\r' && c != ',' && c != EOF)
    {
        // Lendo um caractere do arquivo e concatenando na string
        buffer[i] = c;
        c = fgetc(arq_csv);
        i++;
    }
}
void bin_ler_string_variavel(FILE *arq_bin, char *buffer)
{
    int len = 0;
    char c;

    // Enqunado um delimitador não for encontrado
    do
    {
        // Lendo um caractere do arquivo e concatenando na string
        fread(&c, sizeof(char), 1, arq_bin);
        buffer[len] = c;
        len++;

    } while(c != DELIM_STRING);
    buffer[len - 1] = '\0';
}
// Lê os critérios de busca (ou edição) em um vetor de PARES
void ler_pares(PAR *vetor_pares, int num_pares, char *campo)
{
    // Iterando no número de PARES
    for(int i = 0; i < num_pares; i++)
    {
        // Lendo o campo de busca
        scanf(" %s", vetor_pares[i].campo);

        // Caso o campo buscado seja do tipo inteiro
        if(strcmp(vetor_pares[i].campo, "idCrime") == 0 || strcmp(vetor_pares[i].campo, "numeroArtigo") == 0)
        {
            // Lendo a chave inteira e atribuíndo um valor nulo à chave do tipo string
            scanf(" %d", &vetor_pares[i].chave_int);
            strcpy(vetor_pares[i].chave_str, "\0");
        }
        // Caso o campo buscado seja do tipo string
        else
        {
            // Lendo a chave do tipo string e atribuindo um valor nulo à chave do tipo inteiro
            scan_quote_string(vetor_pares[i].chave_str);
            vetor_pares[i].chave_int = -1;
        }
    }
    for(int i = 1; i < num_pares; i++)
    {
        // Caso uma das edições corresponda ao campo indexado
        if(strcmp(vetor_pares[i].campo, campo) == 0)
        {
            // Colocando o PAR referido na primeira posição do vetor
            PAR aux = vetor_pares[0];
            vetor_pares[0] = vetor_pares[i];
            vetor_pares[i] = aux;
            break;
        }
    }
}
// Mensagem de erro caso a busca da funcionalidade 4 falhe
void print_reg_inexistente()
{
    printf("Registro inexistente.\n");
}