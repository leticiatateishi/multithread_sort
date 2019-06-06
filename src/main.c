#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define N_WORKERS 4

pthread_mutex_t trava;
pthread_t workers[N_WORKERS];
int numeros[100001];
int auxiliar[100001];
int n = 0;

/*	Mergesort: recebe como parametros dois indices de posicoes no vetor numeros.
 *	Divide a porcao do vetor limitada pelos indices em duas partes, chama o 
 *	mergesort recursivamente nessas duas partes e as intercala.
 */
void mergesort(int inicio, int fim){
	if (inicio < fim){
		int meio = (inicio+fim)/2;
		mergesort(inicio, meio);
		mergesort(meio+1, fim);
		intercala(inicio, meio, fim);
	}
}


/*	Intercala: recebe como parametros tres indices de posicoes no vetor numeros.
 *	Primeiro, copia a primeira porcao do vetor (do inicio ao meio) no vetor 
 *	auxiliar de maneira direta e depois copia a segunda parte do vetor (do meio
 *	ao fim) de maneira inversa. Depois, utiliza os indices i e j para intercalar
 *	as duas partes do vetor auxiliar, ordenando os numeros.
 */
void intercala(int inicio, int meio, int fim){
	pthread_mutex_lock(&trava);
	for (int i = inicio; i <= meio; i++)
		auxiliar[i] = numeros[i];
	for (int i = meio+1; i <= fim; i++)
		auxiliar[fim+meio+1-i] = numeros[i];
	pthread_mutex_unlock(&trava);
	int i = inicio;
	int j = fim;
	for (int k = inicio; k <= fim; k++){
		if (auxiliar[i] <= auxiliar[j]){
			pthread_mutex_lock(&trava);
			numeros[k] = auxiliar[i++];
			pthread_mutex_unlock(&trava);
		}
		else{
			pthread_mutex_lock(&trava);
			numeros[k] = auxiliar[j--];
			pthread_mutex_unlock(&trava);
		}
	}
}


/*	Worker: distribui o quarto do vetor que sera tarefa de uma thread. Chama
 *	a funcao mergesort para esse um quarto dos numeros.
 */
void* worker(void *arg) {

	int tamanho = (int *)arg;

	pthread_mutex_lock(&trava);
	int inicio = (tamanho*n)/4;
	int fim = (((n+1)*tamanho)/4)-1;
	n++;
	pthread_mutex_unlock(&trava);

	mergesort(inicio, fim);

  	return NULL;
}


/*	Cria quatro threads, atribuindo a cada uma um quarto do vetor numeros
 *	(dentro da funcao worker). Depois, espera que todas as threads finalizem
 *	suas tarefas. Finalmente, intercala os quatro quartos de vetor e imprime
 *	o resultado ordenado.
 */
int main() {

	int i = 0;
	int numero;
	while (1){
		if (scanf("%d", &numero) == EOF) 
      		break;
      	numeros[i++] = numero;
    }  	

	for(int j = 0; j < N_WORKERS; j++) 
    	pthread_create(& (workers[j]), NULL, worker, (void*) i);
	
	for (int j = 0; j < N_WORKERS; j++) 
    	pthread_join(workers[j], NULL);

    intercala(0, i/4-1, ((i*2)/4)-1);
    intercala(((i*2)/4), (3*i)/4-1, i-1);
    intercala(0, ((i*2)/4)-1, i-1);

    for(int j = 0; j < i-1; j++){
    	printf("%d ", numeros[j]);
    }
    printf("%d\n", numeros[i-1]);

   	return 0;
}
