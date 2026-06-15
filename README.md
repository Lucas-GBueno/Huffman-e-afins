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


# Guia de Execução dos Módulos

## Passo 0: Preparando a Máquina (Pré-requisitos)

Antes de rodar qualquer código, a máquina precisa das ferramentas de tradução (compiladores).

1. **Para os códigos em C:** É necessário ter o compilador GCC instalado (no Windows, geralmente feito via MinGW ou MSYS2). Para testar se já existe, abra o PowerShell e digite:
   ```
   gcc --version
   ```
   Se aparecer um texto com a versão, está pronto.

2. **Para os gráficos em R:** É preciso baixar e instalar dois programas gratuitos nesta ordem:
   - **R (O motor base):** Baixe no site oficial (CRAN R-Project).
   - **RStudio (A interface visual):** Baixe no site oficial da Posit. O RStudio evita todos os problemas chatos de configuração do Windows.

---

## Módulo 1: Compactador de Huffman

Este projeto tem os arquivos divididos em pastas (`src` e `libs`), então precisamos ensinar o compilador a olhar para os lugares certos.

1. Abra o PowerShell e navegue até a pasta raiz do Huffman:
   ```powershell
   cd C:\Caminho\Para\O\Seu\Projeto\Estrutura-de-Dados\huffman
   ```

2. Compile o código executando este comando exato (ele junta tudo e busca os cabeçalhos na pasta `libs`):
   ```powershell
   gcc -I libs -o huffman src/main.c src/utils.c src/algoritimo_compressao.c src/algoritimo_descompressao.c src/huffman.c
   ```

3. Execute o programa:
   ```powershell
   .\huffman.exe
   ```

4. O menu interativo aparecerá na tela. Basta digitar `1` para comprimir ou `2` para descomprimir, e fornecer o caminho do arquivo de texto quando solicitado.

---

## Módulo 2: SAT Solver (Lógica Booleana)

Este é um arquivo único, focado em descobrir se uma fórmula lógica pode ser verdadeira.

1. Navegue até a pasta do SAT Solver:
   ```powershell
   cd C:\Caminho\Para\O\Seu\Projeto\Estrutura-de-Dados\Sat_Smt\SAT
   ```

2. Compile o código:
   ```powershell
   gcc -o sat_solver sat_solver.c
   ```

3. Execute o programa passando um arquivo de teste `.cnf` como argumento:
   ```powershell
   .\sat_solver.exe arquivo_de_teste.cnf
   ```

---

## Módulo 3: SMT Solver (Lógica Matemática)

Este módulo lida com inequações e usa funções matemáticas avançadas de arredondamento.

1. Navegue até a pasta do SMT Solver:
   ```powershell
   cd C:\Caminho\Para\O\Seu\Projeto\Estrutura-de-Dados\Sat_Smt\SMT
   ```

2. Compile o código (atenção à flag `-lm` no final — ela é **obrigatória** para habilitar a biblioteca matemática `<math.h>`):
   ```powershell
   gcc -o smt_solver smt_solver.c -lm
   ```

3. Execute o programa passando um arquivo de teste `.smt` como argumento:
   ```powershell
   .\smt_solver.exe arquivo_de_teste.smt
   ```

---

## Módulo 4: Contagem e Plotagem (C + RStudio)

Este módulo funciona em duas etapas: o C gera os dados em uma planilha (CSV) e o RStudio desenha o gráfico baseado nesses dados.

### Etapa A: Gerar os Dados (C)

1. Navegue até a pasta raiz da contagem:
   ```powershell
   cd C:\Caminho\Para\O\Seu\Projeto\Estrutura-de-Dados\Contagem_Plotagem
   ```

2. Compile e execute o gerador:
   ```powershell
   gcc -o gerador_dados main.c
   .\gerador_dados.exe
   ```
   O programa rodará silenciosamente e criará um arquivo `dados.csv` na pasta de saída.

### Etapa B: Gerar o Gráfico (RStudio)

1. Abra o aplicativo **RStudio** no seu computador.

2. No menu superior esquerdo, clique em **File > Open File...** e abra o arquivo `plotagem.R` (que está dentro da pasta `output`).

3. **O Pulo do Gato (Sincronizar a pasta):** No menu superior, clique em **Session > Set Working Directory > To Source File Location**. Isso garante que o RStudio encontre o arquivo `.csv` gerado pelo C.

4. Para rodar o código e ver o gráfico, clique no botão **Source** (ícone de uma página com uma setinha azul) na parte superior do painel onde está o seu código.

5. O gráfico aparecerá no painel inferior direito, na aba **Plots**. Você pode clicar em **Export** logo acima dele para salvar a imagem.
