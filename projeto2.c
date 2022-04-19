#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define IDINVAL "%s: no such airport ID\n"
#define MAXID 4
#define MAXPAIS 31
#define MAXCIDADE 51
#define MAXCODIGO_VOO 7
#define MAXDATA 11
#define MAXHORA 6
#define MAXDURACAO 6
#define MAXCAPACIDADE 4
#define MAXVOOS 30000
#define MAXAIRPORTS 40
#define MAXINPUT 65535

typedef struct {
    int Dia;
    int Mes;
    int Ano;
} data;

typedef struct {
    int Horas;
    int Minutos;
} hora;

typedef struct{
    char* Codigo_reserva;
    int Num_passageiros_reserva;
} reserva;

typedef struct nohs{
    reserva reservas;
    struct nohs* prox;
} *Ligacao;


typedef struct{
    char Codigo_voo[MAXCODIGO_VOO];
    char IDpartida[MAXID];
    char IDchegada[MAXID];
    data Data;
    hora Horas;
    hora Duracao;
    int Capacidade;
    Ligacao reservas_list;
    int Num_passageiros;
    int Num_reservas;
} voo;

typedef struct {
    char ID[MAXID];      
    char cidade[MAXCIDADE];           
    char pais[MAXPAIS];
    int N_voos;
} sistema;


int contador_aeroportos = 0;
int contador_voos = 0;
sistema aeroportos[MAXAIRPORTS] = {0};
voo voos[MAXVOOS] = {0};
data Data = {01,01,2022};
hora Hora = {0};

int adiciona_aeroportos();
int ordena_aeroportos();
int adiciona_ou_lista_voos();
int lista_voos_por_ordem_de_partida();
int lista_voos_por_ordem_de_chegada();
int muda_data();
int adiciona_ou_lista_reservas();
int elimina_voos_ou_reserva();

/*Devolve 1 se Hora1 > Hora2 ou 0 se Hora1 < Hora2*/
int Hora_Maior(hora Hora1, hora Hora2){
    if(Hora1.Horas == Hora2.Horas && Hora1.Minutos > Hora2.Minutos){
        return 1;
    }
    else if(Hora1.Horas > Hora2.Horas){
        return 1;
    }
    else{
        return 0;
    }
    return 0;
}

/*Devolve 1 se Data1 > Data2 ou 0 se Data1 < Data 2*/
int Data_Maior(data Data1, data Data2, hora Hora1, hora Hora2){
    if (Data1.Dia > Data2.Dia && Data1.Mes == Data2.Mes && 
            Data1.Ano == Data2.Ano){
        return 1;
    }
    else if(Data1.Mes > Data2.Mes && Data1.Ano == Data2.Ano){
        return 1;
    }
    else if(Data1.Ano > Data2.Ano){
        return 1;
    }
    else if (Data1.Dia == Data2.Dia && Data1.Mes == Data2.Mes && 
                Data1.Ano == Data2.Ano){
        if (Hora_Maior(Hora1, Hora2) == 1){
            return 1;
        }
        else{
            return 0;
        }
    }
    else{
        return 0;
    }
    return 0;
}


/*Funcao ordena os aeroportos por ordem alfabetica de IDs*/
void insertionSort_aeroportos(){
    int i, j;
    sistema chave;
    for (i = 1; i < contador_aeroportos; i++) {
        chave = aeroportos[i];
        j = i - 1;
        while (j >= 0 && strcmp(aeroportos[j].ID,chave.ID) > 0){
            aeroportos[j + 1] = aeroportos[j];
            j--;
        }
        aeroportos[j + 1] = chave;
    }
}

/*Funcao ordena os voos de um unico aeroporto por data antigos -> recentes*/
void insertionSort_Voos_Data(voo *voos_p_ord, int n){
    int i, j;
    voo chave;
    for (i = 1; i < n; i++) {
        chave = voos_p_ord[i];
        j = i - 1;
        while(j >= 0 && Data_Maior(voos_p_ord[j].Data,chave.Data, 
            voos_p_ord[j].Horas,chave.Horas) != 0){
            voos_p_ord[j + 1] = voos_p_ord[j];
            j--;
        }
        voos_p_ord[j + 1] = chave;
    }
}

