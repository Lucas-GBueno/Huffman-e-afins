#include "algoritimo_compressao.h" // Conecta as assinaturas do cabeçalho com as funções deste arquivo.

// Início da função que conta a frequência de cada byte do arquivo e os organiza em uma fila.
FilaPrioridade* obter_frequencia_caracter(FILE *arquivo, int arr_frequencia[TAMANHO_ASCII]) {
    unsigned char caracter; // Declara uma variável local de 1 byte para ler os caracteres.
    
    // Executa o laço enquanto o fread conseguir ler com sucesso blocos de 1 byte do arquivo.
    while (fread(&caracter, sizeof(unsigned char), 1, arquivo) == 1) {
        arr_frequencia[caracter]++; // Incrementa em 1 a contagem da posição correspondente ao byte lido.
    }

    FilaPrioridade *fila = criar_fila(); // Inicializa uma nova fila de prioridade vazia na memória Heap.

    // Percorre todas as 256 posições possíveis do array de frequências (tabela ASCII).
    for (int i = 0; i < TAMANHO_ASCII; i++) {
        // Se o byte atual (i) apareceu pelo menos uma vez no arquivo original...
        if (arr_frequencia[i] > 0) {
            // Cria uma caixinha na memória para guardar esse caractere de forma genérica (void*).
            void *item = encapsular_byte((unsigned char)i);
            
            // Cria um nó de árvore folha, passando o item, sua frequência e os filhos esq/dir como NULL.
            NoArvore *no_arvore = criar_no_arvore(item, arr_frequencia[i], NULL, NULL);
            
            // Insere esse nó de árvore dentro da fila, usando a frequência como critério de ordenação.
            inserir_fila(fila, no_arvore, arr_frequencia[i]);
        }
    }
    return fila; // Retorna a fila populada e perfeitamente ordenada do menor para o maior peso.
} // Fim da função obter_frequencia_caracter.

// Início da função que aglutina os nós da fila para erguer a Árvore de Huffman.
NoArvore* construir_arvore_huffman(FilaPrioridade *fila) {
    if (fila_vazia(fila)) return NULL; // Se a fila estiver vazia por algum motivo, aborta retornando nulo.

    // Executa o algoritmo guloso enquanto houver mais de 1 elemento na fila de prioridade.
    while (fila->tamanho > 1) {
        // Remove o nó com a menor frequência absoluta da fila e o batiza de 'esquerda'.
        NoArvore *esquerda = (NoArvore *)remover_fila(fila);
        
        // Remove o segundo nó com a menor frequência restante e o batiza de 'direita'.
        NoArvore *direita = (NoArvore *)remover_fila(fila);

        // Soma as frequências dos dois nós filhos para descobrir o peso do novo nó pai.
        int freq_combinada = esquerda->frequencia + direita->frequencia;
        
        // Cria um caractere asterisco encapsulado para marcar visualmente que este é um nó interno (não é folha).
        void *item_interno = encapsular_byte('*'); 
        
        // Aloca o nó pai na Heap, ligando os ponteiros esq/dir nos nós que acabamos de remover da fila.
        NoArvore *pai = criar_no_arvore(item_interno, freq_combinada, esquerda, direita);

        // Insere o nó pai de volta na fila de prioridade para ser reavaliado nos próximos laços.
        inserir_fila(fila, pai, freq_combinada);
    }
    // Quando o laço termina, resta apenas 1 nó na fila. Esse nó único é a raiz da árvore completa.
    return (NoArvore *)remover_fila(fila); 
} // Fim da função construir_arvore_huffman.

