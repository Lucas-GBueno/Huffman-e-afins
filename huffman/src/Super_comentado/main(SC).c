#include "utils.h"   // Dá acesso à função mensagem_inicial e ao cabeçalho geral de estruturas.
#include "huffman.h" // Conecta a main com as funções globais de comprimir e descomprimir.

// Início da função principal, o ponto de partida obrigatório de qualquer programa em C.
int main() {
    int escolha;       // Declara uma variável inteira local chamada "escolha" para registrar a opção do menu.
    char caminho[512]; // Cria um array (vetor) de caracteres de 512 posições para armazenar o caminho do arquivo.

    // Inicia um laço de repetição do tipo do-while (garante que o menu vai rodar pelo menos uma vez).
    do {
        // Invoca a função de menu e armazena o número retornado (0, 1 ou 2) na variável "escolha".
        escolha = mensagem_inicial();
        
        // Verifica se o usuário digitou 0 (opção de fechar o programa).
        if (escolha == 0) {
            break; // Quebra o laço do-while imediatamente, direcionando o programa para o encerramento.
        } 
        // Se a escolha não for 0, mas também for diferente de 1 (comprimir) E diferente de 2 (descomprimir)...
        else if (escolha != 1 && escolha != 2) {
            printf("\nOpção inválida! Tente novamente.\n\n"); // Alerta o usuário sobre o dígito incorreto.
            continue; // Força o laço a pular o resto do bloco e voltar direto para o início do do-while (reexibe o menu).
        }

        // Se passou pelas validações acima, a escolha é válida (1 ou 2). Pede o arquivo ao usuário.
        printf("Digite o caminho do arquivo: ");
        
        // Lê o texto digitado até o usuário apertar espaço ou enter. 
        // O "%255s" limita a leitura a no máximo 255 caracteres, atuando como defesa contra estouro de buffer (Buffer Overflow).
        scanf("%255s", caminho); 

        // Se o usuário escolheu a opção 1...
        if (escolha == 1) {
            comprimir(caminho); // Dispara a função mestre que inicia todo o processo de compactação do arquivo informado.
        } 
        // Caso contrário, se o usuário escolheu a opção 2...
        else if (escolha == 2) {
            descomprimir(caminho); // Dispara a função mestre que inicia a leitura do cabeçalho e descompactação do .huff.
        }
        
        // Imprime uma linha estética no terminal para separar visualmente as rodadas de uso do programa.
        printf("\n-----------------------------------\n");

    } while (escolha != 0); // O laço continuará repetindo enquanto o valor da variável "escolha" for diferente de zero.

    return 0; // Retorna zero para o sistema operacional, indicando que o programa encerrou sua execução sem erros críticos.
} // Fim da função principal main.