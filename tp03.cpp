#include<iostream>
#include<iomanip>
#include<cstring>
#include<cstdlib>
#include<locale.h>
#include<fstream>
#include<ctime>

#define tcache1 3
#define tcache2 3
#define tcache3 3
#define tram    12

#define tempC1  1
#define tempC2  2
#define tempC3  4
#define tempRam 10
#define tempHD  20
#define tamIst  15
#define tamDados 15

using namespace std;

struct BlocoHD{
    string palavras[4];
};

struct BlocoMP{
    int endBloco=-1;
    int vazio=0;
    int tempo=0;
    bool atualiza=false;
    BlocoHD bloco;
};

struct LinhaCache{
    BlocoHD bloco;
    int endBloco=-1;
    int tempo=0;
    int vazio=0;
    int interrup = 0;
    bool atualiza=false;
};

struct Instrucao{
    int endBloco;
    int endPalvra;
    int opcode;
};

void criarMemHD(BlocoHD*);
Instrucao *preencherMemInst(void);

//Funções Programa
LinhaCache *completamenteAss(int, int, LinhaCache*, LinhaCache*, LinhaCache*, BlocoMP*, BlocoHD*);
LinhaCache verificaCache2(int, int, LinhaCache, LinhaCache*, LinhaCache*, BlocoMP*, BlocoHD*);
LinhaCache verificaCache3(int, int, LinhaCache, LinhaCache*, BlocoMP*, BlocoHD*);
LinhaCache verificaRam(int, int, LinhaCache, BlocoMP*, BlocoHD*);
BlocoMP    verificaHD(int, BlocoMP, BlocoHD*);
void verificaAlteracoes(LinhaCache*, LinhaCache*, LinhaCache*, BlocoMP*, BlocoHD*);
void imprimirHamb(BlocoHD*);

//Funções Interrupção
//void criarMemInt(BlocoHD *memoria);
void criarInstrucoes(Instrucao *inst);
//void mostrarHamb(BlocoHD *memoria);
void interrupcao(LinhaCache*, LinhaCache*, LinhaCache*, BlocoMP*);



int cache1Hit=0,  cache2Hit=0,  cache3Hit=0,  ramHit=0;
int cache1Miss=0, cache2Miss=0, cache3Miss=0, ramMiss=0;
int HDHit=0;

int tempoProg=0;

int main() {
    setlocale(LC_ALL, "Portuguese");
    srand(time(NULL));

    Instrucao *memoriaInst;

    int opcode=0;
    int endBloco=-1;
    int endPalavra=-1;
    int cont=0;

    LinhaCache *dados;
    string regIng = "/0";

    LinhaCache cache1[tcache1];
    LinhaCache cache2[tcache2];
    LinhaCache cache3[tcache3];
    BlocoMP   memoriaRam[tram];
    BlocoHD memoria[tamDados];

    criarMemHD(memoria);
    cout << "HD  criado" << endl;

    memoriaInst = preencherMemInst();

    // Começo em si

    while(opcode != -1){
        if(rand()%2 == 0){ //interrupções

            opcode = memoriaInst[cont].opcode;
            endBloco = memoriaInst[cont].endBloco;
            endPalavra = memoriaInst[cont].endPalvra;

            if(opcode != -1){
                dados = completamenteAss(0,endBloco, cache1, cache2, cache3, memoriaRam, memoria);
            }
            else{
                verificaAlteracoes(cache1, cache2, cache3, memoriaRam, memoria);
            }

            switch(opcode){
            case -1:
                imprimirHamb(memoria);
                break;
            case 0:
                regIng = dados->bloco.palavras[endPalavra];
                break;
            case 1:
                dados->bloco.palavras[endPalavra] = regIng;
                dados->atualiza = true;
                break;
            case 2:
                regIng = dados->bloco.palavras[endPalavra];
                dados->bloco.palavras[endPalavra] = "/0";
                dados->atualiza = true;
                break;
            default:
                cout << "Opção invalida!" << endl;
            }

            cont++;
        }
        else{
            tempoProg = tempoProg + 1;
            interrupcao(cache1, cache2, cache3, memoriaRam);
        }
    }

    cout << endl << "Cache1Hit:  " << cache1Hit << endl;
    cout << "Cache1Miss: " << cache1Miss << endl << endl;

    cout << "Cache2Hit:  " << cache2Hit << endl;
    cout << "Cache2Miss: " << cache2Miss << endl << endl;

    cout << "Cache3Hit:  " << cache3Hit << endl;
    cout << "Cache3Miss: " << cache3Miss << endl << endl;

    cout << "RamHit:     " << ramHit  << endl;
    cout << "RamMiss:    " << ramMiss << endl << endl;

    cout << "HDHit:      " << HDHit   << endl << endl;

    cout << "Tempo:      " << tempoProg << endl;

    delete [] memoriaInst;
    return 0;
}