/*retorna 0 se data invalida e 1 se valida*/
int verificar_data(int Dia, int Mes, int Ano){
    if (Data.Ano > Ano || (Data.Ano == Ano && Data.Mes > Mes) || 
        (Data.Ano == Ano && Data.Mes == Mes && Data.Dia > Dia)){
        return 0;
    }
    else if((Data.Ano < Ano - 1) || (Data.Ano == Ano - 1 && Data.Mes < Mes) || 
            (Data.Ano == Ano - 1 && Data.Mes == Mes && Data.Dia < Dia)){
        return 0;
    }
    else{
        return 1;
    }
}

/* Soma a Hora de Partida com a Duracao*/
void Avancar_Hora(hora *HoraP, hora *Duracao){
    HoraP->Minutos += Duracao->Minutos; 
    HoraP->Horas += Duracao->Horas;
    if(HoraP->Minutos >= 60){
        HoraP->Horas += 1;
        HoraP->Minutos -= 60;  
    }  
}

/*verifica se a hora/minutos sao menores que 24/60 e se o dia existe no mes 
    correspondente, se isso nao se verificar corrije a data */
void corrige_Data(data *DataP, hora *HoraP){
    if(HoraP->Horas >= 24){
        HoraP->Horas -= 24;
        DataP->Dia += 1;
        if(DataP->Mes == 2 && DataP->Dia > 28){
            DataP->Mes++;
            DataP->Dia -= 28; 
        }
        else if(DataP->Mes == 12 && DataP->Dia > 31){
            DataP->Mes = 1;
            DataP->Dia -= 31;
            DataP->Ano++;
        }
        else if(DataP->Mes < 7 && DataP->Mes % 2 == 0 && DataP->Dia > 30){
            DataP->Mes++;
            DataP->Dia -= 30; 
        }
        else if(DataP->Mes > 7 && DataP->Mes % 2 != 0 && DataP->Dia > 30){
            DataP->Mes++;
            DataP->Dia -= 30; 
        }
        else if(DataP->Dia > 31){
            DataP->Mes++;
            DataP->Dia -= 31; 
        }
    }
}


/*Funcao para acabar programa caso o sistema operativo nao tenha 
mais memoria disponivel*/
void fail(){
    printf("No memory.\n");
    exit(EXIT_FAILURE);
}

int main()
{   
    char cmd;
    int i;
    cmd = getchar();
    if (cmd == 'q'){
        for(i = 0; i < contador_voos; i++){
            free(voos[i].reservas_list);
        exit(0);
        }
    }
    else if (cmd == 'a'){
        adiciona_aeroportos(cmd);
        main();
    }

    else if (cmd == 'l'){
        ordena_aeroportos(cmd);
        main();   
    }
    
    else if (cmd == 'v'){
        adiciona_ou_lista_voos(cmd);
        main();
    }
    else if (cmd == 'p'){
        lista_voos_por_ordem_de_partida(cmd);
        main();
    }
    
    else if (cmd == 'c'){
        lista_voos_por_ordem_de_chegada(cmd);
        main();
    }
    else if (cmd == 't'){
        muda_data(cmd);
        main();
    }
    
    else if (cmd == 'r'){
        adiciona_ou_lista_reservas();
        main();
    }
    else if (cmd == 'e'){
        elimina_voos_ou_reserva();
        main();
    }
    else{
        main();
    }
    return 0;
}

