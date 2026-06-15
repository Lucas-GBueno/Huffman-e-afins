#include "algoritimo_compressao.h"

FilaPrioridade* obter_frequencia_caracter(FILE *arquivo, int arr_frequencia[TAMANHO_ASCII]) {
    unsigned char caracter;
    // Defesa: fread é melhor que fgetc aqui porque trata dados binários puros
    // de forma mais segura, sem confundir o EOF com bytes reais como 0xFF.
    while (fread(&caracter, sizeof(unsigned char), 1, arquivo) == 1) {
        arr_frequencia[caracter]++;
    }

    FilaPrioridade *fila = criar_fila();

    for (int i = 0; i < TAMANHO_ASCII; i++) {
        if (arr_frequencia[i] > 0) {
            void *item = encapsular_byte((unsigned char)i);
            NoArvore *no_arvore = criar_no_arvore(item, arr_frequencia[i], NULL, NULL);
            inserir_fila(fila, no_arvore, arr_frequencia[i]);
        }
    }
    return fila;
}

NoArvore* construir_arvore_huffman(FilaPrioridade *fila) {
    if (fila_vazia(fila)) return NULL;

    // Defesa: Lógica Greedy (Guloso). Pega os dois menores da fila, 
    // soma, e joga o pai de volta na fila. Repete até sobrar a raiz (tamanho 1).
    while (fila->tamanho > 1) {
        NoArvore *esquerda = (NoArvore *)remover_fila(fila);
        NoArvore *direita = (NoArvore *)remover_fila(fila);

        int freq_combinada = esquerda->frequencia + direita->frequencia;
        
        // Nó interno recebe '*' apenas por convenção para depuração.
        void *item_interno = encapsular_byte('*'); 
        NoArvore *pai = criar_no_arvore(item_interno, freq_combinada, esquerda, direita);

        inserir_fila(fila, pai, freq_combinada);
    }
    return (NoArvore *)remover_fila(fila);
}

void gerar_tabela_huffman(NoArvore *raiz, dado_objeto tabela[], unsigned int caminho_atual, int tamanho_atual) {
    if (raiz == NULL) return;

    if (e_folha(raiz)) {
        unsigned char byte = extrair_byte(raiz->item);
        tabela[byte].conteudo = caminho_atual;
        tabela[byte].tamanho = tamanho_atual;
        return;
    }

    // Defesa: Bitwise Operations. Ao descer para a esquerda, empurramos o caminho 1 bit 
    // para a esquerda (coloca um 0 no final). Para a direita, empurramos e fazemos OR com 1.
    gerar_tabela_huffman(raiz->esquerda, tabela, caminho_atual << 1, tamanho_atual + 1);
    gerar_tabela_huffman(raiz->direita, tabela, (caminho_atual << 1) | 1, tamanho_atual + 1);
}

int calcular_tamanho_lixo(int frequencia[], dado_objeto tabela[]) {
    long long total_bits = 0;
    for (int i = 0; i < TAMANHO_ASCII; i++) {
        if (frequencia[i] > 0) {
            total_bits += ((long long)frequencia[i] * tabela[i].tamanho);
        }
    }
    // O lixo é a quantidade de bits necessários para fechar o último byte (múltiplo de 8).
    int resto = total_bits % 8;
    return (resto == 0) ? 0 : (8 - resto);
}

int calcular_tamanho_arvore(NoArvore *raiz) {
    if (raiz == NULL) return 0;
    int tamanho = 1; 
    
    if (e_folha(raiz)) {
        unsigned char byte = extrair_byte(raiz->item);
        // Se a folha for um asterisco ou barra originais do arquivo, precisamos 
        // avisar na descompressão escapando com '\'. Isso custa 2 bytes no arquivo final.
        if (byte == '*' || byte == '\\') {
            tamanho++; 
        }
    }
    
    tamanho += calcular_tamanho_arvore(raiz->esquerda);
    tamanho += calcular_tamanho_arvore(raiz->direita);
    return tamanho;
}

void escrever_cabecalho_inicial(FILE *arquivo_comprimido, int tamanho_lixo, int tamanho_arvore) {
    // Defesa: Isolamos os bits do tamanho_lixo (3 bits) empurrando-os para a extrema esquerda do byte1.
    // Depois, pegamos os 5 bits superiores do tamanho_arvore e colamos do lado.
    unsigned char byte1 = (tamanho_lixo << 5) | (tamanho_arvore >> 8);
    // Mascara (0xFF) garante que pegamos apenas os 8 bits finais do inteiro de tamanho_arvore.
    unsigned char byte2 = tamanho_arvore & 0xFF; 
    
    fwrite(&byte1, sizeof(unsigned char), 1, arquivo_comprimido);
    fwrite(&byte2, sizeof(unsigned char), 1, arquivo_comprimido);
}

void escrever_arvore_pre_ordem(FILE *arquivo_comprimido, NoArvore *raiz) {
    if (raiz == NULL) return;
    unsigned char byte = extrair_byte(raiz->item);

    // Escape para diferenciar nós internos de folhas que por acaso sejam '*'
    if (e_folha(raiz) && (byte == '*' || byte == '\\')) {
        unsigned char escape = '\\';
        fwrite(&escape, sizeof(unsigned char), 1, arquivo_comprimido);
    }

    fwrite(&byte, sizeof(unsigned char), 1, arquivo_comprimido);
    escrever_arvore_pre_ordem(arquivo_comprimido, raiz->esquerda);
    escrever_arvore_pre_ordem(arquivo_comprimido, raiz->direita);
}

void gravarCodigos(FILE *arquivo_comprimido, FILE *arquivo_para_comprimir, dado_objeto tabela[]) {
    unsigned char buffer = 0;
    int bits_no_buffer = 0;
    int caractere;

    // Varre o arquivo original pela segunda vez.
    while ((caractere = fgetc(arquivo_para_comprimir)) != EOF) {
        dado_objeto cod = tabela[caractere];

        // Varre o caminho binário da tabela da direita para a esquerda.
        for (int i = cod.tamanho - 1; i >= 0; i--) {
            // Isola o bit atual do código
            unsigned int bit = (cod.conteudo >> i) & 1;
            
            // Defesa: Vai empurrando o buffer para dar espaço ao novo bit.
            buffer = (buffer << 1) | bit;
            bits_no_buffer++;

            // Quando fecha 1 byte (8 bits), descarrega no disco e zera o buffer.
            if (bits_no_buffer == 8) {
                fwrite(&buffer, sizeof(unsigned char), 1, arquivo_comprimido);
                buffer = 0;
                bits_no_buffer = 0;
            }
        }
    }

    // Defesa Crítica: Se o arquivo terminou e sobrou bit no buffer, 
    // empurramos eles para a esquerda (fechando com zeros à direita). Esse é o nosso "lixo".
    if (bits_no_buffer > 0) {
        int bits_lixo = 8 - bits_no_buffer;
        buffer = buffer << bits_lixo; 
        fwrite(&buffer, sizeof(unsigned char), 1, arquivo_comprimido);
    }
}