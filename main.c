#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_HEAP_NOZES 20

typedef struct Data {
  int dia; 
  int mes; 
  int ano;
} Data;

typedef struct Paciente {
  char nome[100];
  int idade;
  char rg[20];
  Data nasc;
} Paciente;

typedef struct Nozes {
  Paciente dados;
  struct Nozes *prox;
} Nozes;

typedef struct Lista {
  Nozes *inicio;
  int qtd;
} Lista;

typedef struct NozesFila {
  Paciente *dados;
  struct NozesFila *prox;
} NozesFila;

typedef struct FilaNozes {
  NozesFila *inicio, *fim;
  int qtd;
} FilaNozes;

typedef struct HeapNozes {
  Paciente *dados[MAX_HEAP_NOZES];
  int qtd;
} HeapNozes;

typedef struct NozesArvore {
  Paciente *dados;
  struct NozesArvore *esq, *dir;
} NozesArvore;

typedef enum {
  POR_IDADE, POR_ANO, POR_MES, POR_DIA
} CriterioNozes;

typedef struct NozesPilha {
  char operacao[200];
  Paciente *paciente;
  struct NozesPilha *prox;
} NozesPilha;

Lista *criarLista() {
  Lista *l = malloc(sizeof(Lista));
  l->inicio = NULL; 
  l->qtd = 0;
  return l;
}

Paciente cadastrarPaciente() {
    Paciente p;
    char buffer[100];

    printf("Nome: ");
    fgets(p.nome, 100, stdin);
    p.nome[strcspn(p.nome, "\n")] = 0;

    printf("Idade: ");
    fgets(buffer, 100, stdin);
    sscanf(buffer, "%d", &p.idade);

    printf("RG: ");
    fgets(p.rg, 20, stdin);
    p.rg[strcspn(p.rg, "\n")] = 0;

    int d = -1, m = -1, a = -1;
    while (1) {
        printf("Nascimento (dd mm aaaa): ");
        fgets(buffer, 100, stdin);
        if (sscanf(buffer, "%d %d %d", &d, &m, &a) == 3 &&
            d >= 1 && d <= 31 && m >= 1 && m <= 12 && a >= 1900 && a <= 2100) {
            break;
        }
        printf("Data inválida. Tente novamente.\n");
    }

    p.nasc.dia = d;
    p.nasc.mes = m;
    p.nasc.ano = a;

    return p;
}


void inserirPaciente(Lista *l, Paciente p) {
  Nozes *novo = malloc(sizeof(Nozes));
  novo->dados = p; 
  novo->prox = l->inicio;
  l->inicio = novo; 
  l->qtd++;
}

Paciente *buscarPaciente(Lista *l, const char *nome) {
  for (Nozes *a = l->inicio; a; a = a->prox)
    if (strcmp(a->dados.nome, nome) == 0)
      return &a->dados;
  return NULL;
}

void mostrarPacientes(Lista *l) {
  Nozes *a = l->inicio;
  while (a) {
    Paciente p = a->dados;
    printf("\nNome: %s\n", p.nome);
    printf("Idade: %d\n", p.idade);
    printf("RG: %s\n", p.rg);
    printf("Nascimento: %02d/%02d/%04d\n", p.nasc.dia, p.nasc.mes, p.nasc.ano);
    a = a->prox;
  }
  printf("Total: %d pacientes\n", l->qtd);
  printf("\n");
}


void atualizarPaciente(Lista *l, const char *nome) {
  Paciente *p = buscarPaciente(l, nome);
  if (p) *p = cadastrarPaciente();
  else printf("Paciente nao encontrado.\n");
}

void removerPaciente(Lista *l, const char *nome) {
  Nozes *a = l->inicio, 
  *ant = NULL;

  while (a) {
    if (strcmp(a->dados.nome, nome) == 0) {
      if (ant) ant->prox = a->prox; 
      else l->inicio = a->prox;
      free(a); 
      l->qtd--; 
      printf("Removido com sucesso.\n"); return;
    }
    ant = a; a = a->prox;
  }
  printf("Paciente nao encontrado.\n");
}

