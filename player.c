#include "player.h"
#include <time.h>
#include <stdio.h>

void criarPersonagem(Personagem *p) {
    printf("Digite o nome do seu personagem: ");
    fgets(p->nome, 50, stdin);
    p->nome[strcspn(p->nome, "\n")] = 0;
    p->idade = 18;

    printf("Escolha o nivel de dinheiro (1-4): ");
    scanf("%d", &p->dinheiro);

    printf("Escolha o nivel de concentracao (1-5): ");
    scanf("%d", &p->concentracao);

    p->fome = 50;
    p->sede = 50;
    p->ansiedade = 0;
    p->ansiedade_pico = 0;
    p->conhecimento = 0;
    p->tick = 0;
    p->documentos_coletados = 0;
    p->aulas_teoricas = 0;
    p->aulas_praticas = 0;
    p->passou_psicotecnico = 0;
    p->passou_prova_teorica = 0;

    // inicializar dinheiro atual com base no nivel escolhido
    // niveis: 1 -> 2, 2 -> 4, 3 -> 8, 4 -> 16 unidades
    if(p->dinheiro < 1) p->dinheiro = 1;
    if(p->dinheiro > 4) p->dinheiro = 4;
    p->dinheiro_atual = 2 << (p->dinheiro - 1);

    printf("\nPersonagem criado: %s\n", p->nome);
}

void escolherAutoEscola(AutoEscola *ae) {
    printf("\nEscolha sua auto-escola:\n");
    printf("1. Auto Escola Popular (barata, qualidade baixa)\n");
    printf("2. Auto Escola Premium (cara, qualidade alta)\n");
    int escolha;
    scanf("%d", &escolha);

    if(escolha == 1) {
        strcpy(ae->nome, "Auto Escola Popular");
        ae->preco = 1000;
        ae->qualidade = 2;
        ae->taxa_aprovados = 50;
        ae->flexibilidade = 4;
    } else {
        strcpy(ae->nome, "Auto Escola Premium");
        ae->preco = 4000;
        ae->qualidade = 5;
        ae->taxa_aprovados = 90;
        ae->flexibilidade = 2;
    }
    printf("Você escolheu: %s\n", ae->nome);
}

void status(Personagem *p) {
    printf("\n--- Status do personagem ---\n");
    printf("Nome: %s\n", p->nome);
    printf("Fome: %d\n", p->fome);
    printf("Sede: %d\n", p->sede);
    printf("Ansiedade: %d\n", p->ansiedade);
    printf("Conhecimento: %d\n", p->conhecimento);
    printf("Dinheiro (nivel): %d; Dinheiro atual: %d\n", p->dinheiro, p->dinheiro_atual);
    printf("Tick (tempo de jogo): %d\n", p->tick);
}

void comer(Personagem *p) {
    if(p->dinheiro_atual > 0) {
        p->fome += 30;
        if(p->fome > 100) p->fome = 100;
        p->dinheiro_atual--;
        printf("Você se alimentou. Fome agora: %d\n", p->fome);
    } else {
        printf("Sem dinheiro para comer!\n");
    }
}

void beber(Personagem *p) {
    if(p->dinheiro_atual > 0) {
        p->sede += 30;
        if(p->sede > 100) p->sede = 100;
        p->dinheiro_atual--;
        printf("Você se hidratou. Sede agora: %d\n", p->sede);
    } else {
        printf("Sem dinheiro para beber!\n");
    }
}

void recolherDocumentos(Personagem *p) {
    printf("\nRecolhendo documentos...\n");
    p->documentos_coletados = 1;
    p->tick += 1;
    printf("Documentos coletados com sucesso!\n");
}

void examePsicotecnico(Personagem *p) {
    if(p->documentos_coletados) {
        int chance = p->concentracao * 20 - p->ansiedade;
        int resultado = rand() % 100;
        printf("\nRealizando exame psicotécnico...\n");
        if(resultado < chance) {
            printf("Você passou no exame psicotécnico!\n");
            p->passou_psicotecnico = 1;
        } else {
            printf("Você falhou no exame psicotécnico, precisa pagar para refazer!\n");
            if(p->dinheiro_atual > 0) p->dinheiro_atual -= 1;
            p->ansiedade += 10;
        }
        p->tick += 1;
    } else {
        printf("Você precisa recolher todos os documentos antes do exame.\n");
    }
}

void estudar(Personagem *p) {
    printf("\nVocê estudou hoje!\n");
    p->conhecimento += p->concentracao * 5;
    if(p->conhecimento > 100) p->conhecimento = 100;
    p->fome -= 10;
    p->sede -= 10;
    p->tick += 1;
    p->aulas_teoricas++;
}
