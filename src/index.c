#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "auxiliar.h"
#include "registros.h"
#include "index.h"

// Trata uma string e a copia pra um INDEX do tipo string
void copia_para_index(STR_INDEX *str_ind, char *buffer)
{
    for(int i = 0; i < 12; i++)
        str_ind->chave[i] = buffer[i];

    for(int i = len_campo(buffer) - 1; i < 12; i++)
        str_ind->chave[i] = LIXO;
}

// Comparando dois INDEX do tipo string para o quick sort
int comp_str(const void *a, const void *b)
{
    STR_INDEX *index_a = (STR_INDEX *)a;
    STR_INDEX *index_b = (STR_INDEX *)b;

    // Percorrendo as strings
    for(int i = 0; i < 12; i++)
    {
        // Caso os caracteres difiram, retornamos a diferença entre eles
        if(index_a->chave[i] != index_b->chave[i])
            return (index_a->chave[i] - index_b->chave[i]);
    }
    // Caso as strings sejam iguais, retornamos 0
    return (index_a->ByteOffset - index_b->ByteOffset);
}
// Comparando dois INDEX inteiros para o quick sort
int comp_int(const void *a, const void *b)
{
    INT_INDEX *index_a = (INT_INDEX *)a;
    INT_INDEX *index_b = (INT_INDEX *)b;

    // Retornando a diferença entre os inteiros
    return (index_a->chave - index_b->chave);
}

// Monta um vetor de INDEX do tipo inteiro
void montar_vec_int_index(INT_INDEX *vec_index, int num_regs, char *campo, FILE *arq_bin)
{
    long int BOs_atual = sizeof(char) + 2 * sizeof(int) + sizeof(long int);
    REGISTRO reg;

    // Percorrendo o arquivo de dados
    for(int i = 0; i < num_regs; i++)
    {
        // Lendo o registro
        bin_ler_registro(&reg, arq_bin);

        // Caso o REGISTRO seja removido, atribuímos uma chave nula, para que o mesmo não seja indexado
        if(reg.removido == '1')
            vec_index[i].chave = -1;        

        else if(strcmp(campo, "idCrime") == 0) // Caso o campo indexado seja ID
            vec_index[i].chave = reg.id;            
        else                                   // Caso seja ARTIGO
            vec_index[i].chave = reg.artigo;

        // Atribuíndo ao INDEX o ByteOffset da atual iteração (ByteOffset do REGISTRO no arquivo de dados)
        vec_index[i].ByteOffset = BOs_atual;

        // Incrementando o ByteOffset
        BOs_atual += tam_reg(&reg);
    }
    // Ordenando o vetor de INDEX
    qsort(vec_index, num_regs, sizeof(INT_INDEX), comp_int);
}
// Monta um vetor de INDEX do tipo string
void montar_vec_str_index(STR_INDEX *vec_index, int num_regs, char *campo, FILE *arq_bin)
{
    long int BOs_atual = sizeof(char) + 2 * sizeof(int) + sizeof(long int);
    REGISTRO reg;

    // Percorrendo o arquivo de dados
    for(int i = 0; i < num_regs; i++)
    {
        // Lendo o registro
        bin_ler_registro(&reg, arq_bin);
        memset(&(vec_index[i]), 0, sizeof(STR_INDEX));

        // Caso o REGISTRO seja removido, atribuímos uma chave nula, para que o mesmo não seja indexado
        if(reg.removido == '1')
            vec_index[i].chave[0] = LIXO;

        else if(strcmp(campo, "dataCrime") == 0)      // Caso o campo indexado seja DATA
            copia_para_index(&(vec_index[i]), reg.data);      
        else if(strcmp(campo, "lugarCrime") == 0)     // Caso seja LOCAL
            copia_para_index(&(vec_index[i]), reg.local);   
        else if(strcmp(campo, "descricaoCrime") == 0) // Caso seja DESCRICAO
            copia_para_index(&(vec_index[i]), reg.descricao); 
        else                                          // Caso seja MARCA
            copia_para_index(&(vec_index[i]), reg.marca); 

        // Atribuíndo ao INDEX o ByteOffset da atual iteração (ByteOffset do REGISTRO no arquivo de dados)
        vec_index[i].ByteOffset = BOs_atual;

        // Incrementando o ByteOffset
        BOs_atual += tam_reg(&reg);
    }
    // Ordenando o vetor de INDEX
    qsort(vec_index, num_regs, sizeof(STR_INDEX), comp_str);
}

