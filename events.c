#include "events.h"
#include "game.h"
#include <stdio.h>
#include <stdlib.h>

// Implementações de eventos (copiadas sem alterações)

void prova_pratica(Personagem *p) {
    printf("\n--- Prova Prática: Cenários ---\n");
    int erros = 0;

    for(int i=0; i<10; i++) {
        
        double base = p->conhecimento;
        base += p->concentracao * 5;
        base -= p->ansiedade * 0.5;
        if(base < 1) base = 1;
        if(base > 99) base = 99;
        int certeza = (int) base;
        const int LIMITE_CERTEZA = 70;
        int confiante = (certeza >= LIMITE_CERTEZA);

        
        double pontuacoes[4];
        int k;
        for(k=0;k<4;k++) pontuacoes[k] = 0.0;
        double restante = 100.0 - (double)certeza;
        for(k=0;k<4;k++) {
            if(k == cenarios_praticos[i].correta) pontuacoes[k] = (double)certeza;
            else pontuacoes[k] = restante / 3.0;
        }
        int predicao = 0; double melhor = pontuacoes[0];
        for(k=1;k<4;k++) if(pontuacoes[k] > melhor) { melhor = pontuacoes[k]; predicao = k; }

        if(confiante) printf("\n(!) Situação %d [COM CERTEZA]: %s\n", i+1, cenarios_praticos[i].texto);
        else printf("\nSituação %d: %s\n", i+1, cenarios_praticos[i].texto);
        for(k=0;k<4;k++) {
            if(confiante && k == predicao) printf("%d) %s  <-- [COM CERTEZA]\n", k+1, cenarios_praticos[i].opcoes[k]);
            else printf("%d) %s\n", k+1, cenarios_praticos[i].opcoes[k]);
        }
        if(confiante) printf("Certeza estimada: %d%% [COM CERTEZA]\n", certeza);
        else printf("Certeza estimada: %d%%\n", certeza);
        printf("Escolha sua resposta (1-4): ");
        int escolha;
        if(scanf("%d", &escolha) != 1) { escolha = 1; }
        escolha = escolha - 1;
        if(escolha == cenarios_praticos[i].correta) {
            printf("Correto! Situação resolvida corretamente.\n");
        } else {
            printf("Resposta incorreta! Você perdeu 1 ponto.\n");
            erros++;
            if(erros >= 3) {
                printf("\nVocê acumulou 3 erros e foi reprovado na prova prática. Será necessário refazer!\n");
                p->ansiedade += 20;
                return;
            }
        }
    }
    printf("\nProva prática concluída! Você foi aprovado.\n");
    p->aulas_praticas += 1;
    p->conhecimento += 10;
    if(p->conhecimento > 100) p->conhecimento = 100;
}

// Aula do instrutor: escolhe `n` perguntas aleatórias, marca como cobertas e aumenta conhecimento
void aulaInstrutor(Personagem *p, Question questoes[], int contagemQuestoes, int n) {
    if(contagemQuestoes == 0) {
        printf("Nenhuma pergunta carregada. Crie um arquivo 'perguntas.txt' com formato: pergunta|op1|op2|op3|op4|idx\n");
        return;
    }
    printf("\nAula do instrutor: cobrindo %d perguntas\n", n);
    int i;
    for(i=0;i<n;i++) {
        int indice = rand() % contagemQuestoes;
        if(!questoes[indice].coberta) {
            questoes[indice].coberta = 1;
            printf("Pergunta: %s\n", questoes[indice].texto);
            printf("Resposta correta (instrutor): %s\n", questoes[indice].opcoes[questoes[indice].correta]);
            // ganhar conhecimento por cada pergunta explicada
            p->conhecimento += 4 + p->concentracao; // depende da concentração
            if(p->conhecimento > 100) p->conhecimento = 100;
        } else {
            // se já coberta, apenas reforça um pouco
            p->conhecimento += 2;
            if(p->conhecimento > 100) p->conhecimento = 100;
        }
    }
    p->tick += 1;
}

