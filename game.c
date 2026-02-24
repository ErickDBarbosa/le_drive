#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Lista de cenários e alternativas (mecânica do jogo)
CenarioPratico cenarios_praticos[10] = {
    {"O jogador acabou de entrar no carro, mas esqueceu de ajustar o retrovisor interno antes de ligar o motor. O que ele deve fazer?",
     {"Ligar o motor e sair normalmente.", "Ajustar imediatamente o retrovisor e o banco antes de ligar o veículo e sair.", "Ajustar só o banco depois de sair.", "Esperar o instrutor avisar para ajustar."}, 1},
    {"O jogador deu a partida, engatou a 1ª marcha e está pronto para sair, mas esqueceu de sinalizar (dar seta). O que deve ser feito?",
     {"Sair sem sinalizar.", "Sinalizar (dar seta) para a direção da saída antes de liberar o freio de mão e mover o veículo.", "Sinalizar só depois de sair.", "Pedir para o instrutor sinalizar."}, 1},
    {"Durante a baliza, a roda traseira vai encostar no balizador ao dar ré. Qual é a melhor atitude?",
     {"Ignorar e seguir.", "Parar o carro, corrigir a manobra se possível, ou reposicionar para o próximo passo.", "Acelerar para terminar rápido.", "Pedir para o instrutor ajudar."}, 1},
    {"O motor apagou logo após arrancar na 1ª marcha. Qual é a ordem correta para reiniciar?",
     {"Ligar o motor direto.", "Acionar o freio de mão, colocar em ponto morto, ligar o motor e recomeçar.", "Tentar sair sem ligar o motor.", "Chamar o instrutor."}, 1},
    {"O jogador está parado em uma rampa. Para arrancar sem que o carro desça, qual o uso correto do freio?",
     {"Soltar tudo e acelerar.", "Usar o freio de pedal ou de mão para estabilizar e soltar só quando o carro estiver pronto para tracionar.", "Deixar o carro descer um pouco.", "Pedir para alguém segurar o carro."}, 1},
    {"Aproxima-se de uma placa de PARE. É suficiente apenas reduzir a velocidade e olhar para os lados?",
     {"Sim, só reduzir e olhar.", "Não. O veículo deve parar totalmente antes da linha de retenção.", "Só parar se houver outros carros.", "Ignorar a placa se estiver devagar."}, 1},
    {"O examinador pede para parar junto ao meio-fio para encerrar a prova. Qual a sequência correta?",
     {"Parar e desligar o motor direto.", "Sinalizar, parar suavemente, ponto morto, freio de mão, desligar o motor.", "Desligar o motor antes de parar.", "Sair do carro sem sinalizar."}, 1},
    {"O jogador está conduzindo de forma instável (ziguezagueando). Qual a falta?",
     {"Condução segura.", "Dirigir sem domínio ou com insegurança.", "Falta leve, sem problema.", "É normal em exame."}, 1},
    {"Precisa fazer conversão à direita, mas só sinaliza quando já está virando. Qual o problema?",
     {"Nenhum problema.", "Sinalização tardia. A seta deve ser dada com antecedência.", "Sinalizar só depois de virar.", "O instrutor deve sinalizar."}, 1},
    {"Vai abrir a porta para sair do carro. O que deve fazer antes?",
     {"Abrir direto.", "Olhar para trás ou pelo retrovisor para verificar aproximação de motos/bicicletas.", "Esperar o instrutor abrir.", "Ignorar o movimento externo."}, 1}
};

// Carrega perguntas (arquivo `perguntas.txt`) para o sistema de Q&A
int carregar_questoes_do_arquivo(const char *caminho, Question questoes[], int maxQ) {
    FILE *arquivo = fopen(caminho, "r");
    if(!arquivo) return 0;
    char linha[1024];
    int contagem = 0;
    while(fgets(linha, sizeof(linha), arquivo) && contagem < maxQ) {
        char *nl = strchr(linha, '\n'); if(nl) *nl = '\0';
        if(strlen(linha) == 0) continue;
        char *partes[6];
        partes[0] = strtok(linha, "|");
        int i;
        for(i=1;i<6;i++) partes[i] = strtok(NULL, "|");
        if(partes[0] && partes[1] && partes[2] && partes[3] && partes[4] && partes[5]) {
            strncpy(questoes[contagem].texto, partes[0], sizeof(questoes[contagem].texto)-1);
            questoes[contagem].texto[sizeof(questoes[contagem].texto)-1] = '\0';
            for(i=0;i<4;i++) {
                strncpy(questoes[contagem].opcoes[i], partes[1+i], sizeof(questoes[contagem].opcoes[i])-1);
                questoes[contagem].opcoes[i][sizeof(questoes[contagem].opcoes[i])-1] = '\0';
            }
            questoes[contagem].correta = atoi(partes[5]);
            if(questoes[contagem].correta < 0 || questoes[contagem].correta > 3) questoes[contagem].correta = 0;
            questoes[contagem].coberta = 0;
            contagem++;
        }
    }
    fclose(arquivo);
    return contagem;
}

// Carrega perguntas de matemática (arquivo `matematica.txt`)
int carregar_questoes_matematica_do_arquivo(const char *caminho, MathQuestion questoes_mat[], int maxQ) {
    FILE *arquivo = fopen(caminho, "r");
    if(!arquivo) return 0;
    char linha[1024];
    int contagem = 0;
    while(fgets(linha, sizeof(linha), arquivo) && contagem < maxQ) {
        char *nl = strchr(linha, '\n'); if(nl) *nl = '\0';
        if(strlen(linha) == 0) continue;
        char *partes[7];
        partes[0] = strtok(linha, "|");
        int i;
        for(i=1;i<7;i++) partes[i] = strtok(NULL, "|");
        if(partes[0] && partes[1] && partes[2] && partes[3] && partes[4] && partes[5] && partes[6]) {
            strncpy(questoes_mat[contagem].texto, partes[0], sizeof(questoes_mat[contagem].texto)-1);
            questoes_mat[contagem].texto[sizeof(questoes_mat[contagem].texto)-1] = '\0';
            for(i=0;i<4;i++) {
                strncpy(questoes_mat[contagem].opcoes[i], partes[1+i], sizeof(questoes_mat[contagem].opcoes[i])-1);
                questoes_mat[contagem].opcoes[i][sizeof(questoes_mat[contagem].opcoes[i])-1] = '\0';
            }
            questoes_mat[contagem].correta = atoi(partes[5]);
            questoes_mat[contagem].aproximada = atoi(partes[6]);
            if(questoes_mat[contagem].correta < 0 || questoes_mat[contagem].correta > 3) questoes_mat[contagem].correta = 0;
            if(questoes_mat[contagem].aproximada < 0 || questoes_mat[contagem].aproximada > 3) questoes_mat[contagem].aproximada = 0;
            contagem++;
        }
    }
    fclose(arquivo);
    return contagem;
}
