#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

typedef struct topo {
  struct tabelaSimbolo *topo;
} Topo;

typedef struct tabelaSimbolo
{
  char lexema[30];
  char tipo[30];
  int nivel;
  int mem;
  struct tabelaSimbolo *prox;
} TabelaSimbolo;

typedef struct token
{
  char lexema[30];
  char simbolo[30];
} Token;

typedef struct list {
    char info[20];
    struct list *prox;
    struct list *ant;
} List;


int Linha = 0;
int BUFFER_SIZE = 255;
int i = 0;
char vector[2048] = {'\0'};

int nivel = 0;
Token token;
Topo *ini = NULL;

int Memoria = 0;
int rotulo;

char errorlog[100]= {'\0'};

int InfixGlobal = 0;
char infix[500][20] = {'\0'};
char postfix[500][20] = {'\0'};
char stack[500][20] = {'\0'};
List *postFixList = NULL;

bool is_digit1(char numeric[]);
bool is_alpha(char alpha[]);
void createList();
void printList();
void checkPostfixTypes();
void infixToPostfix();
void InsereInfix();
void GerarCodigoPostFix();
void iniciaTopo();
void insereTabela(char lexema[], char nometabela[], int nivel, int mem);
void PrintTabela();
void gerartoken(char lexema[], char simbolo[]);
bool verificarSimbolo(char lexema[]);
bool verificarCaracter(char caracter);
bool funcaofreitas(char caracter);
void pegartoken();
void Sintatico();
void analisa_bloco();
int analisa_et_variaveis();
void Gera(int params1, char params2[], int params3, int params4);
void Desempilha();
int procuraDuplicado(char lexemaAtual[], int nivel);
int procuraDuplicado1(char lexemaAtual[], int nivel);
int pesquisaTabela(char lexemaAtual[]);
bool colocaTipo(char lexema[], int nivel, char tipo[]);
int pesquisaVariavel(char lexemaAtual[]);
int pesquisaFuncProc(char lexemaAtual[]);
int analisa_variaveis();
void analisa_Tipo();
void analisa_comandos();
void analisa_comando_simples();
void analisa_atrib_chprocedimento();
void chamada_procedimento(char token[]);
void analisa_atribuicao();
void analisa_leia();
void analisa_escreva();
void analisa_enquanto();
void analisa_se();
void analisa_subrotinas();
void analisa_declaracao_procedimento();
void analisa_declaracao_funcao();
void analisa_expressao();
void analisa_expressao_simples();
void analisa_termo();
void analisa_fator();
void analisa_chamada_funcao(char lexema[]);
int is_variavel(char lexemaAtual[]);

bool is_digit(char numeric[])
{
    for(int i=0 ; numeric[i] != '\0';i++)
    {
        if(!isdigit(numeric[i])) return false;
    }
    return true;
}

bool is_alpha(char alpha[])
{
    for(int i=0 ; alpha[i] != '\0';i++)
    {
        if(isalpha(alpha[i])) return true;
    }
    return false;
}

void createList () {
    int i = 0;
    List *auxAnt = NULL;
    List *aux = NULL;

    while(strcmp(postfix[i], "\0")){
        //Enquanto a lista não acabar
        List *new;
        new = (List*) malloc(sizeof(List));

        if(postFixList == NULL){
            //Primeiro item da lista
            strcpy(new->info, postfix[i]);
            new->prox = NULL;
            new->ant = NULL;
            postFixList = new;

        } else if(postFixList->prox == NULL) {
            //Segundo elemento
            strcpy(new->info, postfix[i]);
            new->prox = NULL;
            new->ant = postFixList;
            postFixList->prox = new;

        } else {
            aux = postFixList;
            auxAnt = aux;
            aux = aux->prox;

            while(aux->prox != NULL){
                aux = aux->prox;
            }

            strcpy(new->info, postfix[i]);
            new->prox = NULL;
            new->ant = aux;
            aux->prox = new;
        }

        i++;
    }
}

void printList(){
    List *aux = postFixList;
    // IMPRIMIR P FRENTE
    do {
        aux = aux->prox;
    } while(aux != NULL);

    /*
    //IMPRIMIR INVERSO
    do {
        aux = aux->prox;
    } while(aux->prox != NULL);

    do {
        printf("%s\n", aux->info);
        aux = aux->ant;
    } while(aux != NULL);
    */
}

