#include <stdio.h>    // Necessário para funções de entrada e saída (printf, fopen, fgets, perror).
#include <stdlib.h>   // Necessário para gerenciamento de memória (malloc, realloc, free) e conversão de texto para número (atoi).
#include <stdbool.h>  // Permite o uso do tipo booleano (true e false).
#include <string.h>   // Fornece funções para fatiar e buscar coisas em textos (strchr, strstr, strlen).
#include <math.h>     // Fornece as funções de arredondamento matemático: floor (piso) e ceil (teto).

// ==========================================
// 1) ESTRUTURA DE DADOS GENÉRICA (void*)
// ==========================================

// Estrutura que representa o nosso array dinâmico sem tipo definido.
typedef struct {
    void **elementos; // Array de ponteiros void*. O duplo asterisco significa "ponteiro apontando para um array de ponteiros".
    int capacidade;   // Guarda o tamanho máximo que o array suporta no momento.
    int quantidade;   // Guarda quantos itens reais estão preenchidos.
} VetorDirecional;

// Função responsável por instanciar (criar) um novo vetor na memória.
VetorDirecional* criar_vetor(void) {
    // Pede ao sistema operacional (malloc) um bloco de memória do tamanho exato da struct VetorDirecional.
    VetorDirecional *v = (VetorDirecional*)malloc(sizeof(VetorDirecional));
    if (!v) return NULL; // Se não houver memória RAM disponível, retorna nulo para evitar travamentos.
    
    v->capacidade = 4; // Define a capacidade inicial do vetor como 4 posições.
    v->quantidade = 0; // Como o vetor acabou de nascer, ele tem 0 elementos.
    
    // Aloca a memória real para o array interno que vai segurar os ponteiros genéricos.
    v->elementos = (void**)malloc(sizeof(void*) * v->capacidade);
    return v; // Devolve o endereço de memória do vetor pronto para uso.
}

// Função que coloca um novo item genérico dentro do vetor.
void vetor_inserir(VetorDirecional *v, void *item) {
    // Checa se o vetor lotou. Se a quantidade for igual à capacidade, precisamos de uma "casa maior".
    if (v->quantidade == v->capacidade) {
        v->capacidade *= 2; // A estratégia de dobrar de tamanho garante alta performance.
        // O realloc pega o array antigo, transfere tudo para um array novo com o dobro do tamanho e apaga o velho.
        v->elementos = (void**)realloc(v->elementos, sizeof(void*) * v->capacidade);
    }
    // Coloca o item na primeira posição vazia e, em seguida (++), atualiza o contador de quantidade.
    v->elementos[v->quantidade++] = item;
}

// Função que devolve um item guardado em uma posição específica do vetor.
void* vetor_obter(const VetorDirecional *v, int indice) {
    // Trava de segurança: impede que o programa tente acessar índices negativos ou que ainda não existem.
    if (indice < 0 || indice >= v->quantidade) return NULL;
    // Devolve o ponteiro void* que está guardado na gaveta escolhida.
    return v->elementos[indice];
}

// Função para destruir o vetor e liberar toda a memória alocada (evita memory leak).
void vetor_limpar(VetorDirecional *v, void (*liberar_elemento)(void*)) {
    if (!v) return; // Proteção: se o vetor for nulo, não tenta limpar nada.
    
    // Se o usuário passou uma função de limpeza customizada, limpa os itens de dentro primeiro.
    if (liberar_elemento) {
        for (int i = 0; i < v->quantidade; i++) {
            liberar_elemento(v->elementos[i]); // Chama a função customizada para cada item.
        }
    }
    free(v->elementos); // Agora que os itens foram limpos, destrói o array interno de ponteiros.
    free(v);            // Destrói a estrutura principal do vetor.
}

// ==========================================
// 2) MODELAGEM DA TEORIA LIA
// ==========================================

