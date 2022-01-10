#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>


//Escopo global
int N;
int blocks = 0;
int *vetor;
pthread_mutex_t mutex;
pthread_cond_t cond;

void mostrar(int *vetor){
    printf("vetor inicial [");
    for(int i = 0;i < N;i++){
        printf(" %d ", vetor[i]);
    }
    printf("]");
}

void barreira( int aux){
    pthread_mutex_lock(&mutex); // Ínicio da seção crítica 
    if (blocks == (aux-1)) { 
      //ultima thread a chegar na barreira
      pthread_cond_broadcast(&cond);
      blocks=0;
    } else {
      blocks++;
      pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex); // Fim da seção crítica 
}

void *tarefa(void *arg){
    int id = *(int*)arg;
    int somatorio = 0;
    int count = 0;
    srand(time(NULL));
    printf("\nA Thread %d está executando... ", id);
    for(int i = 0; i< N ; i++){
        if(count > 0){
            vetor[id] = rand() % 10;
            barreira(N);
            printf("\nA posição %d do vetor recebe %d", (id + 1), vetor[id]);
        }
        for(int i = 0; i< N; i++){
            somatorio = somatorio + vetor[i];
            barreira(N);
        }
        printf("\nO somatorio da Thread %d eh %d", id, somatorio);
        somatorio = 0;
        count++;
    }
    pthread_exit(NULL);
}

//Função principal
int main(int argc, char *argv[]){

//recebe e valida os parametros de entrada (dimensao do vetor, numero de threads)
if(argc < 2){
    fprintf(stderr," Digite %s < valor de Threads >\n", argv[0]);
    return 1;
}

N = atoi(argv[1]);
pthread_t threads[N];
int id[N];

//Inicialização
pthread_mutex_init(&mutex, NULL);
pthread_cond_init(&cond, NULL);

vetor = (int*) malloc(sizeof(int) * N); 
if(vetor == NULL){fprintf(stderr, "ERRO -- malloc");}

srand(time(NULL)); //objetiva inicializar o gerador de números aleatórios com o valor da função time(NULL) 
for(int i = 0; i < N; i++){
    vetor[i] = rand() % 10;
}
mostrar(vetor);

//Criação das threads 
for( int i = 0; i < N; i++){
    id[i] = i;
    pthread_create(&threads[i], NULL, tarefa,(void*)&id[i]);
}

//esperar todas as threads completarem
for(int i = 0; i < N; i++){
    pthread_join(threads[i], NULL);
}

//Desalocação das variáveis e termina
pthread_mutex_destroy(&mutex);
pthread_cond_destroy(&cond);
return 0;
}