// Trabalho 2 - Arvore B*
// Organização de arquivos 
// ICMC - USP, 1º semestre de 2023
// Enzo Nunes Sedenho - 13671810 | 100%
// João Pedro Mori Machado - 13671831 | {}%

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crimes.h"
#include "fornecido.h"
#include "ab.h"

#define MAX_TAM_STRING 100

int main(int argc, char *argv[])
{
    char nome_arq_csv[MAX_TAM_STRING], nome_arq_bin[MAX_TAM_STRING];
    int op;

    // Lendo a funcionalidade requisitada pelo usuário
    scanf(" %d", &op);
    switch(op)
    {
        case 1:
            // Lendo o nome dos arquivos e chamando a funcionalidade 1
            scanf(" %s %s", nome_arq_csv, nome_arq_bin);
            crimes_create(nome_arq_csv, nome_arq_bin);
            binarioNaTela(nome_arq_bin);
            break;

        case 2:
            // Lendo o nome do arquivo binário e chamando a funcionalidade 2
            scanf(" %s", nome_arq_bin);
            crimes_select_all(nome_arq_bin);
            break;

        case 3:
        {
            char nome_arq_ind[MAX_TAM_STRING], campo[MAX_TAM_STRING], tipo_dado[MAX_TAM_STRING];

            // Lendo os dados da entrada e chamando a funcionalidade 3
            scanf(" %s %s %s %s", nome_arq_bin, campo, tipo_dado, nome_arq_ind);

            crimes_indexar(nome_arq_bin, campo, tipo_dado, nome_arq_ind);
            binarioNaTela(nome_arq_ind);
        }
            break;

        case 4:
        {
            int num_buscas;
            char nome_arq_ind[MAX_TAM_STRING], campo[MAX_TAM_STRING], tipo_dado[MAX_TAM_STRING];

            // Lendo os dados da entrada e chamando a funcionalidade 4
            scanf(" %s %s %s %s %d", nome_arq_bin, campo, tipo_dado, nome_arq_ind, &num_buscas);

            crimes_buscar(nome_arq_bin, campo, tipo_dado, nome_arq_ind, num_buscas);
        }

            break;

        case 5:
        {
            int num_remocoes;
            char nome_arq_ind[MAX_TAM_STRING], campo[MAX_TAM_STRING], tipo_dado[MAX_TAM_STRING];

            // Lendo os dados da entrada e chamando a funcionalidade 5
            scanf(" %s %s %s %s %d", nome_arq_bin, campo, tipo_dado, nome_arq_ind, &num_remocoes);

            crimes_remover(nome_arq_bin, campo, tipo_dado, nome_arq_ind, num_remocoes);
            binarioNaTela(nome_arq_bin);
            binarioNaTela(nome_arq_ind);
        }
            break;

        case 6:
        {
            int num_insercoes;
            char nome_arq_ind[MAX_TAM_STRING], campo[MAX_TAM_STRING], tipo_dado[MAX_TAM_STRING];

            // Lendo os dados da entrada e chamando a funcionalidade 6
            scanf(" %s %s %s %s %d", nome_arq_bin, campo, tipo_dado, nome_arq_ind, &num_insercoes);

            crimes_inserir(nome_arq_bin, campo, tipo_dado, nome_arq_ind, num_insercoes);
            binarioNaTela(nome_arq_bin);
            binarioNaTela(nome_arq_ind);
        }

            break;
        
        case 7:
        {
            int num_edicoes;
            char nome_arq_ind[MAX_TAM_STRING], campo[MAX_TAM_STRING], tipo_dado[MAX_TAM_STRING];

            // Lendo os dados da entrada e chamando a funcionalidade 7
            scanf(" %s %s %s %s %d", nome_arq_bin, campo, tipo_dado, nome_arq_ind, &num_edicoes);

            crimes_editar(nome_arq_bin, campo, tipo_dado, nome_arq_ind, num_edicoes);
            binarioNaTela(nome_arq_bin);
            binarioNaTela(nome_arq_ind);
        }
            break;

        case 8:
        {
            char nome_arq_ind[MAX_TAM_STRING], campo[MAX_TAM_STRING], tipo_dado[MAX_TAM_STRING];
            
            // Lendo os dados da entrada e chamando a funcionalidade 9
            scanf(" %s %s %s %s", nome_arq_bin, campo, tipo_dado, nome_arq_ind);

            crimes_criar_ab(nome_arq_bin, campo, tipo_dado, nome_arq_ind);
            binarioNaTela(nome_arq_ind);
        }
            break;

        case 9:
        {
            int num_buscas;
            char nome_arq_ind[MAX_TAM_STRING], campo[MAX_TAM_STRING], tipo_dado[MAX_TAM_STRING];
            
            // Lendo os dados da entrada e chamando a funcionalidade 9
            scanf(" %s %s %s %s %d", nome_arq_bin, campo, tipo_dado, nome_arq_ind, &num_buscas);

            crimes_buscar_ab(nome_arq_bin, campo, tipo_dado, nome_arq_ind, num_buscas);
        }
            break;

        case 10:
        {
            int num_insercoes;
            char nome_arq_ind[MAX_TAM_STRING], campo[MAX_TAM_STRING], tipo_dado[MAX_TAM_STRING];
            
            // Lendo os dados da entrada e chamando a funcionalidade 9
            scanf(" %s %s %s %s %d", nome_arq_bin, campo, tipo_dado, nome_arq_ind, &num_insercoes);

            crimes_inserir_ab(nome_arq_bin, campo, tipo_dado, nome_arq_ind, num_insercoes);
            binarioNaTela(nome_arq_bin);
            binarioNaTela(nome_arq_ind);
        }
            break;

        default:
            break;
    }
    
    return 0;
}