/* arquivoLISCH.dat ->  para os arquivos gerados com LISCH
 * arquivoEISCH.dat ->  quando usa o EISCH
 * 
 * Aluno: Marivaldo Carneiro Mascarenhas Junior
 * 
 * */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX 11
#define NOME_ARQUIVO "arquivoLISCH.dat"
#define NOME_ARQUIVO_E "arquivoEISCH.dat"

struct noAluno{
	int chave;
	char nome[20];
	int idade;
	int apontador;
};

int hash(int chave){
	return chave%MAX;
}
 
void inicializar (FILE *f){
	struct noAluno no;
	int i,r;
		
	r=10;	
	
	//o arquivo terá um cabeçalho com a localização do r
	fwrite(&r,sizeof(int),1,f);	
		
	strcpy(no.nome, "NULL");
	no.chave=-1;
	no.apontador = -1;
			
	for(i=0;i<MAX;i++)
		fwrite(&no,sizeof(struct noAluno),1,f);
	fclose(f);
}

struct noAluno buscaChave(FILE *f, int chave){
	int r, cont;
	struct noAluno no;
	
	rewind(f);
	
	fread(&r,sizeof(int),1,f);
	
	fseek(f, hash(chave)*sizeof(struct noAluno),SEEK_CUR);
	fread(&no,sizeof(struct noAluno),1,f);
	
	if(no.chave == chave){
		fseek(f, -1*sizeof(struct noAluno),SEEK_CUR);
		return no;
	}
	
	cont = 0;
	while((no.chave != chave)&&(cont<MAX)&&(no.apontador!=-1)){
		cont++;
		rewind(f);
		fread(&r, sizeof(int),1,f);
		fseek(f, (no.apontador)*sizeof(struct noAluno),SEEK_CUR);
		fread(&no, sizeof(struct noAluno),1,f);
		fseek(f, -1*sizeof(struct noAluno),SEEK_CUR);
	}
		
	if(no.chave == chave){
		return no;
	}
		
	else{
		no.chave = -1;
	    return no;
	}
	

}

void mostrarConteudo(FILE *f){
	int i,r;
	struct noAluno no;

	rewind(f);
	
	fread(&r,sizeof(int),1,f); 
	
	for(i=0;i<11;i++){
		fread(&no,sizeof(struct noAluno),1,f);
		if(no.chave == -1){
			printf("%d: vazio\n",i);
		}
		
		else{
			if(no.apontador == -1){
				printf("%d: %d %s %d nulo\n",i, no.chave, no.nome, no.idade);
				//close(f);
			}
			else{
				printf("%d: %d %s %d %d\n",i, no.chave, no.nome, no.idade, no.apontador);
				//fclose(f);
			}	
		}
		
	}
	
	fclose(f);
}

void mudaPonteiro(FILE *f){
	int r;
	struct noAluno no;
	
	fseek(f, -1*sizeof(struct noAluno),SEEK_END);
	r = MAX - 1;
	fread(&no,sizeof(struct noAluno),1, f);
	
	//volta para o 10
	if(no.chave == -1){
		rewind(f);
		fwrite(&r,sizeof(int),1,f);
	}
	else{
		fseek(f, -1*sizeof(struct noAluno),SEEK_END);
		r = MAX - 1;
	
		while((no.chave != -1) && (r>0)){
			r--;
			fseek(f, -1*sizeof(struct noAluno),SEEK_CUR);
			fread(&no,sizeof(struct noAluno),1, f);
			fseek(f, -1*sizeof(struct noAluno),SEEK_CUR);
		}
	
		fread(&no,sizeof(struct noAluno),1, f);
	
		rewind(f);
		fwrite(&r,sizeof(int),1,f);
	}
}