void salvarArquivo(Lista *l, const char *nomeArq) {
  FILE *f = fopen(nomeArq, "w");
  for (Nozes *a = l->inicio; 
    a; 
    a = a->prox) {
    Paciente p = a->dados;
    fprintf(f, "%s;%d;%s;%02d/%02d/%04d\n",
      p.nome, p.idade, p.rg, p.nasc.dia, p.nasc.mes, p.nasc.ano);
  }
  fclose(f);
}

void carregarArquivo(Lista *l, const char *nomeArq) {
  FILE *f = fopen(nomeArq, "r");
  if (!f) return;
  char linha[200];
  while (fgets(linha, sizeof(linha), f)) {
    Paciente p;
    sscanf(linha, "%[^;];%d;%[^;];%d/%d/%d",
      p.nome, &p.idade, p.rg, &p.nasc.dia, &p.nasc.mes, &p.nasc.ano);
    inserirPaciente(l, p);
  }
  fclose(f);
}

FilaNozes *criarFilaNozes() {
  FilaNozes *f = malloc(sizeof(FilaNozes));
  f->inicio = f->fim = NULL; 
  f->qtd = 0;
  return f;
}

void enfileirarNozes(FilaNozes *f, Paciente *p) {
  NozesFila *n = malloc(sizeof(NozesFila));
  n->dados = p; 
  n->prox = NULL;
  if (f->fim) f->fim->prox = n; 
  else f->inicio = n;
  f->fim = n; 
  f->qtd++;
}

void desenfileirarNozes(FilaNozes *f) {
  if (!f->inicio) return;
  NozesFila *rem = f->inicio;
  f->inicio = rem->prox;
  if (!f->inicio) f->fim = NULL;
  printf("Chamando paciente: %s\n", rem->dados->nome);
  free(rem); 
  f->qtd--;
}

void mostrarFilaNozes(FilaNozes *f) {
  printf("\n--- Fila de Atendimento ---\n");
  for (NozesFila *a = f->inicio; a; a = a->prox)
    printf("%s | Idade: %d\n", a->dados->nome, a->dados->idade);
}

HeapNozes *criarHeapNozes() {
  HeapNozes *h = malloc(sizeof(HeapNozes)); 
  h->qtd = 0; 
  return h;
}

void trocarNozes(Paciente **a, Paciente **b) {
  Paciente *t = *a; 
  *a = *b; 
  *b = t;
}

void subirNozes(HeapNozes *h, int i) {
  while (i > 0) {
    int pai = (i - 1)/2;
    if (h->dados[i]->idade > h->dados[pai]->idade) {
      trocarNozes(&h->dados[i], &h->dados[pai]);
      i = pai;
    } else break;
  }
}

void enfileirarPrioridadeNozes(HeapNozes *h, Paciente *p) {
  if (h->qtd >= MAX_HEAP_NOZES) return;
  h->dados[h->qtd] = p;
  subirNozes(h, h->qtd);
  h->qtd++;
}

void descerNozes(HeapNozes *h, int i) {
  int maior = i, esq = 2*i+1, dir = 2*i+2;
  if (esq < h->qtd && h->dados[esq]->idade > h->dados[maior]->idade) maior = esq;
  if (dir < h->qtd && h->dados[dir]->idade > h->dados[maior]->idade) maior = dir;
  if (maior != i) {
    trocarNozes(&h->dados[i], &h->dados[maior]);
    descerNozes(h, maior);
  }
}

void desenfileirarPrioridadeNozes(HeapNozes *h) {
  if (h->qtd == 0) return;
  printf("Chamando prioritario: %s (%d anos)\n", h->dados[0]->nome, h->dados[0]->idade);
  h->dados[0] = h->dados[h->qtd - 1];
  h->qtd--;
  descerNozes(h, 0);
}

void mostrarHeapNozes(HeapNozes *h) {
  printf("\n--- Fila Prioritaria ---\n");
  for (int i = 0; i < h->qtd; i++)
    printf("%s (%d anos)\n", h->dados[i]->nome, h->dados[i]->idade);
}

