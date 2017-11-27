#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct tabela dado; //struct que guarda a palavra e o ponteiro para o proximo
struct tabela{
	char palavra[50];
	int ascii;
	dado *prox;
};

typedef struct vetor hash; //struct que guarda ponteiro pra hash, numero de posicoes e total de palavras adicionadas
struct vetor{
	dado **lista;
	int tamanho, totalDado;
};


hash *initiHash(hash *dicionario);
hash *fatorCarga(hash *dicionario);
hash *rehash(hash *dicionario);
int buscaPalavra(hash *dicionario, char consulta[], int valorAscii);
void adicionaPalavra(hash *dicionario, char novaPalavra[], int valorAscii);
void deletaPalavra(hash *dicionario, char consulta[], int valorAscii);
void reconhecimento(hash *dicionario,char entrada[],int valorAscii);
int letraMais(hash *dicionario, char consulta[], int valorAscii);
int letraMenos(hash *dicionario, char consulta[], int valorAscii);
int letrasVizinhas(hash *dicionario, char consulta[], int valorAscii);
int umaLetraErrada(hash *dicionario, char consulta[], int valorAscii);
hash* destroiHash(hash *dicionario);

int main(void){
	char entrada[50], anterior[50];
	hash *dicionario = NULL;
	int flag = 0; 
	int valorAscii = -1; //soma do valor ascii dos caracteres da palavra
	dicionario = initiHash(dicionario);

	while(entrada[0] != '*'){
		scanf("%s\n", entrada);	

		if((entrada[0] > 96 && entrada[0] < 123) || (entrada[0] > 64 && entrada[0] < 91)) { //se entrada eh palavra			
			valorAscii = 0;
			for(int i=0; entrada[i]!= '\0'; ++i){ //transformando a entrada em minuscula
				if(entrada[i] > 64 && entrada[i] < 91) //se o caractere atual for maiusculo
					entrada[i] = entrada[i] + 32;

				valorAscii+= entrada[i]; //valorAscii contem a soma dos ascii dos caracteres da palavra
			}

			if(buscaPalavra(dicionario, entrada, valorAscii)){  //se a palavra existe
				printf("ok %s\n",entrada);
			}
			else{ //se ela nao esta correta
				reconhecimento(dicionario, entrada, valorAscii);
			}
			
			strcpy(anterior, entrada); //guarda a string para fazer a operacao escolhida sobre ela nos proximo ciclos
		}					
		else if((entrada[0] == '+' || entrada[0] == '-' || entrada[0] == '*')) { //caso a entrada seja uma operacao			
			if(entrada[0] == '+'){ //ADICIONA PALAVRA
				flag = buscaPalavra(dicionario, anterior, valorAscii);//verifica se a palavra existe
				if(flag == 0){
					adicionaPalavra(dicionario, anterior, valorAscii);
					printf("ok %s\n", anterior);
					dicionario = fatorCarga(dicionario);
				} else {
					printf("fail %s\n", anterior);
				}
			}
			else if(entrada[0]== '-'){ //REMOVE PALAVRA
				flag = buscaPalavra(dicionario, anterior, valorAscii);//verifica se a palavra existe
				if(flag == 1){ //se a palavra existir
					deletaPalavra(dicionario, anterior, valorAscii);
					printf("ok %s\n", anterior);
				} else {
					printf("fail %s\n", anterior);
				}
			}
			else if(entrada[0] == '*'){
				printf("SAIU!");
				break;
			}
		}
				
	}
	dicionario = destroiHash(dicionario);

	return 0;
}

hash *initiHash(hash *dicionario){

	dicionario = malloc(sizeof(hash));
	dicionario->lista = calloc(sizeof(dado*), 50);
	dicionario->tamanho = 50;
	dicionario->totalDado = 0;

	return dicionario;
}

hash *fatorCarga(hash *dicionario){
	float fc = (float)dicionario->totalDado/(float)dicionario->tamanho;
	if(fc < 0.70) //70% de fator de carga
		return dicionario;
	dicionario = rehash(dicionario);
	return dicionario;

}

hash *rehash(hash *dicionario){
	hash *novoDicionario;

	novoDicionario = malloc(sizeof(hash));
	novoDicionario->lista = calloc(sizeof(dado*), dicionario->tamanho*2);
	novoDicionario->tamanho = dicionario->tamanho*2; //dobro do tamanho
	dicionario->totalDado = 0;

	for (int i = 0; i < dicionario->tamanho; ++i){ //percorre as posicoes da hash
		while(dicionario->lista[i] != NULL){	//passa os dados da antiga para a nova
				//adiciona na 'nova' hash
				adicionaPalavra(novoDicionario, dicionario->lista[i]->palavra, dicionario->lista[i]->ascii);
				//deleta da antiga
				deletaPalavra(dicionario,dicionario->lista[i]->palavra, dicionario->lista[i]->ascii);
		}
	}
	free(dicionario);
	
	return novoDicionario;
}

int buscaPalavra(hash *dicionario, char consulta[], int valorAscii){
	int posicao = valorAscii % dicionario->tamanho;
	if (valorAscii < 0) {
		return 1;
	}
	if(dicionario->lista[posicao] == NULL){
		return 0;
	}
	else{
		dado *aux = dicionario->lista[posicao];
		do{
			//printf("atual: %s\n", aux->palavra);
			if(!strcmp(consulta, aux->palavra)){
				return 1;
			}
			aux = aux->prox;
		}while(aux != NULL);
	}
	return 0;
}

