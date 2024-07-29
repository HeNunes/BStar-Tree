#ifndef REGISTROS_H
#define REGISTROS_H

#include <stdbool.h>
#include "auxiliar.h"

// Registro de cabecalho
typedef struct cabecalho_t
{
    char status; 
    long int proxByteOffset; 
    int NumRegArq; 
    int NumRegRem; 
} CABECALHO;

// Registro de dados
typedef struct registro_t
{
    char removido;
    int id;
    char data[10];
    int artigo;
    char local[MAX_TAM_STRING];
    char descricao[MAX_TAM_STRING];
    char marca[12];
} REGISTRO;

// As próximas funções (ler_id até ler_marca) tratam o dado lido do CSV e armazenam no REGISTRO
void ler_id(REGISTRO *reg, char *buffer);
void ler_data(REGISTRO *reg, char *buffer);
void ler_artigo(REGISTRO *reg, char *buffer);
void ler_local(REGISTRO *reg, char *buffer);
void ler_descricao(REGISTRO *reg, char *buffer);
void ler_marca(REGISTRO *reg, char *buffer);

// Inicializa o CABECALHO
void inicializa_cabecalho(CABECALHO *cab);
// Função utilizada após um REGISTRO ser lido
void atualizar_cabecalho(CABECALHO *cab, char *local, char *descricao);
// Escreve o CABECALHO no arquivo binário
void escreve_cabecalho(CABECALHO *cab, FILE *arq_bin);
// Escreve o REGISTRO no arquivo binário
void escreve_registro(REGISTRO *reg, FILE *arq_bin);

// Imprime um REGISTRO 
void imprime_registro(REGISTRO reg);
// Le o CABECALHO do arquivo de dados
void bin_ler_cabecalho(CABECALHO *cab, FILE *arq_bin);
// Le o CABECALHO do arquivo de dados e trata as exceções
void tentar_ler_cabecalho(CABECALHO *cab, FILE **arq_bin);
// Le um REGISTRO do arquivo de dados
void bin_ler_registro(REGISTRO *reg, FILE *arq_bin);

// Retorna o tamanho do REGISTRO
int tam_reg(REGISTRO *reg);

// Verifica se um REGISTRO atende determinada condição de busca
bool comp_busca(REGISTRO reg, char *campo_buscado, int int_buscado, char *str_buscado);
// Checa todos os critérios de busca para um REGISTRO
bool checar_busca(REGISTRO reg, PAR *vetor_pares, int num_pares);

// Refina o vetor de REGISTROS criado pela busca indexada com base nos outros critérios de busca
void refina_vetor(REGISTRO *vetor_registros, PAR *vetor_pares, int tam, int num_pares);

// Imprime o resultado de uma busca
void print_res_busca(REGISTRO *vetor_registros, int tam);

// Remove logicamente um REGISTRO
void remover_registro(FILE *arq_bin, CABECALHO *cab, REGISTRO *reg, long int byte_offset);

// Le um REGISTRO da entrada padrão
void ler_reg_stdin(REGISTRO *reg);

// Checa se a edição à ser realizada cabe no REGISTRO de dados
int registro_cabe(REGISTRO reg, PAR *vetor_edicoes, int num_edicoes);
// Formata os campos do tipo string de um REGISTRO para realizar a edição
void formata_reg_edit(REGISTRO *reg_edit, REGISTRO reg);

#endif