// Escrevendo o CABECALHO de INDEX no arquivo de index
void escreve_cab_index(CAB_INDEX *cab_ind, FILE *arq_index)
{
    fwrite(&(cab_ind->status), sizeof(char), 1, arq_index);
    fwrite(&(cab_ind->NumRegs), sizeof(int), 1, arq_index);
}
// Escreve um INDEX do tipo inteiro no arquivo de INDEX
void escreve_int_index(INT_INDEX *index, CAB_INDEX *cab_ind, FILE *arq_index)
{
    // Escrevendo o INDEX
    fwrite(&(index->chave), sizeof(int), 1, arq_index);
    fwrite(&(index->ByteOffset), sizeof(long int), 1, arq_index);

    // Incrementando o número de 
    cab_ind->NumRegs++;
}
// Escreve um INDEX do tipo string no arquivo de INDEX
void escreve_str_index(STR_INDEX *index, CAB_INDEX *cab_ind, FILE *arq_index)
{
    // Escrevendo um INDEX do tipo string no arquivo de index
    fwrite(&(index->chave), sizeof(char), 12, arq_index);
    fwrite(&(index->ByteOffset), sizeof(long int), 1, arq_index);
    cab_ind->NumRegs++;
}

// Lê o CABECALHO do arquivo de INDEX
void bin_ler_cab_index(CAB_INDEX *cab_ind, FILE *arq_index)
{
    fread(&(cab_ind->status), sizeof(char), 1, arq_index);
    fread(&(cab_ind->NumRegs), sizeof(int), 1, arq_index);
}
// Lendo o CABECALHO do arquivo de INDEX e tratando as exceções
void tenta_ler_cab_index(CAB_INDEX *cab_ind, FILE **arq_index)
{
    // Lendo o CABECALHO do arquivo de INDEX
    bin_ler_cab_index(cab_ind, *arq_index);

    // Verificando o status do CABECALHO
    if(cab_ind->status == INCONSISTENTE)
    {
        fclose(*arq_index);
        erro_arquivo();
        return;
    }
}
// Lê um INDEX do tipo inteiro do arquivo de INDEX
void bin_ler_int_index(INT_INDEX *int_ind, FILE *arq_index)
{
    fread(&(int_ind->chave), sizeof(int), 1, arq_index);
    fread(&(int_ind->ByteOffset), sizeof(long int), 1, arq_index);
}
// Lê um INDEX do tipo string do arquivo de INDEX
void bin_ler_str_index(STR_INDEX *str_ind, FILE *arq_index)
{
    fread(&(str_ind->chave), sizeof(char), 12, arq_index);
    fread(&(str_ind->ByteOffset), sizeof(long int), 1, arq_index);
}

// Busca binária em um vetor de INDEX do tipo inteiro
int busca_bin_int(INT_INDEX *vec, int inicio, int fim, int chave)
{
    int meio = (inicio + fim) / 2;
    
    if(inicio > fim)
        return -1;
    
    if(vec[meio].chave == chave)
        return meio;

    else if(vec[meio].chave < chave)
        return busca_bin_int(vec, meio + 1, fim, chave);

    else
        return busca_bin_int(vec, inicio, meio - 1, chave);
}
// Busca a primeira posição do vetor de INDEX do tipo inteiro que apresente a chave buscada
int buscar_int_index(INT_INDEX *vec_index, int tam, int chave)
{
    // Buscando uma posição com busca binária
    int pos = busca_bin_int(vec_index, 0, tam, chave);
    if(pos != -1) // Caso o INDEX exista
    {
        // Enquanto o INDEX anterior apresentar a mesma chave, decrementamos a posição
        while(pos > 0 && vec_index[pos - 1].chave == chave)
            pos--;
    }
    return pos; // Retornando a posição
}

