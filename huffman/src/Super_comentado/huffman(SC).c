#include "huffman.h"                  // Inclui o cabeçalho próprio deste módulo.
#include "algoritimo_compressao.h"    // Inclui as funções de compressão (obter_frequencia, construir_arvore...).
#include "algoritimo_descompressao.h" // Inclui as funções de descompressão (obter_lixo, reconstruir_arvore...).
#include <string.h>                   // Inclui funções padrão de strings (strncpy, strcat, strstr).

// Início da função que gerencia e executa o fluxo completo de compressão.
void comprimir(char *caminho_arquivo) {
    // Abre o arquivo original que o usuário digitou no modo de leitura binária ("rb").
    FILE *arquivo_entrada = fopen(caminho_arquivo, "rb");
    
    // Verifica se o arquivo falhou ao abrir (caminho errado, arquivo inexistente, etc).
    if (!arquivo_entrada) {
        printf("Erro ao abrir o arquivo original. Verifique o caminho.\n"); // Avisa o erro na tela.
        return; // Aborta a função de compressão imediatamente.
    }

    char caminho_saida[512]; // Declara um buffer de texto de 512 bytes para montar o nome do arquivo final.
    
    // Copia o nome do arquivo original para o buffer, deixando espaço livre no final de 6 bytes para o ".huff\0".
    strncpy(caminho_saida, caminho_arquivo, sizeof(caminho_saida) - 6);
    
    // Concatena (gruda) a extensão ".huff" no final da string do caminho de saída.
    strcat(caminho_saida, ".huff");

    // Cria/abre o novo arquivo comprimido (".huff") no modo de escrita binária puros ("wb").
    FILE *arquivo_saida = fopen(caminho_saida, "wb");
    
    // Verifica se o sistema operacional falhou ao criar o arquivo comprimido (falta de permissão, disco cheio...).
    if (!arquivo_saida) {
        printf("Erro ao criar o arquivo comprimido.\n"); // Avisa o erro na tela.
        fclose(arquivo_entrada); // Lembra de fechar o arquivo de entrada que já estava aberto para não vazar recurso.
        return; // Aborta a função.
    }

    // ==========================================================================
    // INÍCIO DAS ETAPAS GLOBAIS DE COMPRESSÃO
    // ==========================================================================
    
    int frequencias[TAMANHO_ASCII] = {0}; // Inicializa um array de 256 posições zerado para contar os caracteres.
    
    // Passo 1: Varre o arquivo de entrada, popula o vetor de frequências e gera a fila ordenada.
    FilaPrioridade *fila = obter_frequencia_caracter(arquivo_entrada, frequencias);
    
    // Passo 2: Consome a fila de prioridade para erguer a nossa Árvore de Huffman na memória Heap.
    NoArvore *raiz = construir_arvore_huffman(fila);

    dado_objeto tabela[TAMANHO_ASCII] = {0}; // Cria uma tabela de dicionário limpa (256 estruturas dado_objeto).
    
    // Passo 3: Percorre a árvore gerada e preenche a tabela com as novas codificações binárias de cada byte.
    gerar_tabela_huffman(raiz, tabela, 0, 0);

    // Passo 4: Faz os cálculos estruturais de tamanho baseados no dicionário e frequências.
    int tamanho_lixo = calcular_tamanho_lixo(frequencias, tabela); // Descobre quantos bits sobrarão no último byte.
    int tamanho_arvore = calcular_tamanho_arvore(raiz);         // Descobre quantos bytes a árvore ocupará no arquivo.

    // Passo 5: Escreve os metadados e a estrutura da árvore na cabeça do arquivo comprimido.
    escrever_cabecalho_inicial(arquivo_saida, tamanho_lixo, tamanho_arvore); // Grava os 2 bytes mágicos de controle.
    escrever_arvore_pre_ordem(arquivo_saida, raiz);                         // Grava a árvore serializada em Pré-Ordem.

    // Passo 6: Como o arquivo original foi lido até o fim na contagem, rebobinamos ele de volta para o início.
    fseek(arquivo_entrada, 0, SEEK_SET); 
    
    // Passo 7: Lê o arquivo original pela segunda vez, traduz bit a bit via dicionário e descarrega no arquivo .huff.
    gravarCodigos(arquivo_saida, arquivo_entrada, tabela);

    // ==========================================================================
    // LIMPEZA E GERENCIAMENTO DE MEMÓRIA (Evitando Memory Leaks)
    // ==========================================================================
    fclose(arquivo_entrada); // Fecha o fluxo de comunicação com o arquivo original.
    fclose(arquivo_saida);   // Fecha o fluxo de comunicação com o arquivo comprimido (salvando tudo em disco).
    free(fila);              // Libera da memória Heap a estrutura de controle do cabeçalho da Fila.
    liberar_arvore(raiz);    // Dispara o free recursivo pós-ordem para deletar todos os nós e itens da árvore.

    printf("Sucesso! Arquivo comprimido salvo como: %s\n", caminho_saida); // Imprime mensagem de vitória.
} // Fim da função comprimir.