//FUNÇÕES PROGRAMA
void criarMemHD(BlocoHD *memoriaDados){

    memoriaDados[0].palavras[0] = "Queijo Cheddar";
    memoriaDados[0].palavras[1] = "Queijo Muçarela";
    memoriaDados[0].palavras[2] = "Queijo Americano";
    memoriaDados[0].palavras[3] = "Queijo Provolone";

    memoriaDados[1].palavras[0] = "Hambúrguer Bovino";
    memoriaDados[1].palavras[1] = "Hambúrguer de Picanha";
    memoriaDados[1].palavras[2] = "Hambúrguer de Frango";
    memoriaDados[1].palavras[3] = "Hambúrguer Suíno";

    memoriaDados[2].palavras[0] = "Bacon";
    memoriaDados[2].palavras[1] = "Filé de Frango";
    memoriaDados[2].palavras[2] = "Hambúrguer Vegetariano";
    memoriaDados[2].palavras[3] = "Ovos";

    memoriaDados[3].palavras[0] = "Tomate";
    memoriaDados[3].palavras[1] = "Cebola";
    memoriaDados[3].palavras[2] = "Alface";
    memoriaDados[3].palavras[3] = "Rucula";

    memoriaDados[4].palavras[0] = "Requeijão Cremoso";
    memoriaDados[4].palavras[1] = "Maionese";
    memoriaDados[4].palavras[2] = "Ketchup";
    memoriaDados[4].palavras[3] = "Molho Rosé";

    memoriaDados[5].palavras[0] = "Molho Barbecue";
    memoriaDados[5].palavras[1] = "Molho Flame";
    memoriaDados[5].palavras[2] = "Molho Parrilero";
    memoriaDados[5].palavras[3] = "Molho Pesto";

    memoriaDados[6].palavras[0] = "Pimentao";
    memoriaDados[6].palavras[1] = "Salsa";
    memoriaDados[6].palavras[2] = "Cebolinha";
    memoriaDados[6].palavras[3] = "Milho";

    memoriaDados[7].palavras[0] = "Peito de Peru";
    memoriaDados[7].palavras[1] = "Batata palha";
    memoriaDados[7].palavras[2] = "Ervilha";
    memoriaDados[7].palavras[3] = "Catupiry";

    memoriaDados[8].palavras[0] = "Calabresa";
    memoriaDados[8].palavras[1] = "Bife";
    memoriaDados[8].palavras[2] = "Pimenta";
    memoriaDados[8].palavras[3] = "Picles";

    memoriaDados[9].palavras[0] = "Omelete";
    memoriaDados[9].palavras[1] = "Requeijão cremoso";
    memoriaDados[9].palavras[2] = "Torresmo";
    memoriaDados[9].palavras[3] = "Pimenta";

    memoriaDados[10].palavras[0] = "Palmitio";
    memoriaDados[10].palavras[1] = "Frango com catupiry";
    memoriaDados[10].palavras[2] = "Salmão";
    memoriaDados[10].palavras[3] = "Beringela";

    memoriaDados[11].palavras[0] = "Fritas";
    memoriaDados[11].palavras[1] = "Carne assada";
    memoriaDados[11].palavras[2] = "Ervilha";
    memoriaDados[11].palavras[3] = "Uva passa";

    memoriaDados[12].palavras[0] = "Açafrão";
    memoriaDados[12].palavras[1] = "Beterraba";
    memoriaDados[12].palavras[2] = "Cenoura";
    memoriaDados[12].palavras[3] = "Purê de batata";

    //Hamburguer
    memoriaDados[13].palavras[0] = "Pão";
    memoriaDados[13].palavras[1] = "/0";
    memoriaDados[13].palavras[2] = "/0";
    memoriaDados[13].palavras[3] = "/0";

    memoriaDados[14].palavras[0] = "/0";
    memoriaDados[14].palavras[1] = "/0";
    memoriaDados[14].palavras[2] = "/0";
    memoriaDados[14].palavras[3] = "/0";

}

