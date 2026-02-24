#ifndef UTIL_H
#define UTIL_H

#include "player.h"

// Mecânicas de tick e regeneração do jogador
void aplicarEfeitosTick(Personagem *p, int delta);
int taxaRegeneracao(int nivel);
void aplicarRegeneracao(Personagem *p, int delta);

#endif // UTIL_H