void checkPostfixTypes () {
    List *aux = postFixList;
    FILE *errorlog;
    do{ //Anda enquanto nao chegou no fim
        if (!strcmp(aux->info, "+")) {
            List *ultimo = aux->ant; //Posicao atual -1
            List *penultimo = ultimo->ant; //Posicao atual -2

            TabelaSimbolo *auxTabela = ini->topo; //Para ultimo elemento
            TabelaSimbolo *auxTabela2 = ini->topo; //Para penultimo elemento

            while(strcmp(auxTabela->lexema, ultimo->info)){
               auxTabela = auxTabela->prox;
            }

            if(auxTabela == NULL){
                //Não achou o elemento
                errorlog = fopen("errorlog.txt","w");
                fprintf(errorlog, "Linha (%d) Erro 201 : Não encontrou na tabela de simbolos.", Linha);
                fclose(errorlog);
                exit(0);
            }

            //Procurar o penultimo elemento na tabela de simbolos
            while(strcmp(auxTabela2->lexema, penultimo->info)){
               auxTabela2 = auxTabela2->prox;
            }

            if(auxTabela2 == NULL){
                //Não achou o elemento
                errorlog = fopen("errorlog.txt","w");
                fprintf(errorlog, "Linha (%d) Erro 202 : Não encontrou na tabela de simbolos.", Linha);
                fclose(errorlog);
                exit(0);
            }

            //Compara os dois elementos
            if(!strcmp(auxTabela->tipo, auxTabela2->tipo)){
                //São iguais
                strcpy(aux->info, "i"); //Salva tipo na posição atual

                //Remover os dois elementos comparados
                if(penultimo->ant != NULL){
                    aux->ant = penultimo->ant;
                    penultimo = penultimo->ant;
                    penultimo->prox = ultimo->prox;
                } else {
                    //penultimo é o primeiro da lista
                    aux = ultimo->prox;
                    aux->ant = NULL;
                }

            } else {
                //São diferentes
                errorlog = fopen("errorlog.txt","w");
                fprintf(errorlog, "Linha (%d) Erro 203 : Tipos não compativeis.", Linha);
                fclose(errorlog);
                exit(0);
            }

        } else if (!strcmp(aux->info, "-")) {
            //Precisa ser int
            List *ultimo = aux->ant; //Posicao atual -1
            List *penultimo = ultimo->ant; //Posicao atual -2

            TabelaSimbolo *auxTabela = ini->topo; //Para ultimo elemento
            TabelaSimbolo *auxTabela2 = ini->topo; //Para penultimo elemento

            while(strcmp(auxTabela->lexema, ultimo->info)){
               auxTabela = auxTabela->prox;
            }

            if(auxTabela == NULL){
                //Não achou o elemento
                errorlog = fopen("errorlog.txt","w");
                fprintf(errorlog, "Linha (%d) Erro 204: Não encontrou na tabela de simbolos.", Linha);
                fclose(errorlog);
                exit(0);
            }

            //Procurar o penultimo elemento na tabela de simbolos
            while(strcmp(auxTabela2->lexema, penultimo->info)){
               auxTabela2 = auxTabela2->prox;
            }

            if(auxTabela2 == NULL){
                //Não achou o elemento
                errorlog = fopen("errorlog.txt","w");
                fprintf(errorlog, "Linha (%d) Erro 205: Não encontrou na tabela de simbolos.", Linha);
                fclose(errorlog);
                exit(0);
            }

            //Compara os dois elementos
            if(!strcmp(auxTabela->tipo, auxTabela2->tipo)){
                //São iguais
                strcpy(aux->info, "i"); //Salva tipo na posição atual

                //Remover os dois elementos comparados
                if(penultimo->ant != NULL){
                    aux->ant = penultimo->ant;
                    penultimo = penultimo->ant;
                    penultimo->prox = ultimo->prox;
                } else {
                    //penultimo é o primeiro da lista
                    aux = ultimo->prox;
                    aux->ant = NULL;
                }

            } else {
                //São diferentes
                errorlog = fopen("errorlog.txt","w");
                fprintf(errorlog, "Linha (%d) Erro 206: Tipos não compativeis.", Linha);
                fclose(errorlog);
                exit(0);
            }

        } else if (!strcmp(aux->info, "*")) {
            //Precisa ser int
            List *ultimo = aux->ant; //Posicao atual -1
            List *penultimo = ultimo->ant; //Posicao atual -2

            TabelaSimbolo *auxTabela = ini->topo; //Para ultimo elemento
            TabelaSimbolo *auxTabela2 = ini->topo; //Para penultimo elemento

            while(strcmp(auxTabela->lexema, ultimo->info)){
               auxTabela = auxTabela->prox;
            }

            if(auxTabela == NULL){
                //Não achou o elemento
                errorlog = fopen("errorlog.txt","w");
                fprintf(errorlog, "Linha (%d) Erro 207: Não encontrou na tabela de simbolos.", Linha);
                fclose(errorlog);
                exit(0);
            }

            //Procurar o penultimo elemento na tabela de simbolos
            while(strcmp(auxTabela2->lexema, penultimo->info)){
               auxTabela2 = auxTabela2->prox;
            }

            if(auxTabela2 == NULL){
                //Não achou o elemento
                errorlog = fopen("errorlog.txt","w");
                fprintf(errorlog, "Linha (%d) Erro 208: Não encontrou na tabela de simbolos.", Linha);
                fclose(errorlog);
                exit(0);
            }

            //Compara os dois elementos
            if(!strcmp(auxTabela->tipo, auxTabela2->tipo)){
                //São iguais
                strcpy(aux->info, "i"); //Salva tipo na posição atual

                //Remover os dois elementos comparados
                if(penultimo->ant != NULL){
                    aux->ant = penultimo->ant;
                    penultimo = penultimo->ant;
                    penultimo->prox = ultimo->prox;
                } else {
                    //penultimo é o primeiro da lista
                    aux = ultimo->prox;
                    aux->ant = NULL;
                }

            } else {
                //São diferentes
                errorlog = fopen("errorlog.txt","w");
                fprintf(errorlog, "Linha (%d) Erro 209: Tipos não compativeis.", Linha);
                fclose(errorlog);
                exit(0);
            }

        } else if (!strcmp(aux->info, "div")) {
            //Precisa ser int
            List *ultimo = aux->ant; //Posicao atual -1
            List *penultimo = ultimo->ant; //Posicao atual -2

            TabelaSimbolo *auxTabela = ini->topo; //Para ultimo elemento
            TabelaSimbolo *auxTabela2 = ini->topo; //Para penultimo elemento

            while(strcmp(auxTabela->lexema, ultimo->info)){
               auxTabela = auxTabela->prox;
            }

            if(auxTabela == NULL){
                //Não achou o elemento
                errorlog = fopen("errorlog.txt","w");
                fprintf(errorlog, "Linha (%d) Erro 210: Não encontrou na tabela de simbolos.", Linha);
                fclose(errorlog);
                exit(0);
            }

            //Procurar o penultimo elemento na tabela de simbolos
            while(strcmp(auxTabela2->lexema, penultimo->info)){
               auxTabela2 = auxTabela2->prox;
            }

            if(auxTabela2 == NULL){
                //Não achou o elemento
                errorlog = fopen("errorlog.txt","w");
                fprintf(errorlog, "Linha (%d) Erro 211: Não encontrou na tabela de simbolos.", Linha);
                fclose(errorlog);
                exit(0);
            }

            //Compara os dois elementos
            if(!strcmp(auxTabela->tipo, auxTabela2->tipo)){
                //São iguais
                strcpy(aux->info, "i"); //Salva tipo na posição atual

                //Remover os dois elementos comparados
                if(penultimo->ant != NULL){
                    aux->ant = penultimo->ant;
                    penultimo = penultimo->ant;
                    penultimo->prox = ultimo->prox;
                } else {
                    //penultimo é o primeiro da lista
                    aux = ultimo->prox;
                    aux->ant = NULL;
                }

            } else {
                //São diferentes
                errorlog = fopen("errorlog.txt","w");
                fprintf(errorlog, "Linha (%d) Erro 212: Tipos não compativeis.", Linha);
                fclose(errorlog);
                exit(0);
            }

        } else if (!strcmp(aux->info, "<")) {
            //Entrada sao 2 int mas saida eh um bool
            List *ultimo = aux->ant; //Posicao atual -1
            List *penultimo = ultimo->ant; //Posicao atual -2

            TabelaSimbolo *auxTabela = ini->topo; //Para ultimo elemento
            TabelaSimbolo *auxTabela2 = ini->topo; //Para penultimo elemento

            while(strcmp(auxTabela->lexema, ultimo->info)){
               auxTabela = auxTabela->prox;
            }

            if(auxTabela == NULL){
                //Não achou o elemento
                errorlog = fopen("errorlog.txt","w");
                fprintf(errorlog, "Linha (%d) Erro 213: Não encontrou na tabela de simbolos.", Linha);
                fclose(errorlog);
                exit(0);
            }

            //Procurar o penultimo elemento na tabela de simbolos
            while(strcmp(auxTabela2->lexema, penultimo->info)){
               auxTabela2 = auxTabela2->prox;
            }

            if(auxTabela2 == NULL){
                //Não achou o elemento
                errorlog = fopen("errorlog.txt","w");
                fprintf(errorlog, "Linha (%d) Erro 214: Não encontrou na tabela de simbolos.", Linha);
                fclose(errorlog);
                exit(0);
            }

            //Compara os dois elementos
            if(!strcmp(auxTabela->tipo, auxTabela2->tipo)){
                //São iguais
                strcpy(aux->info, "i"); //Salva tipo na posição atual

                //Remover os dois elementos comparados
                if(penultimo->ant != NULL){
                    aux->ant = penultimo->ant;
                    penultimo = penultimo->ant;
                    penultimo->prox = ultimo->prox;
                } else {
                    //penultimo é o primeiro da lista
                    aux = ultimo->prox;
                    aux->ant = NULL;
                }

            } else {
                //São diferentes
                errorlog = fopen("errorlog.txt","w");
                fprintf(errorlog, "Linha (%d) Erro 215: Tipos não compativeis.", Linha);
                fclose(errorlog);
                exit(0);
            }
        } else if (!strcmp(aux->info, ">")) {
            //Entrada sao 2 int mas saida eh um bool
            List *ultimo = aux->ant; //Posicao atual -1
            List *penultimo = ultimo->ant; //Posicao atual -2

            TabelaSimbolo *auxTabela = ini->topo; //Para ultimo elemento
            TabelaSimbolo *auxTabela2 = ini->topo; //Para penultimo elemento

            while(strcmp(auxTabela->lexema, ultimo->info)){
               auxTabela = auxTabela->prox;
            }

            if(auxTabela == NULL){
                //Não achou o elemento
                errorlog = fopen("errorlog.txt","w");
                fprintf(errorlog, "Linha (%d) Erro 216: Não encontrou na tabela de simbolos.", Linha);
                fclose(errorlog);
                exit(0);
            }

            //Procurar o penultimo elemento na tabela de simbolos
            while(strcmp(auxTabela2->lexema, penultimo->info)){
               auxTabela2 = auxTabela2->prox;
            }

            if(auxTabela2 == NULL){
                //Não achou o elemento
                errorlog = fopen("errorlog.txt","w");
                fprintf(errorlog, "Linha (%d) Erro 217: Não encontrou na tabela de simbolos.", Linha);
                fclose(errorlog);
                exit(0);
            }

            //Compara os dois elementos
            if(!strcmp(auxTabela->lexema, auxTabela2->tipo)){
                //São iguais
                strcpy(aux->info, "i"); //Salva tipo na posição atual

                //Remover os dois elementos comparados
                if(penultimo->ant != NULL){
                    aux->ant = penultimo->ant;
                    penultimo = penultimo->ant;
                    penultimo->prox = ultimo->prox;
                } else {
                    //penultimo é o primeiro da lista
                    aux = ultimo->prox;
                    aux->ant = NULL;
                }

            } else {
                //São diferentes
                errorlog = fopen("errorlog.txt","w");
                fprintf(errorlog, "Linha (%d) Erro 218: Tipos não compativeis.", Linha);
                fclose(errorlog);
                exit(0);
            }
        } else if (!strcmp(aux->info, ">=")) {
            //Entrada sao 2 int mas saida eh um bool
            List *ultimo = aux->ant; //Posicao atual -1
            List *penultimo = ultimo->ant; //Posicao atual -2

            TabelaSimbolo *auxTabela = ini->topo; //Para ultimo elemento
            TabelaSimbolo *auxTabela2 = ini->topo; //Para penultimo elemento

            while(strcmp(auxTabela->lexema, ultimo->info)){
               auxTabela = auxTabela->prox;
            }

            if(auxTabela == NULL){
                //Não achou o elemento
                errorlog = fopen("errorlog.txt","w");
                fprintf(errorlog, "Linha (%d) Erro 219: Não encontrou na tabela de simbolos.", Linha);
                fclose(errorlog);
                exit(0);
            }

            //Procurar o penultimo elemento na tabela de simbolos
            while(strcmp(auxTabela2->lexema, penultimo->info)){
               auxTabela2 = auxTabela2->prox;
            }

            if(auxTabela2 == NULL){
                //Não achou o elemento
                errorlog = fopen("errorlog.txt","w");
                fprintf(errorlog, "Linha (%d) Erro 220: Não encontrou na tabela de simbolos.", Linha);
                fclose(errorlog);
                exit(0);
            }

            //Compara os dois elementos
            if(!strcmp(auxTabela->tipo, auxTabela2->tipo)){
                //São iguais
                strcpy(aux->info, "i"); //Salva tipo na posição atual

                //Remover os dois elementos comparados
                if(penultimo->ant != NULL){
                    aux->ant = penultimo->ant;
                    penultimo = penultimo->ant;
                    penultimo->prox = ultimo->prox;
                } else {
                    //penultimo é o primeiro da lista
                    aux = ultimo->prox;
                    aux->ant = NULL;
                }

            } else {
                //São diferentes
                errorlog = fopen("errorlog.txt","w");
                fprintf(errorlog, "Linha (%d) Erro 221: Tipos não compativeis.", Linha);
                fclose(errorlog);
                exit(0);
            }
        } else if (!strcmp(aux->info, "<=")) {
            //Entrada sao 2 int mas saida eh um bool
            List *ultimo = aux->ant; //Posicao atual -1
            List *penultimo = ultimo->ant; //Posicao atual -2

            TabelaSimbolo *auxTabela = ini->topo; //Para ultimo elemento
            TabelaSimbolo *auxTabela2 = ini->topo; //Para penultimo elemento

            while(strcmp(auxTabela->lexema, ultimo->info)){
               auxTabela = auxTabela->prox;
            }

            if(auxTabela == NULL){
                //Não achou o elemento
                errorlog = fopen("errorlog.txt","w");
                fprintf(errorlog, "Linha (%d) Erro 222: Não encontrou na tabela se simbolos.", Linha);
                fclose(errorlog);
                exit(0);
            }

            //Procurar o penultimo elemento na tabela de simbolos
            while(strcmp(auxTabela2->lexema, penultimo->info)){
               auxTabela2 = auxTabela2->prox;
            }

            if(auxTabela2 == NULL){
                //Não achou o elemento
                errorlog = fopen("errorlog.txt","w");
                fprintf(errorlog, "Linha (%d) Erro 223: Não encontrou na tabela se simbolos.", Linha);
                fclose(errorlog);
                exit(0);
            }

            //Compara os dois elementos
            if(!strcmp(auxTabela->lexema, auxTabela2->tipo)){
                //São iguais
                strcpy(aux->info, "i"); //Salva tipo na posição atual

                //Remover os dois elementos comparados
                if(penultimo->ant != NULL){
                    aux->ant = penultimo->ant;
                    penultimo = penultimo->ant;
                    penultimo->prox = ultimo->prox;
                } else {
                    //penultimo é o primeiro da lista
                    aux = ultimo->prox;
                    aux->ant = NULL;
                }

            } else {
                //São diferentes
                errorlog = fopen("errorlog.txt","w");
                fprintf(errorlog, "Linha (%d) Erro 224: Tipos não compativeis.", Linha);
                fclose(errorlog);
                exit(0);
            }
        } else {
            //So anda
        }

        aux = aux->prox;
    } while(aux != NULL);

}

