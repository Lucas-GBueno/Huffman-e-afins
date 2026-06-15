#include "algoritimo_descompressao.h"

unsigned int bit_ta_ativo(unsigned char caracter, int i) {
    // Defesa: Máscara AND. Se quisermos saber se o 3º bit está ativo, criamos a 
    // máscara 0b00001000 e fazemos o AND. Se for zero, o bit está apagado.
    unsigned char mascara = (1 << i);
    return caracter & mascara;
}

short int obter_lixo(FILE *arquivoCompactado) {
    if (arquivoCompactado == NULL) return -1;

    unsigned char byte_lido;
    fread(&byte_lido, sizeof(unsigned char), 1, arquivoCompactado);
    
    // O lixo está nos 3 bits iniciais. Shiftamos 5 casas para a direita para isolá-los.
    int lixo = byte_lido >> 5; 
    
    fseek(arquivoCompactado, 0, SEEK_SET); // Rewind
    return lixo;
}

short int obter_tamanho_arvore(FILE *arquivoCompactado) {
    if (arquivoCompactado == NULL) return -1;

    unsigned char byte1, byte2;
    fread(&byte1, sizeof(unsigned char), 1, arquivoCompactado);
    fread(&byte2, sizeof(unsigned char), 1, arquivoCompactado);

    // Defesa: Zera os 3 bits de lixo no byte1 usando um AND bit a bit (0b00011111)
    byte1 &= 0b00011111; 

    // Costura o byte1 (deslocado para ser os 8 bits altos) com o byte2 (8 bits baixos)
    short int tamanho_arvore = (byte1 << 8) | byte2;
    return tamanho_arvore;
}

unsigned long long int obterTamanhoCompressao(FILE *arquivo) {
    unsigned long long int posicaoAtual = ftell(arquivo);
    fseek(arquivo, 0, SEEK_END);
    unsigned long long int tamanho = ftell(arquivo);
    fseek(arquivo, posicaoAtual, SEEK_SET);
    return tamanho;
}

NoArvore* reconstruir_arvore_huffman(FILE *arquivoCompactado, short int *tamanho_arvore) {
    if (*tamanho_arvore <= 0) return NULL;

    unsigned char byte_lido;
    fread(&byte_lido, sizeof(unsigned char), 1, arquivoCompactado);
    (*tamanho_arvore)--;

    // Defesa: Se lemos um '\\', ignoramos a semântica do Huffman e pegamos o 
    // PRÓXIMO caractere de forma literal como folha.
    if (byte_lido == '\\') {
        fread(&byte_lido, sizeof(unsigned char), 1, arquivoCompactado);
        (*tamanho_arvore)--;
        void *item = encapsular_byte(byte_lido);
        return criar_no_arvore(item, 0, NULL, NULL); // Frequência 0 (irrelevante agora)
    }

    if (byte_lido == '*') {
        // Se é um nó interno na pré-ordem, o que vem depois obrigatoriamente
        // é a subárvore esquerda, seguida da direita.
        NoArvore *esquerda = reconstruir_arvore_huffman(arquivoCompactado, tamanho_arvore);
        NoArvore *direita = reconstruir_arvore_huffman(arquivoCompactado, tamanho_arvore);
        
        void *item_interno = encapsular_byte('*');
        return criar_no_arvore(item_interno, 0, esquerda, direita);
    }

    void *item = encapsular_byte(byte_lido);
    return criar_no_arvore(item, 0, NULL, NULL);
}

void descomprimir_arquivo_usando_huffman(FILE *arquivo_comprimido, int tamanho_lixo, unsigned long long int tamanho_arq_comprimido_sem_arvore_sem_lixo, NoArvore *arvore_huffman, FILE *arquivo_descomprimido) {
    
    unsigned char byte_lido;
    NoArvore *atual = arvore_huffman;
    
    // Defesa: unsigned long long int previne "Integer Overflow" ao descompactar 
    // arquivos grandes (como vídeos pesados ou ISOs).
    unsigned long long int total_bits = (tamanho_arq_comprimido_sem_arvore_sem_lixo * 8) - tamanho_lixo;

    for (unsigned long long int i = 0; i < total_bits; i++) {
        if (i % 8 == 0) {
            fread(&byte_lido, sizeof(unsigned char), 1, arquivo_comprimido);
        }

        // Lê da esquerda para a direita (bit 7 até o bit 0)
        int bit_atual = 7 - (i % 8);

        if (bit_ta_ativo(byte_lido, bit_atual)) {
            atual = atual->direita; // Bit 1
        } else {
            atual = atual->esquerda; // Bit 0
        }

        if (e_folha(atual)) {
            unsigned char char_decodificado = extrair_byte(atual->item);
            fwrite(&char_decodificado, sizeof(unsigned char), 1, arquivo_descomprimido);
            
            atual = arvore_huffman; // Reset para a raiz para buscar a próxima letra
        }
    }
}