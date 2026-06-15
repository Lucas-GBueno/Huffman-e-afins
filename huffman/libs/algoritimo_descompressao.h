#ifndef ALGORITMOS_DESCOMPRESAO_H
#define ALGORITMOS_DESCOMPRESAO_H

#include "utils.h"

/* ==========================================================================
 * FLUXO DE DESCOMPRESSÃO (LEITURA DO CABEÇALHO E RECONSTRUÇÃO)
 * Defesa: O fluxo lê estritamente o cabeçalho antes de começar a extrair bits.
 * ========================================================================== */

/**
 * bit_ta_ativo: Mascaramento bitwise fundamental.
 * Gatilho: Faz um AND bit a bit (c & (1 << i)) para checar se o bit atual é 0 ou 1.
 */
unsigned int bit_ta_ativo(unsigned char caracter, int i);

/**
 * Leitura de metadados binários.
 * Gatilho (Obter Lixo): Aplica shift à direita (>> 5) para isolar os 3 primeiros bits do arquivo.
 * Gatilho (Tamanho Árvore): Limpa os bits de lixo e combina dois bytes usando deslocamento de 8 bits.
 */
short int obter_lixo(FILE *arquivoCompactado);
short int obter_tamanho_arvore(FILE *arquivoCompactado);

/**
 * obterTamanhoCompressao: Medição do arquivo comprimido em disco.
 * Gatilho: Usa fseek no fim (SEEK_END) e ftell para obter o tamanho total em bytes do binário.
 */
unsigned long long int obterTamanhoCompressao(FILE *arquivo);

/**
 * reconstruir_arvore_huffman: Reconstrói a árvore via Pré-Ordem.
 * Gatilho: Se ler '*', sabe que é nó interno e chama recursivamente esquerda/direita.
 * Se ler '\\', sabe que o próximo caractere é uma folha literal, tratando escapes de caracteres de controle.
 */
NoArvore* reconstruir_arvore_huffman(FILE *arquivoCompactado, short int *tamanho_arvore);

/**
 * descomprimir_arquivo_usando_huffman: Tradução de bits para bytes.
 * Gatilho: Usa unsigned long long int no cálculo de bits totais para evitar estouro de inteiro 
 * em arquivos grandes. Percorre os bits direcionando o ponteiro na árvore até achar uma folha.
 */
void descomprimir_arquivo_usando_huffman(FILE *arquivo_comprimido, int tamanho_lixo, unsigned long long int tamanho_arq_comprimido_sem_arvore_sem_lixo, NoArvore *arvore_huffman, FILE *arquivo_descomprimido);

#endif