// Início da função que gerencia e executa o fluxo completo de descompressão.
void descomprimir(char *caminho_arquivo_comprimido) {
    // Abre o arquivo compactado ".huff" no modo de leitura binária ("rb").
    FILE *arquivo_entrada = fopen(caminho_arquivo_comprimido, "rb");
    
    // Valida se o ponteiro do arquivo comprimido abriu corretamente.
    if (!arquivo_entrada) {
        printf("Erro ao abrir o arquivo comprimido.\n"); // Alerta na tela caso falhe.
        return; // Aborta a descompressão.
    }

    char caminho_saida[512]; // Buffer de 512 bytes para recompor o nome do arquivo original descompactado.
    
    // Copia o nome do arquivo compactado para a nossa variável de manipulação de string.
    strncpy(caminho_saida, caminho_arquivo_comprimido, sizeof(caminho_saida) - 1);
    
    // Procura pela substring ".huff" dentro do nome do arquivo.
    char *extensao = strstr(caminho_saida, ".huff");
    if (extensao) {
        *extensao = '\0'; // Se achar, substitui a letra '.' por '\0', "amputando" a extensão .huff do nome.
    }

    // Cria/abre o arquivo limpo (sem o .huff) no modo de escrita binária ("wb") para ser restaurado.
    FILE *arquivo_saida = fopen(caminho_saida, "wb");
    
    // Valida se o arquivo de saída pôde ser gerado no disco.
    if (!arquivo_saida) {
        printf("Erro ao criar o arquivo de saida.\n"); // Avisa o erro.
        fclose(arquivo_entrada); // Fecha o arquivo de entrada para não vazar recursos do sistema operacional.
        return; // Aborta a função.
    }

    // ==========================================================================
    // INÍCIO DAS ETAPAS GLOBAIS DE DESCOMPRESSÃO
    // ==========================================================================
    
    // Passo 1: Lê o primeiro byte absoluto do cabeçalho e calcula o tamanho do lixo em bits (shift para a direita).
    short int lixo = obter_lixo(arquivo_entrada);
    
    // Passo 2: Limpa os bits de lixo do cabeçalho e junta os bytes para descobrir o tamanho total da árvore gravada.
    short int tamanho_arvore = obter_tamanho_arvore(arquivo_entrada);
    
    // Passo 3: Pula o cabeçalho de 2 bytes e avança o cursor além do tamanho da árvore para calcular o tamanho restante de dados puros.
    unsigned long long int tamanho_total_dados = obterTamanhoCompressao(arquivo_entrada) - (2 + tamanho_arvore);

    // Passo 4: Move o ponteiro do arquivo para a posição exata onde a árvore começa e reconstrói a estrutura idêntica na memória Heap.
    NoArvore *raiz = reconstruir_arvore_huffman(arquivo_entrada, &tamanho_arvore);
    
    // Passo 5: Lê o fluxo de bits restante, caminha pela árvore ressuscitada e cospe os caracteres recuperados no arquivo de saída.
    descomprimir_arquivo_usando_huffman(arquivo_entrada, lixo, tamanho_total_dados, raiz, arquivo_saida);

    // ==========================================================================
    // LIMPEZA FINAL DE RECURSOS
    // ==========================================================================
    fclose(arquivo_entrada); // Fecha o arquivo .huff de entrada.
    fclose(arquivo_saida);   // Fecha o arquivo restaurado de saída.
    liberar_arvore(raiz);    // Varre e apaga a árvore temporária da descompressão da memória Heap.

    printf("Sucesso! Arquivo descomprimido salvo como: %s\n", caminho_saida); // Imprime a mensagem de finalização.
} // Fim da função descomprimir.