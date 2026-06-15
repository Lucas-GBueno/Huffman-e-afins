#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include <stdlib.h>

/* ==========================================================================
 * INTERFACES PÚBLICAS DO MÓDULO HUFFMAN
 * Defesa: Facade Pattern simplificado. A main só conhece estas duas funções,
 * escondendo toda a complexidade de ponteiros e estruturas do usuário final.
 * ========================================================================== */

// Coordena o fluxo de abertura, contagem, compressão e fechamento de memória dinâmica.
void comprimir(char *caminho_arquivo);

// Coordena a leitura de metadados, reconstrução da árvore e escrita do arquivo traduzido.
void descomprimir(char *caminho_arquivo_comprimido);

#endif