NozesArvore *criarNoNozes(Paciente *p) {
  NozesArvore *n = malloc(sizeof(NozesArvore));
  n->dados = p; 
  n->esq = n->dir = NULL;
  return n;
}

int compararNozes(Paciente *a, Paciente *b, CriterioNozes crit) {
  if (crit == POR_IDADE) return a->idade - b->idade;
  if (crit == POR_ANO) return a->nasc.ano - b->nasc.ano;
  if (crit == POR_MES) return a->nasc.mes - b->nasc.mes;
  return a->nasc.dia - b->nasc.dia;
}

NozesArvore *inserirNaArvoreNozes(NozesArvore *r, Paciente *p, CriterioNozes c) {
  if (!r) return criarNoNozes(p);
  if (compararNozes(p, r->dados, c) < 0)
    r->esq = inserirNaArvoreNozes(r->esq, p, c);
  else
    r->dir = inserirNaArvoreNozes(r->dir, p, c);
  return r;
}

void mostrarArvoreNozes(NozesArvore *r) {
  if (!r) return;
  mostrarArvoreNozes(r->esq);
  Paciente *p = r->dados;
  printf("%s | Idade: %d | %02d/%02d/%04d\n", p->nome, p->idade, p->nasc.dia, p->nasc.mes, p->nasc.ano);
  mostrarArvoreNozes(r->dir);
}

void push(NozesPilha **topo, const char *operacao, Paciente *p) {
  NozesPilha *novo = malloc(sizeof(NozesPilha));
  strcpy(novo->operacao, operacao);
  novo->paciente = p;
  novo->prox = *topo;
  *topo = novo;
}

void popComDesfazer(NozesPilha **topo, FilaNozes *fila) {
  if (!*topo) return;
  printf("Deseja desfazer: %s? (s/n): ", (*topo)->operacao);
  char r; 
  scanf(" %c", &r); 
  getchar();
  if (r == 's' || r == 'S') {
    if (strcmp((*topo)->operacao, "Enfileirou paciente") == 0 && fila->fim) {
      NozesFila *a = fila->inicio, *ant = NULL;
      while (a && a->prox) { ant = a; a = a->prox; }
      if (ant) ant->prox = NULL; 
      else fila->inicio = NULL;
      free(a); 
      fila->fim = ant; 
      fila->qtd--;
      printf("Desfeito: paciente removido da fila.\n");
    }
    NozesPilha *rem = *topo; 
    *topo = rem->prox; 
    free(rem);
  } else {
    printf("Cancelado.\n");
  }
}

void mostrarLog(NozesPilha *topo) {
  while (topo) {
    printf("- %s\n", topo->operacao);
    topo = topo->prox;
  }
}

void sobre() {
  printf("\nNome: Pedro Da Silva Cunha\n");
  printf("Ciclo: 4 semestre\n");
  printf("Curso: Ciencia da Computacao\n");
  printf("Disciplina: Estrutura de dados\n");
  printf("Data: 21/06/2025\n");
  printf("\n");
}

void menu() {
  printf("1-Cadastrar paciente\n");
  printf("2-Mostrar paciente\n");
  printf("3-Atualizar paciente\n");
  printf("4-Remover paciente\n");
  printf("5-Salvar\n");
  printf("6-Carregar\n");
  printf("7-Desfazer\n");
  printf("8-Log\n");
  printf("9-Sobre\n");
  printf("10-Fila\n");
  printf("11-Chamar fila\n");
  printf("12-Mostrar fila\n");
  printf("13-Fila prioritaria\n");
  printf("14-Chamar prioritario\n");
  printf("15-Mostrar heap\n");
  printf("16-Inserir em arvore\n");
  printf("17-Ordenado idade\n");
  printf("18-ano\n");
  printf("19-mes\n");
  printf("20-dia\n");
  printf("0-Sair\n");
  printf("Escolha: ");
}

