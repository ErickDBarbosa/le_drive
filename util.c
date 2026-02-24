#include "util.h"
#include <stdio.h>

// Aplicar efeitos por tick: desgaste de fome/sede, decaimento de conhecimento,
// e comportamento de congelamento/despenca em picos de ansiedade.
void aplicarEfeitosTick(Personagem *p, int delta) {
    if(delta <= 0) return;

    const int FOME_DECAY = 2;       // por tick (decay suave)
    const int SEDE_DECAY = 2;       // por tick (decay suave)
    const int CONHEC_DECAY = 1;     // por tick
    const int ANSIEDADE_PICO_LIMITE = 80; // valor que define pico
    const int QUEDA_APOS_PICO = 40; // queda súbita em fome/sede após pico

    // Se atualmente no pico (ansiedade >= limite), marque e congele
    if(p->ansiedade >= ANSIEDADE_PICO_LIMITE) {
        if(!p->ansiedade_pico) {
            p->ansiedade_pico = 1;
            printf("\nAnsiedade atingiu pico: fome e sede congelaram temporariamente.\n");
        }
        // enquanto em pico, não há desgaste de fome/sede
    } else {
        // se estava em pico e agora saiu, aplicar queda súbita
        if(p->ansiedade_pico) {
            p->ansiedade_pico = 0;
            p->fome -= QUEDA_APOS_PICO;
            p->sede -= QUEDA_APOS_PICO;
            if(p->fome < 0) p->fome = 0;
            if(p->sede < 0) p->sede = 0;
            printf("\nAnsiedade diminuiu após pico: fome e sede despencaram para níveis alarmantes!\n");
        }

        // aplicar desgaste normal para cada tick
        int totalFome = FOME_DECAY * delta;
        int totalSede = SEDE_DECAY * delta;
        p->fome -= totalFome;
        p->sede -= totalSede;
    }

    // conhecimento decai com o tempo (esquecimento natural)
    int perdaConhec = CONHEC_DECAY * delta;
    p->conhecimento -= perdaConhec;
    if(p->conhecimento < 0) p->conhecimento = 0;
}

// Retorna taxa de regeneração por tick baseada no nível de dinheiro (1-4)
int taxaRegeneracao(int nivel) {
    switch(nivel) {
        case 1: return 0; // nível pobre: sem regeneração
        case 2: return 1; // regeneração lenta
        case 3: return 2; // regeneração moderada
        case 4: return 4; // regeneração alta
        default: return 0;
    }
}

// Aplica regeneração de dinheiro para `delta` ticks
void aplicarRegeneracao(Personagem *p, int delta) {
    if(delta <= 0) return;
    int taxa = taxaRegeneracao(p->dinheiro);
    if(taxa <= 0) return;
    long ganho = (long) taxa * delta;
    p->dinheiro_atual += (int) ganho;
    printf("\nRegeneracao: +%d unidades de dinheiro (por %d tick(s)). Novo saldo: %d\n", (int)ganho, delta, p->dinheiro_atual);
}
