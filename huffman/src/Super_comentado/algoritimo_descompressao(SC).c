#include "algoritimo_descompressao.h" // Conecta as assinaturas do cabeçalho com as funções deste arquivo.

// Início da função que checa se um bit específico dentro de um byte está ligado (1) ou desligado (0).
unsigned int bit_ta_ativo(unsigned char caracter, int i) {
    // Cria uma máscara binária empurrando o bit '1' exatamente para a posição 'i' que queremos testar.
    unsigned char mascara = (1 << i);
    
    // Aplica o operador AND (&) bit a bit. Se o resultado for diferente de zero, o bit na posição 'i' está ativo (1).
    return caracter & mascara;
} // Fim da função bit_ta_ativo.

// Início da função que extrai a quantidade de bits de lixo escondida nos metadados do arquivo.
short int obter_lixo(FILE *arquivoCompactado) {
    if (arquivoCompactado == NULL) return -1; // Se o ponteiro do arquivo for inválido, aborta retornando erro (-1).

    unsigned char byte_lido; // Declara uma variável local de 1 byte para ler os dados do disco.
    
    // Lê o primeiro byte absoluto do arquivo compactado (o byte de controle 1).
    fread(&byte_lido, sizeof(unsigned char), 1, arquivoCompactado);
    
    // Como o tamanho do lixo ocupa os 3 bits mais altos desse byte, empurramos ele 5 posições para a direita (>> 5).
    int lixo = byte_lido >> 5; 
    
    // Reseta o ponteiro de leitura do arquivo de volta para a posição inicial zero (SEEK_SET).
    fseek(arquivoCompactado, 0, SEEK_SET); 
    return lixo; // Retorna o valor numérico do lixo (um inteiro de 0 a 7).
} // Fim da função obter_lixo.

// Início da função que calcula o tamanho total em nós que a árvore serializada possui no cabeçalho.
short int obter_tamanho_arvore(FILE *arquivoCompactado) {
    if (arquivoCompactado == NULL) return -1; // Proteção contra ponteiros nulos.

    unsigned char byte1, byte2; // Declara duas variáveis para capturar os 2 primeiros bytes do arquivo.
    
    // Lê o primeiro byte (que contém lixo + 5 bits superiores do tamanho da árvore).
    fread(&byte1, sizeof(unsigned char), 1, arquivoCompactado);
    
    // Lê o segundo byte (que contém os 8 bits inferiores restantes do tamanho da árvore).
    fread(&byte2, sizeof(unsigned char), 1, arquivoCompactado);

    // Limpa os 3 bits de lixo do byte1 aplicando um AND binário com a máscara 0b00011111 (0x1F).
    byte1 &= 0b00011111; 

    // Junta os pedaços: empurra o byte1 8 posições para a esquerda para virar a parte alta e cola o byte2 usando OR.
    short int tamanho_arvore = (byte1 << 8) | byte2;
    return tamanho_arvore; // Retorna o tamanho total da árvore em bytes/nós.
} // Fim da função obter_tamanho_arvore.

// Início da função de 64 bits que mede o tamanho total do arquivo comprimido em disco.
unsigned long long int obterTamanhoCompressao(FILE *arquivo) {
    // Salva a posição em que a leitura do arquivo estava no momento (para não quebrar fluxos em andamento).
    unsigned long long int posicaoAtual = ftell(arquivo);
    
    // Força o cursor de leitura do arquivo a ir até o último byte físico dele (SEEK_END).
    fseek(arquivo, 0, SEEK_END);
    
    // O ftell() agora nos devolve a distância exata do início até o fim, ou seja, o tamanho total do arquivo.
    unsigned long long int tamanho = ftell(arquivo);
    
    // Devolve o cursor de leitura exatamente para onde ele estava antes da medição iniciar.
    fseek(arquivo, posicaoAtual, SEEK_SET);
    return tamanho; // Retorna o tamanho total calculado em bytes.
} // Fim da função obterTamanhoCompressao.

