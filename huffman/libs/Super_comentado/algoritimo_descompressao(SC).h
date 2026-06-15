#ifndef ALGORITMOS_DESCOMPRESAO_H // Se a constante ALGORITMOS_DESCOMPRESAO_H não existe...
#define ALGORITMOS_DESCOMPRESAO_H // ...defina-a agora.

#include "utils.h" // Dá acesso às estruturas base e funções genéricas.

// Declara a função que usa máscaras de bits para checar se um bit específico está ligado (1) ou desligado (0).
unsigned int bit_ta_ativo(unsigned char caracter, int i);

// Declara a função que extrai os 3 bits do tamanho do lixo localizados no início do arquivo comprimido.
short int obter_lixo(FILE *arquivoCompactado);

// Declara a função que extrai os 13 bits do tamanho da árvore contidos no cabeçalho binário.
short int obter_tamanho_arvore(FILE *arquivoCompactado);

// Declara a função que mede o tamanho total do arquivo em disco em formato inteiro de 64 bits.
unsigned long long int obterTamanhoCompressao(FILE *arquivo);

// Declara a função que reconstrói a árvore de Huffman idêntica à original lendo a string em Pré-Ordem.
NoArvore* reconstruir_arvore_huffman(FILE *arquivoCompactado, short int *tamanho_arvore);

// Declara o motor de descompressão, que lê os bits restantes e traduz de volta para o arquivo de texto/imagem.
void descomprimir_arquivo_usando_huffman(FILE *arquivo_comprimido, int tamanho_lixo, unsigned long long int tamanho_arq_comprimido_sem_arvore_sem_lixo, NoArvore *arvore_huffman, FILE *arquivo_descomprimido);

#endif // Encerra o bloco de proteção do cabeçalho.