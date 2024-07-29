#ifndef INDEX_H
#define INDEX_H

#include "auxiliar.h"

// Cabecalho de indexação
typedef struct cab_index_t
{
    char status;
    int NumRegs;
} CAB_INDEX;
// Registro de index do tipo inteiro
typedef struct int_index_t
{
    int chave;
    long int ByteOffset;
} INT_INDEX;
// Registro de index do tipo string
typedef struct str_index_t
{
    char chave[12];
    long int ByteOffset;
} STR_INDEX;

// Monta um vetor de INDEX do tipo inteiro a partir do arquivo de dados
void montar_vec_int_index(INT_INDEX *vec_index, int num_regs, char *campo, FILE *arq_bin);
// Monta um vetor de INDEX do tipo inteiro a partir do arquivo de dados
void montar_vec_str_index(STR_INDEX *vec_index, int num_regs, char *campo, FILE *arq_bin);

// Escrevendo o CABECALHO de INDEX no arquivo de INDEX
void escreve_cab_index(CAB_INDEX *cab_ind, FILE *arq_index);
// Escreve um INDEX do tipo inteiro no arquivo de INDEX
void escreve_int_index(INT_INDEX *index, CAB_INDEX *cab_ind, FILE *arq_index);
// Escreve um INDEX do tipo string no arquivo de INDEX
void escreve_str_index(STR_INDEX *index, CAB_INDEX *cab_ind, FILE *arq_index);

// Lê o CABECALHO do arquivo de INDEX
void bin_ler_cab_index(CAB_INDEX *cab_ind, FILE *arq_index);
// Lendo o CABECALHO do arquivo de INDEX e tratando as exceções
void tenta_ler_cab_index(CAB_INDEX *cab_ind, FILE **arq_index);
// Lê um INDEX do tipo inteiro do arquivo de INDEX
void bin_ler_int_index(INT_INDEX *int_ind, FILE *arq_index);
// Lê um INDEX do tipo string do arquivo de INDEX
void bin_ler_str_index(STR_INDEX *str_ind, FILE *arq_index);

// Retorna a posição do primeiro INDEX no vetor que satisfaça o critério de busca
int buscar_int_index(INT_INDEX *vec_index, int tam, int chave);
// Retorna a posição do primeiro INDEX no vetor que satisfaça o critério de busca
int buscar_str_index(STR_INDEX *vec_index, int tam, char *chave);

// Lê um vetor do arquivo de INDEX
void ler_vec_index(INT_INDEX **vec_index_int, STR_INDEX **vec_index_str, CAB_INDEX *cab_ind, char *tipo_dado, FILE *arq_index);

// Ordena um vetor de INDEX usando a função Qsort
void ordenar_vec_index(INT_INDEX *vec_index_int, STR_INDEX *vec_index_str, CAB_INDEX cab_ind, char *tipo_dado);

// Reescreve o arquivo de INDEX a partir de um vetor de INDEX já inicializado
void re_indexar(char *nome_arq_ind, INT_INDEX *vec_index_int, STR_INDEX *vec_index_str, CAB_INDEX *cab_ind, int max, char *tipo_dado);

// Remove um INDEX do vetor
void remove_index(INT_INDEX *vec_index_int, STR_INDEX *vec_index_str, int tam, long int byte_offset, char *tipo_dado);

// Adiciona um novo INDEX ao fim de um vetor de INDEX
void adiciona_index(INT_INDEX *vec_index_int, STR_INDEX *vec_index_str, REGISTRO reg, CABECALHO cab, char *campo, int pos);

// Altera o vetor de INDEX com base nas edições anteriormente realizadas
void edita_vec_index(INT_INDEX **vec_index_int, STR_INDEX **vec_index_str, CAB_INDEX *cab_ind, REGISTRO *reg_edit, long int BOs_atual, long int ultimo_BOs, char *campo, int flag_cabe);

// Liberando a memória alocada para o vetor de INDEX
void liberar_vetor_index(INT_INDEX **vec_index_int, STR_INDEX **vec_index_str);

#endif