Instrucao *preencherMemInst(void){
    /*Opcodes:
        -1 Mostrar Hamburguer montando e sair
         0 Selecionar um ingrediente e armazenar em regIng
         1 Inserir ingrediente do regIng no hamburguer
         2 Retirar ingrediente do hambuguer e armazenar em regIng
    */

    ifstream arq;
    Instrucao *memoriaIst = new Instrucao [tamIst+1];

    string *aux;
    aux = new string [tamIst];

    arq.open("programa.txt");
    srand(time(NULL));

    for(int i=0; i<tamIst; i++){
        arq >> aux[i];
        memoriaIst[i].opcode = aux[i][0] - 48;

        if(memoriaIst[i].opcode != 0){
                memoriaIst[i].endBloco = (rand()%2)+13;
                if(memoriaIst[i].endBloco == 13)
                    memoriaIst[i].endPalvra = (rand()%3)+1;
                else
                    memoriaIst[i].endPalvra = rand()%4;
        }
        else{
            memoriaIst[i].endBloco = aux[i][2] - 48;
            if((int)aux[i][3] != 0)
               memoriaIst[i].endBloco =  memoriaIst[i].endBloco*10 +  (aux[i][3]-48);

            memoriaIst[i].endPalvra = rand()%4;
        }
    }
    memoriaIst[tamIst].opcode = -1;

    /*for(int i=0; i<tamIst; i++){
        cout << memoriaIst[i].opcode << " ";
        cout << memoriaIst[i].endBloco << " ";
        cout << memoriaIst[i].endPalvra << endl;
    }
    cout << memoriaIst[tamIst].opcode << endl;*/

    return memoriaIst;
}

LinhaCache *completamenteAss(int interrup, int bloco, LinhaCache *cache1, LinhaCache *cache2, LinhaCache *cache3, BlocoMP *memoriaRam, BlocoHD *memoria){
    int maiorTempo=0;
    int auxLinha;

    //cout << "1" << endl;
    if(interrup == 0){
        tempoProg = tempoProg + tempC1;
        for(int i=0; i<tcache1; i++){
            if(cache1[i].endBloco == bloco && cache1[i].interrup == 0){
                cache1Hit++;
                cache1[i].tempo = 0;

                for(int j=i+1; j<tcache1; j++)
                    cache1[j].tempo++;
                return &cache1[i];
            }
            else
                cache1[i].tempo++;
        }
        cache1Miss++;
        for(int i=0; i<tcache1; i++){
            if(cache1[i].vazio == 0){
                cache1[i] = verificaCache2(0, bloco, cache1[i], cache2, cache3, memoriaRam, memoria);
                return &cache1[i];
            }
        }
        //LRU
        maiorTempo=0;
        for(int i=0; i<tcache1; i++){
            if(maiorTempo <= cache1[i].tempo){
                auxLinha = i;
            }
        }
        cache1[auxLinha] = verificaCache2(0, bloco, cache1[auxLinha], cache2, cache3, memoriaRam, memoria);
        return &cache1[auxLinha];
    }
    else{
        for(int i=0; i<tcache1; i++){
            if(cache1[i].endBloco == bloco && cache1[i].interrup == 1){
                cache1[i].tempo = 0;

                for(int j=i+1; j<tcache1; j++)
                    cache1[j].tempo++;
                return &cache1[i];
            }
            else
                cache1[i].tempo++;
        }
        for(int i=0; i<tcache1; i++){
            if(cache1[i].vazio == 0){
                cache1[i] = verificaCache2(1, bloco, cache1[i], cache2, cache3, memoriaRam, memoria);
                return &cache1[i];
            }
        }
        //LRU
        maiorTempo=0;
        for(int i=0; i<tcache1; i++){
            if(maiorTempo <= cache1[i].tempo){
                auxLinha = i;
            }
        }
        cache1[auxLinha] = verificaCache2(1, bloco, cache1[auxLinha], cache2, cache3, memoriaRam, memoria);
        return &cache1[auxLinha];
    }
}