void inserirLisch (int chave, char nome[20], int idade, FILE *f){
	
	struct noAluno no;
	int r;
	
	no = buscaChave(f,chave);
	
	if(no.chave != -1){
		printf("chave ja existente: %d\n", chave);
		return;
	}
	 
	
	rewind(f);	
	fread(&r,sizeof(int),1,f);
	fseek(f, hash(chave)*sizeof(struct noAluno),SEEK_CUR);
	fread(&no,sizeof(struct noAluno),1, f); 
	
	//se a inserção for na mesma casa que r aponta tem que mudar o valor do apontador
	if(hash(chave) == r){
		no.chave = chave;
		strcpy(no.nome, nome);
		no.idade = idade;
		no.apontador = -1;
		fseek(f, -1*sizeof(struct noAluno),SEEK_CUR);
		fwrite(&no, sizeof(struct noAluno), 1, f);
		mudaPonteiro(f);
		fclose(f);
		return;
	}
	
	if(no.chave != -1){
		//se o campo que aponta não tiver nada
		if(no.apontador == -1){
			no.apontador = r;
			
			//reinsere o nó com o ponteiro
			fseek(f, -1*sizeof(struct noAluno),SEEK_CUR);
			fwrite(&no,sizeof(struct noAluno),1, f);
			
			no.chave = chave;
			strcpy(no.nome, nome);
			no.idade = idade;
			no.apontador = -1;
			
			rewind(f);
			fread(&r,sizeof(int),1,f);
			fseek(f, r*sizeof(struct noAluno),SEEK_CUR);
			
			fwrite(&no,sizeof(struct noAluno),1, f);
			
			mudaPonteiro(f);	
			fclose(f);
				
		}
		//se já estiver apontando para algum lugar
		else{
			rewind(f);
			fread(&r,sizeof(int), 1,f);
			
			fseek(f, (no.apontador)*sizeof(struct noAluno),SEEK_CUR);
			fread(&no,sizeof(struct noAluno),1,f);
			
			while(no.apontador != -1){
				rewind(f);
				fread(&r,sizeof(int), 1,f);
				fseek(f, (no.apontador)*sizeof(struct noAluno),SEEK_CUR);
				fread(&no,sizeof(struct noAluno),1,f);
			}
			
			//reinsere o nó com o apontador
			fseek(f, -1*sizeof(struct noAluno),SEEK_CUR);
			no.apontador = r;
			fwrite(&no,sizeof(struct noAluno), 1, f);
			
			fseek(f, -1*sizeof(struct noAluno),SEEK_CUR);
			strcpy(no.nome, nome);
			no.chave = chave;
			no.apontador = -1;
			no.idade = idade;
			rewind(f);
			
			fread(&r,sizeof(int), 1,f);
			fseek(f, r*sizeof(struct noAluno),SEEK_CUR);
			fwrite(&no, sizeof(struct noAluno),1,f);
			
			mudaPonteiro(f);
			
			fclose(f);
		}
		
		return;
	}
	
	rewind(f);
	fread(&r,sizeof(int),1,f);
	
	fseek(f, hash(chave)*sizeof(struct noAluno),SEEK_CUR);
	
	no.chave = chave;
	no.idade = idade;
	strcpy(no.nome, nome);
	
	fwrite(&no,sizeof(struct noAluno),1, f);
	
	fclose(f);
}