void provaTeoricaComPerguntas(Personagem *p, AutoEscola ae, Question questoes[], int contagemQuestoes) {
    int totalQuestoes = 30;
    if(contagemQuestoes < 5) {
        printf("Poucas perguntas disponíveis (%d). Precisa ter pelo menos 5.\n", contagemQuestoes);
        return;
    }
    int indices[1000];
    int contagemIdx = 0;
    int i;
    // primeiro, adicionar questoes cobertas
    for(i=0;i<contagemQuestoes;i++) if(questoes[i].coberta) indices[contagemIdx++] = i;
    // preencher com outras questoes
    for(i=0;i<contagemQuestoes && contagemIdx < totalQuestoes;i++) {
        int jaUsado = 0; int j;
        for(j=0;j<contagemIdx;j++) if(indices[j]==i) { jaUsado=1; break; }
        if(!jaUsado) indices[contagemIdx++] = i;
    }
    if(contagemIdx < totalQuestoes) totalQuestoes = contagemIdx;

    int acertadas = 0;
    for(i=0;i<totalQuestoes;i++) {
        Question *questao = &questoes[indices[i]];
        double base = p->conhecimento; // 0..100
        if(questao->coberta) base += 20; // vantagem se coberta
        base += p->concentracao * 5; // concentração ajuda
        base -= p->ansiedade * 0.5; // ansiedade reduz chance
        if(base < 1) base = 1;
        if(base > 99) base = 99;
        int certeza = (int) base; // 1..99
        const int LIMITE_CERTEZA = 70; // limite para marcar "com certeza"
        int confiante = (certeza >= LIMITE_CERTEZA);

        // calcular alternativa mais provável (predicao simples)
        double pontuacoes[4];
        int k;
        for(k=0;k<4;k++) pontuacoes[k] = 0.0;
        // distribuir probabilidade: correta recebe 'base', demais compartilham o restante
        double restante = 100.0 - (double)certeza;
        for(k=0;k<4;k++) {
            if(k == questao->correta) pontuacoes[k] = (double)certeza;
            else pontuacoes[k] = restante / 3.0;
        }
        // encontrar indice com maior pontuacao
        int predicao = 0; double melhor = pontuacoes[0];
        for(k=1;k<4;k++) if(pontuacoes[k] > melhor) { melhor = pontuacoes[k]; predicao = k; }

        if(confiante) printf("\n(!) Questao %d [COM CERTEZA]: %s\n", i+1, questao->texto);
        else printf("\nQuestao %d: %s\n", i+1, questao->texto);
        for(k=0;k<4;k++) {
            if(confiante && k == predicao) printf("%d) %s  <-- [COM CERTEZA]\n", k+1, questao->opcoes[k]);
            else printf("%d) %s\n", k+1, questao->opcoes[k]);
        }
        if(confiante) printf("Certeza estimada: %d%% [COM CERTEZA]\n", certeza);
        else printf("Certeza estimada: %d%%\n", certeza);
        printf("Escolha sua resposta (1-4): ");
        int escolha;
        if(scanf("%d", &escolha) != 1) { escolha = 1; }
        escolha = escolha - 1;
        if(escolha == questao->correta) {
            printf("Resposta correta!\n");
            acertadas++;
        } else {
            printf("Resposta errada. Resposta certa: %s\n", questao->opcoes[questao->correta]);
            p->ansiedade += 5;
        }
        // não contamos ticks por questão para manter controle simples
    }
    printf("\nVoce acertou %d de %d questoes.\n", acertadas, totalQuestoes);
    if(acertadas >= (totalQuestoes * ae.taxa_aprovados / 100)) {
        printf("Parabéns! Você passou na prova teórica!\n");
        p->passou_prova_teorica = 1;
    } else {
        printf("Você não passou na prova teórica. Estude mais!\n");
        p->ansiedade += 10;
    }
}

void provaTeorica(Personagem *p, AutoEscola ae) {
    if(p->aulas_teoricas < 3) {
        printf("Você precisa assistir mais aulas teóricas antes da prova.\n");
        return;
    }
    int chance = p->conhecimento + (p->concentracao * 10) + (ae.qualidade * 5) - p->ansiedade;
    int resultado = rand() % 150;
    printf("\nRealizando a prova teórica...\n");
    if(resultado < chance) {
        printf("Parabéns! Você passou na prova teórica!\n");
        p->tick += 1;
    } else {
        printf("Você não passou na prova teórica. Estude mais!\n");
        p->ansiedade += 10;
        p->tick += 1;
    }
}