LinhaCache verificaCache2(int interrup, int bloco, LinhaCache cache1, LinhaCache *cache2, LinhaCache *cache3, BlocoMP *memoriaRam, BlocoHD *memoria){
    int maiorTempo=0;
    int auxLinha;
    LinhaCache troca;
    //cout << "2" << endl;
    if(interrup == 0){
        tempoProg = tempoProg + tempC2;
        for(int i=0; i<tcache2; i++){
            if(cache2[i].endBloco == bloco && cache2[i].interrup == 0){
                cache2Hit++;
                cache2[i].tempo = 0;

                for(int j=i+1; j<tcache2; j++)
                    cache2[j].tempo++;

                troca.interrup = cache2[i].interrup;
                troca.atualiza = cache2[i].atualiza;
                troca.bloco    = cache2[i].bloco;
                troca.endBloco = cache2[i].endBloco;
                troca.tempo    = cache2[i].tempo;
                troca.vazio    = cache2[i].vazio;

                cache2[i].interrup = cache1.interrup;
                cache2[i].atualiza = cache1.atualiza;
                cache2[i].bloco    = cache1.bloco;
                cache2[i].endBloco = cache1.endBloco;
                cache2[i].tempo    = cache1.tempo;
                cache2[i].vazio    = cache1.vazio;

                return troca;
            }
            else
                cache2[i].tempo++;
        }
        cache2Miss++;
        for(int i=0; i<tcache2; i++){
            if(cache2[i].vazio == 0){
                cache2[i] = verificaCache3(0, bloco, cache2[i], cache3, memoriaRam, memoria);

                troca.interrup = cache2[i].interrup;
                troca.atualiza = cache2[i].atualiza;
                troca.bloco    = cache2[i].bloco;
                troca.endBloco = cache2[i].endBloco;
                troca.tempo    = cache2[i].tempo;
                troca.vazio    = cache2[i].vazio;

                cache2[i].interrup = cache1.interrup;
                cache2[i].atualiza = cache1.atualiza;
                cache2[i].bloco    = cache1.bloco;
                cache2[i].endBloco = cache1.endBloco;
                cache2[i].tempo    = cache1.tempo;
                cache2[i].vazio    = cache1.vazio;

                return troca;
            }
        }
        //LRU
        maiorTempo=0;
        for(int i=0; i<tcache2; i++){
            if(maiorTempo <= cache2[i].tempo){
                auxLinha = i;
            }
        }
        cache2[auxLinha] = verificaCache3(0, bloco, cache2[auxLinha], cache3, memoriaRam, memoria);

        troca.interrup = cache2[auxLinha].interrup;
        troca.atualiza = cache2[auxLinha].atualiza;
        troca.bloco    = cache2[auxLinha].bloco;
        troca.endBloco = cache2[auxLinha].endBloco;
        troca.tempo    = cache2[auxLinha].tempo;
        troca.vazio    = cache2[auxLinha].vazio;

        cache2[auxLinha].interrup = cache1.interrup;
        cache2[auxLinha].atualiza = cache1.atualiza;
        cache2[auxLinha].bloco    = cache1.bloco;
        cache2[auxLinha].endBloco = cache1.endBloco;
        cache2[auxLinha].tempo    = cache1.tempo;
        cache2[auxLinha].vazio    = cache1.vazio;

        return troca;
    }
    else{
        for(int i=0; i<tcache2; i++){
            if(cache2[i].endBloco == bloco && cache2[i].interrup == 1){
                cache2[i].tempo = 0;

                for(int j=i+1; j<tcache2; j++)
                    cache2[j].tempo++;

                troca.atualiza = cache2[i].atualiza;
                troca.interrup = cache2[i].interrup;
                troca.atualiza = cache2[i].atualiza;
                troca.bloco    = cache2[i].bloco;
                troca.endBloco = cache2[i].endBloco;
                troca.tempo    = cache2[i].tempo;
                troca.vazio    = cache2[i].vazio;

                cache2[i].interrup = cache1.interrup;
                cache2[i].interrup = cache1.interrup;
                cache2[i].atualiza = cache1.atualiza;
                cache2[i].bloco    = cache1.bloco;
                cache2[i].endBloco = cache1.endBloco;
                cache2[i].tempo    = cache1.tempo;
                cache2[i].vazio    = cache1.vazio;

                return troca;
            }
            else
                cache2[i].tempo++;
        }
        for(int i=0; i<tcache2; i++){
            if(cache2[i].vazio == 0){
                cache2[i] = verificaCache3(1, bloco, cache2[i], cache3, memoriaRam, memoria);

                troca.interrup = cache2[i].interrup;
                troca.atualiza = cache2[i].atualiza;
                troca.bloco    = cache2[i].bloco;
                troca.endBloco = cache2[i].endBloco;
                troca.tempo    = cache2[i].tempo;
                troca.vazio    = cache2[i].vazio;

                cache2[i].interrup = cache1.interrup;
                cache2[i].atualiza = cache1.atualiza;
                cache2[i].bloco    = cache1.bloco;
                cache2[i].endBloco = cache1.endBloco;
                cache2[i].tempo    = cache1.tempo;
                cache2[i].vazio    = cache1.vazio;

                return troca;
            }
        }
        //LRU
        maiorTempo=0;
        for(int i=0; i<tcache2; i++){
            if(maiorTempo <= cache2[i].tempo){
                auxLinha = i;
            }
        }
        cache2[auxLinha] = verificaCache3(1, bloco, cache2[auxLinha], cache3, memoriaRam, memoria);

        troca.interrup = cache2[auxLinha].interrup;
        troca.atualiza = cache2[auxLinha].atualiza;
        troca.bloco    = cache2[auxLinha].bloco;
        troca.endBloco = cache2[auxLinha].endBloco;
        troca.tempo    = cache2[auxLinha].tempo;
        troca.vazio    = cache2[auxLinha].vazio;

        cache2[auxLinha].interrup = cache1.interrup;
        cache2[auxLinha].atualiza = cache1.atualiza;
        cache2[auxLinha].bloco    = cache1.bloco;
        cache2[auxLinha].endBloco = cache1.endBloco;
        cache2[auxLinha].tempo    = cache1.tempo;
        cache2[auxLinha].vazio    = cache1.vazio;

        return troca;
    }
}