/*Funcao verifica a validade do input e adiciona aeroportos ao sistema*/
int adiciona_aeroportos(){
    char id[MAXID], pais[MAXPAIS], cidade[MAXCIDADE];
    int i;
    scanf(" %s %s %[^\n]%*c", id, pais, cidade); 
    for(i = 0; i < 3; i++){ /*scanf da scan ate encontrar um \n*/
        if (!(isupper(id[i]))){
            printf("invalid airport ID\n");
            return 0;
        }
    }
    if (contador_aeroportos == 40){
        printf("too many airports\n");
        return 0;
    }
    else if(contador_aeroportos != 0){
        for (i = 0; i <= contador_aeroportos; i++){
            if (strcmp(aeroportos[i].ID, id) == 0){
                printf("duplicate airport\n");
                return 0;
            }
        }
    }
    strcpy(aeroportos[contador_aeroportos].ID, id);
    strcpy(aeroportos[contador_aeroportos].pais, pais);
    strcpy(aeroportos[contador_aeroportos].cidade, cidade);
    printf("airport %s\n", aeroportos[contador_aeroportos].ID);
    contador_aeroportos++;
    return 0;
        
            
}
/*Funcao lista os aeroportos por ordem alfabetica ou por ordem de Input*/ 
int ordena_aeroportos(){
    char caracter = ' ', ID[MAXID];
    int  i, index, verificador; 
    if(getchar() == '\n'){
        insertionSort_aeroportos();
        for(i = 0; i < contador_aeroportos; i++){
            printf("%s %s %s %d\n", aeroportos[i].ID, 
            aeroportos[i].cidade, aeroportos[i].pais, aeroportos[i].N_voos);
        }
        return 0;
    }
    else{
        while(caracter == ' '){
            scanf("%s%c", ID, &caracter);
            verificador = 0;
            for(i = 0; i < contador_aeroportos; i++){
                if(strcmp(ID,aeroportos[i].ID) == 0){
                    verificador++;
                    index = i;
                }
            }
            if(verificador == 0){
                printf(IDINVAL, ID);
                continue;
            }
            printf("%s %s %s %d\n", aeroportos[index].ID, 
                aeroportos[index].cidade, aeroportos[index].pais, 
                aeroportos[index].N_voos);
        }
        return 0;
    }
} 

/*Funcao adiciona voos ao sistema ou imprime uma lista com todos os voos
        no fim do for loop, se Verificador_IDs = 3, existem os 2 IDs ,
                se Verificadores_IDs = 2 não existe IDpartida
                se Verificadores_IDs = 1 não existe IDchegada
                se Verificadores_IDs = 0 não existem IDs*/
int adiciona_ou_lista_voos(){
    char Codigo_voo[MAXCODIGO_VOO], IDpartida[MAXID], IDchegada[MAXID];
    int i, Verificador_IDs = 0, Dia, Mes, Ano, Horas_data, Minutos_data;       
    int Horas_Dur, Minutos_Dur, Capacidade;
    if(getchar() == '\n'){
        for(i = 0; i < contador_voos; i++){
            printf("%s %s %s %02d-%02d-%02d %02d:%02d\n", voos[i].Codigo_voo, 
                voos[i].IDpartida, voos[i].IDchegada, voos[i].Data.Dia, 
                voos[i].Data.Mes, voos[i].Data.Ano, voos[i].Horas.Horas, 
                voos[i].Horas.Minutos);
        }
        return 0;
    }
    else{
        scanf("%s %s %s %d-%d-%d %d:%d %d:%d %d", Codigo_voo, IDpartida, 
            IDchegada, &Dia, &Mes, &Ano, &Horas_data, &Minutos_data, 
            &Horas_Dur, &Minutos_Dur, &Capacidade);
        if(!(isupper(Codigo_voo[0]) && isupper(Codigo_voo[1]) && 
            isdigit(Codigo_voo[2]) && Codigo_voo[2] > 0 + '0')){
            printf("invalid flight code\n");
            return 0;
        }
        for(i = 3; Codigo_voo[i] != '\0'; i++){
            if (!(10 + '0' > Codigo_voo[i] && Codigo_voo[i] >= '0' + 0)){
                printf("invalid flight code\n");
                return 0;
            }
        }
        for(i = 0; i < contador_voos; i++){
            if (strcmp(Codigo_voo, voos[i].Codigo_voo) == 0 && 
                Dia == voos[i].Data.Dia && Mes == voos[i].Data.Mes && 
                Ano == voos[i].Data.Ano){
                printf("flight already exists\n");
                return 0;
            }
        }
        for(i = 0; i < contador_aeroportos; i++){
            if (strcmp(IDpartida, aeroportos[i].ID) == 0){ 
                Verificador_IDs += 1; 
            }                   
            if(strcmp(IDchegada, aeroportos[i].ID) == 0){
                Verificador_IDs += 2;
            }                                                   
        }                                                           
        if (Verificador_IDs == 2 || Verificador_IDs == 0){          
            printf(IDINVAL, IDpartida);            
            return 0;
        }
        else if (Verificador_IDs == 1){
            printf(IDINVAL, IDchegada);
            return 0;
        }
        else if (contador_voos >= MAXVOOS){
            printf("too many flights\n");
            return 0;
        }
        else if (verificar_data(Dia,Mes,Ano) == 0){
            printf("invalid date\n");
            return 0;
        }
        else if ((Horas_Dur < 0) || (Horas_Dur == 12 && Minutos_Dur > 0)
                || (Horas_Dur > 12)){
            printf("invalid duration\n");
            return 0;
        }
        else if (Capacidade < 10){
            printf("invalid capacity\n");
            return 0;    
        }
        else{
            strcpy(voos[contador_voos].Codigo_voo, Codigo_voo);
            strcpy(voos[contador_voos].IDpartida, IDpartida);
            strcpy(voos[contador_voos].IDchegada, IDchegada);
            voos[contador_voos].Data.Ano = Ano;
            voos[contador_voos].Data.Mes = Mes;
            voos[contador_voos].Data.Dia = Dia;
            voos[contador_voos].Horas.Horas = Horas_data;
            voos[contador_voos].Horas.Minutos = Minutos_data;
            voos[contador_voos].Duracao.Horas = Horas_Dur;
            voos[contador_voos].Duracao.Minutos = Minutos_Dur;
            voos[contador_voos].Capacidade = Capacidade;
            for(i = 0; i < contador_aeroportos; i++){
                if(strcmp(aeroportos[i].ID,IDpartida) == 0){
                    aeroportos[i].N_voos += 1;
                }
            }
            contador_voos++;
        }
    }
    return 0; 
}