void inserirEisch (int chave, char nome[20], int idade, FILE *f){
struct noAluno no;
	int r, apontadorAux;
	
	no = buscaChave(f,chave);
	
	if(no.chave != -1){
		printf("chave ja existente: %d\n", chave);
		return;
	}
	 	
	rewind(f);	
	
	fread(&r,sizeof(int),1,f);
	
	fseek(f, hash(chave)*sizeof(struct noAluno),SEEK_CUR);
	
	fread(&no,sizeof(struct noAluno),1, f); 
	
	//se a inserção for na mesma casa que r aponta tem que mudar o valor do apontador
	if(hash(chave) == r){
		no.chave = chave;
		strcpy(no.nome, nome);
		no.idade = idade;
		no.apontador = -1;
		fseek(f, -1*sizeof(struct noAluno),SEEK_CUR);
		fwrite(&no, sizeof(struct noAluno), 1, f);
		mudaPonteiro(f);
		fclose(f);
		return;
	}
	
	if(no.chave != -1){
		//se o campo que aponta não tiver nada
		if(no.apontador == -1){
			no.apontador = r;
			
			//reinsere o nó com o ponteiro
			fseek(f, -1*sizeof(struct noAluno),SEEK_CUR);
			fwrite(&no,sizeof(struct noAluno),1, f);
			
			no.chave = chave;
			no.idade = idade;
			strcpy(no.nome, nome);
			no.apontador = -1;
			
			rewind(f);
			fread(&r,sizeof(int),1,f);
			fseek(f, r*sizeof(struct noAluno),SEEK_CUR);
			
			fwrite(&no,sizeof(struct noAluno),1, f);
			
			mudaPonteiro(f);	
			fclose(f);
				
		}
		//se já estiver apontando para algum lugar
		else{
			apontadorAux = no.apontador;
			
			//reinsere o nó com o apontador
			fseek(f, -1*sizeof(struct noAluno),SEEK_CUR);
			no.apontador = r;
			fwrite(&no,sizeof(struct noAluno), 1, f);
			
			rewind(f);
			fread(&r,sizeof(int), 1,f);
			
			//insere o novo no na posição do R
			fseek(f, r*sizeof(struct noAluno),SEEK_CUR);
			strcpy(no.nome, nome);
			no.chave = chave;
			no.apontador = apontadorAux;
			no.idade = idade;
			fwrite(&no, sizeof(struct noAluno),1,f);			
			
			mudaPonteiro(f);
			
			fclose(f);
		}
		
		return;
	}
	
	rewind(f);
	fread(&r,sizeof(int),1,f);
	
	fseek(f, hash(chave)*sizeof(struct noAluno),SEEK_CUR);
	
	no.chave = chave;
	strcpy(no.nome, nome);
	no.idade = idade;
	
	fwrite(&no,sizeof(struct noAluno),1, f);
	
	fclose(f);
}
//encontra o local onde o ponteiro deve ser colocado, 
//retorna a posição onde o nó que aponta para onde o nó em questão está
int encontraAntecessor(FILE *f, int chave){
	int r, cont;
	struct noAluno no, noAnt, noAntAnt;
	rewind(f);
	
	fread(&r,sizeof(int),1,f);
	
	fseek(f, hash(chave)*sizeof(struct noAluno),SEEK_CUR);
	fread(&no,sizeof(struct noAluno),1,f);
	
	noAnt = no;
	
	if(no.chave == chave){
		return hash(no.chave);
	}
	
	noAntAnt.chave = hash(chave);
	
	cont = 0;
	while((no.chave != chave)&&(cont<MAX)&&(no.apontador!=-1)){
		if(cont > 0){
			noAntAnt = noAnt;
		}
		noAnt = no;
		rewind(f);
		fread(&r, sizeof(int),1,f);
		fseek(f, (no.apontador)*sizeof(struct noAluno),SEEK_CUR);
		fread(&no, sizeof(struct noAluno),1,f);
		fseek(f, -1*sizeof(struct noAluno),SEEK_CUR);
		cont++;
	}
	if(cont == 1){
		noAntAnt.apontador = hash(chave);
	}
		
	return noAntAnt.apontador;
	
}

//retorna a posição onde a chave removida está
int posicaoRemovida(FILE *f, int chave){
	int r, cont, pos;
	struct noAluno no;
	
	rewind(f);
	
	fread(&r,sizeof(int),1,f);
	
	fseek(f, hash(chave)*sizeof(struct noAluno),SEEK_CUR);
	fread(&no,sizeof(struct noAluno),1,f);
	
	//se for a primeira retorna o hash(chave)
	if(no.chave == chave){
		fseek(f, -1*sizeof(struct noAluno),SEEK_CUR);
		return hash(chave);
	}
	
	cont = 0;
	while((no.chave != chave)&&(cont<MAX)&&(no.apontador!=-1)){
		pos = no.apontador;
		cont++;
		rewind(f);
		fread(&r, sizeof(int),1,f);
		fseek(f, (no.apontador)*sizeof(struct noAluno),SEEK_CUR);
		fread(&no, sizeof(struct noAluno),1,f);
		fseek(f, -1*sizeof(struct noAluno),SEEK_CUR);
	}
	return pos;

}

