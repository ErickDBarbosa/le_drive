#ifndef EVENTS_H
#define EVENTS_H

#include "player.h"
#include "game.h"

// Eventos: provas, aulas e ações do jogador
void prova_pratica(Personagem *p);
void aulaInstrutor(Personagem *p, Question questoes[], int contagemQuestoes, int n);
int carregar_questoes_do_arquivo(const char *caminho, Question questoes[], int maxQ);
int carregar_questoes_matematica_do_arquivo(const char *caminho, MathQuestion questoes_mat[], int maxQ);
void provaTeoricaComPerguntas(Personagem *p, AutoEscola ae, Question questoes[], int contagemQuestoes);
void provaTeorica(Personagem *p, AutoEscola ae);
void aulaPratica(Personagem *p, MathQuestion questoesMat[], int contagemMat);

#endif // EVENTS_H