LinhaCache verificaCache3(int interrup, int bloco, LinhaCache cache2, LinhaCache *cache3, BlocoMP *memoriaRam, BlocoHD *memoria){
    int maiorTempo=0;
    int auxLinha;
    LinhaCache troca;
    //cout << "3" << endl;
    if(interrup == 0){
        tempoProg = tempoProg + tempC3;
        for(int i=0; i<tcache3; i++){
            if(cache3[i].endBloco == bloco && cache3[i].interrup == 0){
                cache3Hit++;
                cache3[i].tempo = 0;

                for(int j=i+1; j<tcache3; j++)
                    cache3[j].tempo++;

                troca.interrup = cache3[i].interrup;
                troca.atualiza = cache3[i].atualiza;
                troca.bloco    = cache3[i].bloco;
                troca.endBloco = cache3[i].endBloco;
                troca.tempo    = cache3[i].tempo;
                troca.vazio    = cache3[i].vazio;

                cache3[i].interrup = cache2.interrup;
                cache3[i].atualiza = cache2.atualiza;
                cache3[i].bloco    = cache2.bloco;
                cache3[i].endBloco = cache2.endBloco;
                cache3[i].tempo    = cache2.tempo;
                cache3[i].vazio    = cache2.vazio;

                return troca;
            }
            else
                cache3[i].tempo++;
        }
        cache3Miss++;
        for(int i=0; i<tcache3; i++){
            if(cache3[i].vazio == 0){
                cache3[i] = verificaRam(0, bloco, cache3[i], memoriaRam, memoria);

                troca.interrup = cache3[i].interrup;
                troca.atualiza = cache3[i].atualiza;
                troca.bloco    = cache3[i].bloco;
                troca.endBloco = cache3[i].endBloco;
                troca.tempo    = cache3[i].tempo;
                troca.vazio    = cache3[i].vazio;

                cache3[i].interrup = cache2.interrup;
                cache3[i].atualiza = cache2.atualiza;
                cache3[i].bloco    = cache2.bloco;
                cache3[i].endBloco = cache2.endBloco;
                cache3[i].tempo    = cache2.tempo;
                cache3[i].vazio    = cache2.vazio;

                return troca;
            }
        }
        //LRU
        maiorTempo=0;
        for(int i=0; i<tcache3; i++){
            if(maiorTempo <= cache3[i].tempo){
                auxLinha = i;
            }
        }
        cache3[auxLinha] = verificaRam(0, bloco, cache3[auxLinha], memoriaRam, memoria);

        troca.interrup = cache3[auxLinha].interrup;
        troca.atualiza = cache3[auxLinha].atualiza;
        troca.bloco    = cache3[auxLinha].bloco;
        troca.endBloco = cache3[auxLinha].endBloco;
        troca.tempo    = cache3[auxLinha].tempo;
        troca.vazio    = cache3[auxLinha].vazio;

        cache3[auxLinha].interrup = cache2.interrup;
        cache3[auxLinha].atualiza = cache2.atualiza;
        cache3[auxLinha].bloco    = cache2.bloco;
        cache3[auxLinha].endBloco = cache2.endBloco;
        cache3[auxLinha].tempo    = cache2.tempo;
        cache3[auxLinha].vazio    = cache2.vazio;

        return troca;
    }
    else{
        for(int i=0; i<tcache3; i++){
            if(cache3[i].endBloco == bloco && cache3[i].interrup == 1){
                cache3[i].tempo = 0;

                for(int j=i+1; j<tcache3; j++)
                    cache3[j].tempo++;

                troca.interrup = cache3[i].interrup;
                troca.atualiza = cache3[i].atualiza;
                troca.bloco    = cache3[i].bloco;
                troca.endBloco = cache3[i].endBloco;
                troca.tempo    = cache3[i].tempo;
                troca.vazio    = cache3[i].vazio;

                cache3[i].interrup = cache2.interrup;
                cache3[i].atualiza = cache2.atualiza;
                cache3[i].bloco    = cache2.bloco;
                cache3[i].endBloco = cache2.endBloco;
                cache3[i].tempo    = cache2.tempo;
                cache3[i].vazio    = cache2.vazio;

                return troca;
            }
            else
                cache3[i].tempo++;
        }
        for(int i=0; i<tcache3; i++){
            if(cache3[i].vazio == 0){
                cache3[i] = verificaRam(1, bloco, cache3[i], memoriaRam, memoria);

                troca.interrup = cache3[i].interrup;
                troca.atualiza = cache3[i].atualiza;
                troca.bloco    = cache3[i].bloco;
                troca.endBloco = cache3[i].endBloco;
                troca.tempo    = cache3[i].tempo;
                troca.vazio    = cache3[i].vazio;

                cache3[i].interrup = cache2.interrup;
                cache3[i].atualiza = cache2.atualiza;
                cache3[i].bloco    = cache2.bloco;
                cache3[i].endBloco = cache2.endBloco;
                cache3[i].tempo    = cache2.tempo;
                cache3[i].vazio    = cache2.vazio;

                return troca;
            }
        }
        //LRU
        maiorTempo=0;
        for(int i=0; i<tcache3; i++){
            if(maiorTempo <= cache3[i].tempo){
                auxLinha = i;
            }
        }
        cache3[auxLinha] = verificaRam(1, bloco, cache3[auxLinha], memoriaRam, memoria);

        troca.interrup = cache3[auxLinha].interrup;
        troca.atualiza = cache3[auxLinha].atualiza;
        troca.bloco    = cache3[auxLinha].bloco;
        troca.endBloco = cache3[auxLinha].endBloco;
        troca.tempo    = cache3[auxLinha].tempo;
        troca.vazio    = cache3[auxLinha].vazio;

        cache3[auxLinha].interrup = cache2.interrup;
        cache3[auxLinha].atualiza = cache2.atualiza;
        cache3[auxLinha].bloco    = cache2.bloco;
        cache3[auxLinha].endBloco = cache2.endBloco;
        cache3[auxLinha].tempo    = cache2.tempo;
        cache3[auxLinha].vazio    = cache2.vazio;

        return troca;
    }
}