// Início da função recursiva que varre a árvore para mapear os novos caminhos em bits.
void gerar_tabela_huffman(NoArvore *raiz, dado_objeto tabela[], unsigned int caminho_atual, int tamanho_atual) {
    if (raiz == NULL) return; // Caso base da recursão: se o nó atual for nulo, apenas retorna e encerra a ramificação.

    // Verifica se o nó atual é uma folha (contém um caractere original).
    if (e_folha(raiz)) {
        unsigned char byte = extrair_byte(raiz->item); // Extrai o caractere guardado dentro do void*.
        tabela[byte].conteudo = caminho_atual;       // Salva a sequência de bits calculada na tabela do dicionário.
        tabela[byte].tamanho = tamanho_atual;         // Salva quantos bits de profundidade esse caminho levou.
        return; // Retorna e encerra este ramo da recursão, pois chegamos ao fim de um caractere.
    }

    // Se não for folha, continua descendo. 
    // Para a esquerda: empurra os bits para o lado (<< 1), injetando um bit '0' no final do caminho.
    gerar_tabela_huffman(raiz->esquerda, tabela, caminho_atual << 1, tamanho_atual + 1);
    
    // Para a direita: empurra os bits para o lado e aplica um operador OR com 1, injetando um bit '1' no final.
    gerar_tabela_huffman(raiz->direita, tabela, (caminho_atual << 1) | 1, tamanho_atual + 1);
} // Fim da função gerar_tabela_huffman.

// Início da função que calcula a sobra de bits (lixo) que ficará no último byte gravado.
int calcular_tamanho_lixo(int frequencia[], dado_objeto tabela[]) {
    long long total_bits = 0; // Inicializa um contador de bits de 64 bits em zero.
    
    // Varre todas as 256 posições possíveis da tabela ASCII.
    for (int i = 0; i < TAMANHO_ASCII; i++) {
        // Se o caractere apareceu no arquivo...
        if (frequencia[i] > 0) {
            // Multiplica a quantidade de vezes que ele apareceu pelo tamanho do seu novo código em bits e soma ao total.
            total_bits += ((long long)frequencia[i] * tabela[i].tamanho);
        }
    }
    // O operador % descobre quantos bits foram usados no byte incompleto.
    int resto = total_bits % 8;
    
    // Se o resto for zero, o arquivo preencheu bytes cheios perfeitamente (lixo = 0). 
    // Caso contrário, o lixo é a diferença matemática para completar 8 bits (8 - resto).
    return (resto == 0) ? 0 : (8 - resto);
} // Fim da função calcular_tamanho_lixo.

// Início da função recursiva que calcula quantos bytes a árvore ocupará quando for salva no cabeçalho.
int calcular_tamanho_arvore(NoArvore *raiz) {
    if (raiz == NULL) return 0; // Se o nó for nulo, ele ocupa 0 bytes.
    int tamanho = 1; // Cada nó válido da árvore ocupa obrigatoriamente 1 byte na string de gravação.
    
    if (e_folha(raiz)) {
        unsigned char byte = extrair_byte(raiz->item); // Extrai o caractere original da folha.
        // Se o caractere da folha for um '*' ou uma '\\' literal do arquivo, precisaremos de um caractere 
        // de escape para não bugar a descompressão. Esse caractere de escape gasta +1 byte no cabeçalho.
        if (byte == '*' || byte == '\\') {
            tamanho++; 
        }
    }
    
    // Soma o tamanho atual com o tamanho calculado recursivamente nas subárvores esquerda e direita.
    tamanho += calcular_tamanho_arvore(raiz->esquerda);
    tamanho += calcular_tamanho_arvore(raiz->direita);
    return tamanho; // Retorna o tamanho total da árvore em bytes.
} // Fim da função calcular_tamanho_arvore.

// Início da função que mescla e grava os primeiros 2 bytes de controle (cabeçalho) do arquivo comprimido.
void escrever_cabecalho_inicial(FILE *arquivo_comprimido, int tamanho_lixo, int tamanho_arvore) {
    // Monta o byte 1: desloca os 3 bits do lixo para o topo (<< 5) e aplica um OR com os 5 bits mais altos do tamanho_arvore (>> 8).
    unsigned char byte1 = (tamanho_lixo << 5) | (tamanho_arvore >> 8);
    
    // Monta o byte 2: aplica uma máscara AND de 8 bits (0xFF) para capturar isoladamente os 8 bits mais baixos do tamanho da árvore.
    unsigned char byte2 = tamanho_arvore & 0xFF; 
    
    // Escreve o primeiro byte composto no arquivo comprimido em disco.
    fwrite(&byte1, sizeof(unsigned char), 1, arquivo_comprimido);
    
    // Escreve o segundo byte composto no arquivo comprimido em disco.
    fwrite(&byte2, sizeof(unsigned char), 1, arquivo_comprimido);
} // Fim da função escrever_cabecalho_inicial.