// Busca binária em um vetor de INDEX do tipo string
int busca_bin_str(STR_INDEX *vec, int inicio, int fim, char *chave)
{
    int meio = (inicio + fim) / 2;

    if(inicio > fim)
        return -1;

    if(strcmp(vec[meio].chave, chave) == 0)
        return meio;

    else if(strcmp(vec[meio].chave, chave) > 0)
        return busca_bin_str(vec, inicio, meio - 1, chave);

    else
        return busca_bin_str(vec, meio + 1, fim, chave);
}
// Busca a primeira posição do vetor de INDEX do tipo string que apresente a chave buscada
int buscar_str_index(STR_INDEX *vec_index, int tam, char *chave)
{
    // Buscando uma posição com busca binária
    int pos = busca_bin_str(vec_index, 0, tam, chave);
    if(pos != -1) // Caso o INDEX exista
    {
        // Enquanto o INDEX anterior apresentar a mesma chave, decrementamos a posição
        while(pos > 0 && strcmp(vec_index[pos - 1].chave, chave) == 0)
            pos--;
    }
    return pos; // Retornando a posição
}

// Obs.: As próximas duas funções realizam uma busca sequencial, as quais são iniciadas pelo fim
// do vetor devido à logica de inserção de INDEX utilizada na funcionalidade 7, o que otimiza nosso código
// Busca sequencialmente por um INDEX do tipo inteiro no vetor, utilizando como chave de busca seu ByteOffset
int busca_index_int_no_vetor(INT_INDEX *vec_index_int, int tam, long int bos_chave)
{
    for(int i = tam - 1; i >= 0; i--)
    {
        if(vec_index_int[i].ByteOffset == bos_chave)
            return i;
    }
    return -1;
}
// Busca sequencialmente por um INDEX do tipo string no vetor, utilizando como chave de busca seu ByteOffset
int busca_index_str_no_vetor(STR_INDEX *vec_index_str, int tam, long int bos_chave)
{
    for(int i = tam - 1; i >= 0; i--)
    {
        if(vec_index_str[i].ByteOffset == bos_chave)
            return i;
    }
    return -1;
}

// Lê um vetor do arquivo de INDEX
void ler_vec_index(INT_INDEX **vec_index_int, STR_INDEX **vec_index_str, CAB_INDEX *cab_ind, char *tipo_dado, FILE *arq_index)
{
    // Caso a indexação seja do tipo inteiro
    if(strcmp(tipo_dado, "inteiro") == 0)
    {
        // Alocando memória em um vetor de INDEX do tipo inteiro e lendo seus dados
        *vec_index_int = calloc (cab_ind->NumRegs, sizeof(INT_INDEX));
        for(int i = 0; i < cab_ind->NumRegs; i++)
            bin_ler_int_index(&(*vec_index_int)[i], arq_index); 
    }
    // Caso a indexação seja do tipo string
    else
    {
        // Alocando memória em um vetor de INDEX do tipo string e lendo seus dados
        *vec_index_str = calloc (cab_ind->NumRegs, sizeof(STR_INDEX));
        for(int i = 0; i < cab_ind->NumRegs; i++)
        {
            bin_ler_str_index(&(*vec_index_str)[i], arq_index);
            trata_string((*vec_index_str)[i].chave, 12);
        }
    }
}

// Ordena um vetor de index usando o Qsort
void ordenar_vec_index(INT_INDEX *vec_index_int, STR_INDEX *vec_index_str, CAB_INDEX cab_ind, char *tipo_dado)
{
    if(strcmp(tipo_dado, "inteiro") == 0) // Orednação de INDEX do tipo inteiro
        qsort(vec_index_int, cab_ind.NumRegs, sizeof(INT_INDEX), comp_int); 
    else                                  // Ordenação de INDEX do tipo string
        qsort(vec_index_str, cab_ind.NumRegs, sizeof(STR_INDEX), comp_str);
}