LinhaCache verificaRam(int interrup, int bloco, LinhaCache cache3, BlocoMP *memoriaRam, BlocoHD *memoria){
    //cout << "ram" << endl;

    if(interrup == 0){
        tempoProg = tempoProg + tempRam;

        for(int i=0; i<tram; i++){
            if(memoriaRam[i].endBloco==bloco){
                ramHit++;
                if(cache3.endBloco==13 || cache3.endBloco==14 && cache3.atualiza==true && cache3.interrup == 0)
                    for(int j=0; j<tram; j++)
                        if(memoriaRam[j].endBloco==cache3.endBloco){
                            memoriaRam[j].bloco = cache3.bloco;
                            memoriaRam[j].atualiza = true;
                        }
                cache3.atualiza = false;
                cache3.bloco    = memoriaRam[i].bloco;
                cache3.endBloco = memoriaRam[i].endBloco;
                cache3.tempo    = 0;
                cache3.vazio    = 1;
                cache3.interrup = 0;
                return cache3;
            }
        }
        ramMiss++;
        for(int i=0; i<tram; i++){
            if(memoriaRam[i].vazio == 0){
                memoriaRam[i] = verificaHD(bloco, memoriaRam[i], memoria);

                cache3.atualiza = false;
                cache3.bloco    = memoriaRam[i].bloco;
                cache3.endBloco = memoriaRam[i].endBloco;
                cache3.tempo    = 0;
                cache3.vazio    = 1;
                cache3.interrup = 0;

                return cache3;
            }
        }
        for(int i=0; i<tram; i++){
            if(memoriaRam[i].endBloco!=13 || memoriaRam[i].endBloco!=14){
                memoriaRam[i] = verificaHD(bloco, memoriaRam[i], memoria);

                cache3.atualiza = false;
                cache3.bloco    = memoriaRam[i].bloco;
                cache3.endBloco = bloco;
                cache3.tempo    = 0;
                cache3.vazio    = 1;
                cache3.interrup = 0;

                return cache3;
            }
        }
    }
    else{
        if(cache3.endBloco==13 || cache3.endBloco==14 && cache3.atualiza==true && cache3.interrup == 1)
            for(int i=0; i<tram; i++)
                if(i == cache3.endBloco){
                    memoria[i] = cache3.bloco;
                }
        cache3.atualiza = false;
        cache3.bloco    = memoria[bloco];
        cache3.endBloco = bloco;
        cache3.tempo    = 0;
        cache3.vazio    = 1;
        cache3.interrup = 1;

        return cache3;
    }
}