// Início da função recursiva que ressuscita a árvore de Huffman idêntica à original lendo a string em Pré-Ordem.
NoArvore* reconstruir_arvore_huffman(FILE *arquivoCompactado, short int *tamanho_arvore) {
    // Caso base: se o contador de nós da árvore zerar, significa que terminamos de reconstruí-la.
    if (*tamanho_arvore <= 0) return NULL;

    unsigned char byte_lido; // Variável auxiliar para ler o caractere atual do fluxo da árvore.
    
    // Lê 1 byte do arquivo (um pedaço da árvore serializada).
    fread(&byte_lido, sizeof(unsigned char), 1, arquivoCompactado);
    (*tamanho_arvore)--; // Decrementa o contador de bytes restantes da árvore usando o ponteiro.

    // Tratamento de caractere de escape: se o byte lido for uma barra invertida '\\'...
    if (byte_lido == '\\') {
        // Ignora a barra e lê IMEDIATAMENTE o próximo byte do arquivo de forma puramente literal.
        fread(&byte_lido, sizeof(unsigned char), 1, arquivoCompactado);
        (*tamanho_arvore)--; // Decrementa novamente o contador, pois consumimos o caractere escapado.
        
        // Encapsula esse caractere em um void* genérico.
        void *item = encapsular_byte(byte_lido);
        
        // Cria um nó folha definitivo na memória e o retorna (frequência vira 0 pois é irrelevante agora).
        return criar_no_arvore(item, 0, NULL, NULL); 
    }

    // Se o byte lido for um asterisco (*), a lógica do Huffman dita que este é um NÓ INTERNO.
    if (byte_lido == '*') {
        // Dispara a recursão para construir toda a ramificação do filho da esquerda primeiro.
        NoArvore *esquerda = reconstruir_arvore_huffman(arquivoCompactado, tamanho_arvore);
        
        // Dispara a recursão para construir toda a ramificação do filho da direita em seguida.
        NoArvore *direita = reconstruir_arvore_huffman(arquivoCompactado, tamanho_arvore);
        
        // Cria um asterisco genérico para envelopar o conteúdo do nó interno.
        void *item_interno = encapsular_byte('*');
        
        // Aloca o nó pai conectando os braços esquerdo e direito que a recursão acabou de levantar.
        return criar_no_arvore(item_interno, 0, esquerda, direita);
    }

    // Se não caiu em nenhum caso acima, o byte lido é uma folha normal contendo um caractere comum.
    void *item = encapsular_byte(byte_lido);
    return criar_no_arvore(item, 0, NULL, NULL); // Aloca e retorna o nó folha correspondente.
} // Fim da função reconstruir_arvore_huffman.

// Início da função que lê a fita de bits comprimidos, navega pela árvore e traduz tudo para o arquivo original.
void descomprimir_arquivo_usando_huffman(FILE *arquivo_comprimido, int tamanho_lixo, unsigned long long int tamanho_arq_comprimido_sem_arvore_sem_lixo, NoArvore *arvore_huffman, FILE *arquivo_descomprimido) {
    
    unsigned char byte_lido;           // Variável para carregar o bloco atual de 8 bits que está sendo processado.
    NoArvore *atual = arvore_huffman; // Cria um ponteiro navegador que começa posicionado na raiz da árvore.
    
    // Descobre a quantidade exata de bits válidos de dados multiplicando os bytes por 8 e subtraindo os bits de lixo.
    unsigned long long int total_bits = (tamanho_arq_comprimido_sem_arvore_sem_lixo * 8) - tamanho_lixo;

    // Inicia o laço principal que vai processar bit por bit até esgotar o total_bits calculado.
    for (unsigned long long int i = 0; i < total_bits; i++) {
        
        // Toda vez que o índice for múltiplo de 8 (0, 8, 16, 24...), significa que precisamos carregar o próximo byte do disco.
        if (i % 8 == 0) {
            fread(&byte_lido, sizeof(unsigned char), 1, arquivo_comprimido);
        }

        // Mapeia o bit atual de forma decrescente: o primeiro bit lido de um byte é o bit 7, o último é o bit 0.
        int bit_atual = 7 - (i % 8);

        // Invoca a função de teste de máscara. Se o bit atual do byte_lido estiver ligado (1)...
        if (bit_ta_ativo(byte_lido, bit_atual)) {
            atual = atual->direita; // Navega com o ponteiro para o filho da direita (ramo do bit 1).
        } else {
            atual = atual->esquerda; // Caso contrário (bit 0), navega para o filho da esquerda (ramo do bit 0).
        }

        // Após mover o ponteiro, checa se ele pousou em um nó folha (que armazena um caractere original).
        if (e_folha(atual)) {
            unsigned char char_decodificado = extrair_byte(atual->item); // Abre o void* e puxa o caractere de volta.
            
            // Grava o caractere traduzido imediatamente no nosso arquivo de saída em disco.
            fwrite(&char_decodificado, sizeof(unsigned char), 1, arquivo_descomprimido);
            
            atual = arvore_huffman; // Reseta o ponteiro navegador de volta para a raiz da árvore para iniciar a próxima busca.
        }
    }
} // Fim da função descomprimir_arquivo_usando_huffman.