void remover(FILE *f, int chave){
	struct noAluno no, no2, noProx, noAnt, noAux;
	int posChave, posAnt, pontAux;
	
	no = buscaChave(f,chave);
	if(no.chave == -1){
		printf("chave nao encontrada: %d\n", chave);
		return;
	}
	
	noAux =no;
	
	fseek(f,sizeof(int),SEEK_SET);
	fseek(f, hash(chave)*sizeof(struct noAluno),SEEK_CUR);
	fread(&no,sizeof(struct noAluno),1,f);
	
	posAnt = encontraAntecessor(f, chave);
	posChave = posicaoRemovida(f, chave);
	
	
	
	//é o primeiro na cadeia e nao aponta para ninguem apenas remove  <<<<<<<<verificar se alguem aponta para ele
	if( (hash(no.chave) == posChave) && (no.apontador == -1) ){
		fseek(f,sizeof(int),SEEK_SET);
		fseek(f, posChave*sizeof(struct noAluno),SEEK_CUR);
		no2.chave = -1;
		strcpy(no.nome, "NULL");
		no2.apontador = -1;
		fwrite(&no2, sizeof(struct noAluno),1,f);
		mudaPonteiro(f);
		return;		
	}
	
	//se for o último da cadeia
	if(noAux.apontador == -1){
		posAnt = encontraAntecessor(f, noAux.chave);
		posChave = posicaoRemovida(f, noAux.chave);
		
		fseek(f, sizeof(int),SEEK_SET);
		fseek(f, (posAnt)*sizeof(struct noAluno),SEEK_CUR);
		fread(&no2, sizeof(struct noAluno), 1, f);
		no2.apontador = -1;
		fseek(f, (-1)*sizeof(struct noAluno),SEEK_CUR);
		fwrite(&no2, sizeof(struct noAluno),1,f);
		
		fseek(f, sizeof(int),SEEK_SET);
		fseek(f, (posChave)*sizeof(struct noAluno),SEEK_CUR);
		no2.chave = -1;
		no2.apontador = -1;
		fwrite(&no2, sizeof(struct noAluno),1,f);
		mudaPonteiro(f);
		return;
	}
	
	noAnt = no;
	//no que vai iterar no while
	noProx.chave = -1;
	noProx.apontador = no.apontador;

	while( /*(hash(noProx.chave) != posChave) && */(noProx.apontador != -1)){
		fseek(f,sizeof(int),SEEK_SET);
		fseek(f,(noProx.apontador)*sizeof(struct noAluno),SEEK_CUR);
		fread(&noProx,sizeof(struct noAluno),1,f);
		fseek(f,-1*sizeof(struct noAluno),SEEK_CUR);
				
		//se achar, troca e muda o posChave
		if(hash(noProx.chave) == posChave){
			//é o ultimo que tem hash igual
			if(noProx.apontador == -1){
				no2.apontador = -1;
				no2.chave = -1;
				fwrite(&no2, sizeof(struct noAluno), 1, f);
				
				
				posAnt = encontraAntecessor(f, noProx.chave);
				fseek(f, sizeof(int), SEEK_SET);
				fseek(f,posAnt*sizeof(struct noAluno),SEEK_CUR);
				fread(&no2, sizeof(struct noAluno), 1, f);
				fseek(f,-1*sizeof(struct noAluno),SEEK_CUR);
				no2.apontador = -1;
				fwrite(&no2, sizeof(struct noAluno), 1, f);
				
				posChave = posicaoRemovida(f, no.chave);
				
				
				noProx.apontador = no.apontador;
				fseek(f, sizeof(int), SEEK_SET);
				fseek(f,posChave*sizeof(struct noAluno),SEEK_CUR);
				fwrite(&noProx, sizeof(struct noAluno), 1, f);
				mudaPonteiro(f);
				return;
			}
			
			else{
				posAnt = encontraAntecessor(f, noProx.chave);
				posChave = posicaoRemovida(f, noProx.chave);
					
				no2 = noProx;
				no2.chave = -1;
				fwrite(&no2, sizeof(struct noAluno), 1, f);
		
				no2.chave = noProx.chave;	
				strcpy(no2.nome, noProx.nome);
				no2.idade = noProx.idade;
				no2.apontador = posChave;		
				
				fseek(f,sizeof(int),SEEK_SET);
				fseek(f, (posAnt)*sizeof(struct noAluno),SEEK_CUR);		
				fwrite(&no2, sizeof(struct noAluno), 1, f);
	
				noAnt = noProx;
				no = noProx;
			}
		}		
	}
	
	
	
	//saiu do while e nao achou mais nenhum, tem que mudar os ponteiros
	fseek(f,sizeof(int),SEEK_SET);
	fseek(f, (posChave)*sizeof(struct noAluno),SEEK_CUR);		
	fread(&no2, sizeof(struct noAluno), 1, f);
	pontAux = no2.apontador;
	
	fseek(f,sizeof(int),SEEK_SET);
	fseek(f, (posAnt)*sizeof(struct noAluno),SEEK_CUR);		
	fread(&no2, sizeof(struct noAluno), 1, f);
	no2.apontador = pontAux;
	fseek(f, -1*sizeof(struct noAluno),SEEK_CUR);
	fwrite(&no2, sizeof(struct noAluno), 1, f);
	
	if((hash(noProx.chave) != posChave)){
		fseek(f,sizeof(int),SEEK_SET);
		fseek(f, (posChave)*sizeof(struct noAluno),SEEK_CUR);
		no2.chave = -1;
		no2.apontador = -1;
		fwrite(&no2, sizeof(struct noAluno), 1, f);	
	}
	
	mudaPonteiro(f);
}

