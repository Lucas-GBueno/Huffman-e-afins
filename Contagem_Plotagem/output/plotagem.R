# Define o diretório de trabalho (se necessário)
# setwd("caminho/para/sua/pasta")

# Lê o arquivo CSV gerado pelo programa em C
dados <- read.csv("dados.csv")

# Encontra o valor máximo de comparações para ajustar o limite do Eixo Y
max_y <- max(max(dados$Comparacoes_Lista), max(dados$Comparacoes_Arvore))

# Plota a linha da Lista (em vermelho)
plot(dados$Sorteio, dados$Comparacoes_Lista, type="l", col="red", lwd=2,
     main="Comparação de Desempenho: Lista vs Árvore Binária",
     xlab="Número do Sorteio (Busca)", 
     ylab="Quantidade de Comparações",
     ylim=c(0, max_y))

# Sobrepõe a linha da Árvore Binária (em azul)
lines(dados$Sorteio, dados$Comparacoes_Arvore, col="blue", type="l", lwd=2)

# Adiciona a legenda no topo
legend("topright", legend=c("Lista Encadeada", "Árvore de Busca Binária"),
       col=c("red", "blue"), lty=1, lwd=2)