BlocoMP  verificaHD(int bloco, BlocoMP memoriaRam, BlocoHD *memoria){
    HDHit++;
    //cout << "HD" << endl;
    if(memoriaRam.atualiza == true){
        memoria[memoriaRam.endBloco] = memoriaRam.bloco;
    }

    memoriaRam.bloco = memoria[bloco];
    memoriaRam.atualiza = false;
    memoriaRam.endBloco = bloco;
    memoriaRam.vazio = 1;
    memoriaRam.tempo = 0;

    return memoriaRam;
}


void verificaAlteracoes(LinhaCache *cache1, LinhaCache *cache2, LinhaCache *cache3, BlocoMP *memoriaRam, BlocoHD *memoria){
    for(int i=0; i<tcache1; i++){
        if(cache1[i].atualiza==true && cache1[i].interrup==0)
            memoria[cache1[i].endBloco] = cache1[i].bloco;
    }

    for(int i=0; i<tcache2; i++){
        if(cache2[i].atualiza==true && cache2[i].interrup==0)
            memoria[cache2[i].endBloco] = cache2[i].bloco;
    }

    for(int i=0; i<tcache3; i++){
        if(cache3[i].atualiza==true && cache3[i].interrup==0)
            memoria[cache3[i].endBloco] = cache3[i].bloco;
    }
    for(int i=0; i<tram; i++){
        if(memoriaRam[i].atualiza == true)
            memoria[memoriaRam[i].endBloco] = memoriaRam[i].bloco;
    }
}