void removerAntigo(FILE *f, int chave){
	struct noAluno no, no2, noCabeca, noAnt, noRemovido, noAnterior, noAntAnterior;
	int cont, posAnt, anterior, antecessor;
	
	no = buscaChave(f,chave);
	if(no.chave == -1){
		printf("chave nao encontrada: %d\n", chave);
		return;
	}
	//guarda os dados do nó que será removido caso precise do apontador
	noRemovido = no;
	
	fseek(f,sizeof(int),SEEK_SET);
	
	fseek(f, hash(chave)*sizeof(struct noAluno),SEEK_CUR);
	fread(&no,sizeof(struct noAluno),1,f);
	
	//guarda quem é o primeiro da cadeia
	noCabeca = no;
	noAnt = no;
	//é o primeiro na cadeia e nao aponta para ninguem apenas remove  <<<<<<<<verificar se alguem aponta para ele
	if( (no.chave == chave) && (no.apontador == -1) ){
		fseek(f, -1*sizeof(struct noAluno),SEEK_CUR);
		no2.chave = -1;
		strcpy(no.nome, "NULL");
		no2.apontador = -1;
		fwrite(&no2, sizeof(struct noAluno),1,f);
		mudaPonteiro(f);
		return;		
	}
	
	cont = 0;

	//procura a chave
	while(no.chave != chave){
		//guarda a posição do no anterior para poder recuperar o poteiro depois
		noAnterior = no;

		rewind(f);
		fseek(f, sizeof(int),SEEK_CUR);
		fseek(f, (no.apontador)*sizeof(struct noAluno),SEEK_CUR);
		fread(&no, sizeof(struct noAluno),1,f);
		//noCabeça =  noAnterior
		if(no.chave != chave){
			noCabeca = no;
		}
		fseek(f, -1*sizeof(struct noAluno),SEEK_CUR);
		cont++;
	}
	
	antecessor = encontraAntecessor(f, no.chave);
	//se for o primeiro tem que comparar com hash(chave)
	if(cont == 0){
		noCabeca = no;
		fseek(f,sizeof(int), SEEK_SET);
		fseek(f, (no.apontador)*sizeof(struct noAluno), SEEK_CUR);
		fread(&no, sizeof(struct noAluno), 1, f);
		posAnt = hash(chave);	
	}
	else{
		posAnt = noCabeca.apontador;
	}
	
	//no = no que vai ser deletado
	//noCabeca = no que encabeça a lista
	//posAnt = posição do no removido
	
	//procura nós com o hash igual à posição do nó que será apagado
	while( (noCabeca.apontador != -1) ){
		while( (hash(no.chave) != posAnt) && (no.apontador != -1) ){
			noAnt = no;
			anterior = noAnt.apontador;
			
			rewind(f);
			fseek(f, sizeof(int), SEEK_CUR);
			fseek(f, (no.apontador)*sizeof(struct noAluno), SEEK_CUR);
			fread(&no, sizeof(struct noAluno), 1, f);
			fseek(f, -1*sizeof(struct noAluno), SEEK_CUR);		
		}
		
		//faz a troca se necessário
		if( hash(no.chave) == posAnt ){
			noRemovido = no;
			antecessor = encontraAntecessor(f, no.chave);
			
			no2.chave = no.chave;
			no2.idade = no.idade;
			strcpy(no2.nome, no.nome);
			no2.apontador = noRemovido.apontador;
			fwrite(&no2, sizeof(struct noAluno), 1, f);
			//printf("chave %d apontador%d\n", no2.chave, no2.apontador);
		}
		//nao achou hash tem que copiar o ponteiro na posição anterior
		else{		
			no2 = noCabeca;
					
			no2.apontador = noRemovido.apontador;
			rewind(f);
			fseek(f, sizeof(int), SEEK_CUR);
			fseek(f, antecessor*sizeof(struct noAluno), SEEK_CUR);
			fwrite(&no2, sizeof(struct noAluno), 1, f);
			
			//apaga
			fseek(f, sizeof(int), SEEK_SET);
			fseek(f, (posAnt)*sizeof(struct noAluno), SEEK_CUR);
			no2.apontador = -1;
			no2.chave = -1;
			fwrite(&no2, sizeof(struct noAluno), 1, f);
			
		}
		
		//anterior = noCabeca.apontador;
		noCabeca = no;
		if(no.apontador != -1){
			posAnt = noAnt.apontador;
		}	
	}
	//está no fim da cadeia
	if( (hash(no.chave) == posAnt) && no.apontador == -1){
		fseek(f, -1*sizeof(struct noAluno), SEEK_CUR);
		fread(&no2, sizeof(struct noAluno),1, f);
		fseek(f, sizeof(int), SEEK_SET);
		fseek(f, (no2.apontador)*sizeof(struct noAluno), SEEK_CUR);
		no2.chave = -1;
		no2.apontador = -1;
		fwrite(&no2, sizeof(struct noAluno), 1, f);
		
		fseek(f, sizeof(int), SEEK_SET);
		fseek(f, antecessor*sizeof(struct noAluno), SEEK_CUR);
		fwrite(&no, sizeof(struct noAluno), 1, f);
	} 
	
	mudaPonteiro(f);
}

