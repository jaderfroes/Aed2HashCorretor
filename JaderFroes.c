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
typedef struct palavraCorreta sugestao;
struct palavraCorreta{//struct que tera as referencias das sugestoes das palavras incorretas
	char *palavra;
	sugestao *prox;
};


hash *initiHash(hash *dicionario); //inicializa hash
hash *fatorCarga(hash *dicionario); //verifica fator de carga
hash *rehash(hash *dicionario); //faz rehash
int buscaPalavra(hash *dicionario, char consulta[], int valorAscii); //busca a palavra e retorna V ou F
dado *buscaEndPalavra(hash *dicionario, char consulta[], int valorAscii); //busca a palavra e retorna o endereco do nodo
void adicionaPalavra(hash *dicionario, char novaPalavra[], int valorAscii); //adiciona 
void deletaPalavra(hash *dicionario, char consulta[], int valorAscii); //delete
void reconhecimento(hash *dicionario,char entrada[],int valorAscii); //chama os algoritmos de reconhecimento de palavras incorretas
sugestao *letraMais(hash *dicionario, char consulta[], int valorAscii, sugestao *sugestoes);
sugestao * letraMenos(hash *dicionario, char consulta[], int valorAscii, sugestao *sugestoes);
sugestao * letrasVizinhas(hash *dicionario, char consulta[], int valorAscii, sugestao *sugestoes);
sugestao * umaLetraErrada(hash *dicionario, char consulta[], int valorAscii, sugestao *sugestoes);
sugestao *adicionaSugestoes(dado *nodoHash, sugestao* sugestoes); //cria uma lista de sugestoes em ordem alfabetica
sugestao *printaListaEDeleta(sugestao *sugestoes); //imprime a lista anteriormente criada e deleta os elementos
hash *destroiHash(hash *dicionario);

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

dado *buscaEndPalavra(hash *dicionario, char consulta[], int valorAscii){
	int posicao = valorAscii % dicionario->tamanho;
	if (valorAscii < 0)
		return NULL;

	if(dicionario->lista[posicao] == NULL){
		return NULL;
	}
	else{
		dado *aux = dicionario->lista[posicao];
		do{
			//printf("eh o atual: %s\n", aux->palavra);
			if(!strcmp(consulta, aux->palavra)){
				return aux;
			}
			aux = aux->prox;
		}while(aux != NULL);
	}
	return NULL;
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
	sugestao *sugestoes=NULL; //Lista de ponteiros que apontarao para as palavras corretas sugeridas

	sugestoes = letraMais(dicionario,entrada, valorAscii, sugestoes); 
	sugestoes = letraMenos(dicionario,entrada, valorAscii, sugestoes);
	sugestoes = letrasVizinhas(dicionario,entrada, valorAscii, sugestoes);
	sugestoes = umaLetraErrada(dicionario,entrada, valorAscii, sugestoes); //caso algum algoritmo encontre uma palavra parecida

	if (sugestoes != NULL){
		// printf(" Antes do printa lista\n");
		sugestoes = printaListaEDeleta(sugestoes);
		printf("\n");
		// printf(" Após o printa lista\n");

	}
	else{ //nenhuma palavra similar encontrada
		printf("not found\n");
	}
	return;
}

sugestao *letraMais(hash *dicionario, char consulta[], int valorAscii, sugestao *sugestoes){
	char aux[50];
	int k = 0; //posicao da letra que sera 'comida'
	int j = 0; //descola na string auxiliar

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
			sugestoes = adicionaSugestoes(buscaEndPalavra(dicionario, aux, valorAscii), sugestoes);
		}
		valorAscii+= consulta[k];
		k++;
	} while (consulta[k]!='\0');
	return sugestoes;
}

sugestao *letraMenos(hash *dicionario, char consulta[], int valorAscii, sugestao *sugestoes){
	char troca;
	char aux[102] = "a";
	int j;
	strcat(aux, consulta); //concatena as strings "adicionando" uma letraMenos
	int i = 0;
	do {
		for (j = 97; j < 123; ++j){ //percorre o alfabeto
			aux[i] = j;
			valorAscii+=j;
			if(buscaPalavra(dicionario, aux, valorAscii)){
				sugestoes = adicionaSugestoes(buscaEndPalavra(dicionario, aux, valorAscii), sugestoes);
			}
			valorAscii-=j;
		}
		troca = aux[i];
		aux[i] = aux[i+1];
		aux[i+1] = troca;
		i++;
	} while (aux[i-1] != '\0');
	
	return sugestoes;
}

sugestao *letrasVizinhas(hash *dicionario, char consulta[], int valorAscii, sugestao *sugestoes){
	char aux;

	for (int i = 0; consulta[i] != '\0'; ++i){
		aux = consulta[i];
		consulta[i] = consulta[i+1];
		consulta[i+1] = aux;
		if(buscaPalavra(dicionario, consulta, valorAscii)){
			sugestoes = adicionaSugestoes(buscaEndPalavra(dicionario, consulta, valorAscii), sugestoes);
		}
		consulta[i+1] = consulta[i];
		consulta[i] =aux;
	}
	
	return sugestoes;
}

sugestao *umaLetraErrada(hash *dicionario, char consulta[], int valorAscii, sugestao *sugestoes){
	char aux;
	int i;
	int j;
	for (i= 0; consulta[i] != '\0'; ++i){ //percorre a palavra
		aux = consulta[i]; //guardando a letra alterada
		valorAscii-= consulta[i];
		for (j = 97; j < 123; ++j){ //percorre o alfabeto
			consulta[i] = j;
			valorAscii+=j;
			
			if(buscaPalavra(dicionario, consulta, valorAscii)){
				sugestoes = adicionaSugestoes(buscaEndPalavra(dicionario, consulta, valorAscii), sugestoes);
			}
			valorAscii-=j;
		}
		valorAscii+= aux;
		consulta[i] = aux;
	}
	return sugestoes;
}

sugestao* adicionaSugestoes(dado *nodoHash, sugestao* sugestoes){
	sugestao *nova;
	nova = malloc(sizeof(sugestao));
	nova->palavra = nodoHash->palavra;
	if (sugestoes == NULL){
		nova->prox = NULL;
		sugestoes = nova;
		return sugestoes;
	}

	sugestao *aux = sugestoes;
	do{
		if(!strcmp(nodoHash->palavra, aux->palavra) ){
			return sugestoes;
		}
			for(int i = 0; nodoHash->palavra[i] != '\0' ; ++i){
				if(nodoHash->palavra[i] < aux->palavra[i]){
					nova->prox = sugestoes;
					sugestoes = nova;
					return sugestoes;
				}
				if(nodoHash->palavra[i] > aux->palavra[i]){
					aux->prox = nova;
					nova->prox = NULL;
					return sugestoes;
				}
			}
			aux = aux->prox;

	}while(aux != NULL);

	return sugestoes;
}

sugestao *printaListaEDeleta(sugestao *sugestoes){
	sugestao *aux = sugestoes;
	// printf("to AQUI \n");
	
	do{
		printf("%s ", (aux->palavra));
		sugestoes = aux->prox;
		free(aux);
		aux = sugestoes;
	} while (aux!= NULL);

	return sugestoes;

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