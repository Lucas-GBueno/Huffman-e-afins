# Estrutura-de-Dados

# Trabalho Prático: Algoritmos e Estruturas de Dados

Este documento descreve os requisitos e as especificações para os 4 tópicos do projeto da disciplina.

## 1. Seminário
O objetivo é apresentar um seminário sobre um algoritmo ou estrutura de dados que não foi vista em sala de aula. 

**Temas disponíveis para escolha:**
* Sweep Line com Árvores Balanceadas
* Índices em Bancos de Dados com Árvore B+
* Máquina de Turing usando Pilhas
* Detecção de Fraudes Bancárias em Grafos
* Algoritmo *Topological Sort*
* *Segment Tree*
* *Binary Decision Diagrams* (BDD)

> **Regra:** Pode haver no máximo **2 equipes** com o mesmo tema para o seminário.

---

## 2. Codificação de Huffman
Implementação do algoritmo de Huffman (ver Aula 20 para detalhes da teoria).

**Requisitos Obrigatórios:**
* O sistema deve realizar a compactação e descompactação de **QUAISQUER** tipos de arquivos.
* **Restrição Técnica:** TODAS as estruturas utilizadas no código têm que estar implementadas utilizando ponteiros genéricos (`void *`).

---

## 3. SAT e SMT Solver
Implementação de solucionadores lógicos e matemáticos.

**Requisitos:**
* Implementação de um **SAT Solver** usando o modelo DIMACS.
* Implementação de um **SMT Solver** usando a Teoria LIA (*Linear Integer Arithmetic*).

---

## 4. Contagem e Plotagem de Comparações
Desenvolver um programa `main` com o objetivo de armazenar muitos dados em duas estruturas de dados diferentes para comparar o desempenho de busca entre elas.

**Pares de estruturas (Escolher UM para implementar):**
* Lista *versus* Árvore de Busca Binária
* Árvore desbalanceada *versus* Árvore AVL
* Fila de Prioridades sem *Heap* *versus* Fila de Prioridades com *Heap*

**Metodologia e Execução:**
1. Faça vários sorteios de números.
2. Conte o número de comparações necessárias para encontrar o número sorteado em cada uma das estruturas.
   * *Exemplo:* Número sorteado `34` $\rightarrow$ Número de comparações na árvore desbalanceada = `13` | Número de comparações na AVL = `4`.
3. Colete o histórico dessas comparações e gere um gráfico demonstrando as diferenças.

> **Apresentação:** Os dados devem ser gerados e plotados **NA HORA** da apresentação! É obrigatório o uso de **R** ou **Matlab** para a geração dos gráficos.