int contaAcessos(FILE *f, int chave){
	int r, cont;
	struct noAluno no;
	
	rewind(f);
	
	fread(&r,sizeof(int),1,f);
	
	fseek(f, hash(chave)*sizeof(struct noAluno),SEEK_CUR);
	fread(&no,sizeof(struct noAluno),1,f);
	
	cont = 1;
	while((no.chave != chave)&&(cont<10)&&(no.apontador!=-1)){
		cont++;
		rewind(f);
		fread(&r, sizeof(int),1,f);
		fseek(f, (no.apontador)*sizeof(struct noAluno),SEEK_CUR);
		fread(&no, sizeof(struct noAluno),1,f);
		fseek(f, -1*sizeof(struct noAluno),SEEK_CUR);
	}
	
	rewind(f);
		
	return cont;
}

void media(FILE *f){
	int i,r, soma, nChaves;
	struct noAluno no;
	float media;
	
	soma = 0;
	nChaves = 0;
	
	fread(&r,sizeof(int), 1, f);
	
	for(i = 0; i<11;i++){
		fread(&no, sizeof(struct noAluno), 1, f);
		
		if(no.chave != -1){
			soma = soma + contaAcessos(f, no.chave);
			nChaves ++;
			fread(&r,sizeof(int), 1, f);
			fseek(f, (i+1)*sizeof(struct noAluno), SEEK_CUR);
		}	
	}
	
	media = (float)soma/nChaves;
	
	printf("%.1f\n",media);
}