void infixToPostfix() {
    int infixAux = 0, postfixAux = 0, stackAux = 0;
    strcpy(stack[stackAux], "(");
    stackAux++;

    while (strcmp(infix[infixAux], "\0")){
        if(strcmp(infix[infixAux], "<") == 0){
            //Desempilha ate achar "(" e coloca o < na stack
            //OBS: stackAux e postfixAux ficam na posição do elemento vazio
            while(strcmp(stack[stackAux-1], "(") != 0){
                strcpy(postfix[postfixAux], stack[stackAux-1]);
                postfixAux++;
                stackAux--;
            }
            //Terminou de desempilhar agora coloca o "<" na stack
            if(stackAux != 1){
                //Se stackAux não for 1 que é uma posição apos o primeiro "(" substituimos o "(" da stack-1 pelo infix[infixAux]
                strcpy(stack[stackAux-1], infix[infixAux]);
                //stackAux ja está na posição vazia então não ha necessidade de incrementa-la
            } else {
                strcpy(stack[stackAux], infix[infixAux]);
                stackAux++;
            }

        } else if(strcmp(infix[infixAux], ">") == 0){
            //Desempilha ate achar "(" e coloca o < na stack
            //OBS: stackAux e postfixAux ficam na posição do elemento vazio
            while(strcmp(stack[stackAux-1], "(") != 0){
                strcpy(postfix[postfixAux], stack[stackAux-1]);
                postfixAux++;
                stackAux--;
            }

            //Terminou de desempilhar agora coloca o ">" na stack
            if(stackAux != 1){
                //Se stackAux não for 1 que é uma posição apos o primeiro "(" substituimos o "(" da stack-1 pelo infix[infixAux]
                strcpy(stack[stackAux-1], infix[infixAux]);
                //stackAux ja está na posição vazia então não ha necessidade de incrementa-la
            } else {
                strcpy(stack[stackAux], infix[infixAux]);
                stackAux++;
            }

        } else if(strcmp(infix[infixAux], "<=") == 0){
            //Desempilha ate achar "(" e coloca o "<=" na stack
            //OBS: stackAux e postfixAux ficam na posição do elemento vazio
            while(strcmp(stack[stackAux-1], "(") != 0){
                strcpy(postfix[postfixAux], stack[stackAux-1]);
                postfixAux++;
                stackAux--;
            }

            //Terminou de desempilhar agora coloca o "<=" na stack
            if(stackAux != 1){
                //Se stackAux não for 1 que é uma posição apos o primeiro "(" substituimos o "(" da stack-1 pelo infix[infixAux]
                strcpy(stack[stackAux-1], infix[infixAux]);
                //stackAux ja está na posição vazia então não ha necessidade de incrementa-la
            } else {
                strcpy(stack[stackAux], infix[infixAux]);
                stackAux++;
            }

        } else if(strcmp(infix[infixAux], ">=") == 0){
            //Desempilha ate achar "(" e coloca o ">=" na stack
            //OBS: stackAux e postfixAux ficam na posição do elemento vazio
            while(strcmp(stack[stackAux-1], "(") != 0){
                strcpy(postfix[postfixAux], stack[stackAux-1]);
                postfixAux++;
                stackAux--;
            }

            //Terminou de desempilhar agora coloca o ">=" na stack
            if(stackAux != 1){
                //Se stackAux não for 1 que é uma posição apos o primeiro "(" substituimos o "(" da stack-1 pelo infix[infixAux]
                strcpy(stack[stackAux-1], infix[infixAux]);
                //stackAux ja está na posição vazia então não ha necessidade de incrementa-la
            } else {
                strcpy(stack[stackAux], infix[infixAux]);
                stackAux++;
            }

        } else if(strcmp(infix[infixAux], "+") == 0){
            if(strcmp(stack[stackAux-1], "(") == 0){ //Se o topo da stack for "(" só insere na stack
                strcpy(stack[stackAux], infix[infixAux]);
                stackAux++;

            } else if(strcmp(stack[stackAux-1], "-") == 0){ //"-" tem a mesma prioridade que "+" errorlogo "-" vai para posfix e lido vai para topo da stack
                strcpy(postfix[postfixAux], stack[stackAux-1]);
                postfixAux++;
                //Nao subtrai o stackAux pq somente vai ser trocado o que esta naquela posicao
                strcpy(stack[stackAux-1], infix[infixAux]);

            } else if(strcmp(stack[stackAux-1], "*") == 0) { // "*" tem maior prioridade, errorlogo ele deve ir para postfix e o "+" ser adicionado na stack
                strcpy(postfix[postfixAux], stack[stackAux-1]);
                postfixAux++;
                //Nao mudo o stackAux pois voce tira o * do stack e coloca o + no lugar
                strcpy(stack[stackAux-1], infix[infixAux]);

            } else if(strcmp(stack[stackAux-1], "div") == 0) { // "*" tem maior prioridade, errorlogo ele deve ir para postfix e o "+" ser adicionado na stack
                strcpy(postfix[postfixAux], stack[stackAux-1]);
                postfixAux++;
                //Nao mudo o stackAux pois voce tira o * do stack e coloca o + no lugar
                strcpy(stack[stackAux-1], infix[infixAux]);

            } else { // Caso Default: Somente adiciona o "+" na stack
                strcpy(stack[stackAux], infix[infixAux]);
                stackAux++;
            }
        } else if(strcmp(infix[infixAux], "-") == 0) {
            if(strcmp(stack[stackAux-1], "(") == 0){
                strcpy(stack[stackAux], infix[infixAux]);
                stackAux++;

            } else if(strcmp(stack[stackAux-1], "+") == 0){
                strcpy(postfix[postfixAux], stack[stackAux-1]);
                postfixAux++;
                //Nao subtrai o stackAux pq somente vai ser trocado o que esta naquela posicao
                strcpy(stack[stackAux-1], infix[infixAux]);

            } else if(strcmp(stack[stackAux-1], "*") == 0) {
                strcpy(postfix[postfixAux], stack[stackAux-1]);
                postfixAux++;
                //Nao mudo o stackAux pois voce tira o * do stack e coloca o + no lugar
                strcpy(stack[stackAux-1], infix[infixAux]);

            } else if(strcmp(stack[stackAux-1], "div") == 0) {
                strcpy(postfix[postfixAux], stack[stackAux-1]);
                postfixAux++;
                //Nao mudo o stackAux pois voce tira o * do stack e coloca o + no lugar
                strcpy(stack[stackAux-1], infix[infixAux]);

            } else {
                strcpy(stack[stackAux], infix[infixAux]);
                stackAux++;

            }
        } else if(strcmp(infix[infixAux], "*") == 0){
            if(strcmp(stack[stackAux-1], "(") == 0){
                strcpy(stack[stackAux], infix[infixAux]);
                stackAux++;

            } else if(strcmp(stack[stackAux-1], "div") == 0) {
                strcpy(postfix[postfixAux], stack[stackAux-1]);
                postfixAux++;
                //Nao mudo o stackAux pois voce tira o * do stack e coloca o + no lugar
                strcpy(stack[stackAux-1], infix[infixAux]);

            }else {
                strcpy(stack[stackAux], infix[infixAux]);
                stackAux++;

            }
        } else if(strcmp(infix[infixAux], "div") == 0){
            if(strcmp(stack[stackAux-1], "(") == 0){
                strcpy(stack[stackAux], infix[infixAux]);
                stackAux++;

            } else if(strcmp(stack[stackAux-1], "*") == 0) {
                strcpy(postfix[postfixAux], stack[stackAux-1]);
                postfixAux++;
                //Nao mudo o stackAux pois voce tira o * do stack e coloca o + no lugar
                strcpy(stack[stackAux-1], infix[infixAux]);

            } else {
                strcpy(stack[stackAux], infix[infixAux]);
                stackAux++;

            }
        } else if(strcmp(infix[infixAux], ")") == 0){
            while(strcmp(stack[stackAux-1], "(") != 0){
                strcpy(postfix[postfixAux], stack[stackAux-1]);
                postfixAux++;
                stackAux--;
            }

            if(stackAux != 1){
                //Se stackAux não for 1 que é uma posição apos o primeiro "(" temos que decrementar o stackAux para remover o "(" remanescente
                stackAux--;
            }

        } else if(strcmp(infix[infixAux], "(") == 0){
            if(strcmp(infix[infixAux+1], "-") == 0){
                infixAux++;
                char auxString[10] = {'\0'};
                strcpy(auxString, "U");
                infixAux++;

                while(strcmp(infix[infixAux], ")") != 0){
                    strcat(auxString, infix[infixAux]);
                    infixAux++;
                }
                strcpy(postfix[postfixAux], auxString);
                postfixAux++;
            }
        } else {
            strcpy(postfix[postfixAux], infix[infixAux]);
            postfixAux++;
        }

        infixAux++;
    }

    //Acabou o arquivo, hora de jogar tudo da stack para postfix
    while(strcmp(stack[stackAux-1], "(") != 0){
        strcpy(postfix[postfixAux], stack[stackAux-1]);
        postfixAux++;
        stackAux--;
    }
}