/*Imprime os voos por ordem de data/hora de partida de um ID dado no Input*/
int lista_voos_por_ordem_de_partida(){
    int i, Verificador_IDs = 0, contador = 0;
    char ID[MAXID];
    voo voos_p_ord[MAXVOOS] = {0};
    scanf("%s", ID);
    for(i = 0; i < contador_aeroportos; i++){
                if(strcmp(ID,aeroportos[i].ID) == 0){
                    Verificador_IDs++;
                }
            }
    if (Verificador_IDs == 0)
    {
        printf(IDINVAL,ID);
        return 0;
    }
    else{ 
        for(i = 0; i < contador_voos; i++){
            if(strcmp(voos[i].IDpartida, ID) == 0){
                voos_p_ord[contador] = voos[i];
                contador++;
            }
        }
        insertionSort_Voos_Data(voos_p_ord, contador);
        for(i = 0; i < contador; i++){
            printf("%s %s %02d-%02d-%d %02d:%02d\n", voos_p_ord[i].Codigo_voo, 
                voos_p_ord[i].IDchegada, voos_p_ord[i].Data.Dia, 
                voos_p_ord[i].Data.Mes, voos_p_ord[i].Data.Ano,
                voos_p_ord[i].Horas.Horas,voos_p_ord[i].Horas.Minutos);
        }
    }
    return 0;
}

/*Imprime os voos por ordem de data/hora de chegada de um ID dado no Input*/
int lista_voos_por_ordem_de_chegada(){
    int i, Verificador_IDs = 0, contador = 0;
    char ID[MAXID];
    voo voos_p_ord[MAXVOOS] = {0};
    scanf("%s", ID);
    for(i = 0; i < contador_aeroportos; i++){
                if(strcmp(ID,aeroportos[i].ID) == 0){
                    Verificador_IDs++;
                }
            }
    if (Verificador_IDs == 0)
    {
        printf(IDINVAL, ID);
        return 0;
    }
    else{ 
        for(i = 0; i < contador_voos; i++){
            if(strcmp(voos[i].IDchegada, ID) == 0){
                voos_p_ord[contador] = voos[i];
                contador++;
            }
        }
        for(i = 0; i < contador; i++){
            Avancar_Hora(&voos_p_ord[i].Horas, &voos_p_ord[i].Duracao);
            corrige_Data(&voos_p_ord[i].Data,&voos_p_ord[i].Horas);
        }
        insertionSort_Voos_Data(voos_p_ord, contador);
        for(i = 0; i < contador; i++){
            printf("%s %s %02d-%02d-%d %02d:%02d\n", voos_p_ord[i].Codigo_voo, 
                voos_p_ord[i].IDpartida, voos_p_ord[i].Data.Dia, 
                voos_p_ord[i].Data.Mes, voos_p_ord[i].Data.Ano,
                voos_p_ord[i].Horas.Horas,voos_p_ord[i].Horas.Minutos);
        }
    }
    return 0;
}