// Reescreve o arquivo de INDEX a partir de um vetor de INDEX já inicializado
void re_indexar(char *nome_arq_ind, INT_INDEX *vec_index_int, STR_INDEX *vec_index_str, CAB_INDEX *cab_ind, int max, char *tipo_dado)
{
    // Abrindo (se possível) o arquivo de INDEX
    FILE *arq_index = fopen(nome_arq_ind, "wb+");
    checar_null(arq_index);

    // Escrevendo o CABECALHO de INDEX
    cab_ind->status = INCONSISTENTE; 
    cab_ind->NumRegs = 0;
    escreve_cab_index(cab_ind, arq_index);

    // Caso a indexação seja do tipo INTEIRO
    if(strcmp(tipo_dado, "inteiro") == 0)
    {
        // Percorrendo o vetor de INDEX
        for(int i = 0; i < max; i++)
        {
            // Caso a chave do INDEX não seja nula, escrevemos no arquivo
            if(vec_index_int[i].chave != -1)
                escreve_int_index(&(vec_index_int[i]), cab_ind, arq_index);
        }
    }
    // Caso a indexação seja do tipo string
    else
    {
        // Percorrendo o vetor de INDEX
        for(int i = 0; i < max; i++)
        {
            // Caso sua chave não seja nula
            if(vec_index_str[i].chave[0] != '$')
            {
                // Tratando o dado conforme as especificações
                for(int j = 0; j < 12; j++)
                {
                    if(vec_index_str[i].chave[j] == '\0')
                        vec_index_str[i].chave[j] = LIXO;
                }
                // Escrevendo o INDEX no arquivo
                escreve_str_index(&(vec_index_str[i]), cab_ind, arq_index);
            }
        }
    }
    // Reescrevendo o CABECALHO
    fseek(arq_index, 0 , SEEK_SET);
    cab_ind->status = CONSISTENTE;
    escreve_cab_index(cab_ind, arq_index);

    // Fechando o arquivo de INDEX
    fclose(arq_index);
}

// Remove um INDEX do vetor
void remove_index(INT_INDEX *vec_index_int, STR_INDEX *vec_index_str, int tam, long int byte_offset, char *tipo_dado)
{
    int pos_index;
    
    // Caso a indexação seja do tipo inteiro
    if(strcmp(tipo_dado, "inteiro") == 0)
    {
        // Procura o INDEX no vetor
        pos_index = busca_index_int_no_vetor(vec_index_int, tam, byte_offset);

        // Caso exista, marca sua chave com um valor nulo, para que o mesmo não seja indexado
        if(pos_index != -1)
            vec_index_int[pos_index].chave = -1;
    }
    // Caso a indexação seja do tipo string
    else
    {
        // Procura o INDEX no vetor
        pos_index = busca_index_str_no_vetor(vec_index_str, tam, byte_offset);

        // Caso exista, marca sua chave com um valor nulo, para que o mesmo não seja indexado
        if(pos_index != -1)
            strcpy(vec_index_str[pos_index].chave, "$$$$$$$$$$$$");
    }  
}