void adicionaPalavra(hash *dicionario, char novaPalavra[], int valorAscii){
	int posicao = valorAscii % dicionario->tamanho;
	dado *new = NULL;
	
	dicionario->totalDado++;
	new = malloc(sizeof(dado));
	new->ascii = valorAscii;
	strcpy(new->palavra, novaPalavra);

	if(dicionario->lista[posicao] == NULL){
		new->prox = NULL;
		dicionario->lista[posicao] = new;
	}
	else{
		dado *aux = dicionario->lista[posicao];
		dicionario->lista[posicao] = new;
		dicionario->lista[posicao]->prox = aux;
	}
	return;
}

void deletaPalavra(hash *dicionario, char consulta[], int valorAscii){
	int posicao = valorAscii % dicionario->tamanho;
	dado *aux = dicionario->lista[posicao];
	dado *ant = aux;
	do{	
		if(strcmp(consulta, aux->palavra)== 0){ //se eh a palavra desejada
			dicionario->totalDado--;
			if(aux == dicionario->lista[posicao]){ //caso seja o primeiro elemento da lista
				dicionario->lista[posicao] = aux->prox;
				free(aux);
				return;
			}
			else if(aux->prox != NULL){ //se eh um elemento do meio
				ant->prox = aux->prox;
				free(aux);
				return;
			}
			else{
				ant->prox = NULL;
				free(aux);
				return;
			}
		}
		ant = aux;
		aux = aux->prox;
	}while(aux != NULL);
}

void reconhecimento(hash *dicionario, char entrada[],int valorAscii){
	/*printf("rec = %d, %d, %d, %d\n", letraMais(dicionario,entrada, valorAscii), letraMenos(dicionario,entrada, valorAscii),
	letrasVizinhas(dicionario,entrada, valorAscii), umaLetraErrada(dicionario,entrada, valorAscii));*/

	if(letraMais(dicionario,entrada, valorAscii) || letraMenos(dicionario,entrada, valorAscii) || 
	letrasVizinhas(dicionario,entrada, valorAscii) || umaLetraErrada(dicionario,entrada, valorAscii)){ //caso algum algoritmo encontre uma palavra parecida
		printf("\n");
	}
	else{ //nenhuma palavra similar encontrada
		printf("not found\n");
	}

	return;
}

int letraMais(hash *dicionario, char consulta[], int valorAscii){
	char aux[50];
	int k = 0; //posicao da letra que sera 'comida'
	int j = 0; //descola na string auxiliar
	int flag = 0;

	do{
		valorAscii -= consulta[k];
		
		j = 0;
		for (int i = 0; consulta[i] != '\0'; ++i){
			if(i != k){
				aux[j] = consulta[i];
				j++;
			}
		}
		aux[j] = '\0';
		if(buscaPalavra(dicionario, aux, valorAscii)){
			printf("%s ",aux );
			flag = 1;
		}
		valorAscii+= consulta[k];
		k++;
	} while (consulta[k]!='\0');
	return flag;
}

int letraMenos(hash *dicionario, char consulta[], int valorAscii){
	char troca;
	char aux[50] = "a";
	int i;
	int j;
	int flag = 0;

	strcat(aux, consulta); //concatena as strings "adicionando" uma letra

	for (int i = 0; i < aux[i] != '\0'; ++i){//percorre a palavra
		for (j = 97; j < 123; ++j){ //percorre o alfabeto
			aux[i] = j;
			valorAscii+=j;
			if(buscaPalavra(dicionario, aux, valorAscii)){
				printf("%s ", aux);
				flag = 1;
			}
			valorAscii-=j;
		}
		troca = aux[i];
		aux[i] = aux[i+1];
		aux[i+1] = troca;
	}
	
	return flag;
}

int letrasVizinhas(hash *dicionario, char consulta[], int valorAscii){
	char aux;
	int flag = 0;

	for (int i = 0; consulta[i] != '\0'; ++i){
		aux = consulta[i];
		consulta[i] = consulta[i+1];
		consulta[i+1] = aux;
		if(buscaPalavra(dicionario, consulta, valorAscii)){
			printf("%s ", consulta);
			flag = 1;
		}
		consulta[i+1] = consulta[i];
		consulta[i] =aux;
	}
	
	return flag;
}

int umaLetraErrada(hash *dicionario, char consulta[], int valorAscii){
	char aux;
	int i;
	int j;
	int flag = 0;
	for (i= 0; consulta[i] != '\0'; ++i){ //percorre a palavra
		aux = consulta[i]; //guardando a letra alterada
		valorAscii-= consulta[i];
		for (j = 97; j < 123; ++j){ //percorre o alfabeto
			consulta[i] = j;
			valorAscii+=j;
			
			if(buscaPalavra(dicionario, consulta, valorAscii)){
				printf("%s ", consulta);
				flag = 1;
			}
			valorAscii-=j;
		}
		valorAscii+= aux;
		consulta[i] = aux;
	}
	return flag;
}

hash* destroiHash(hash *dicionario){
	
	for (int i = 0; i < dicionario->tamanho; ++i){ //percorre as posicoes da hash
		while(dicionario->lista[i] != NULL){	//deleta todos os dados
				deletaPalavra(dicionario,dicionario->lista[i]->palavra, dicionario->lista[i]->ascii);
		}
	}
	free(dicionario);
	dicionario = NULL;
	return dicionario;

}