// Estrutura que representa matematicamente a nossa inequação: Ax + B (op) C
typedef struct {
    int a;  // O coeficiente numérico colado no 'x' (ex: o 2 de "2x").
    int b;  // O número que soma ou subtrai (ex: o +1 de "+ 1").
    int op; // Código numérico para o operador (1 = "<=", 2 = ">=").
    int c;  // O valor numérico que fica do lado direito da inequação.
} Inequacao;

// Função auxiliar (callback) que sabe liberar uma Inequacao da memória.
void liberar_inequacao(void *eq) {
    // Converte (faz cast) do ponteiro void* cego para Inequacao* e libera a memória.
    free((Inequacao*)eq);
}

// ==========================================
// 3) PARSER DO ARQUIVO (Leitor de Expressões Literais)
// ==========================================

// Função que abre o arquivo de texto e transforma o texto puro em structs numéricas de inequações.
void mapear_arquivo_smt(const char *caminho, VetorDirecional *equacoes) {
    FILE *arquivo = fopen(caminho, "r"); // Abre o arquivo no modo "read" (apenas leitura).
    if (!arquivo) { // Verifica se a abertura falhou (arquivo não existe, erro de digitação, etc).
        perror("Falha ao abrir o arquivo especificado");
        exit(EXIT_FAILURE); // Mata o programa imediatamente informando falha.
    }

    char buffer[1024]; // String temporária que vai armazenar os caracteres lidos linha por linha.

    // O fgets lê a linha atual do arquivo e guarda no 'buffer'. Retorna nulo quando o arquivo acaba.
    while (fgets(buffer, sizeof(buffer), arquivo)) {
        
        // Pula sumariamente a linha caso o primeiro caractere seja um comentário, cabeçalho ou quebra de linha.
        if (buffer[0] == 'c' || buffer[0] == 'p' || buffer[0] == '\n' || buffer[0] == '\r') continue;

        // --- FASE 1: LIMPEZA DA STRING ---
        // Vamos criar uma nova string ('linha_limpa') que é igual à original, mas sem espaços em branco.
        char linha_limpa[1024];
        int j = 0;
        // Percorre cada letra do texto que veio do arquivo...
        for (int i = 0; buffer[i] != '\0'; i++) {
            // Se não for espaço, nem tab, nem quebra de linha (enter)...
            if (buffer[i] != ' ' && buffer[i] != '\t' && buffer[i] != '\n' && buffer[i] != '\r') {
                linha_limpa[j++] = buffer[i]; // Copia o caractere útil para a linha_limpa.
            }
        }
        linha_limpa[j] = '\0'; // Adiciona o caractere nulo obrigatório para finalizar strings no C.

        // Validações pós-limpeza para evitar tentar processar linhas "vazias" disfarçadas de espaços.
        if (strlen(linha_limpa) == 0) continue; 
        if (linha_limpa[0] == 'c') continue;    

        // Aloca espaço para a nova equação na memória.
        Inequacao *nova_eq = (Inequacao*)malloc(sizeof(Inequacao));
        // Preenche com valores neutros para evitar comportamentos inesperados.
        nova_eq->a = 1; nova_eq->b = 0; nova_eq->op = 0; nova_eq->c = 0;

        // --- FASE 2: BUSCA E FATIAMENTO (PARSING) ---
        // A função strchr procura a primeira aparição da letra 'x' e devolve um ponteiro para ela.
        char *ptr_x = strchr(linha_limpa, 'x');
        
        // A função strstr procura a aparição da palavra "<=" e devolve um ponteiro para ela.
        char *ptr_op = strstr(linha_limpa, "<=");
        
        // Se encontrou o sinal "<=", marca o operador como 1.
        if (ptr_op) {
            nova_eq->op = 1;
        } else {
            // Se não encontrou "<=", procura pelo ">="
            ptr_op = strstr(linha_limpa, ">=");
            if (ptr_op) nova_eq->op = 2; // Se achou, marca como 2.
        }

        // Se a linha matemática não tiver a letra 'x' ou não tiver um operador suportado, descarta.
        if (!ptr_x || !ptr_op) { 
            free(nova_eq); 
            continue; 
        }

        // --- EXTRAÇÃO DO 'A' (O que vem antes do x) ---
        if (ptr_x == linha_limpa) {
            // Se o 'x' é a primeira letra da linha limpa (ex: "x-2>=1"), o multiplicador é implicitamente 1.
            nova_eq->a = 1; 
        } else if (ptr_x == linha_limpa + 1 && linha_limpa[0] == '-') {
            // Se o 'x' está na segunda posição e a primeira letra é um '-', significa "-x" (multiplicador -1).
            nova_eq->a = -1;
        } else if (ptr_x == linha_limpa + 1 && linha_limpa[0] == '+') {
            // Mesmo raciocínio, mas para o cenário raro de alguém digitar "+x".
            nova_eq->a = 1;  
        } else {
            // Se chegou aqui, existe um número de verdade antes do x (ex: o "2" do "2x").
            *ptr_x = '\0'; // Mágica: trocamos o caractere 'x' por '\0', quebrando a string ao meio!
            nova_eq->a = atoi(linha_limpa); // Agora mandamos ler a string (que só vai até antes do x) e converte em inteiro.
        }

        // --- EXTRAÇÃO DO 'B' (O que está entre o x e o <= ou >=) ---
        // Como transformamos o 'x' em um '\0', andamos uma posição pra frente na memória para achar o começo do B.
        char *ptr_b = ptr_x + 1; 
        
        *ptr_op = '\0'; // Segunda mágica: trocamos o sinal de menor/maior por '\0', quebrando a string de novo!
        
        if (strlen(ptr_b) > 0) {
            // Se sobrou algum texto aqui (ex: "+1" ou "-2"), converte para inteiro e guarda em B.
            nova_eq->b = atoi(ptr_b); 
        }

        // --- EXTRAÇÃO DO 'C' (O que está depois do <= ou >=) ---
        // Andamos duas posições para frente do operador (pois o sinal de "<=" ou ">=" ocupa 2 letras).
        char *ptr_c = ptr_op + 2; 
        nova_eq->c = atoi(ptr_c); // Lê os números restantes do lado direito.

        // Por fim, injeta a struct Inequacao perfeitamente mapeada dentro do vetor genérico.
        vetor_inserir(equacoes, (void*)nova_eq);
    }
    // Fim da leitura. Fecha a ponte de acesso ao arquivo no sistema operacional.
    fclose(arquivo); 
}