// Adiciona um novo INDEX ao fim de um vetor de INDEX
void adiciona_index(INT_INDEX *vec_index_int, STR_INDEX *vec_index_str, REGISTRO reg, CABECALHO cab, char *campo, int pos)
{
    if(strcmp(campo, "idCrime") == 0) // Caso o campo indexado seja ID           
    {
        vec_index_int[pos].chave = reg.id;
        vec_index_int[pos].ByteOffset = cab.proxByteOffset;
    }
    else if(strcmp(campo, "dataCrime") == 0) // Caso seja DATA
    {
        copia_para_index(&(vec_index_str[pos]), reg.data);
        vec_index_str[pos].ByteOffset = cab.proxByteOffset;
    }
    else if(strcmp(campo, "numeroArtigo") == 0) // Caso seja ARTIGO
    {
        vec_index_int[pos].chave = reg.artigo;
        vec_index_int[pos].ByteOffset = cab.proxByteOffset;
    }
    else if(strcmp(campo, "lugarCrime") == 0) // Caso seja LOCAL
    {
        copia_para_index(&(vec_index_str[pos]), reg.local);
        vec_index_str[pos].ByteOffset = cab.proxByteOffset;
    }
    else if(strcmp(campo, "descricaoCrime") == 0) // Caso seja DESCRICAO
    { 
        copia_para_index(&(vec_index_str[pos]), reg.descricao);
        vec_index_str[pos].ByteOffset = cab.proxByteOffset;
    }
    else // Caso seja MARCA
    {
        copia_para_index(&(vec_index_str[pos]), reg.marca);
        vec_index_str[pos].ByteOffset = cab.proxByteOffset;
    }
}

// Altera o vetor de INDEX com base nas edições anteriormente realizadas
void edita_vec_index(INT_INDEX **vec_index_int, STR_INDEX **vec_index_str, CAB_INDEX *cab_ind, REGISTRO *reg_edit, long int BOs_atual, long int ultimo_BOs, char *campo, int flag_cabe)
{
    // Caso o campo indexado seja do tipo inteiro
    if(strcmp(campo, "idCrime") == 0 || strcmp(campo, "numeroArtigo") == 0)
    {
        // Buscando a posição do REGISTRO recém editado no vetor de INDEX
        int pos_index = busca_index_int_no_vetor(*vec_index_int, cab_ind->NumRegs, BOs_atual);
        if(pos_index != -1) // Caso o REGISTRO tenha sido indexado
        {
            // Copiando o conteúdo editado para o vetor de INDEX

            if(strcmp(campo, "idCrime") == 0) // Caso o campo indexado seja ID
                (*vec_index_int)[pos_index].chave = reg_edit->id;
            else                              // Caso seja seja ARTIGO
                (*vec_index_int)[pos_index].chave = reg_edit->artigo;

            // Caso o REGISTRO tenha sido reescrito, seu ByteOffset corresponde ao último REGISTRO no arquivo de dados
            if(flag_cabe == 0) 
                (*vec_index_int)[pos_index].ByteOffset = ultimo_BOs;
        }
    }
    // Caso seja do tipo string
    else
    {
        // Buscando a posição do REGISTRO recém editado no vetor de INDEX
        int pos_index = busca_index_str_no_vetor((*vec_index_str), cab_ind->NumRegs, BOs_atual);
        if(pos_index != -1) // Caso o REGISTRO tenha sido indexado
        {
            // Copiando o conteúdo editado para o vetor de INDEX

            if(strcmp(campo, "dataCrime") == 0)           // Caso o campo indexado seja DATA
                copia_para_index(&((*vec_index_str)[pos_index]), reg_edit->data);
            else if(strcmp(campo, "lugarCrime") == 0)     // Caso seja LOCAL
                copia_para_index(&((*vec_index_str)[pos_index]), reg_edit->local);
            else if(strcmp(campo, "descricaoCrime") == 0) // Caso seja DESCRICAO
                copia_para_index(&((*vec_index_str)[pos_index]), reg_edit->descricao);
            else                                          // Caso seja MARCA
                copia_para_index(&((*vec_index_str)[pos_index]), reg_edit->marca);

            // Caso o REGISTRO tenha sido reescrito, seu ByteOffset corresponde ao último REGISTRO no arquivo de dados
            if(flag_cabe == 0)
                (*vec_index_str)[pos_index].ByteOffset = ultimo_BOs;
        }
    }
}

// Liberando a memória alocada para o vetor de INDEX
void liberar_vetor_index(INT_INDEX **vec_index_int, STR_INDEX **vec_index_str)
{
    if(*vec_index_int == NULL)
    {
        free(*vec_index_str);
        *vec_index_str = NULL;
    }
    else
    {
        free(*vec_index_int);
        *vec_index_int = NULL;
    }
}