#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "time.h"
#define m 10
/*
O CODIGO ESTARA DIVIDIDO EM 2 TIPOS DE FUNCOES: AUXILIARES E PRINCIPAIS
	FUNCOES AUXILIARES SO SERVEM PRA SEREM CHAMADAS PELAS FUNCOES PRINCIPAIS
	FUNCOES PRINCIPAIS SAO AS QUE SAO CHAMADAS PELO USUARIO
*/
typedef struct ArvB{ //ESTRUTURA DA ARVORE
	long int chave[2*m-1]; //vetor de chaves, usei a definição do Thomas H. Cormen, em que cada pagina segura (2*m)-1 chaves
	int num_chave; //numero de chaves que estao atuamente na pagina, usei um int separado porque tava ficando muito confuso manipular os dois vetores, sendo um começando em 0 e outro em 1
	struct ArvB* filho[2*m]; //vetor de ponteiro pros filhos da pagina
	int folha; // para ser mais eficiente, e nao precisar comparando 100 filhos pra saber se é folha, eu dediquei um int pra essa função, 1 para folha e 0 para no intermediario
}ArvB;
ArvB* RemoveIntermediario(ArvB* pagina, int posicao); //so declarei a funcao, ela ta la pra baixo
ArvB* CriaPagina(int EhFolha){ //FUNCAO AUXILIAR, funcao que cria uma pagina, o parametro é um inteiro dizendo se essa pagina é folha ou não
	ArvB* pag = malloc(sizeof(ArvB)); //aloco memoria
	pag->num_chave = 0; //começa com 0 chaves sendo seguradas
	pag->folha = EhFolha;
	return pag;
}
ArvB* QuebraPagina(ArvB* pagina,int QualFilho, ArvB* filho ){ //FUNCAO AUXILIAR,essa funcao é usada para dividir uma pagina em 2, é usada na inserção
	ArvB* novo = CriaPagina(filho->folha); //primeiro se cria uma pagina nova
	novo->num_chave = m-1; //essa pagina vai segurar metade das chaves do filho a ser quebrado
	int i;
	for(i = 0;i<m-1;i++){
		novo->chave[i] = filho->chave[i+m]; //ai eu copio a segunda metade das chaves do filho original pra pagina nova
	}
	if(filho->folha == 0){ //se  esse nao for folha, quer dizer que tem filho, entao preciso copiar a segunda metade dos filhos tambem
		for(i=0;i<m;i++){
			novo->filho[i] = filho->filho[i+m];// que é feito nessa linha
		}
	}
	filho->num_chave = m-1; //atualizo o numero de chaves do filho original
	for( i = pagina->num_chave; i >= QualFilho+1; i--){ //dou shift pra frente nos filhos do pai pra poder abrir espaço pro filho novo
		pagina->filho[i+1] = pagina->filho[i];
	}
	pagina->filho[QualFilho+1] = novo; //coloco o filho novo no novo espaço
	for(i = pagina->num_chave-1;i >= QualFilho; i--){ //faço a mesma coisa com as chaves
		pagina->chave[i+1] = pagina->chave[i];
	}
	pagina->chave[QualFilho] = filho->chave[m-1]; //e boto a chave central do filho no pai
	pagina->num_chave++; // incremento o numero de chaves do pai
	return pagina;
}
ArvB* InsereSecundaria(ArvB* pagina, long int chave){ //FUNCAO AUXILIAR essa funcao vai inserir a chave na arvore e se necessario vai quebrar
	if(pagina->folha == 1){ //se é folha entao eu insiro essa chave
		int i = pagina->num_chave-1;
		while(i>=0 && pagina->chave[i] > chave){ //dou shift nas chaves  pra frente
			pagina->chave[i+1] = pagina->chave[i];
			i--;
		}
		pagina->chave[i+1] = chave; //e insiro a chave no lugar que foi aberto
		pagina->num_chave++; //incremento o numero de chaves
		return pagina;
	}
	else{ // se nao é folha, procuro o filho que essa chave deveria entrar
		int i = pagina->num_chave-1;
		while(i >= 0 && pagina->chave[i] > chave){
			i--;
		}
		if(pagina->filho[i+1]->num_chave == 2*m-1){ //se o filho está cheio, entao quebro a pagina
			pagina = QuebraPagina(pagina,i+1,pagina->filho[i+1]);
			if(pagina->chave[i+1]<chave){ //se a chave deveria estar na segunda metade do filho, entao eu procuro no filho i+1
				i++;
			}
		}
		pagina->filho[i+1] = InsereSecundaria(pagina->filho[i+1],chave); //chamo recursivamente a funcao ate inserir
		return pagina;
	}
}
ArvB* Insere(ArvB* pagina,long int chave){ //FUNCAO PRINCIPAL, essa funcao vai inserir e quebrar a raiz se necessario
	if(pagina == NULL){ // se a arvore ta vazia, eu crio uma pagina e insiro nela
		ArvB* raiz = CriaPagina(1); // indico que a nova pagina é uma folha
		raiz->chave[0] = chave; //insiro a chave
		raiz->num_chave = 1; // e incremento o numero de chaves
		return raiz;
	}
	else{
		if(pagina->num_chave == 2*m-1){ //se a raiz ta cheia entao crio uma nova raiz
			ArvB* NovaRaiz = CriaPagina(0); //indico que a nova pagina nao é folha
			NovaRaiz->filho[0] = pagina; //antiga raiz é o filho da nova raiz
			NovaRaiz = QuebraPagina(NovaRaiz,0,pagina); //quebro a antiga raiz
			int i = 0;
			if(NovaRaiz->chave[0]<chave){ //mesma coisa, se a chave deveria estar na pagina nova, entao incremento o i
				i++;
			}
			NovaRaiz->filho[i] = InsereSecundaria(NovaRaiz->filho[i],chave); //chamo recursivamente
			return NovaRaiz;
		}
		else{
			pagina = InsereSecundaria(pagina,chave); // se a arvore nao ta vazia , e a raiz nao ta cheia, simplesmente insiro normalmente
			return pagina;
		}
	}
}
ArvB* RemoveFolha(ArvB* pagina, int posicao){ //FUNCAO AUXILIAR ela remove uma chave de uma folha
	int i;
	for(i=posicao+1;i<pagina->num_chave;i++){//simplesmente dou shift pra tras em todas as chaves, assim sobreescrevendo ela e apagando ela
		pagina->chave[i-1]= pagina->chave[i];
	}
	pagina->num_chave--; // diminuo numero de chaves
	return pagina;
}
long int MaiorDosMenores(ArvB* pagina, int posicao){ //FUNCAO AUXILIAR, acha o maior dos menores de uma determinada pagina
	ArvB* caminhador = pagina->filho[posicao]; //eu caminho na arvore ate achar um no folha co
	while(caminhador->folha==0){
		caminhador = caminhador->filho[caminhador->num_chave];
	}
	int i = caminhador->num_chave;
	long int MDM = caminhador->chave[i-1]; // o MDM é a ultima chave dessa pagina
	return MDM;
}
long int MenorDosMaiores(ArvB* pagina, int posicao){ //FUNCAO AUXILIAR, mesma coisa so que eu pego o maior dos menores, faço o processo inverso
	ArvB* caminhador = pagina->filho[posicao+1];
	while(caminhador->folha==0){
		caminhador = caminhador->filho[0];
	}
	long int MDM = caminhador->chave[0]; //so que ao inves de pegar o ultimo elemento, pego o primeiro
	return MDM;
}
ArvB* PegaFilhoRicoEsquerdo(ArvB* pagina,int posicao){ //FUNCAO AUXILIAR pego um elemento de um filho à esquerda que tenha elementos para dar
	ArvB* irmaorico = pagina->filho[posicao-1];// irmao que vai dar uma chave
	ArvB* filhopobre = pagina->filho[posicao]; //filho que vai receber uma chave
	int i;
	for(i=(filhopobre->num_chave)-1;i>=0;i--){ //dou shift em todas as chaves do filho pobre
		filhopobre->chave[i+1] = filhopobre->chave[i];
	}
	if(filhopobre->folha==0){ //verifico se é folha
		for(i=filhopobre->num_chave;i>=0;i--){
			filhopobre->filho[i+1] = filhopobre->filho[i]; // se  nao é folha entao preciso dar shift nos filhos tambem
		}
	}
	filhopobre->chave[0] = pagina->chave[posicao-1]; //faço uma rotacao de chaves,pego um elemento da pagina e jogo pro filho pobre
	filhopobre->num_chave++;
	if(filhopobre->folha == 0){  //se nao for folha, o filho pobre recebe tambem um filho do irmaorico
		filhopobre->filho[0] = irmaorico->filho[irmaorico->num_chave];
	}
	pagina->chave[posicao-1] = irmaorico->chave[irmaorico->num_chave-1]; //e uma chave do irmao rico sobe pra pagina
	irmaorico->num_chave--;
	return pagina;
}
ArvB* PegaFilhoRicoDireito(ArvB* pagina,int posicao){ //FUNCAO AUXILIAR, faço a mesma coisa, so que com o filho da direita, por razões de economizar dedo não irei comentar essa parte, é a mesma coisa so que espelhado
	ArvB* irmaorico = pagina->filho[posicao+1];
	ArvB* filhopobre = pagina->filho[posicao];
	filhopobre->chave[filhopobre->num_chave] = pagina->chave[posicao];
	if(filhopobre->folha == 0){
		filhopobre->filho[(filhopobre->num_chave)+1] = irmaorico->filho[0];
	}
	pagina->chave[posicao] = irmaorico->chave[0];
	int i;
	for(i=1;i<irmaorico->num_chave;i++){
		irmaorico->chave[i-1] = irmaorico->chave[i];
	}
	if(irmaorico->folha==0){
		for(i=1;i<=irmaorico->num_chave;i++){
			irmaorico->filho[i-1]=irmaorico->filho[i];
		}
	}
	filhopobre->num_chave += 1;
	irmaorico->num_chave -= 1;
	return pagina;
}
ArvB* Concatena(ArvB* pagina, int posicao){ //FUNCAO AUXILIAR pego 2 paginas que tem o numero minimo de chaves e junto elas
	ArvB* irmao = pagina->filho[posicao+1]; //irmao que vai ser concatenada
	ArvB* filhoB = pagina->filho[posicao]; //filho que vai ser concatenado
	filhoB->chave[m-1]= pagina->chave[posicao]; //uma chave do pai desce pro filho
	int i;
	for (i=0;i<irmao->num_chave;i++){ //a segunda metade do filho passa a ser as chaves do irmao
		filhoB->chave[i+m] = irmao->chave[i];
	}
	if(filhoB->folha==0){ // se nao for folha, faço o mesmo com os filhos
		for(i=0;i<irmao->num_chave;i++){
			filhoB->filho[i+m] = irmao->filho[i];
		}
	}
	for(i=posicao+1;i<pagina->num_chave;i++){ // como uma chave desceu do pai pro filho, é preciso dar shift nas chaves
		pagina->chave[i-1]= pagina->chave[i];
	}
	for(i=posicao+2;i<=pagina->num_chave;i++){ //mesma coisa com os filhos
		pagina->filho[i-1] = pagina->filho[i];
	}
	filhoB->num_chave += (irmao->num_chave)+1; // somo o numero de chaves do filho com o irmao
	pagina->num_chave--; // diminuo numero de chaves do pai
	free(irmao); // e deleto o irmao
	return pagina;
}
ArvB* Rotacao(ArvB* pagina,int posicao){ //FUNCAO AUXILIAR nela eu uso as funções anteriores pra deletar a chave
	if(posicao!=0 && pagina->filho[posicao-1]->num_chave >= m){ //se a posicao nao é zero e o filho esquerdo é rico, entao pego um elemento dele
        pagina = PegaFilhoRicoEsquerdo(pagina,posicao);
        return pagina;
	}
	else if(posicao!=pagina->num_chave && pagina->filho[posicao+1]->num_chave >= m){ //se a posicao nao é a ultima  e o filho direito é rico, pego um dele
		pagina = PegaFilhoRicoDireito(pagina,posicao);
		return pagina;
	}
	else{ //se a posicao nao é a ultima
		if(posicao!=pagina->num_chave){ //se a posicao nao é a ultima entao mando a pagina e a posicao como argumento, significa que vai se juntar com o irmao direito
			pagina = Concatena(pagina,posicao);
		}
		else{// se a posicao for a ultima
			pagina = Concatena(pagina,posicao-1); // junto com o irmao esquerdo
		}
		return pagina;
	}
}