int main(){
	FILE *file;
	FILE *fileE;
	
	struct noAluno noAux;
	
	char op,op2;
	int  chave,idade;
	char nome[20];
	
	op = 'z';	
	
	scanf(" %c",&op2);
	if(op2 == 'l'){
		
		file = fopen(NOME_ARQUIVO,"r");
	
		if(file==NULL){
			file=fopen(NOME_ARQUIVO,"w+");
			inicializar(file);
		}
		
		while(op!='e'){
			scanf(" %c", &op);
			switch(op){
				case 'i':
					file = fopen(NOME_ARQUIVO,"r+");
					scanf(" %d", &chave);
					scanf(" %[^\n]s", nome);
					scanf(" %d", &idade);
					inserirLisch(chave,nome,idade,file);
					break;
	
				case 'c':
					file = fopen(NOME_ARQUIVO, "r");
					scanf(" %d", &chave);
				
					noAux = buscaChave(file, chave);
				
					if(noAux.chave != -1){
						printf("chave: %d\n%s\n%d\n", noAux.chave,noAux.nome,noAux.idade);
					}
					else{
						printf("chave nao encontrada %d\n", chave);
					}
					fclose(file);
					break;
					
				case 'p':
					file = fopen(NOME_ARQUIVO,"r+");
					mostrarConteudo(file);
					break;
					
				case 'm':
					file = fopen(NOME_ARQUIVO, "r");
					media(file);
					break;
				
				case 'r':
					file = fopen(NOME_ARQUIVO, "r+");
					scanf(" %d", &chave);
					remover(file, chave);
					fclose(file);
					break;
					
			}
			}
	}

	if(op2 == 'e'){
			
		fileE = fopen(NOME_ARQUIVO_E,"r");
	
		if(fileE==NULL){
			//printf("Arquivo EISCH vai ser criado\n");
			fileE=fopen(NOME_ARQUIVO_E,"w+");
			inicializar(fileE);
		}
		
		while(op!='e'){
			scanf(" %c", &op);
			switch(op){
				case 'i':
					file = fopen(NOME_ARQUIVO_E,"r+");
					scanf(" %d", &chave);
					scanf(" %[^\n]s", nome);
					scanf(" %d", &idade);
					inserirEisch(chave,nome,idade,file);
					break;
	
				case 'c':
					file = fopen(NOME_ARQUIVO_E, "r");
					scanf(" %d", &chave);
				
					noAux = buscaChave(file, chave);
				
					if(noAux.chave != -1){
						printf("chave: %d\n%s\n%d\n", noAux.chave,noAux.nome,noAux.idade);
					}
					else{
						printf("chave nao encontrada %d\n", chave);
					}
					fclose(file);
					break;
				
				case 'p':
					file = fopen(NOME_ARQUIVO_E,"r+");
					mostrarConteudo(file);
					break;
					
				case 'm':
					file = fopen(NOME_ARQUIVO_E, "r");
					media(file);
					break;
					
				case 'r':
					file = fopen(NOME_ARQUIVO_E, "r+");
					scanf(" %d", &chave);
					remover(file, chave);
					fclose(file);
					break;
				/*	
				case 't':
					file = fopen(NOME_ARQUIVO_E, "r+");
					scanf(" %d", &chave);
					printf("%d\n",encontraAntecessor(file, chave));
					fclose(file);
					break;
				case 'f':
					file = fopen(NOME_ARQUIVO_E, "r+");
					scanf(" %d", &chave);
					printf("%d\n",posicaoRemovida(file, chave));
					fclose(file);
					break;
					* 
			*/
			}
			}
		
		
	}
	
	return 0;	
}