// ==========================================
// 4) MOTOR SMT (Normalização e Interseção)
// ==========================================

// Função que aplica a regra matemática de limites para avaliar se as equações podem coexistir
void resolver_smt_lia(VetorDirecional *equacoes) {
    if (equacoes->quantidade == 0) {
        printf("Erro: Nenhuma equacao valida foi lida.\n");
        return; // Sai da função mais cedo caso o arquivo não tivesse nada útil
    }

    // Define barreiras teóricas (infinitos) para o início dos cálculos matemáticos.
    int limite_inferior = -999999;
    int limite_superior = 999999;

    // Itera (passa por) cada uma das inequações processadas do arquivo.
    for (int i = 0; i < equacoes->quantidade; i++) {
        // Converte o void* salvo no vetor de volta para um Inequacao* que possamos manipular
        Inequacao *eq = (Inequacao*)vetor_obter(equacoes, i);
        
        // Fase 1 do Isolamento: Subtrai o valor B do lado C. Salvamos como double para evitar perdas de precisão na futura divisão.
        double lado_direito = (double)(eq->c - eq->b);
        double a = (double)eq->a; 

        if (a == 0) continue; // Proteção estrita de segurança matemática (nunca dividir por zero).

        // Fase 2 do Isolamento: Passa o A dividindo o que sobrou no lado direito.
        double valor_exato = lado_direito / a;
        int op_atual = eq->op; // Resgata o operador original salvo na memória.

        // Propriedade das Inequações: Se dividirmos por um número negativo, o sinal é obrigatoriamente invertido.
        if (a < 0) {
            op_atual = (op_atual == 1) ? 2 : 1; // Transforma menor-igual em maior-igual e vice-versa.
        }

        // Se a inequação resultante for "x <= algo"...
        if (op_atual == 1) { 
            // O comando floor pega o limite inteiro exato "pra baixo". Ex: x <= 4.5 vira x <= 4 (pois 5 estouraria).
            int limite = (int)floor(valor_exato);
            // Reduz o "teto" total apenas se o novo valor encontrado for ainda mais restritivo.
            if (limite < limite_superior) limite_superior = limite;
        } 
        // Se a inequação resultante for "x >= algo"...
        else if (op_atual == 2) { 
            // O comando ceil pega o limite inteiro exato "pra cima". Ex: x >= 2.3 vira x >= 3 (pois 2 seria inválido).
            int limite = (int)ceil(valor_exato);
            // Eleva o "piso" total apenas se o novo valor encontrado for ainda mais restritivo.
            if (limite > limite_inferior) limite_inferior = limite;
        }
    }

    // --- CHECAGEM DA INTERSEÇÃO ---
    // A matemática só é satisfatível (SAT) se o piso do intervalo não tiver ultrapassado o teto.
    if (limite_inferior <= limite_superior) { 
        printf("SAT! Solucoes na intersecao:\n");

        // Casos de saída adaptados para cenários onde uma das pontas do limite não existe
        if (limite_inferior == -999999 && limite_superior == 999999) {
            printf("x E Todos os Numeros Inteiros (Reta Infinita)\n"); // Nenhuma restrição imposta
        } 
        else if (limite_inferior == -999999) {
            printf("Limites exatos: x <= %d\n", limite_superior); // Apenas um teto existe
        } 
        else if (limite_superior == 999999) {
            printf("Limites exatos: x >= %d\n", limite_inferior); // Apenas um piso existe
        } 
        else {
            // O cenário ideal: existe um início e um fim.
            printf("x E [%d, %d]\n", limite_inferior, limite_superior);
            printf("Inteiros validos: ");
            // Imprime individualmente todos os números pertencentes a esse intervalo exato.
            for (int x = limite_inferior; x <= limite_superior; x++) {
                printf("x=%d ", x); 
                // Coloca o 'or' se este não for o último número do intervalo a ser impresso.
                if (x < limite_superior) printf("or "); 
            }
            printf("\n");
        }
    } else {
        // Se os valores não batem (ex: matemática apontou x <= 2 e x >= 5), é insatisfatível.
        printf("UNSAT! Limites em conflito (x >= %d e x <= %d)\n", limite_inferior, limite_superior);
    }
}

// ==========================================
// 5) PONTO DE ENTRADA
// ==========================================

// Função disparada no início absoluto da execução do programa. argc = num parâmetros, argv = textos parâmetros
int main(int argc, char *argv[]) {
    // Se o array de parâmetros não tiver pelo menos o nome do executável (1) e o nome do arquivo (2).
    if (argc < 2) {
        printf("Modo de uso: %s <arquivo.smt>\n", argv[0]);
        return EXIT_FAILURE; // Reporta erro código 1 pro Windows/Linux
    }

    // Inicializa o coração da estrutura de dados: nosso vetor dinâmico.
    VetorDirecional *equacoes = criar_vetor();
    
    // Repassa o nome do arquivo recebido pelo terminal e o vetor vazio para o parser agir.
    mapear_arquivo_smt(argv[1], equacoes);
    
    // Repassa o vetor populado para as avaliações lógicas do solver.
    resolver_smt_lia(equacoes);
    
    // Devolve toda a memória do vetor dinâmico para o sistema operacional.
    vetor_limpar(equacoes, liberar_inequacao);
    
    return EXIT_SUCCESS; // Reporta encerramento perfeito, código 0.
}