ArvB* RemoveGeral(ArvB* pagina, long int chave){ //FUNCAO AUXILIAR/PRINCIPAL essa funcao ela junta todas as outras, ela pode ser usada tanto sozinha quanto chamada pela RemoveRaiz
	int i=0;
	while(i<pagina->num_chave && pagina->chave[i]<chave){ //acho onde a chave deve estar
		i++;
	}
	if(i<pagina->num_chave && pagina->chave[i]==chave){ // se ela esta na pagina
		if(pagina->folha == 1){ //verifico se é folha e removo
			pagina = RemoveFolha(pagina,i);
		}
		else{ //se não chamo a que trata de remoção intermediaria
            pagina = RemoveIntermediario(pagina,i);
		}
	}
	else{
		if(pagina->folha == 1){//se nao ta na pagina, e ela é folha, entao esse numero não existe na arvore
			return pagina;
		}
		int flag = 0; //flag pra verificar se a posição que deveria estar é a ultima
		if(i == pagina->num_chave){
			flag = 1;
		}
		if(pagina->filho[i]->num_chave < m){ // se esse filho que deveria estar nao tem chave o suficiente, chamo a rotação
			pagina = Rotacao(pagina,i);
		}
		if(flag == 1 && i > pagina->num_chave){//pela rotação, o numero de chaves pode ter mudado entao eu uso a flag anteriormente
			pagina->filho[i-1] = RemoveGeral(pagina->filho[i-1],chave); //e chamo recursivamente com i-1
		}
		else{
			pagina->filho[i] = RemoveGeral(pagina->filho[i],chave);// se não eu chamo recursivamente com i;
		}
	}
	return pagina;
}
ArvB* RemoveIntermediario(ArvB* pagina, int posicao){ //FUNCAO AUXILIAR, serve pra remover uma chave de uma pagina intermediario
	int i = pagina->chave[posicao];
	if(pagina->filho[posicao]->num_chave >m-1){ //se o filho nessa posicao tem muitos elementos pra dar entao eu pego o MDM dele e boto no lugar da chave a ser removida
		long int MDM = MaiorDosMenores(pagina,posicao);
		pagina->chave[posicao] = MDM;
		pagina->filho[posicao] = RemoveGeral(pagina->filho[posicao],MDM);
	}
	else if(pagina->filho[posicao+1]->num_chave > m-1){  // se o filho na posicao +1 tem muitos elementos entao pego o MDM e ponho no lugar da chave a ser removida
		long int MDM = MenorDosMaiores(pagina,posicao);
		pagina->chave[posicao] = MDM;
		pagina->filho[posicao+1] = RemoveGeral(pagina->filho[posicao+1],MDM);
	}
	else{ // se ambos os filhos tem m-1 chaves entao eu junto eles
		pagina = Concatena(pagina,posicao);
		pagina->filho[posicao] = RemoveGeral(pagina->filho[posicao],i);
	}
	return pagina;
}
ArvB* RemoveRaiz(ArvB* raiz, long int chave){ //FUNCAO PRINCIPAL, junta todas as funções anteriores, e pode remover a raiz
	if(raiz == NULL){ // se a raiz é nula, esse elemento nao existe
		printf("arvore vazia\n");
		return raiz;
	}
	raiz = RemoveGeral(raiz,chave); //se não, remove com as outras funções
	if(raiz->num_chave == 0){ //se a raiz nao tem nenhum elemento  depois da remoção
		ArvB* pagina = raiz;
		if(raiz->folha == 1){ //e se ela é folha, entao nao existe mais arvore
			raiz = NULL;
		}
		else{ // se nao, o primeiro filho passa a ser a nova raiz
			raiz = raiz->filho[0];
		}
		free(pagina); // e a raiz antiga é deletada
	}
	return raiz;
}
void PrintaTudo(ArvB* pagina){ //FUNCAO PRINCIPAL, printa todos os elementos da arvore, não irei comentar pois eu so usei pra poder testar e tirar erros
	int i;
	if(pagina!=NULL){
        for(i=0;i<pagina->num_chave;i++){
            if(pagina->folha==0){
                PrintaTudo(pagina->filho[i]);
            }
            printf("%ld\n",pagina->chave[i]);
        }
        if(pagina->folha==0){
            PrintaTudo(pagina->filho[i]);
        }
    }
}