void aulaPratica(Personagem *p, MathQuestion questoesMat[], int contagemMat) {
    printf("\nAula pratica (mini-game): responda 3 perguntas para demonstrar raciocinio pratico enquanto dirige.\n");
    if(contagemMat <= 0) {
        // fallback behavior se não houver questões de matemática: comportamento antigo simplificado
        printf("(Nenhum questionario de matematica carregado — aula pratica padrao)\n");
        int acidente = rand() % 100;
        if(acidente < 10) {
            printf("Ops! Voce bateu levemente o carro. Ansiedade aumenta!\n");
            p->ansiedade += 20;
        } else {
            printf("A aula pratica foi tranquila.\n");
            p->conhecimento += 10;
            if(p->conhecimento > 100) p->conhecimento = 100;
        }
        p->fome -= 15;
        p->sede -= 15;
        p->tick += 1;
        p->aulas_praticas++;
        return;
    }

    int indices[100];
    int usados = 0;
    int i;
    int refazerAula = 0;
    for(i=0;i<3;i++) {
        int indice;
        // escolher indice aleatorio nao usado
        do {
            indice = rand() % contagemMat;
            int encontrado = 0;
            int _j;
            for(_j = 0; _j < usados; _j++) {
                if(indices[_j] == indice) { encontrado = 1; break; }
            }
            if(!encontrado) break;
        } while(1);
        indices[usados++] = indice;
        MathQuestion *questaoMat = &questoesMat[indice];

        // Cálculo de certeza igual à prova teórica
        double base = p->conhecimento;
        base += p->concentracao * 5;
        base -= p->ansiedade * 0.5;
        if(base < 1) base = 1;
        if(base > 99) base = 99;
        int certeza = (int) base;
        const int LIMITE_CERTEZA = 70;
        int confiante = (certeza >= LIMITE_CERTEZA);

        
        double pontuacoes[4];
        int k;
        for(k=0;k<4;k++) pontuacoes[k] = 0.0;
        double restante = 100.0 - (double)certeza;
        for(k=0;k<4;k++) {
            if(k == questaoMat->correta) pontuacoes[k] = (double)certeza;
            else pontuacoes[k] = restante / 3.0;
        }
        int predicao = 0; double melhor = pontuacoes[0];
        for(k=1;k<4;k++) if(pontuacoes[k] > melhor) { melhor = pontuacoes[k]; predicao = k; }

        if(confiante) printf("\n(!) Pergunta pratica %d [COM CERTEZA]: %s\n", i+1, questaoMat->texto);
        else printf("\nPergunta pratica %d: %s\n", i+1, questaoMat->texto);
        for(k=0;k<4;k++) {
            if(confiante && k == predicao) printf("%d) %s  <-- [COM CERTEZA]\n", k+1, questaoMat->opcoes[k]);
            else printf("%d) %s\n", k+1, questaoMat->opcoes[k]);
        }
        if(confiante) printf("Certeza estimada: %d%% [COM CERTEZA]\n", certeza);
        else printf("Certeza estimada: %d%%\n", certeza);
        printf("Escolha sua resposta (1-4): ");
        int escolha;
        if(scanf("%d", &escolha) != 1) { escolha = 1; }
        escolha = escolha - 1;

        if(escolha == questaoMat->correta) {
            printf("Voce respondeu corretamente e dirigiu corretamente no exercicio.\n");
            p->conhecimento += 3;
            if(p->conhecimento > 100) p->conhecimento = 100;
        } else if(escolha == questaoMat->aproximada) {
            printf("Resposta aproximada. Instrutor faz um alerta: cuidado! Ansiedade aumenta levemente.\n");
            p->ansiedade += 5;
            p->conhecimento += 1;
            if(p->conhecimento > 100) p->conhecimento = 100;
        } else {
            printf("Resposta incorreta. O instrutor ficou irritado e mandou refazer a aula. Ansiedade aumenta consideravelmente.\n");
            p->ansiedade += 15;
            refazerAula = 1;
        }
    }

    // reduzir fome/sede por aula
    p->fome -= 15;
    p->sede -= 15;
    p->tick += 1;

    if(refazerAula) {
        printf("Aula nao validada - voce precisara refazer esta aula pratica mais tarde.\n");
        // nao incrementa aulas_praticas
    } else {
        p->aulas_praticas++;
        printf("Aula pratica concluida com sucesso. Total de aulas praticas: %d\n", p->aulas_praticas);
    }
}
