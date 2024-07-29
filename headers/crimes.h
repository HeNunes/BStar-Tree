#ifndef CRIMES_H
#define CRIMES_H

// Funcionalidade 1
void crimes_create(char *nome_arq_csv, char *nome_arq_bin);

// Funcionalidade 2
void crimes_select_all(char *nome_arq_bin);

// Funcionalidade 3
void crimes_indexar(char *nome_arq_bin, char *campo, char *tipo_dado, char *nome_arq_ind);

// Funcionalidade 4
void crimes_buscar(char *nome_arq_bin, char *campo, char *tipo_dado, char *nome_arq_ind, int num_buscas);

// Funcionalidade 5
void crimes_remover(char *nome_arq_bin, char *campo, char *tipo_dado, char *nome_arq_ind, int num_remocoes);

// Funcionalidade 6
void crimes_inserir(char *nome_arq_bin, char *campo, char *tipo_dado, char *nome_arq_ind, int num_insercoes);

// Funcionalidade 7
void crimes_editar(char *nome_arq_bin, char *campo, char *tipo_dado, char *nome_arq_ind, int num_edicoes);

// Funcionalidade 8
void crimes_criar_ab(char *nome_arq_bin, char *campo, char *tipo_dado, char *nome_arq_ind);

// Funcionalidade 9
void crimes_buscar_ab(char *nome_arq_bin, char *campo, char *tipo_dado, char *nome_arq_ind, int num_buscas);

// Funcionalidade 10
void crimes_inserir_ab(char *nome_arq_bin, char *campo, char *tipo_dado, char *nome_arq_ind, int num_insercoes);

#endif