ArvB* VerificaInsere(ArvB* Antigo,ArvB* pagina,long int chave){ //FUNCAO PRINCIPAL essa funcao verifica se existe algum elemento mais velho que 12 meses  e insere em outra arvore, ele compara com a nova chave a ser inserida
	int p=0; // contador
	if(pagina!=NULL){ //  se a arvore é nula entao com certeza nao existe e retornamos a arvore
	for(p=0;p<pagina->num_chave;p++){ //percorro todos os elementos da pagina
		if(pagina->folha==0){ // se tem filho, entao faço o mesmo com os filhos da pagina
			Antigo = VerificaInsere(Antigo,pagina->filho[p],chave);
		}
		if((pagina->chave[p] - pagina->chave[p]%10000000) <= (chave - chave%10000000)){ // o que isso faz é, se o dia e mes da pagina forem menores ou iguais ao dia e mes da chave a ser inserida
			if(pagina->chave[p]%100 < chave%100){  // se forem menores ou iguais entao verifico se é do ano passado
				Antigo = Insere(Antigo,pagina->chave[p]); // se for, a arvore dos antigos tem um elemento a mais
			}
		}
	}
	if(pagina->folha==0){ // faço a mesma coisa com os filhos
		Antigo = VerificaInsere(Antigo,pagina->filho[p],chave);
		}
	}
	return Antigo; // e retorno a pagina antiga com os novos elementos
}
ArvB* VerificaRemove(ArvB* raiz,ArvB* pagina,long int chave){ //FUNCAO AUXILIAR, essa funcao remove todas as entradas com mais de um ano em relacao a chave
	int p=0;// mesma coisa que a funcao verificainsere
	if(pagina!=NULL){
        while(p<pagina->num_chave){
            if(pagina->folha==0){
            raiz = VerificaRemove(raiz,pagina->filho[p],chave);
            }
            if((pagina->chave[p] - pagina->chave[p]%10000000) <= (chave - chave%10000000)){
                if(pagina->chave[p]%100 < chave%100){
                    raiz = RemoveRaiz(raiz,pagina->chave[p]);// a diferença ta aqui, eu removo o elemento da pagina
                    p--; // como eu dei shift pra tras em todos os elementos, tenho que refazer o processo
                }
            }
            p++;
        }
            if(pagina->folha==0){
                raiz = VerificaRemove(raiz,pagina->filho[p],chave);
        }
	}
	return raiz; // e retorno a pagina com os elementos removidos
}
int Busca01(ArvB* pagina,long int chave){ //FUNCAO AUXILIAR, essa funcao verifica se a data da chave ja existe, se sim retorna 1, se não retorna 0, serve pra facilitar na hora de inserir
	int i = 0;
	int j = 0;
	if(pagina!=NULL){ //se a pagina nao é nula, talvez exista a data la, entao eu a procuro
        for(i=0;i<pagina->num_chave;i++){ //busco esse elemento na arvore
            if(pagina->folha==0){
                j+= Busca01(pagina->filho[i],chave); //busco recursivamente
            }
            if(pagina->chave[i] - pagina->chave[i]%10000000 == chave - chave%10000000){ // se eu achei a data
                if(pagina->chave[i]%100 == chave%100){ // com o mesmo ano
                    return 1;// retorno 1

                }
            }
    	}
    	if(pagina->folha==0){
            j+= Busca01(pagina->filho[i],chave); //busco recursivamente
        }
	}
	// se a pagina é nula, com certeza nao existe e retorno j com 0
		return j;

}
int BuscaElemento(ArvB* pagina,long int chave,int ano ){ // FUNCAO PRINCIPAL, eu busco uma data em um determinado ano e retorno o valor que eu recebi ou paguei nesse dia
	int i = 0;
	int caminhador = 0; // esse é o valor que a funcao retorna
	if(pagina!=NULL){ //se a pagina é nula nem faço nada e retorno 0
        for(i=0;i<pagina->num_chave;i++){ //busco esse elemento na arvore
            if(pagina->folha==0){
                caminhador+= BuscaElemento(pagina->filho[i],chave,ano); //busco recursivamente
            }
            if(pagina->chave[i] - pagina->chave[i]%10000000 == chave*10000000){ //se ele tem a mesma data
                if(pagina->chave[i]%100 == ano){ // e o mesmo ano
                    return (pagina->chave[i]%10000000 - pagina->chave[i]%100)/100; // eu retorno o valor que recebi ou paguei
                }
            }
        }
        if(pagina->folha==0){
            caminhador+= BuscaElemento(pagina->filho[i],chave,ano);
        }
	}
	return caminhador;//e retorno o  caminhador com o valor
}
ArvB* Incrementa(ArvB* pagina,long int chave){ //FUNCAO AUXILIAR, se uma data existe na arvore, entao eu so preciso incrementar o valor que recebi ou paguei naquela data, isso que essa funcao faz
	int i = 0;
	while(i<pagina->num_chave && chave - chave%10000000 > pagina->chave[i] - pagina->chave[i]%10000000){//busco onde o elemento deveria estar
		i++;
	}
	if(pagina->chave[i] - pagina->chave[i]%10000000 == chave - chave%10000000){//se tem a mesma data
        if(pagina->chave[i]%100 == chave%100){//e mesmo ano
            pagina->chave[i] += (chave%10000000 - chave%100); //eu somo o valor que recebi ou paguei com o da chave
            return pagina; //e retorno a pagina com o valor incrementado
        }
	}
	else if (pagina->folha != 1){ //se nao ta na pagina e essa pagina tem filho, procuro no filho
		pagina->filho[i] = Incrementa(pagina->filho[i],chave);
	}
	else{//se não, retorno a pagina
		return pagina;
	}
}
ArvB* InsereVista(ArvB* atual,long int chave){ //FUNCAO PRINCIPAL, funcao que insere um valor a vista na arvore

    int flag = 0; // flag 0 ou 1
    atual = VerificaRemove(atual,atual,chave);//vai verificar se tem algum valor antigo a ser removido
    flag = Busca01(atual,chave); //vai fazer uma busca com 0 ou 1 sendo retornado
    if(flag == 0){ // se retornou 0 entao nao existe na arvore
        atual = Insere(atual,chave); //e insere aqui
    }
    else{ // se retornou 1 entao existe
        atual = Incrementa(atual,chave);// entao so incremento o valor
    }

    return atual; // e retorno a arvore atualizada
}
long int VerificaParcela(ArvB* antiga,long int chave, int parcela){ //FUNCAO PRINCIPAL, essa funcao pega uma chave e uma parcela, e retorna a data da ultima parcela
    long int Parcelado = ((chave%10000000 - chave%100)/100)/parcela; //eu pego o valor de cada parcela
	int ano = chave%100; //pego o ano
	long int chaveParcelada = (chave - chave%10000000) + (Parcelado*100) + ano; //e transformo em uma chave parcelada
	int i ; //contador
	int mes = (chave%1000000000 - chave%10000000)/10000000; //pego o mes dessa chave
	for(i=1;i<parcela;i++){
		if(i+mes > 12){ //se a parcela for estourar os 12 meses
			chaveParcelada = chaveParcelada - 110000000; // a chave parcelada volta pro mes 1
			chaveParcelada++; //mas com um ano a mais
			mes = 1; // e o mes volta pra 1
		}
		else{
            chaveParcelada = chaveParcelada+10000000; //eu somo um mes na chaveparcelada

		}
	}
	return chaveParcelada; //e retorno a chave correspondente a ultima parcela, eu so usso essa funcao pra facilitar na hora de verificar parcelas
}
ArvB* InsereParcelado(ArvB* atual,long int chave, int parcela){ //FUNCAO PRINCIPAL, mesma coisa que a anterior, apenas que eu insiro na atual
	long int Parcelado = ((chave%10000000 - chave%100)/100)/parcela;// pego uma parcela
	int ano = chave%100; //pego o ano
	long int chaveParcelada = (chave - chave%10000000) + (Parcelado*100) + ano; //e crio uma chave que indica uma parcela
	int i ;
	int mes = (chave%1000000000 - chave%10000000)/10000000; //pego o mes
	atual = InsereVista(atual,chaveParcelada); //insiro uma parcela
	for(i=1;i<parcela;i++){
		if(i+mes > 12){ //se o mes + contador for maior que 12 preciso resetar o mes e incrementar o ano
			chaveParcelada = chaveParcelada - 110000000; // que é feito aqui
			chaveParcelada++; //aqui
			mes = 1; //e aqui
			atual = InsereVista(atual,chaveParcelada ); // e insiro essa nova chave resetada
		}
		else{ //se nao eu simplesmente insiro ela com um mes a mais
            chaveParcelada = chaveParcelada+10000000;
			atual = InsereVista(atual,chaveParcelada); //que é feito aqui
		}
	}
	return atual; //e retorno a arvore com esses valores novos
}
int BuscaMes(ArvB* pagina,int mes,int ano){ //FUNCAO PRINCIPAL, essa funcao busca todos os valores de um determinado mes na arvore
	long int chave = mes + 100; //pego esse mes e transformo em uma data que a buscaelemento pode ler, exemplo: mes 1 vira 101
	int i;
	int soma = 0; //é a soma de todos os valores do mes
	for(i=0;i<30;i++){
		soma+= BuscaElemento(pagina,chave,ano); //vou somando os valores na variavel
		chave+= 100; // e incrementando um dia
	}
	return soma; // e retorno a soma dos valores
}
int BuscaPeriodo(ArvB* pagina, long int chave,int dias,int ano){ // FUNCAO PRINCIPAL, busca a partir da data inicial o valor recebido nesse periodo de dias
	int i;
	int soma = 0; //soma de todos os valores
	for(i=0;i<dias;i++){
		if(chave - chave%100 > 3000){ //se os dias da chave forem maiores que 30
			chave++; //entao incremento um mes
			if(chave%100>12){ //se o mes da chave for maior que 12
				ano++; // incremento um ano
				chave = chave - 12; //e o mes da chave passa a ser 1
			}
			chave = chave - 3000; //e o dia da chave passa a ser 1
		}
		soma+= BuscaElemento(pagina,chave,ano);// entao somo os valores que forem encontrados nesse dia mes e ano
		chave+=100; // e incremento o dia
	}
	return soma; //e retorno o valor
}
int main(){ //FUNCAO MAIN, ESSA FUNCAO SO FUNCIONA COM BASE NO TEMPO, QUER DIZER QUE DEPOIS DO DIA 22/01/01 NAO DA PRA INSERIR NO DIA 21/01/01, ASSIM COMO NAO PODE INSERIR EM INTERVALOS MUITO GRANDES ENTRE AS VENDAS
   //OBS: SE FOR USAR O PROGRAMA, POR FAVOR UTILIZAR APENAS ENTRADAS VALIDAS E QUE SEJAM PROGRESSIVAS, POIS ESSE PROGRAMA FOI FEITO COM  BASE NO TEMPO REAL ENTAO ELE NÃO FUNCIONA MUITO BEM QUANDO AS ENTRADAS NÃO TEM TEMPO PROGRESSIVO
	ArvB *antigaP; //ARVORE QUE GUARDA OS VALORES DOS PAGAMENTOS PASSADOS
	ArvB *atualP; //ARVORE QUE GUARDA VALORES DOS PAGAMENTOS ATUAIS
	ArvB *antigaR; //ARVORE QUE GUARDA VALORES DOS RECEBIMENTOS PASSADOS
	ArvB *atualR; // ARVORE QUE GUARDA VALORES DOS RECEBIMENTOS ATUAIS
	antigaR = NULL; //SETO TODAS COMO NULL
	atualR = NULL;
	antigaP = NULL;
	atualP = NULL;
	long int ultimaParcela;
	long int chave;
	int i;
	int OP;// OP1,2,3 E 4 SAO VARIAVEIS QUE AUXILIAM NA OPERAÇÃO E TROCA DE OPERAÇÕES
	int OP2;
	int OP3;
	int OP4;
	while(OP!=5){ //ESSE É O MENU DO PROGRAMA ESSA PARTE NAO IREI COMENTAR POIS ACHO QUE OS PRINTF JA FALAM POR SI SO
        printf("DIGITE A OPERACAO DESEJADA\n");
        printf("1)PAGAMENTO\n");
        printf("2)RECEBIMENTO\n");
        printf("3)CONSULTA ANO ATUAL\n");
        printf("4)CONSULTA ANOS ANTERIORES\n");
        printf("5)FINALIZAR OPERACOES\n");
        scanf("%d",&OP);
        if(OP == 1){
            printf("1)PARCELADO\n");
            printf("2)A VISTA\n");
            scanf("%d",&OP2);
            if(OP2 == 1){
                printf("ESCOLHA A PARCELA, ATE 6 PARCELAS\n");
                scanf("%d",&OP2);
					 while(OP2<1 || OP2>6){
						 printf("Escolha um numero entre 1 e 6\n");
						 scanf("%d",&OP2);
					 }
                printf("INSIRA DIA MES 5 DIGITOS DE PAGAMENTO E ANO NO MESMO NUMERO, EXEMPLO: 1010000100\n");
                scanf("%ld",&chave);
                ultimaParcela = VerificaParcela(antigaP,chave,OP2);
					 antigaP = VerificaInsere(antigaP,atualP,ultimaParcela);
                atualP = InsereParcelado(atualP,chave,OP2);
            }
            else if(OP2 == 2){
                printf("INSIRA MES ANO 5 DIGITOS DE PAGAMENTO E ANO NO MESMO NUMERO, EXEMPLO: 1010000100\n");
                scanf("%ld",&chave);
                antigaP = VerificaInsere(antigaP,atualP,chave);
                atualP = InsereVista(atualP,chave);
            }
        }
        else if(OP == 2){
            printf("1)PARCELADO\n");
            printf("2)A VISTA\n ");
            scanf("%d",&OP2);
            if(OP2 == 1){
                printf("ESCOLHA A PARCELA, ATE 6 PARCELAS\n");
                scanf("%d",&OP2);
					while(OP2<1 || OP2>6){
						 printf("Escolha um numero entre 1 e 6\n");
						 scanf("%d",&OP2);
					 }
                printf("INSIRA DIA MES 5 DIGITOS DE PAGAMENTO E ANO NO MESMO NUMERO, EXEMPLO: 1010000100\n");
                scanf("%ld",&chave);
                ultimaParcela = VerificaParcela(antigaR,chave,OP2);
					 antigaR = VerificaInsere(antigaR,atualR,ultimaParcela);
                atualR = InsereParcelado(atualR,chave,OP2);
            }
            else if(OP2 == 2){
                printf("INSIRA MES ANO 5 DIGITOS DE PAGAMENTO E ANO NO MESMO NUMERO, EXEMPLO: 1010000100\n");
                scanf("%ld",&chave);
                antigaR = VerificaInsere(antigaR,atualR,chave);
                atualR = InsereVista(atualR,chave);
            }
        }
        else if(OP == 3){
            printf("1) DIA\n");
            printf("2) MES\n");
            printf("3) PERIODO\n");
            scanf("%d",&OP2);
            if(OP2 == 1){
                printf("INSIRA O DIA E MES DESEJADO EXEMPLO: 2104 \n");
                scanf("%d",&OP2);
					while(OP2<101 || (OP2-OP2%100) > 3000 || OP2%100 > 12){
						printf("Dia e Mes Invalido, por favor digite um numero valido\n");
						scanf("%d",&OP2);
					}
                printf("INSIRA O ANO\n");
                scanf("%d",&OP3);
					 while(OP3<0 || OP3>99){
						printf("Por favor digite um ano entre 0 e 99\n");
						scanf("%d",&OP3);
					}
                printf("RECEBEU: %d PAGOU: %d SALDO: %d\n",BuscaElemento(atualR,OP2,OP3),BuscaElemento(atualP,OP2,OP3),(BuscaElemento(atualR,OP2,OP3)-BuscaElemento(atualP,OP2,OP3)));
            }
            else if(OP2 == 2){
                printf("INSIRA O MES DESEJADO\n");
                scanf("%d",&OP2);
					 while(OP2<1 || OP2 > 12){
						 printf("Digite um mes entre 1 e 12\n");
						 scanf("%d",&OP2);
					 }
                printf("INSIRA O ANO\n");
                scanf("%d",&OP3);
					 while(OP3<0 || OP3>99){
						printf("Por favor digite um ano entre 0 e 99\n");
						scanf("%d",&OP3);
					}
                printf("RECEBEU: %d PAGOU: %d SALDO: %d\n",BuscaMes(atualR,OP2,OP3),BuscaMes(atualP,OP2,OP3),(BuscaMes(atualR,OP2,OP3)-BuscaMes(atualP,OP2,OP3)));
            }
            else if(OP2 == 3){
                printf("INSIRA A DATA INICIAL PARA BUSCAR EXEMPLO : 2607\n");
                scanf("%d",&OP2);
					 while(OP2<101 || (OP2-OP2%100) > 3000 || OP2%100 > 12){
						printf("Dia e Mes Invalido, por favor digite um numero valido\n");
						scanf("%d",&OP2);
					}
                printf("INSIRA O ANO\n");
                scanf("%d",&OP3);
					 while(OP3<0 || OP3>99){
						printf("Por favor digite um ano entre 0 e 99\n");
						scanf("%d",&OP3);
					}
                printf("INSIRA O PERIODO EM DIAS QUE GOSTARIA DE BUSCAR\n");
                scanf("%d",&OP4);
					 while(OP4<1){
					 	printf("Digite um periodo maior que 0\n");
						scanf("%d",&OP4);
					 }
                printf("RECEBEU: %d PAGOU: %d SALDO: %d\n",BuscaPeriodo(atualR,OP2,OP4,OP3),BuscaPeriodo(atualP,OP2,OP4,OP3),BuscaPeriodo(atualR,OP2,OP4,OP3)-BuscaPeriodo(atualP,OP2,OP4,OP3));
            }
        }
        else if(OP == 4){
            printf("1) DIA\n");
            printf("2) MES\n");
            printf("3) PERIODO\n");
            scanf("%d",&OP2);
            if(OP2 == 1){
                printf("INSIRA O DIA E MES DESEJADO EXEMPLO: 2104 \n");
                scanf("%d",&OP2);
					 while(OP2<101 || (OP2-OP2%100) > 3000 || OP2%100 > 12){
						printf("Dia e Mes Invalido, por favor digite um numero valido\n");
						scanf("%d",&OP2);
					}
                printf("INSIRA O ANO DESEJADO\n");
                scanf("%d",&OP3);
					 while(OP3<0 || OP3>99){
						printf("Por favor digite um ano entre 0 e 99\n");
						scanf("%d",&OP3);
					}
                printf("RECEBEU: %d PAGOU: %d SALDO: %d\n",BuscaElemento(antigaR,OP2,OP3),BuscaElemento(antigaP,OP2,OP3),(BuscaElemento(antigaR,OP2,OP3)-BuscaElemento(antigaP,OP2,OP3)));
            }
            else if(OP2 == 2){
                printf("INSIRA O MES DESEJADO\n");
                scanf("%d",&OP2);
					while(OP2<1 || OP2 > 12){
						 printf("Digite um mes entre 1 e 12\n");
						 scanf("%d",&OP2);
					 }
                printf("INSIRA O ANO DESEJADO\n");
                scanf("%d",&OP3);
					 while(OP3<0 || OP3>99){
						printf("Por favor digite um ano entre 0 e 99\n");
						scanf("%d",&OP3);
					}
                printf("RECEBEU: %d PAGOU: %d SALDO: %d\n",BuscaMes(antigaR,OP2,OP3),BuscaMes(antigaP,OP2,OP3),(BuscaMes(antigaR,OP2,OP3)-BuscaMes(antigaP,OP2,OP3)));
            }
            else if(OP2 == 3){
                printf("INSIRA A DATA INICIAL PARA BUSCAR EXEMPLO : 2607\n");
                scanf("%d",&OP2);
					while(OP2<101 || (OP2-OP2%100) > 3000 || OP2%100 > 12){
						printf("Dia e Mes Invalido, por favor digite um numero valido\n");
						scanf("%d",&OP2);
					}
                printf("INSIRA O ANO DESEJADO\n");
                scanf("%d",&OP3);
					 while(OP3<0 || OP3>99){
						printf("Por favor digite um ano entre 0 e 99\n");
						scanf("%d",&OP3);
					}
                printf("INSIRA O PERIODO EM DIAS QUE GOSTARIA DE BUSCAR\n");
                scanf("%d",&OP4);
					 while(OP4<1){
					 	printf("Digite um periodo maior que 0\n");
						scanf("%d",&OP4);
					 }
                printf("RECEBEU: %d PAGOU: %d SALDO: %d\n",BuscaPeriodo(antigaR,OP2,OP4,OP3),BuscaPeriodo(antigaP,OP2,OP4,OP3),BuscaPeriodo(antigaR,OP2,OP4,OP3)-BuscaPeriodo(antigaP,OP2,OP4,OP3));
            }
        }
    }
}
