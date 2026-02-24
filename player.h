#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char nome[50];
    int idade;
    int dinheiro;       // nivel 1 a 4 (mantido por compatibilidade: representa o nivel)
    int dinheiro_atual;  // unidades de dinheiro atual (gasta/recebe durante o jogo)
    int concentracao;   // 1 a 5
    int fome;           // 0 a 100
    int sede;           // 0 a 100
    int ansiedade;      // 0 a 100
    int ansiedade_pico;  // flag: 1 se estava em pico de ansiedade e congelou fome/sede
    int conhecimento;   // 0 a 100
    int tick;           // tempo de jogo
    int documentos_coletados;
    int aulas_teoricas;
    int aulas_praticas;
    int passou_psicotecnico; // 0/1
    int passou_prova_teorica; // 0/1
} Personagem;

typedef struct {
    char nome[50];
    int preco;
    int qualidade;      // 1 a 5
    int taxa_aprovados;  // 0 a 100
    int flexibilidade;  // 1 a 5
} AutoEscola;

// Question types (from questions/quiz system)
typedef struct {
    char texto[256];
    char opcoes[4][128];
    int correta; // 0..3
    int coberta; // 0/1 se o instrutor cobriu
} Question;

typedef struct {
    char texto[256];
    char opcoes[4][128];
    int correta;    // 0..3
    int aproximada; // 0..3 (indice da alternativa 'aproximada')
} MathQuestion;

typedef struct {
    char texto[256];
    char opcoes[4][128];
    int correta; // índice da alternativa correta
} CenarioPratico;

// Prototypes
void criarPersonagem(Personagem *p);
void escolherAutoEscola(AutoEscola *ae);
void status(Personagem *p);
void comer(Personagem *p);
void beber(Personagem *p);
void recolherDocumentos(Personagem *p);
void examePsicotecnico(Personagem *p);
void estudar(Personagem *p);

// Event and game functions (defined elsewhere)
void prova_pratica(Personagem *p);
void provaTeorica(Personagem *p, AutoEscola ae);
void provaTeoricaComPerguntas(Personagem *p, AutoEscola ae, Question questoes[], int contagemQuestoes);
void aulaPratica(Personagem *p, MathQuestion questoesMat[], int contagemMat);
void aulaInstrutor(Personagem *p, Question questoes[], int contagemQuestoes, int n);

// Question loading functions
int carregar_questoes_do_arquivo(const char *caminho, Question questoes[], int maxQ);
int carregar_questoes_matematica_do_arquivo(const char *caminho, MathQuestion questoes_mat[], int maxQ);

#endif // PLAYER_H