// Início da função recursiva que salva o desenho da árvore no arquivo em modo Pré-Ordem (Raiz -> Esq -> Dir).
void escrever_arvore_pre_ordem(FILE *arquivo_comprimido, NoArvore *raiz) {
    if (raiz == NULL) return; // Caso base: se o nó for nulo, encerra a função.
    unsigned char byte = extrair_byte(raiz->item); // Recupera o caractere guardado no nó.

    // Se o nó for uma folha e contiver um caractere de controle literal ('*' ou '\\')...
    if (e_folha(raiz) && (byte == '*' || byte == '\\')) {
        unsigned char escape = '\\'; // Cria o byte de escape barra invertida.
        // Grava a barra invertida imediatamente antes do caractere para avisar ao descompressor que a folha é literal.
        fwrite(&escape, sizeof(unsigned char), 1, arquivo_comprimido);
    }

    // Grava o byte do nó atual (seja ele o caractere original ou o '*' de nó interno).
    fwrite(&byte, sizeof(unsigned char), 1, arquivo_comprimido);
    
    escrever_arvore_pre_ordem(arquivo_comprimido, raiz->esquerda); // Dispara a recursão para o ramo esquerdo.
    escrever_arvore_pre_ordem(arquivo_comprimido, raiz->direita);  // Dispara a recursão para o ramo direito.
} // Fim da função escrever_arvore_pre_ordem.

// Início da função que re-lê o arquivo original e traduz seus caracteres em fluxo compactado bit a bit.
void gravarCodigos(FILE *arquivo_comprimido, FILE *arquivo_para_comprimir, dado_objeto tabela[]) {
    unsigned char buffer = 0; // Cria um acumulador de bits zerado (daqui sairão os bytes para gravação).
    int bits_no_buffer = 0;   // Contador para saber quantos bits já foram guardados dentro do buffer atual.
    int caractere;            // Variável do tipo int para receber a leitura do fgetc e checar o EOF com segurança.

    // Executa um laço lendo caractere por caractere do arquivo original até chegar ao fim (EOF).
    while ((caractere = fgetc(arquivo_para_comprimir)) != EOF) {
        dado_objeto cod = tabela[caractere]; // Busca no dicionário o código binário equivalente àquele caractere.

        // Percorre o código binário armazenado da esquerda para a direita.
        for (int i = cod.tamanho - 1; i >= 0; i--) {
            // Aplica máscaras bitwise para isolar o bit atual da sequência que queremos gravar (0 ou 1).
            unsigned int bit = (cod.conteudo >> i) & 1;
            
            // Abre espaço no buffer empurrando os bits existentes para a esquerda (<< 1) e injeta o novo bit usando OR.
            buffer = (buffer << 1) | bit;
            bits_no_buffer++; // Incrementa o medidor de preenchimento do buffer.

            // Se o contador atingir 8, significa que completamos com sucesso 1 byte completo de dados compactados.
            if (bits_no_buffer == 8) {
                // Despeja esse byte preenchido de forma definitiva no arquivo comprimido em disco.
                fwrite(&buffer, sizeof(unsigned char), 1, arquivo_comprimido);
                buffer = 0;         // Reseta o acumulador para receber novos bits do zero.
                bits_no_buffer = 0; // Zera o contador de bits acumulados.
            }
        }
    }

    // Tratamento de fim de arquivo (Sobra de bits): se o arquivo acabou mas restaram bits órfãos no buffer...
    if (bits_no_buffer > 0) {
        int bits_lixo = 8 - bits_no_buffer; // Descobre quantas posições vazias restaram para fechar o bloco de 8 bits.
        buffer = buffer << bits_lixo;       // Empurra os bits válidos para o topo do byte, preenchendo o fundo com zeros.
        
        // Escreve o último byte parcial (contendo o nosso lixo calculado) no disco.
        fwrite(&buffer, sizeof(unsigned char), 1, arquivo_comprimido);
    }
} // Fim da função gravarCodigos.