void imprimirHamb(BlocoHD *memoriaDados){
    cout << setw(22 )<< "_______________Hamburguer_______________" << endl;
    cout << setw(22) << memoriaDados[13].palavras[0] << endl;

    for(int i=1; i<4; i++){
        if(memoriaDados[13].palavras[i] != "/0")
            cout << setw(22) << memoriaDados[13].palavras[i] << endl;
    }

    for(int i=0; i<4; i++){
        if(memoriaDados[14].palavras[i] != "/0")
            cout << setw(22) << memoriaDados[14].palavras[i] << endl;
    }

    cout << setw(22) << memoriaDados[13].palavras[0] << endl;
}


//FUNÇÕES INTERRUPÇÃO
void interrupcao(LinhaCache *cache1, LinhaCache *cache2, LinhaCache *cache3, BlocoMP *memoriaRam){
    /*Opcodes:
         -1 Mostrar Hamburguer montando e sair
         0 Selecionar um ingrediente e armazenar em regIng
         1 Inserir ingrediente do regIng no hamburguer
         2 Retirar ingrediente do hambuguer e armazenar em regIng
    */
    Instrucao inst[50];
    criarInstrucoes(inst);

    BlocoHD memoriaInt[tamDados];
    criarMemHD(memoriaInt);

    int opcode;
    int endBloco;
    int endPalavra;
    string RegIng = "/0";
    LinhaCache *dados;

    int i = 0; // Contadores
    do{
        opcode = inst[i].opcode;
        endBloco = inst[i].endBloco;
        endPalavra = inst[i].endPalvra;

        i++;
        if(opcode != -1){
            dados = completamenteAss(1, endBloco, cache1, cache2, cache3, memoriaRam, memoriaInt);
        }
        switch(opcode){
            case -1:
                //imprimirHamb(memoriaInt);
                //cout << "Volte sempre!!" << endl;
                break;

            case 0:
                RegIng = dados->bloco.palavras[endPalavra];
                break;

            case 1:
                dados->bloco.palavras[endPalavra] = RegIng;
                dados->atualiza = true;
                break;

            case 2:
                RegIng = dados->bloco.palavras[endPalavra];
                dados->bloco.palavras[endPalavra] = "/0";
                dados->atualiza = true;
                break;

            default:
                cout << "ERRO!" << endl;
        }

    }while(opcode != -1);
}


void criarInstrucoes(Instrucao *inst){
    srand(time(NULL));

    for(int i=0; i<50-1; i++){
        inst[i].opcode = rand() % 3;
        if(inst[i].opcode != 0){
            inst[i].endBloco =(rand()%2)+13;
            if(inst[i].endBloco == 13)
                inst[i].endPalvra = (rand()%3)+1;
            else
                inst[i].endPalvra = rand()%4;
        }
        inst[i].endBloco = rand()%13;
        inst[i].endPalvra = rand()%4;
    }
    inst[50-1].opcode = -1;

    /*cout << "Interrupcao" << endl;
    for(int i=0; i<50; i++){
        cout << "OP " << inst[i].opcode << endl;
        cout << "Bl " << inst[i].endbloco << endl;
        cout << "Pa " << inst[i].endPalavra << endl;
        cout << endl;
    }*/
}