/*Funcao altera a data do sistema*/
int muda_data(){
    int Novo_Dia, Novo_Mes, Novo_Ano;
    scanf("%d-%d-%d", &Novo_Dia, &Novo_Mes, &Novo_Ano);
    if (verificar_data(Novo_Dia, Novo_Mes, Novo_Ano) == 0){
        printf("invalid date\n");
        return 0;
    }
    else{
        Data.Dia = Novo_Dia;
        Data.Mes = Novo_Mes;
        Data.Ano = Novo_Ano;
        printf("%02d-%02d-%02d\n", Novo_Dia, Novo_Mes, Novo_Ano);
        return 0;
    }
}

/*Cria um novo Noh do tipo Ligacao*/
Ligacao Novo_noh(reserva buffer)
{
    Ligacao x = (Ligacao) malloc(sizeof(struct nohs));
    if(x == NULL){
        fail();
    }
    x->reservas = buffer;
    x->prox = NULL;
    return x;
}

/*adiciona o noh criado pela funcao Novo_noh ah linked listed do input*/
Ligacao adiciona_noh_inicio(Ligacao head, reserva Nova_reserva)
{
    Ligacao x = Novo_noh(Nova_reserva);
    x->prox = head;
    return x;
}

/*apaga o node com o codigo de reserva iguasl ao codigo do input*/
struct nohs* delete(char Codigo[], Ligacao inicio){

    Ligacao atual = inicio;
    Ligacao anterior = NULL;
    if(inicio == NULL) {
        return NULL;
    }

    while(strcmp(atual->reservas.Codigo_reserva, Codigo) != 0){
        if(atual->prox == NULL) {
            return NULL;
        } else {
            anterior = atual;
            atual = atual->prox;
        }
    }
    if(atual == inicio) {
        inicio = inicio->prox;
    } 
    else {
        anterior->prox = atual->prox;
    }   
    return inicio;
}

/*Ordena lista de reservas por ordem lexicografica*/
void insertionSort_Voos_reservas(reserva reservas_p_ord[], int Num_reservas){
    int i, j;
    reserva chave;
    for (i = 1; i < Num_reservas; i++) {
        chave = reservas_p_ord[i];
        j = i - 1;
        while(j >= 0 && strcmp(reservas_p_ord[j].Codigo_reserva, 
            chave.Codigo_reserva) > 0){
            reservas_p_ord[j + 1] = reservas_p_ord[j];
            j--;
        }
        reservas_p_ord[j + 1] = chave;
    }
}


/*Transforma a linked list numa array*/
reserva* linked_list_to_array(struct nohs* linked_list, int Num_reservas){
    int i = 0;
    struct nohs* iterador = linked_list;
    reserva* array = (reserva*)malloc(sizeof(reserva)*Num_reservas);
    if(array == NULL){
        fail();
    }
    for(;iterador != NULL; iterador = iterador->prox){
        array[i] = iterador->reservas;
        i++;
    }
    return array;
}


