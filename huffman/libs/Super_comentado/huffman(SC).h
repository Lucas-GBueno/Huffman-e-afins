#ifndef HUFFMAN_H // Se a constante HUFFMAN_H não existe...
#define HUFFMAN_H // ...defina-a agora.

#include <stdio.h>  // Inclui recursos de manipulação de arquivos da biblioteca padrão.
#include <stdlib.h> // Inclui funções de sistema e memória.

// Declara a função pública que recebe o nome de um arquivo qualquer e gera a versão comprimida ".huff".
void comprimir(char *caminho_arquivo);

// Declara a função pública que recebe um arquivo ".huff" e recupera a sua extensão e dados originais.
void descomprimir(char *caminho_arquivo_comprimido);

#endif // Encerra o bloco de proteção do cabeçalho.