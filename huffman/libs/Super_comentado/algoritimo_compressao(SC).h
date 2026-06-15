#ifndef ALGORITMOS_COMPRESSAO_H // Se a constante ALGORITMOS_COMPRESSAO_H não existe...
#define ALGORITMOS_COMPRESSAO_H // ...defina-a agora.

#include "utils.h" // Inclui o utils.h para ter acesso às definições de estruturas e tipos.

// Declara a função que conta a frequência dos bytes do arquivo e cria a fila de prioridade inicial.
FilaPrioridade* obter_frequencia_caracter(FILE *arquivo, int arr_frequencia[TAMANHO_ASCII]);

// Declara a função que aplica o Algoritmo Guloso para montar a Árvore de Huffman a partir da fila.
NoArvore* construir_arvore_huffman(FilaPrioridade *fila);

// Declara a função recursiva que caminha pela árvore gerando os códigos binários de cada caractere.
void gerar_tabela_huffman(NoArvore *raiz, dado_objeto tabela[], unsigned int caminho_atual, int tamanho_atual);

// Declara a função que calcula quantos bits sobram sem uso no último byte do arquivo compactado.
int calcular_tamanho_lixo(int frequencia[], dado_objeto tabela[]);

// Declara a função que calcula o tamanho exato que a árvore ocupará em bytes dentro do cabeçalho.
int calcular_tamanho_arvore(NoArvore *raiz);

// Declara a função que grava os primeiros 2 bytes de metadados (lixo + tamanho da árvore) no disco.
void escrever_cabecalho_inicial(FILE *arquivo_comprimido, int tamanho_lixo, int tamanho_arvore);

// Declara a função que salva o desenho estrutural da árvore no arquivo usando varredura Pré-Ordem.
void escrever_arvore_pre_ordem(FILE *arquivo_comprimido, NoArvore *raiz);

// Declara a função que re-lê o arquivo e escreve os novos caminhos compactados bit a bit no buffer.
void gravarCodigos(FILE *arquivo_comprimido, FILE *arquivo_para_comprimir, dado_objeto tabela[]);

#endif // Encerra o bloco de proteção do cabeçalho.