int main() {
  Lista *cadastro = criarLista();
  FilaNozes *fila = criarFilaNozes();
  HeapNozes *heap = criarHeapNozes();
  NozesArvore *arv_idade = NULL;
  NozesArvore *arv_ano = NULL;
  NozesArvore *arv_mes = NULL;
  NozesArvore *arv_dia = NULL;
  NozesPilha *log = NULL;
  
  int opcao = -1;
  char nome[100];
  char linha[10];

  while (opcao != 0) {
    menu();
    if (!fgets(linha, sizeof(linha), stdin)) continue;
    if (sscanf(linha, "%d", &opcao) != 1) continue;

    if (opcao == 1) {
      Paciente p = cadastrarPaciente();
      inserirPaciente(cadastro, p);
      push(&log, "Cadastrou paciente", NULL);
    } 

    else if (opcao == 2) {
      mostrarPacientes(cadastro);
    } 

    else if (opcao == 3) {
      printf("Nome: ");
      fgets(nome, sizeof(nome), stdin);
      nome[strcspn(nome, "\n")] = '\0';
      atualizarPaciente(cadastro, nome);
      push(&log, "Atualizou paciente", NULL);
    } 

    else if (opcao == 4) {
      printf("Nome: ");
      fgets(nome, sizeof(nome), stdin);
      nome[strcspn(nome, "\n")] = '\0';
      removerPaciente(cadastro, nome);
      push(&log, "Removeu paciente", NULL);
    } 

    else if (opcao == 5) {
      salvarArquivo(cadastro, "pacientes.txt");
    } 

    else if (opcao == 6) {
      carregarArquivo(cadastro, "pacientes.txt");
    } 

    else if (opcao == 7) {
      popComDesfazer(&log, fila);
    } 

    else if (opcao == 8) {
      mostrarLog(log);
    } 

    else if (opcao == 9) {
      sobre();
    } 

    else if (opcao == 10) {
      printf("Nome: ");
      fgets(nome, sizeof(nome), stdin);
      nome[strcspn(nome, "\n")] = '\0';
      Paciente *p = buscarPaciente(cadastro, nome);
      if (p) {
        enfileirarNozes(fila, p);
        push(&log, "Enfileirou paciente", p);
      }
    } 

    else if (opcao == 11) {
      desenfileirarNozes(fila);
      push(&log, "Atendeu paciente", NULL);
    } 

    else if (opcao == 12) {
      mostrarFilaNozes(fila);
    } 

    else if (opcao == 13) {
      printf("Nome: ");
      fgets(nome, sizeof(nome), stdin);
      nome[strcspn(nome, "\n")] = '\0';
      Paciente *p = buscarPaciente(cadastro, nome);
      if (p) {
        enfileirarPrioridadeNozes(heap, p);
        push(&log, "Enfileirou paciente prioritario", p);
      }
    } 

    else if (opcao == 14) {
      desenfileirarPrioridadeNozes(heap);
      push(&log, "Atendeu paciente prioritario", NULL);
    } 

    else if (opcao == 15) {
      mostrarHeapNozes(heap);
    } 

    else if (opcao == 16) {
      printf("Nome: ");
      fgets(nome, sizeof(nome), stdin);
      nome[strcspn(nome, "\n")] = '\0';
      Paciente *p = buscarPaciente(cadastro, nome);
      if (p) {
        arv_idade = inserirNaArvoreNozes(arv_idade, p, POR_IDADE);
        arv_ano = inserirNaArvoreNozes(arv_ano, p, POR_ANO);
        arv_mes = inserirNaArvoreNozes(arv_mes, p, POR_MES);
        arv_dia = inserirNaArvoreNozes(arv_dia, p, POR_DIA);
        push(&log, "Inseriu paciente na arvore", NULL);
      }
    } 

    else if (opcao == 17) {
      printf("\n--- Ordenado por idade ---\n");
      mostrarArvoreNozes(arv_idade);
    } 

    else if (opcao == 18) {
      printf("\n--- Ordenado por ano ---\n");
      mostrarArvoreNozes(arv_ano);
    } 

    else if (opcao == 19) {
      printf("\n--- Ordenado por mes ---\n");
      mostrarArvoreNozes(arv_mes);
    } 

    else if (opcao == 20) {
      printf("\n--- Ordenado por dia ---\n");
      mostrarArvoreNozes(arv_dia);
    }
  }

  return 0;
}
