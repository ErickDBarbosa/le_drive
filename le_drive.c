#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Structs principais
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

// Funções

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

// Lista de cenários e alternativas
CenarioPratico cenarios_praticos[10] = {
    {"O jogador acabou de entrar no carro, mas esqueceu de ajustar o retrovisor interno antes de ligar o motor. O que ele deve fazer?",
     {"Ligar o motor e sair normalmente.", "Ajustar imediatamente o retrovisor e o banco antes de ligar o veículo e sair.", "Ajustar só o banco depois de sair.", "Esperar o instrutor avisar para ajustar."}, 1},
    {"O jogador deu a partida, engatou a 1ª marcha e está pronto para sair, mas esqueceu de sinalizar (dar seta). O que deve ser feito?",
     {"Sair sem sinalizar.", "Sinalizar (dar seta) para a direção da saída antes de liberar o freio de mão e mover o veículo.", "Sinalizar só depois de sair.", "Pedir para o instrutor sinalizar."}, 1},
    {"Durante a baliza, a roda traseira vai encostar no balizador ao dar ré. Qual é a melhor atitude?",
     {"Ignorar e seguir.", "Parar o carro, corrigir a manobra se possível, ou reposicionar para o próximo passo.", "Acelerar para terminar rápido.", "Pedir para o examinador ajudar."}, 1},
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

// Função da prova prática com cenários
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

// Carrega perguntas de um arquivo simples: cada linha -> pergunta|op1|op2|op3|op4|idx
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

// Carrega perguntas de matemática: cada linha -> pergunta|op1|op2|op3|op4|idx_correta|idx_aproximada
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

// Prova teórica usando perguntas: mistura perguntas cobertas com outras
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

