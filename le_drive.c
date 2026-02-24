#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// incluir definições do jogador (Personagem, AutoEscola, funções) em player.h
#include "player.h"
#include "util.h"
#include "events.h"


int main() {
    // Declaração antecipada para evitar erro de tipo desconhecido
    void prova_pratica(Personagem *p);
    srand(time(NULL));
    Personagem jogador;
    AutoEscola escola;

    // carregar perguntas (arquivo `perguntas.txt` no mesmo diretório)
    Question questoes[200];
    int contagem_questoes = carregar_questoes_do_arquivo("perguntas.txt", questoes, 200);
    if(contagem_questoes > 0) printf("%d perguntas carregadas de 'perguntas.txt'\n", contagem_questoes);
    // carregar perguntas de matematica para minigame (arquivo `matematica.txt`)
    MathQuestion questoes_mat[200];
    int contagem_matematica = carregar_questoes_matematica_do_arquivo("matematica.txt", questoes_mat, 200);
    if(contagem_matematica > 0) printf("%d perguntas de matematica carregadas de 'matematica.txt'\n", contagem_matematica);

    criarPersonagem(&jogador);
    escolherAutoEscola(&escola);

    int lastTick = jogador.tick;

    int opcao;
    do {
        printf("\n--- Menu do Dia ---\n");
        // mostrar status rápido no menu
        printf("Fome: %d  Sede: %d  Ansiedade: %d\n", jogador.fome, jogador.sede, jogador.ansiedade);
        printf("1. Comer\n");
        printf("2. Beber\n");
        printf("3. Recolher documentos\n");
        // exame psicotécnico disponível apenas após recolher documentos e se ainda não passou
        if(jogador.documentos_coletados && !jogador.passou_psicotecnico) printf("4. Fazer exame psicotecnico\n");
        // aulas teóricas e instrutor apenas após passar no psicotécnico
        if(jogador.passou_psicotecnico) {
            printf("5. Estudar / Aulas teoricas\n");
            printf("10. Aula do instrutor (perguntas)\n");
        }
        // prova teórica só aparece se o jogador já teve pelo menos 3 aulas e passou no psicotécnico
        if(jogador.passou_psicotecnico && jogador.aulas_teoricas >= 3) printf("6. Fazer prova teorica\n");
        // aulas práticas e prova prática só após passar na prova teórica
        if(jogador.passou_prova_teorica) {
            printf("7. Aula pratica\n");
            if(jogador.aulas_praticas >= 3) printf("8. Fazer prova pratica\n");
        }
        printf("9. Ver status\n");
        printf("0. Sair\n");
        scanf("%d", &opcao);

        switch(opcao) {
            case 1: comer(&jogador); break;
            case 2: beber(&jogador); break;
            case 3: recolherDocumentos(&jogador); break;
            case 4: examePsicotecnico(&jogador); break;
            case 5: estudar(&jogador); break;
            case 6:
                if(contagem_questoes > 0) provaTeoricaComPerguntas(&jogador, escola, questoes, contagem_questoes);
                else provaTeorica(&jogador, escola);
                break;
            case 10: aulaInstrutor(&jogador, questoes, contagem_questoes, 5); break;
            case 7: aulaPratica(&jogador, questoes_mat, contagem_matematica); break;
            case 8: prova_pratica(&jogador); break;
            case 9: status(&jogador); break;
            case 0: printf("Saindo do jogo...\n"); break;
            default: printf("Opção inválida!\n");
        }

        // aplicar regeneração de dinheiro para ticks que avançaram durante a ação
        if(jogador.tick > lastTick) {
            int delta = jogador.tick - lastTick;
            aplicarEfeitosTick(&jogador, delta);
            aplicarRegeneracao(&jogador, delta);
            lastTick = jogador.tick;
        }

        if(jogador.fome <= 0) {
            printf("Voce desmaiou de fome! Voce teve que retornar para a casa de seus pais para se recuperar e perdeu o prazo. O jogo acabou.\n");
            break;
        }
        if(jogador.sede <= 0) {
            printf("Voce desmaiou de sede! Voce teve que retornar para a casa de seus pais para se recuperar e perdeu o prazo. O jogo acabou.\n");
            break;
        }

    } while(opcao != 0);

    return 0;
}