/* Funcao Lista ou adiciona uma reserva a um voo */
int adiciona_ou_lista_reservas(){
    char carater, Codigo_voo[MAXCODIGO_VOO], Codigo_reserva[MAXINPUT];
    int Numero_de_pessoas, Dia, Mes, Ano, i, j, Length_codigo_reserva,
    ind_voo = -1;
    reserva aux;
    reserva* linkedlist_array;
    struct nohs* iterador;
    scanf("%s %d-%d-%d%c", Codigo_voo, &Dia, &Mes, &Ano, &carater);
    for(i = 0; i < contador_voos; i++){
        if(strcmp(Codigo_voo, voos[i].Codigo_voo)==0 && Dia == voos[i].Data.Dia 
                && Mes == voos[i].Data.Mes && Ano == voos[i].Data.Ano){
            ind_voo = i;
            break;
        }
    }
    if(ind_voo == -1){
        printf("%s: flight does not exist\n", Codigo_voo);
        return 0;
    }
    else if(verificar_data(Dia, Mes, Ano) == 0){
        printf("invalid date\n");
        return 0;
    }
    else if(carater == ' '){ /*Se houver input para alem da data/codigo_voo*/
        scanf("%s %d", Codigo_reserva, &Numero_de_pessoas);
        Length_codigo_reserva = strlen(Codigo_reserva);
        aux.Codigo_reserva=(char*)malloc(sizeof(char)*(Length_codigo_reserva+1));
        if(aux.Codigo_reserva == NULL){
            fail();
        }
        strcpy(aux.Codigo_reserva, Codigo_reserva);
        aux.Num_passageiros_reserva = Numero_de_pessoas;
        for(i = 0; i < Length_codigo_reserva; i++){
            if(Length_codigo_reserva < 10 || (isdigit(Codigo_reserva[i]) == 0 &&
             isupper(Codigo_reserva[i]) == 0)){
                printf("invalid reservation code\n");
                return 0;
            }
        }
        for(i = 0; i < contador_voos; i++){
            for(iterador = voos[i].reservas_list;iterador != NULL;
                 iterador = iterador->prox){
                if(strcmp(iterador->reservas.Codigo_reserva,Codigo_reserva)==0){
                    printf("%s: flight reservation already used\n", 
                        Codigo_reserva);
                    return 0;
                }
                
            }
        }
        
        if(voos[ind_voo].Num_passageiros + Numero_de_pessoas >
             voos[ind_voo].Capacidade){
            printf("too many reservations\n");
            return 0;
        }
        else if(Numero_de_pessoas <= 0){
            printf("invalid passenger number\n");
            return 0;
        }
        voos[ind_voo].Num_passageiros += Numero_de_pessoas;
        voos[ind_voo].reservas_list = adiciona_noh_inicio(voos[ind_voo].reservas_list, aux);
        voos[ind_voo].Num_reservas++;
            
    }
    else{/*Se nao houver input para alem da data/codigo_voo*/
        linkedlist_array = linked_list_to_array(voos[ind_voo].reservas_list, 
            voos[ind_voo].Num_reservas);
        insertionSort_Voos_reservas(linkedlist_array,voos[ind_voo].Num_reservas);
        if(voos[ind_voo].Num_reservas > 0){
            for(j = 0; j < voos[ind_voo].Num_reservas; j++){
                printf("%s %d\n", linkedlist_array[j].Codigo_reserva, 
                    linkedlist_array[j].Num_passageiros_reserva);
            }
        }
        free(linkedlist_array);
    }
    return 0;
}


/*Funcao apaga i voo com o Codigo de voo = Codigo[] do input*/
void apagar_voo(char Codigo[], char ID_partida[]){
    int i, j, k, Indice_voo = 0;
    struct nohs* iterador;
    for(i = 0; i < contador_voos; i++){
        if(strcmp(Codigo, voos[i].Codigo_voo) == 0){
            Indice_voo = i;
            for(iterador = voos[Indice_voo].reservas_list;iterador != NULL; 
                iterador = iterador->prox){
                delete(iterador->reservas.Codigo_reserva,
                    voos[Indice_voo].reservas_list);
                voos[Indice_voo].Num_reservas--;
            }
            for(k = Indice_voo; k < contador_voos; k++){
                voos[k] = voos[k+1];
            }
            for(j = 0; j < contador_aeroportos; j++){
                if(strcmp(ID_partida, aeroportos[j].ID) == 0){
                    aeroportos[j].N_voos--;
                }
            }
            contador_voos--;
            i--;
        }
    }
    
}

/* Elimina voos ou uma reserva de acordo com o codigo tirado do utilizador*/
int elimina_voos_ou_reserva(){
    char Codigo[MAXINPUT];    
    int  Indice_voo = 0, i, verificador = 0;
    struct nohs* iterador;
    scanf("%s", Codigo);

    for(i = 0; i < contador_voos; i++){
        if((strcmp(Codigo, voos[i].Codigo_voo) == 0)){
            verificador++;/*Se o codigo for de um ou mais voo*/
        }
        if(voos[i].Num_reservas > 0){
            for(iterador = voos[i].reservas_list;iterador != NULL;iterador = iterador->prox){
                if(strcmp(iterador->reservas.Codigo_reserva, Codigo) == 0){
                    verificador = -5;/*Se for encontrado o codigo de reserva*/
                    Indice_voo = i;
                }
            }
        }
    }
    if(verificador == 0){
        printf("not found\n");
        return 0;
    }
    else if(verificador == -5){
        voos[Indice_voo].Num_reservas--;
        voos[Indice_voo].Num_passageiros -= voos[Indice_voo].reservas_list->reservas.Num_passageiros_reserva;
        voos[Indice_voo].reservas_list = delete(Codigo, voos[Indice_voo].reservas_list);
        return 0;
    }
    else{
        apagar_voo(Codigo, voos[Indice_voo].IDpartida);
    }
    return 0;
}