void InsereInfix(){
    strcpy(infix[InfixGlobal],token.lexema);
    InfixGlobal++;
}

void GerarCodigoPostFix(){
    int postfixAux = 0;
    int mem, i = 0;
    printf("\n\n\n\t\t\t AQUI\n\n\n");
    while(strcmp(postfix[i],"\0")){
        printf("\n\t\t\t %s",postfix[i]);
        printf("\n\t\t\t %d",is_alpha(postfix[i]));
        i++;
    }

    while(strcmp(postfix[postfixAux],"\0")){

        if(is_digit(postfix[postfixAux])){
            if(postfix[postfixAux][0] == 85){
                Gera(-1,"INV",-1,-1);
            }
            Gera(-1,"LDC",atoi(postfix[postfixAux]),-1);
        }else{
            if(is_alpha(postfix[postfixAux]) && strcmp(postfix[postfixAux],"div")){
                if(postfix[postfixAux][0] == 85){
                    Gera(-1,"INV",-1,-1);
                }
                if(!strcmp(postfix[postfixAux], "verdadeiro") || !strcmp(postfix[postfixAux], "falso")){
                  if(!strcmp(postfix[postfixAux], "verdadeiro")) {Gera(-1,"LDC",1,-1);}
                  else {Gera(-1,"LDC",0,-1);}
                }else{
                  if(is_variavel(postfix[postfixAux])){
                    mem = pesquisaVariavel(postfix[postfixAux]);
                    Gera(-1,"LDV",mem,-1);
                  }else{
                    analisa_chamada_funcao(postfix[postfixAux]);
                  }
                }

            }else{
                if(!strcmp(postfix[postfixAux],"+")){
                    Gera(-1,"ADD",-1,-1);
                }else{
                    if(!strcmp(postfix[postfixAux],"-")){
                        Gera(-1,"SUB",-1,-1);
                    }else{
                        if(!strcmp(postfix[postfixAux],"*")){
                            Gera(-1,"MULT",-1,-1);
                        }else{
                           if(!strcmp(postfix[postfixAux],"div")){
                                Gera(-1,"DIVI",-1,-1);
                            }else{
                                if(!strcmp(postfix[postfixAux],">")){
                                    Gera(-1,"CMA",-1,-1);
                                }else{
                                    if(!strcmp(postfix[postfixAux],"<")){
                                        Gera(-1,"CME",-1,-1);
                                    }else{
                                        if(!strcmp(postfix[postfixAux],"=")){
                                            Gera(-1,"CEQ",-1,-1);
                                        }else{
                                            if(!strcmp(postfix[postfixAux],"!=")){
                                                Gera(-1,"CDIF",-1,-1);
                                            }else{
                                                if(!strcmp(postfix[postfixAux],"<=")){
                                                    Gera(-1,"CMEQ",-1,-1);
                                                }else{
                                                    if(!strcmp(postfix[postfixAux],">=")){
                                                        Gera(-1,"CMAQ",-1,-1);
                                                    }else{
                                                        if(!strcmp(postfix[postfixAux],"e")){
                                                            Gera(-1,"AND",-1,-1);
                                                        }else{
                                                            if(!strcmp(postfix[postfixAux],"ou")){
                                                                Gera(-1,"OR",-1,-1);
                                                            }else{
                                                                if(!strcmp(postfix[postfixAux],"nao")){
                                                                    Gera(-1,"NEG",-1,-1);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        postfixAux++;
    }

    InfixGlobal = 0;
    memset(infix, '\0', sizeof(infix));
    memset(postfix, '\0', sizeof(postfix));
}

void iniciaTopo(){
    Topo *aux = ( Topo *) malloc(sizeof (Topo));
    ini = aux;
    ini->topo = NULL;
    printf("\n\n%s",ini->topo);
}

void insereTabela(char lexema[], char nometabela[], int nivel, int mem){

    TabelaSimbolo *aux = ( TabelaSimbolo *) malloc(sizeof (TabelaSimbolo));
    strcpy(aux->lexema,lexema);
    strcpy(aux->tipo,nometabela);
    aux->nivel= nivel;
    aux->mem = mem;


    aux->prox = ini->topo;
    ini->topo = aux;

    printf("\n\n             %s", ini->topo->lexema);
    printf("\n\n             %s", aux->tipo);
    printf("\n\n             %d", aux->nivel);
    printf("\n\n             %d", aux->mem);
     printf("\n\n             %s\n", aux->prox);

}

void PrintTabela(){
    TabelaSimbolo *aux = ( TabelaSimbolo *) malloc(sizeof (TabelaSimbolo));
    aux = ini->topo;

    while(aux != NULL){
        printf("\n\n\t\t\tlexema: %s",aux->lexema);
        printf("\n\n\t\t\ttipo: %s", aux->tipo);
        printf("\n\n\t\t\tnivel: %d", aux->nivel);
        printf("\n\n\t\t\tmemoria: %d", aux->mem);
        printf("\n\n\t\t\tabaixo: %s\n", aux->prox);
         aux = aux->prox;
    }
}

void gerartoken(char lexema[], char simbolo[])
{
    strcpy(token.lexema, lexema);
    strcpy(token.simbolo, simbolo);
    printf("%s %s\n",token.simbolo,token.lexema );
}

bool verificarSimbolo(char lexema[])
{

  if (!strcmp(lexema, "programa"))
  {
    gerartoken(lexema, "sprograma");
  }
  else
  {
    if (!strcmp(lexema, "inicio"))
    {
      gerartoken(lexema, "sinicio");
    }
    else
    {
      if (!strcmp(lexema, "fim"))
      {
        gerartoken(lexema, "sfim");
      }
      else
      {
        if (!strcmp(lexema, "procedimento"))
        {
          gerartoken(lexema, "sprocedimento");
        }
        else
        {
          if (!strcmp(lexema, "funcao"))
          {
            gerartoken(lexema, "sfuncao");
          }
          else
          {
            if (!strcmp(lexema, "se"))
            {
              gerartoken(lexema, "sse");
            }
            else
            {
              if (!strcmp(lexema, "entao"))
              {
                gerartoken(lexema, "sentao");
              }
              else
              {
                if (!strcmp(lexema, "enquanto"))
                {
                  gerartoken(lexema, "senquanto");
                }
                else
                {
                  if (!strcmp(lexema, "faca"))
                  {
                    gerartoken(lexema, "sfaca");
                  }
                  else
                  {
                    if (!strcmp(lexema, ":="))
                    {
                      gerartoken(lexema, "satribuicao");
                    }
                    else
                    {
                      if (!strcmp(lexema, "escreva"))
                      {
                        gerartoken(lexema, "sescreva");
                      }
                      else
                      {
                        if (!strcmp(lexema, "leia"))
                        {
                          gerartoken(lexema, "sleia");
                        }
                        else
                        {
                          if (!strcmp(lexema, "var"))
                          {
                            gerartoken(lexema, "svar");
                          }
                          else
                          {
                            if (!strcmp(lexema, "inteiro"))
                            {
                              gerartoken(lexema, "sinteiro");
                            }
                            else
                            {
                              if (!strcmp(lexema, "booleano"))
                              {
                                gerartoken(lexema, "sbooleano");
                              }
                              else
                              {
                                if (is_digit(lexema))
                                {
                                  gerartoken(lexema, "snumero");
                                }
                                else
                                {
                                  if (!strcmp(lexema, "."))
                                  {
                                    gerartoken(lexema, "sponto");
                                  }
                                  else
                                  {
                                    if (!strcmp(lexema, ";"))
                                    {
                                      gerartoken(lexema, "sponto_virgula");
                                    }
                                    else
                                    {
                                      if (!strcmp(lexema, ","))
                                      {
                                        gerartoken(lexema, "svirgula");
                                      }
                                      else
                                      {
                                        if (!strcmp(lexema, "("))
                                        {
                                          gerartoken(lexema, "sabre_parenteses");
                                        }
                                        else
                                        {
                                          if (!strcmp(lexema, ")"))
                                          {
                                            gerartoken(lexema, "sfecha_parenteses");
                                          }
                                          else
                                          {
                                            if (!strcmp(lexema, ">"))
                                            {
                                              gerartoken(lexema, "smaior");
                                            }
                                            else
                                            {
                                              if (!strcmp(lexema, ">="))
                                              {
                                                gerartoken(lexema, "smaiorig");
                                              }
                                              else
                                              {
                                                if (!strcmp(lexema, "="))
                                                {
                                                  gerartoken(lexema, "sig");
                                                }
                                                else
                                                {
                                                  if (!strcmp(lexema, "<"))
                                                  {
                                                    gerartoken(lexema, "smenor");
                                                  }
                                                  else
                                                  {
                                                    if (!strcmp(lexema, "<="))
                                                    {
                                                      gerartoken(lexema, "smenorig");
                                                    }
                                                    else
                                                    {
                                                      if (!strcmp(lexema, "!="))
                                                      {
                                                        gerartoken(lexema, "sdif");
                                                      }
                                                      else
                                                      {
                                                        if (!strcmp(lexema, "+"))
                                                        {
                                                          gerartoken(lexema, "smais");
                                                        }
                                                        else
                                                        {
                                                          if (!strcmp(lexema, "-"))
                                                          {
                                                            gerartoken(lexema, "smenos");
                                                          }
                                                          else
                                                          {
                                                            if (!strcmp(lexema, "*"))
                                                            {
                                                              gerartoken(lexema, "smult");
                                                            }
                                                            else
                                                            {
                                                              if (!strcmp(lexema, "div"))
                                                              {
                                                                gerartoken(lexema, "sdiv");
                                                              }
                                                              else
                                                              {
                                                                if (!strcmp(lexema, "e"))
                                                                {
                                                                  gerartoken(lexema, "se");
                                                                }
                                                                else
                                                                {
                                                                  if (!strcmp(lexema, "ou"))
                                                                  {
                                                                    gerartoken(lexema, "sou");
                                                                  }
                                                                  else
                                                                  {
                                                                    if (!strcmp(lexema, "senao"))
                                                                    {
                                                                      gerartoken(lexema, "ssenao");
                                                                    }
                                                                    else
                                                                    {
                                                                      if (!strcmp(lexema, ":"))
                                                                      {
                                                                        gerartoken(lexema, "sdoispontos");
                                                                      }
                                                                      else
                                                                      {
                                                                        if (!strcmp(lexema, "verdadeiro"))
                                                                        {
                                                                          gerartoken(lexema, "sverdadeiro");
                                                                        }
                                                                        else
                                                                        {
                                                                          if (!strcmp(lexema, "falso"))
                                                                          {
                                                                            gerartoken(lexema, "sfalso");
                                                                          }
                                                                          else
                                                                          {
                                                                            if(!strcmp(lexema,"nao"))
                                                                            {
                                                                                gerartoken(lexema,"snao");
                                                                            }else
                                                                            {
                                                                                gerartoken(lexema, "sidentificador");
                                                                            }
                                                                          }
                                                                        }
                                                                      }
                                                                    }
                                                                  }
                                                                }
                                                              }
                                                            }
                                                          }
                                                        }
                                                      }
                                                    }
                                                  }
                                                }
                                              }
                                            }
                                          }
                                        }
                                      }
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

bool verificarCaracter(char caracter)
{
    char result;
    result = tolower(caracter);
    // Verifica se o caracter faz parte do alfabeto
    if ((result >= 97 && result <= 122) || (result >= 48 && result <= 57)) { // Entre a e z (minusculo) ou Entre 0 e 9
        return true;
    }

  return false;
}

bool funcaofreitas(char caracter)
{
    char result;
    char aux[2]= {'\0'};
    aux[0] = caracter;
    result = tolower(caracter);

    if (!((result >= 97 && result <= 122) || (result >= 48 && result <= 62) || (result == '\0' || result == 9 || result == 10 || result == 32 || result == 40 || result == 41 ||  caracter == 42 || result == 43 || result == 44 || result == 45 || result == 46))) { // Entre a e z (minusculo) ou Entre 0 e teste_9
            //gerartoken(aux, "sLinha (%d) Erro");
            printf("\nLinha (%d) Error -> '%c' encontrado\n\n", Linha, caracter);
            //exit(0);
            return true;
    }
    return false;
}

void pegartoken()
{
  int auxtoken = 0;  // AUXTOKEN PARA CONTROLARMOS A FORMACAO DO TOKEN
  char lexema[30] = {'\0'}; // Inicar string zerada

  while (vector[i])
  {
    // Verificar se e terminal
    if (vector[i] == 13 || vector[i] == 9 || vector[i] == 32 || vector[i] == 59 || vector[i] == 10 || vector[i] == 44 || vector[i] == 61 || vector[i] == 43 || vector[i] == 45 || vector[i] == 40 || vector[i] == 41 || vector[i] == 46  || vector[i] == 42)
    { // 32 = space; 59 = ponto e virgula; 10 = new line , 44 == , , 58 == : , 43 == + , 45 == - , 40 == ( , 41 == ) , 46 == . , 42 == *

      if (vector[i] == 10){
        Linha++;
      }
      if(lexema[0] != '\0'){
      verificarSimbolo(lexema);
      break;
      }

      if (vector[i] != 32 && vector[i] != 10 && vector[i] != 9 && vector[i] != 13)
      {
        lexema[0] = vector[i];
        verificarSimbolo(lexema);
        i++;
        break;

      }
      i++;
    }

    if (vector[i] == 60 || vector[i] == 62  || vector[i] == 58 || vector[i] == 33)
    {                      ///  61 == = , 60 == < , 62 == > , 33 == ! , 58 == :
      if (vector[i] == 60) /// <
      {
        if (vector[i + 1] == 61) /// =
        {
            if(lexema[0] != '\0'){
                verificarSimbolo(lexema);
                break;}

          verificarSimbolo("<=");
          i += 2;
          break;
        }
        else
        {
            if(lexema[0] != '\0'){
                verificarSimbolo(lexema);
                break;}

          verificarSimbolo("<");
          ++i;
          break;

        }
      }
      else
      {
        if (vector[i] == 62) /// >
        {
          if (vector[i + 1] == 61) /// =
          {
            if(lexema[0] != '\0'){
                verificarSimbolo(lexema);
                break;}


            verificarSimbolo(">=");
            i += 2;
            break;
            ;
          }
          else
          {
              if(lexema[0] != '\0'){
                verificarSimbolo(lexema);
                break;}

            verificarSimbolo(">");
            ++i;
            break;

          }
        }
        else
        {
          if (vector[i] == 58) /// :
          {
            if (vector[i + 1] == 61) /// =
            {
                if(lexema[0] != '\0'){
                verificarSimbolo(lexema);
                break;}

              verificarSimbolo(":=");
              i += 2;
              break;

            }
            else
            {
                if(lexema[0] != '\0'){
                verificarSimbolo(lexema);
                break;}

              verificarSimbolo(":");
              ++i;
              break;

            }
          }
          if (vector[i] == 33) /// !
          {
            if (vector[i + 1] == 61) /// =
            {
                if(lexema[0] != '\0'){
                verificarSimbolo(lexema);
                break;}

              verificarSimbolo("!=");
              i += 2;
              break;

            }
          }
        }
      }
    }

    if (vector[i] == 123) /// PARA PODER FAZER COMENTARIO -> 123 = {
    {
      while (vector[i] != 125 && vector[i]) /// ENQUANTO NAO ACHAR O FECHA COMENTARIO IGNORA TUDO -> 125 = }
      {
        i++;
      }
      i++; /// PARA PEGAR O PROXIMO DPS QUE FECHAR O COMENTARIO
    }

    if(funcaofreitas(vector[i]))
    {
        i++;
    }

    while (vector[i] == 32 || vector[i] == 10 || vector[i] == 9 || vector[i] == 13)
    {
      if(vector[i] == 10) Linha++;
      i++;
    } /// PARA GARANTIR QUE NAO IRÁ VERIFICAR UM ESPAÇO NO IF ABAIXO

    if (verificarCaracter(vector[i])) /// PARA VER SE E DO ALFABETO
    {
      lexema[auxtoken] = vector[i];
      auxtoken++;
      ++i;
    }
  }
}

void main(int argc, char **argv)
{
  FILE *program, *program1;
  char buffer[BUFFER_SIZE];

  iniciaTopo();

  program = fopen(argv[1], "r");
  while (fgets(buffer, sizeof buffer, program) != NULL) {
    // Salvar o programa inteiro dentro de um vector
    strcat(vector, buffer);
  }
  program1 = fopen("gera2.obj", "w");
  fclose(program1);
  program1 = fopen("errorlog.txt", "w");
  fclose(program1);
  Sintatico();
  PrintTabela();
}

void Sintatico()
{
    FILE *errorlog;
    printf("\n Sintatico\n");
    pegartoken();
    insereTabela("#","marca", 0, 0);
    rotulo = 1;
    if(!strcmp(token.simbolo,"sprograma"))
    {
        pegartoken();
        if(!strcmp(token.simbolo , "sidentificador"))
        {
            insereTabela(token.lexema, "nomedeprograma", 0, 0);
            Gera(-1,"START", -1, -1);
            Gera(-1,"ALLOC", 0, 1);
            Memoria++;
            pegartoken();
            if(!strcmp(token.simbolo , "sponto_virgula"))
            {
                analisa_bloco();
                if(!strcmp(token.simbolo , "sponto"))
                {
                    Gera(-1,"DALLOC",0, 1);
                    Gera(-1,"HLT", -1, -1);
                    
                    errorlog = fopen("errorlog.txt","w");
                    fprintf(errorlog, "SUCESSO: Programa Compilado com sucesso!");
                    fclose(errorlog); exit(0);
                }
                else{
                    errorlog = fopen("errorlog.txt","w");
                    fprintf(errorlog, "Linha (%d) Erro 1 : Espera-se um ponto.", Linha);
                    fclose(errorlog); exit(0);
                }
            }
            else{
                errorlog = fopen("errorlog.txt","w");
                fprintf(errorlog, "Linha (%d) Erro 2 : Espera-se um ponto e virgula.", Linha);
                fclose(errorlog); exit(0);
            }
        }
        else{
            errorlog = fopen("errorlog.txt","w");
            fprintf(errorlog, "Linha (%d) Erro 3 : Espera-se um identificador.", Linha);
            fclose(errorlog); exit(0);
        }
    }
    else{
        errorlog = fopen("errorlog.txt","w");
        fprintf(errorlog, "Linha (%d) Erro 4 : Espera-se um programa.", Linha);
        fclose(errorlog); exit(0);
    }
}

void analisa_bloco()
{
    FILE *errorlog;
    int desaloc = 0;
    int mems = Memoria;
    printf("\n -------- analisa_bloco\n");
    pegartoken();

    desaloc = desaloc + analisa_et_variaveis();
    analisa_subrotinas();

    analisa_comandos();
    Gera(-1,"DALLOC",mems,desaloc);
}

int analisa_et_variaveis(){
    FILE *errorlog;
    printf("\n -------- analisa_et_variaveis\n");
    int desaloc = 0;
      if(!strcmp(token.simbolo, "svar"))
      {
        pegartoken();
        if(!strcmp(token.simbolo, "sidentificador"))
        {
            while(!strcmp(token.simbolo, "sidentificador")){
                desaloc = desaloc + analisa_variaveis();
                if(!strcmp(token.simbolo, "sponto_virgula")){
                    pegartoken();
                }
                else{
                    errorlog = fopen("errorlog.txt","w");
                    fprintf(errorlog, "Linha (%d) Erro 5 : Espera-se um ponto e virgula.", Linha);
                    fclose(errorlog); exit(0);
                }
                if(!strcmp(token.simbolo, "svar")){
                    pegartoken();
                    if(strcmp(token.simbolo, "sidentificador")){
                        errorlog = fopen("errorlog.txt","w");
                        fprintf(errorlog, "Linha (%d) Erro 6 : Espera-se um identificador.", Linha);
                        fclose(errorlog); exit(0);
                    }
                }
            }
        }
        else{
            errorlog = fopen("errorlog.txt","w");
            fprintf(errorlog, "Linha (%d) Erro 6 : Espera-se um identificador.", Linha);
            fclose(errorlog); exit(0);
        }
      }
  return desaloc;
}

void Gera(int params1, char params2[], int params3, int params4){
  FILE *Assembly;
  char linha[20] = {'\0'};
  char teste[10] = {'\0'};
  char snum[5]= {'\0'};

  if(params1 == -1){
    strcat(linha,"    ");
  }else{
    sprintf(snum, "%d", params1);
    strcat(linha,snum);
    if(strlen(snum) == 1){
      strcat(linha,"   ");
    }else{
      if(strlen(snum) == 2){
        strcat(linha,"  ");
      }
      else{
        if(strlen(snum) == 3){
          strcat(linha," ");
        }
      }
    }
  }

  if(params2 != NULL){
    strcpy(teste, params2);
    if(strlen(teste) == 0){
    strcat(teste, "        ");
    }
    else{
      if(strlen(teste) == 1){
        strcat(teste, "       ");
      }
      else {
        if(strlen(teste) == 2){
          strcat(teste, "      ");
        }
        else{
          if(strlen(teste) == 3){
            strcat(teste, "     ");
          }
          else{
            if(strlen(teste) == 4){

              strcat(teste,"    ");
              printf("\na%sa",linha);
            }
            else{
              if(strlen(teste) == 5){
                strcat(teste, "   ");
              }
              else {
                if(strlen(teste) == 6){
                  strcat(teste, "  ");
                }
                else{
                    if(strlen(teste) == 7){
                      strcat(teste, " ");
                    }
                  }
                }
              }
            }
          }
        }
      }
    strcat(linha, teste);
  }else{
    strcat(linha,"NULL    ");
  }

  if(params3 == -1){
    strcat(linha, "    ");
  }else{
    sprintf(snum, "%d", params3);
    strcat(linha,snum);
    if(strlen(snum) == 1){
      strcat(linha,"   ");
    }else{
      if(strlen(snum) == 2){
        strcat(linha,"  ");
      }
      else{
        if(strlen(snum) == 3){
          strcat(linha," ");
        }
      }
    }
  }

  if(params4 == -1){
    strcat(linha, "    ");
  }else{
    sprintf(snum, "%d", params4);
    strcat(linha,snum);
    if(strlen(snum) == 1){
      strcat(linha,"   ");
    }else{
      if(strlen(snum) == 2){
        strcat(linha,"  ");
      }
      else{
        if(strlen(snum) == 3){
          strcat(linha," ");
        }
      }
    }

  }

  Assembly = fopen("gera2.obj","a");


  printf("\n\n%sa\n\n", linha);
  strcat(linha, "\n");
  fputs(linha, Assembly);
  fclose(Assembly);
}

void Desempilha(){
    TabelaSimbolo *aux = ini->topo;
    TabelaSimbolo *aux1;
    printf("\n\n----------------------------- DESIMPILHANDO");
    PrintTabela();
    while(strcmp(aux->lexema,"#")){
        aux1 = aux->prox;
        free(aux);
        aux = aux1;
    }

    if(!strcmp(aux->lexema,"#")){
        ini->topo= aux->prox;
        free(aux);
    }
    else{
        ini->topo = aux;
    }
    printf("\n\n\n----------------------------- NOVA TABELA");
    PrintTabela();
}

int procuraDuplicado(char lexemaAtual[], int nivel){
    TabelaSimbolo *aux = ini->topo;
    PrintTabela();
	do{

		if(!strcmp(lexemaAtual, aux->lexema)){

            if(aux->nivel == nivel) return 1;
            return 2;
		}
		aux = aux->prox;

	}while(aux != NULL);


	return 0;
}

int procuraDuplicado1(char lexemaAtual[], int nivel){
    TabelaSimbolo *aux = ini->topo;
    PrintTabela();
	do{

		if(!strcmp(lexemaAtual, aux->lexema)){
            return 1;
		}
		aux = aux->prox;

	}while(aux != NULL);


	return 0;
}

int pesquisaTabela(char lexemaAtual[]){
  TabelaSimbolo *aux = ini->topo;
  PrintTabela();
  do{
        if(!strcmp(lexemaAtual, aux->lexema)){
            if(!strcmp(aux->tipo,"funcao inteiro") || !strcmp(aux->tipo,"funcao booleano") || !strcmp(aux->tipo, "procedimento")) return 1; //Checar retorno
            return 2;
        }
        aux = aux->prox;
    }while(aux != NULL);

  return 0;
}

bool colocaTipo(char lexema[], int nivel, char tipo[]){
	TabelaSimbolo *aux = ini->topo;
	printf("----------------------------------------------------------------%s", lexema);
	while(aux->nivel != nivel){
		aux = aux->prox;
		if(aux == NULL){
			return false;
		}
	}
	//Achou o nivel
	while(aux->nivel == nivel){
        printf("\n      ||||| %s ||||||||", aux->lexema);
        printf("\n      ||||| %s ||||||||", aux->tipo);
        printf("\n      ||||| %s ||||||||\n", lexema);


		if(!strcmp("variavel", aux->tipo)){
				strcpy(aux->tipo, tipo);
		}else{
		    if(!strcmp("funcao", aux->tipo)){
				strcpy(aux->tipo, tipo);
            }
		}


		if(aux->prox == NULL){
			return false;
		}
		//Não é vazio
		aux = aux->prox;

	}
}

int is_variavel(char lexemaAtual[]){
  TabelaSimbolo *aux = ini->topo;
  PrintTabela();
  do{
        if(!strcmp(lexemaAtual, aux->lexema)){
            if(!strcmp(aux->tipo,"inteiro") || !strcmp(aux->tipo,"booleano")) return 1;
            return 0;
        }
        aux = aux->prox;
    }while(aux != NULL);

}

int pesquisaVariavel(char lexemaAtual[]){
  TabelaSimbolo *aux = ini->topo;
  PrintTabela();
  do{
        if(!strcmp(lexemaAtual, aux->lexema)){
            if(!strcmp(aux->tipo,"inteiro") || !strcmp(aux->tipo,"booleano")) return aux->mem;
        }
        aux = aux->prox;
    }while(aux != NULL);

  return 0;
}

int pesquisaFuncProc(char lexemaAtual[]){
  TabelaSimbolo *aux = ini->topo;
  PrintTabela();
  do{
        if(!strcmp(lexemaAtual, aux->lexema)){
            if(!strcmp(aux->tipo,"funcao inteiro") || !strcmp(aux->tipo,"funcao booleano") || !strcmp(aux->tipo, "procedimento"))return aux->mem;
        }
        aux = aux->prox;
    }while(aux != NULL);

  return 0;
}

int analisa_variaveis() {
    FILE *errorlog;
    printf("\n -------- analisa_variaveis\n");
    int Alloc = 0;
    do {
        if(!strcmp(token.simbolo, "sidentificador")){
            if(procuraDuplicado(token.lexema,nivel) != 1)
            {
                printf("\n\nalo");
                insereTabela(token.lexema,"variavel",nivel,Memoria+Alloc);
                pegartoken();
                if(!strcmp(token.simbolo, "svirgula") || !strcmp(token.simbolo, "sdoispontos")){
                    Alloc++;
                    if(!strcmp(token.simbolo, "svirgula")){
                        pegartoken();
                        if(!strcmp(token.simbolo, "sdoispontos")){
                            errorlog = fopen("errorlog.txt","w");
                            fprintf(errorlog, "Linha (%d) Erro X : Espera-se identificador.", Linha);
                            fclose(errorlog); exit(0);
                        }
                    }
                } else {
                    errorlog = fopen("errorlog.txt","w");
                    fprintf(errorlog, "Linha (%d) Erro 7 : Espera-se dois pontos.", Linha);
                    fclose(errorlog); exit(0);
                }
            }
            else{
                errorlog = fopen("errorlog.txt","w");
                fprintf(errorlog, "Linha (%d) Erro 100: Encontrou-se duplicidade de %s.", Linha, token.lexema);
                fclose(errorlog); exit(0);
            }
        } else {
            errorlog = fopen("errorlog.txt","w");
            fprintf(errorlog, "Linha (%d) Erro 8 : Espera-se um identificador.", Linha);
            fclose(errorlog); exit(0);
        }
    } while(strcmp(token.simbolo, "sdoispontos"));
    pegartoken();
    Gera(-1, "ALLOC", Memoria, Alloc);
    Memoria = Memoria + Alloc;
    analisa_Tipo();
    return Alloc;
}

void analisa_Tipo()
{
    FILE *errorlog;
    printf("\n -------- analisa_Tipo\n");
    if(!strcmp(token.simbolo, "sinteiro") || !strcmp(token.simbolo, "sbooleano"))
    {
        if(!strcmp(token.simbolo, "sinteiro")) {colocaTipo(token.lexema, nivel, "inteiro");}
        else{
            colocaTipo(token.lexema, nivel, "booleano");
        }
        pegartoken();
    }
    else{
        errorlog = fopen("errorlog.txt","w");
        fprintf(errorlog, "Linha (%d) Erro 9 : Espera-se um inteiro ou booleano.", Linha);
        fclose(errorlog); exit(0);
    }
}

void analisa_comandos()
{
    FILE *errorlog;
    printf("\n -------- analisa_comandos\n");
    if(!strcmp(token.simbolo , "sinicio"))
    {
            pegartoken();
            analisa_comando_simples();
            while(strcmp(token.simbolo , "sfim"))
            {
                if(!strcmp(token.simbolo , "sponto_virgula"))
                {
                    pegartoken();
                    if(strcmp(token.simbolo , "sfim"))
                       analisa_comando_simples();
                }
                else{
                    errorlog = fopen("errorlog.txt","w");
                    fprintf(errorlog, "Linha (%d) Erro 10 : Espera-se um ponto e virgula.", Linha);
                    fclose(errorlog); exit(0);
                }
            }
            pegartoken();
    }
    else{
        errorlog = fopen("errorlog.txt","w");
        fprintf(errorlog, "Linha (%d) Erro 11 : Espera-se inicio.", Linha);
        fclose(errorlog); exit(0);
    }
}

void analisa_comando_simples (){
    printf("\n -------- analisa_comando_simples\n");
    if(!strcmp(token.simbolo,"sidentificador")){
        analisa_atrib_chprocedimento();
    }
    else{
        if(!strcmp(token.simbolo,"sse")){
            analisa_se();
        }
        else{
            if(!strcmp(token.simbolo,"senquanto")){
                analisa_enquanto();
            }
            else{
                if(!strcmp(token.simbolo,"sleia")){
                    analisa_leia();
                }
                else{
                    if(!strcmp(token.simbolo,"sescreva")){
                        analisa_escreva();
                    }
                    else{
                        analisa_comandos();
                    }
                }
            }
        }
    }
}


void analisa_atrib_chprocedimento (){
    char lexema[30]= {'\0'};
    int mem;
    strcpy(lexema,token.lexema);
    printf("\n -------- analisa_atrib_chprocedimento\n");
    mem = pesquisaVariavel(token.lexema);
    pegartoken();
    if(!strcmp(token.simbolo,"satribuicao")){
        analisa_atribuicao();
        Gera(-1, "STR", mem, -1);
    }
    else{
        chamada_procedimento(lexema);
    }
}

void chamada_procedimento(char token[]){
    int mem;
    printf("\n chamada_procedimento\n");
    mem = pesquisaFuncProc(token);
    Gera(-1,"CALL",mem,-1);
}

void analisa_atribuicao()
{
    FILE *errorlog;
    printf("\n -------- analisa_atribuicao\n");
    pegartoken();
    if(!strcmp(token.simbolo, "sidentificador") || !strcmp(token.simbolo, "snumero") || !strcmp(token.simbolo, "sabre_parenteses") ||!strcmp(token.simbolo, "sfecha_parenteses"))
    {
           analisa_expressao();
    }else{
        errorlog = fopen("errorlog.txt","w");
        fprintf(errorlog, "Linha (%d) Erro 12 : Espera-se identificador.", Linha);
        fclose(errorlog); exit(0);
    }

}

void analisa_leia (){
    FILE *errorlog;
    int mem;
    printf("\n -------- analisa_leia\n");
    pegartoken();
    Gera(-1,"RD",-1,-1);
    if(!strcmp(token.simbolo,"sabre_parenteses")){
        pegartoken();

        if(!strcmp(token.simbolo,"sidentificador")){
                if(procuraDuplicado(token.lexema,nivel))
                {
                    mem = pesquisaVariavel(token.lexema);
                    Gera(-1,"STR",mem,-1);
                        pegartoken();
                        if(!strcmp(token.simbolo,"sfecha_parenteses")){
                            pegartoken();
                        }
                        else{
                            errorlog = fopen("errorlog.txt","w");
                            fprintf(errorlog, "Linha (%d) Erro 13 : Espera-se um fecha parenteses.", Linha);
                            fclose(errorlog); exit(0);
                        }
                }
                else{
                    errorlog = fopen("errorlog.txt","w");
                    fprintf(errorlog, "Linha (%d) Erro 101: Nao encontrou declaracao de %s.", Linha, token.lexema);
                    fclose(errorlog); exit(0);
                }
        }
        else{
            errorlog = fopen("errorlog.txt","w");
            fprintf(errorlog, "Linha (%d) Erro 14 : Espera-se identificador.", Linha);
            fclose(errorlog); exit(0);
        }

    }
    else{
        errorlog = fopen("errorlog.txt","w");
        fprintf(errorlog, "Linha (%d) Erro 15 : Espera-se um abre parenteses.", Linha);
        fclose(errorlog); exit(0);
    }
}

void analisa_escreva ()
{
    FILE *errorlog;
    int mem;
    printf("\n -------- analisa_escreva\n");
    pegartoken();

    if(!strcmp(token.simbolo , "sabre_parenteses"))
    {
        pegartoken();
        if(!strcmp(token.simbolo , "sidentificador"))
        {
            if(procuraDuplicado(token.lexema,nivel))
                {
                    mem = pesquisaVariavel(token.lexema);
                    Gera(-1,"LDV",mem,-1);
                    pegartoken();
                    if(!strcmp(token.simbolo , "sfecha_parenteses"))
                    {
                        pegartoken();
                    }
                    else{
                        errorlog = fopen("errorlog.txt","w");
                        fprintf(errorlog, "Linha (%d) Erro 16 : Espera-se um fecha parenteses.", Linha);
                        fclose(errorlog); exit(0);
                    }
                }
                else{
                    errorlog = fopen("errorlog.txt","w");
                    fprintf(errorlog, "Linha (%d) Erro 102: Nao encontrou declaracao de %s.", Linha, token.lexema);
                    fclose(errorlog); exit(0);
                }

        }
        else{
            errorlog = fopen("errorlog.txt","w");
            fprintf(errorlog, "Linha (%d) Erro 17 : Espera-se identificador.", Linha);
            fclose(errorlog); exit(0);
        }
    }
    else{
        errorlog = fopen("errorlog.txt","w");
        fprintf(errorlog, "Linha (%d) Erro 18 : Espera-se um abre parenteses.", Linha);
        fclose(errorlog); exit(0);
    }
    Gera(-1,"PRN",-1,-1);
}

void analisa_enquanto()
{
    FILE *errorlog;
  int auxrot1,auxrot2;
  auxrot1 = rotulo;
  Gera(rotulo, NULL,-1,-1);
  rotulo++;
    printf("\n -------- analisa_enquanto\n");
    pegartoken();
    analisa_expressao();
    if(!strcmp(token.simbolo , "sfaca"))
    {
      auxrot2 = rotulo;
      Gera(-1, "JMPF" ,rotulo,-1);
      rotulo++;
        pegartoken();
        analisa_comando_simples();
      Gera(-1, "JMP" ,auxrot1,-1);
      Gera(auxrot2, NULL ,-1,-1);
    }
    else{
        errorlog = fopen("errorlog.txt","w");
        fprintf(errorlog, "Linha (%d) Erro 19 : Espera-se faca.", Linha);
        fclose(errorlog); exit(0);
    }
}

void analisa_se(){
    FILE *errorlog;
    printf("\n -------- analisa_se\n");
    int auxrot1,auxrot2;
    pegartoken();
    analisa_expressao();
    Gera(-1, "JMPF",rotulo,-1);
    auxrot1 = rotulo;
    rotulo++;
    if(!strcmp(token.simbolo, "sentao")){
        pegartoken();
        analisa_comando_simples();
        if(!strcmp(token.simbolo, "ssenao")){
            Gera(-1, "JMP",rotulo,-1);
            auxrot2 = rotulo;
            rotulo++;
            pegartoken();
            Gera(auxrot1,NULL,-1,-1);
            analisa_comando_simples();
            Gera(auxrot2,NULL,-1,-1);
        }else{
            Gera(auxrot1,NULL,-1,-1);
        }

    } else {
        errorlog = fopen("errorlog.txt","w");
        fprintf(errorlog, "Linha (%d) Erro 20 : Espera-se entao.", Linha);
        fclose(errorlog); exit(0);
    }
}

void analisa_subrotinas (){
    FILE *errorlog;
  int auxrot, flag;
  flag = 0;
  if(!strcmp(token.simbolo,"sprocedimento") || !strcmp(token.simbolo,"sfuncao")){
    auxrot= rotulo;
    Gera(-1, "JMP" ,rotulo,-1);
    rotulo++;
    flag = 1;
  }
    printf("\n -------- analisa_subrotinas\n");
    while(!strcmp(token.simbolo,"sprocedimento") || !strcmp(token.simbolo,"sfuncao")){
        if(!strcmp(token.simbolo,"sprocedimento")){
            analisa_declaracao_procedimento();
        }
        else{
            analisa_declaracao_funcao();
        }
        if(!strcmp(token.simbolo,"sponto_virgula")){
            pegartoken();
        }
        else{
            errorlog = fopen("errorlog.txt","w");
            fprintf(errorlog, "Linha (%d) Erro 21 : Espera-se um ponto e virgula.", Linha);
            fclose(errorlog); exit(0);
        }
    }
    if(flag == 1){
      Gera(auxrot,NULL,-1, -1);
    }
}

void analisa_declaracao_procedimento()
{
    FILE *errorlog;
    printf("\n -------- analisa_declaracao_procedimento\n");
    pegartoken();
    if(!strcmp(token.simbolo , "sidentificador"))
    {
        if(!procuraDuplicado1(token.lexema,nivel))
            {
                insereTabela(token.lexema,"procedimento",nivel,rotulo);
                Gera(rotulo, NULL,-1,-1);
                insereTabela("#","marca",nivel,rotulo);
                rotulo++;
                nivel += 1;
                pegartoken();
                if(!strcmp(token.simbolo , "sponto_virgula"))
                {
                    analisa_bloco();
                }
                else{
                    errorlog = fopen("errorlog.txt","w");
                    fprintf(errorlog, "Linha (%d) Erro 22 : Espera-se um ponto e virgula.", Linha);
                    fclose(errorlog); exit(0);
                }
            }
            else{
                errorlog = fopen("errorlog.txt","w");
                fprintf(errorlog, "Linha (%d) Erro 103: Encontrou duplicidade de %s.", Linha, token.lexema);
                fclose(errorlog); exit(0);
            }
    }
    else{
        errorlog = fopen("errorlog.txt","w");
        fprintf(errorlog, "Linha (%d) Erro 23 : Espera-se identificador.", Linha);
        fclose(errorlog); exit(0);
    }
    Desempilha();
    Gera(-1,"RETURN",-1,-1);
}

void analisa_declaracao_funcao(){
    FILE *errorlog;
    printf("\n -------- analisa_declaracao_funcao\n");
    pegartoken();
    if(!strcmp(token.simbolo, "sidentificador")){
            if(!procuraDuplicado1(token.lexema, nivel))
            {
                    insereTabela(token.lexema,"funcao",nivel,rotulo);
                    Gera(rotulo, NULL,-1,-1);
                    insereTabela("#","marca",nivel,rotulo);
                    rotulo++;
                    pegartoken();
                    if(!strcmp(token.simbolo, "sdoispontos")){
                      pegartoken();
                        if(!strcmp(token.simbolo, "sinteiro") || !strcmp(token.simbolo, "sbooleano")){
                            if(!strcmp(token.simbolo, "sinteiro")){
                                colocaTipo(token.lexema, nivel, "funcao inteiro");
                            }
                            else{
                                colocaTipo(token.lexema, nivel, "funcao booleano");
                            }
                            nivel +=1;
                            pegartoken();
                            if(!strcmp(token.simbolo, "sponto_virgula")){
                                analisa_bloco();
                            }
                        } else {
                            errorlog = fopen("errorlog.txt","w");
                            fprintf(errorlog, "Linha (%d) Erro 24 : Espera-se um inteiro ou booleano.", Linha);
                            fclose(errorlog); exit(0);
                        }
                    } else{
                        errorlog = fopen("errorlog.txt","w");
                        fprintf(errorlog, "Linha (%d) Erro 25 : Espera-se dois pontos.", Linha);
                        fclose(errorlog); exit(0);
                    }
            }
            else{
                errorlog = fopen("errorlog.txt","w");
                fprintf(errorlog, "Linha (%d) Erro 104 :Encontrou duplicidade de %s.", Linha,token.lexema);
                fclose(errorlog); exit(0);
            }
    } else {
        errorlog = fopen("errorlog.txt","w");
        fprintf(errorlog, "Linha (%d) Erro 26 : Espera-se identificador.", Linha);
        fclose(errorlog); exit(0);
    }
    Desempilha();
    Gera(-1,"RETURN",-1,-1);
}


void analisa_expressao(){
    int i=0;
    printf("\n -------- analisa_expressao\n");
    analisa_expressao_simples();
    infixToPostfix();
    //checkPostfixTypes();
    GerarCodigoPostFix();
    if(!strcmp(token.simbolo,"smaior") || !strcmp(token.simbolo,"smenor") || !strcmp(token.simbolo,"sig") || !strcmp(token.simbolo,"smaiorig") || !strcmp(token.simbolo,"smenorig") || !strcmp(token.simbolo,"sdif")){
        InsereInfix();
        pegartoken();
        analisa_expressao_simples();
        infixToPostfix();
        GerarCodigoPostFix();
    }
}


void analisa_expressao_simples()
{
    printf("\n -------- analisa_expressao_simples\n");

    if(!strcmp(token.simbolo , "smais") || !strcmp(token.simbolo , "smenos"))
    {
        InsereInfix();
        pegartoken();
    }
    analisa_termo();
    while(!strcmp(token.simbolo , "smais") || !strcmp(token.simbolo , "smenos") || !strcmp(token.simbolo , "sou"))
    {
        InsereInfix();
        pegartoken();
        analisa_termo();
    }
}



void analisa_termo (){
    printf("\n -------- analisa_termo\n");
    analisa_fator();
    while( !strcmp(token.simbolo,"smult") || !strcmp(token.simbolo,"sdiv") || !strcmp(token.simbolo,"se")){
        InsereInfix();
        pegartoken();
        analisa_fator();
    }
}


void analisa_fator (){
    FILE *errorlog;
    int aux;
    printf("\n -------- analisa_fator\n");
    if(!strcmp(token.simbolo,"sidentificador")){
      aux = pesquisaTabela(token.lexema);
        if(aux > 0) {
            if(aux == 1){InsereInfix();pegartoken();}
              else{InsereInfix();pegartoken();}
        }
        else {
            errorlog = fopen("errorlog.txt","w");
            fprintf(errorlog, "Linha (%d) Erro 105: Não encontrou declaracao de %s.", Linha, token.lexema);
            fclose(errorlog); exit(0);}
    }
    else{
        if(!strcmp(token.simbolo,"snumero")){
            InsereInfix();
            pegartoken();
        }
        else{
            if(!strcmp(token.simbolo,"snao")){
                InsereInfix();
                pegartoken();
                analisa_fator();
            }
            else{
                if(!strcmp(token.simbolo,"sabre_parenteses")){
                    InsereInfix();
                    pegartoken();
                    analisa_expressao();
                    if(!strcmp(token.simbolo,"sfecha_parenteses")){
                        InsereInfix();
                        pegartoken();
                    }
                    else{
                        errorlog = fopen("errorlog.txt","w");
                        fprintf(errorlog, "Linha (%d) Erro 27 : Espera-se um fecha parenteses.", Linha);
                        fclose(errorlog); exit(0);
                    }
                }
                else{
                    if(!strcmp(token.simbolo,"sverdadeiro") || !strcmp(token.simbolo,"sfalso")){
                        InsereInfix();
                        pegartoken();
                    }
                    else{
                        errorlog = fopen("errorlog.txt","w");
                        fprintf(errorlog, "Linha (%d) Erro 28 : Espera-se boolean.", Linha);
                        fclose(errorlog); exit(0);
                    }
                }
            }
        }
    }
}

void analisa_chamada_funcao(char lexema[])
{
    int mem;
    printf("\n -------- analisa_chamada_funcao1\n");
    mem = pesquisaFuncProc(lexema);

    Gera(-1,"CALL",mem,-1);
    Gera(-1,"LDV",0,-1);
}
