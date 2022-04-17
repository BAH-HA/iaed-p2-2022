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
    char Codigo_voo[30000][MAXCODIGO_VOO];
    char IDpartida[30000][MAXID];
    char IDchegada[30000][MAXID];
    data Data[30000];
    hora Horas[30000];
    hora Duracao[30000];
    int Capacidade[30000];
    Ligacao reservas_list[30000];
    int Num_passageiros[30000];
    int Num_reservas[30000];
} voo;

typedef struct {
    char ID[40][MAXID];      
    char cidade[40][MAXCIDADE];           
    char pais[40][MAXPAIS];
    int N_voos[40];
} sistema;


int contador_aeroportos = 0;
int contador_voos = 0;
sistema aeroportos = {0};
voo voos = {0};
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
    int i, j, chave_N_voos;
    char chave[MAXID], chave_cidade[MAXCIDADE], chave_pais[MAXPAIS];
    for (i = 1; i < contador_aeroportos; i++) {
        strcpy(chave,aeroportos.ID[i]);
        strcpy(chave_cidade, aeroportos.cidade[i]);
        strcpy(chave_pais,aeroportos.pais[i]);
        chave_N_voos = aeroportos.N_voos[i];
        j = i - 1;
        while (j >= 0 && strcmp(aeroportos.ID[j],chave) > 0){
            strcpy(aeroportos.ID[j + 1], aeroportos.ID[j]);
            strcpy(aeroportos.cidade[j + 1], aeroportos.cidade[j]);
            strcpy(aeroportos.pais[j + 1], aeroportos.pais[j]);
            aeroportos.N_voos[j + 1] = aeroportos.N_voos[j];
            j--;
        }
        strcpy(aeroportos.ID[j + 1], chave);
        strcpy(aeroportos.cidade[j + 1], chave_cidade);
        strcpy(aeroportos.pais[j + 1], chave_pais);
        aeroportos.N_voos[j + 1] = chave_N_voos;
    }
}

/*Funcao ordena os voos de um unico aeroporto por data antigos -> recentes*/
void insertionSort_Voos_Data(voo *voos_p_ord, int n){
    int i, j, chave_Capacidade;
    data chave_Data = {0};
    hora chave_Horas = {0};
    char chave_Codigo_voo[MAXCODIGO_VOO], chave_IDpartida[MAXID];
    char chave_IDchegada[MAXID];
    for (i = 1; i < n; i++) {
        strcpy(chave_Codigo_voo, voos_p_ord->Codigo_voo[i]);
        strcpy(chave_IDpartida, voos_p_ord->IDpartida[i]);
        strcpy(chave_IDchegada, voos_p_ord->IDchegada[i]);
        chave_Capacidade = voos_p_ord->Capacidade[i];
        chave_Data = voos_p_ord->Data[i];
        chave_Horas = voos_p_ord->Horas[i];
        j = i - 1;
        while(j >= 0 && Data_Maior(voos_p_ord->Data[j],chave_Data,
                voos_p_ord->Horas[j], chave_Horas) != 0){
            strcpy(voos_p_ord->Codigo_voo[j + 1], voos_p_ord->Codigo_voo[j]);
            strcpy(voos_p_ord->IDpartida[j + 1], voos_p_ord->IDpartida[j]);
            strcpy(voos_p_ord->IDchegada[j + 1], voos_p_ord->IDchegada[j]);
            voos_p_ord->Capacidade[j + 1] = voos_p_ord->Capacidade[j];
            voos_p_ord->Data[j + 1] = voos_p_ord->Data[j];
            voos_p_ord->Horas[j + 1] = voos_p_ord->Horas[j];
            j--;
        }
        strcpy(voos_p_ord->Codigo_voo[j + 1], chave_Codigo_voo);
        strcpy(voos_p_ord->IDpartida[j + 1], chave_IDpartida);
        strcpy(voos_p_ord->IDchegada[j + 1], chave_IDchegada);
        voos_p_ord->Capacidade[j + 1] = chave_Capacidade;
        voos_p_ord->Data[j + 1] = chave_Data;
        voos_p_ord->Horas[j + 1] = chave_Horas;
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
    printf("No memory.");
    exit(EXIT_FAILURE);
}

int main()
{   
    char cmd;
    cmd = getchar();
    if (cmd == 'q'){
        exit(0);
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
            if (strcmp(aeroportos.ID[i], id) == 0){
                printf("duplicate airport\n");
                return 0;
            }
        }
    }
    strcpy(aeroportos.ID[contador_aeroportos], id);
    strcpy(aeroportos.pais[contador_aeroportos], pais);
    strcpy(aeroportos.cidade[contador_aeroportos], cidade);
    printf("airport %s\n", aeroportos.ID[contador_aeroportos]);
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
            printf("%s %s %s %d\n", aeroportos.ID[i], 
            aeroportos.cidade[i], aeroportos.pais[i], aeroportos.N_voos[i]);
        }
        return 0;
    }
    else{
        while(caracter == ' '){
            scanf("%s%c", ID, &caracter);
            verificador = 0;
            for(i = 0; i < contador_aeroportos; i++){
                if(strcmp(ID,aeroportos.ID[i]) == 0){
                    verificador++;
                    index = i;
                }
            }
            if(verificador == 0){
                printf(IDINVAL, ID);
                continue;
            }
            printf("%s %s %s %d\n", aeroportos.ID[index], 
                aeroportos.cidade[index], aeroportos.pais[index], 
                aeroportos.N_voos[index]);
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
            printf("%s %s %s %02d-%02d-%02d %02d:%02d\n", voos.Codigo_voo[i], 
                voos.IDpartida[i], voos.IDchegada[i], voos.Data[i].Dia, 
                voos.Data[i].Mes, voos.Data[i].Ano, voos.Horas[i].Horas, 
                voos.Horas[i].Minutos);
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
            if (strcmp(Codigo_voo, voos.Codigo_voo[i]) == 0 && 
                Dia == voos.Data[i].Dia && Mes == voos.Data[i].Mes && 
                Ano == voos.Data[i].Ano){
                printf("flight already exists\n");
                return 0;
            }
        }
        for(i = 0; i < contador_aeroportos; i++){
            if (strcmp(IDpartida, aeroportos.ID[i]) == 0){ 
                Verificador_IDs += 1; 
            }                   
            if(strcmp(IDchegada, aeroportos.ID[i]) == 0){
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
            strcpy(voos.Codigo_voo[contador_voos], Codigo_voo);
            strcpy(voos.IDpartida[contador_voos], IDpartida);
            strcpy(voos.IDchegada[contador_voos], IDchegada);
            voos.Data[contador_voos].Ano = Ano;
            voos.Data[contador_voos].Mes = Mes;
            voos.Data[contador_voos].Dia = Dia;
            voos.Horas[contador_voos].Horas = Horas_data;
            voos.Horas[contador_voos].Minutos = Minutos_data;
            voos.Duracao[contador_voos].Horas = Horas_Dur;
            voos.Duracao[contador_voos].Minutos = Minutos_Dur;
            voos.Capacidade[contador_voos] = Capacidade;
            for(i = 0; i < contador_aeroportos; i++){
                if(strcmp(aeroportos.ID[i],IDpartida) == 0){
                    aeroportos.N_voos[i] += 1;
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
    voo voos_p_ord = {0};
    scanf("%s", ID);
    for(i = 0; i < contador_aeroportos; i++){
                if(strcmp(ID,aeroportos.ID[i]) == 0){
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
            if(strcmp(voos.IDpartida[i], ID) == 0){
                strcpy(voos_p_ord.Codigo_voo[contador], voos.Codigo_voo[i]);
                strcpy(voos_p_ord.IDpartida[contador], voos.IDpartida[i]);
                strcpy(voos_p_ord.IDchegada[contador], voos.IDchegada[i]);
                voos_p_ord.Data[contador].Ano = voos.Data[i].Ano;
                voos_p_ord.Data[contador].Mes = voos.Data[i].Mes;
                voos_p_ord.Data[contador].Dia = voos.Data[i].Dia; 
                voos_p_ord.Horas[contador].Horas = voos.Horas[i].Horas;
                voos_p_ord.Horas[contador].Minutos = voos.Horas[i].Minutos;
                voos_p_ord.Duracao[contador].Horas = voos.Duracao[i].Horas;
                voos_p_ord.Duracao[contador].Minutos = voos.Duracao[i].Minutos;
                voos_p_ord.Capacidade[contador] = voos.Capacidade[i];
                contador++;
            }
        }
        insertionSort_Voos_Data(&voos_p_ord, contador);
        for(i = 0; i < contador; i++){
            printf("%s %s %02d-%02d-%d %02d:%02d\n", voos_p_ord.Codigo_voo[i], 
                voos_p_ord.IDchegada[i], voos_p_ord.Data[i].Dia, 
                voos_p_ord.Data[i].Mes, voos_p_ord.Data[i].Ano,
                voos_p_ord.Horas[i].Horas,voos_p_ord.Horas[i].Minutos);
        }
    }
    return 0;
}

/*Imprime os voos por ordem de data/hora de chegada de um ID dado no Input*/
int lista_voos_por_ordem_de_chegada(){
    int i, Verificador_IDs = 0, contador = 0;
    char ID[MAXID];
    voo voos_p_ord = {0};
    scanf("%s", ID);
    for(i = 0; i < contador_aeroportos; i++){
                if(strcmp(ID,aeroportos.ID[i]) == 0){
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
            if(strcmp(voos.IDchegada[i], ID) == 0){
                strcpy(voos_p_ord.Codigo_voo[contador], voos.Codigo_voo[i]);
                strcpy(voos_p_ord.IDpartida[contador], voos.IDpartida[i]);
                strcpy(voos_p_ord.IDchegada[contador], voos.IDchegada[i]);
                voos_p_ord.Data[contador].Ano = voos.Data[i].Ano;
                voos_p_ord.Data[contador].Mes = voos.Data[i].Mes;
                voos_p_ord.Data[contador].Dia = voos.Data[i].Dia; 
                voos_p_ord.Horas[contador].Horas = voos.Horas[i].Horas;
                voos_p_ord.Horas[contador].Minutos = voos.Horas[i].Minutos;
                voos_p_ord.Duracao[contador].Horas = voos.Duracao[i].Horas;
                voos_p_ord.Duracao[contador].Minutos = voos.Duracao[i].Minutos;
                voos_p_ord.Capacidade[contador] = voos.Capacidade[i];
                contador++;
            }
        }
        for(i = 0; i < contador; i++){
            Avancar_Hora(&voos_p_ord.Horas[i], &voos_p_ord.Duracao[i]);
            corrige_Data(&voos_p_ord.Data[i],&voos_p_ord.Horas[i]);
        }
        insertionSort_Voos_Data(&voos_p_ord, contador);
        for(i = 0; i < contador; i++){
            printf("%s %s %02d-%02d-%d %02d:%02d\n", voos_p_ord.Codigo_voo[i], 
                voos_p_ord.IDpartida[i], voos_p_ord.Data[i].Dia, 
                voos_p_ord.Data[i].Mes, voos_p_ord.Data[i].Ano,
                voos_p_ord.Horas[i].Horas,voos_p_ord.Horas[i].Minutos);
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

Ligacao Novo_noh(reserva buffer)
{
    Ligacao x = (Ligacao) malloc(sizeof(struct nohs));
    x->reservas = buffer;
    x->prox = NULL;
    return x;
}
Ligacao adiciona_noh_inicio(Ligacao head, reserva Nova_reserva)
{
    Ligacao x = Novo_noh(Nova_reserva);
    x->prox = head;
    return x;
}

void apagar_noh(struct nohs* lista, int num_elementos, char Codigo_reserva[MAXINPUT]){
    struct nohs* aux;
    if(lista == NULL){
        return;
    }
    for(aux = lista;aux != NULL; aux = aux->prox){
        if(strcmp(lista->reservas.Codigo_reserva, Codigo_reserva) == 0){
            aux = lista;
            lista = lista->prox;
            num_elementos--;
            return;
        }
    }
}

void insertionSort_Voos_reservas(reserva reservas_p_ord[], int Num_reservas){
    int i, j, chave_Num_Passageiros;
    char chave_codigo_reserva[MAXINPUT];
    for (i = 1; i < Num_reservas; i++) {
        strcpy(chave_codigo_reserva, reservas_p_ord[i].Codigo_reserva);
        chave_Num_Passageiros = reservas_p_ord[i].Num_passageiros_reserva;
        j = i - 1;
        while(j >= 0 && strcmp(reservas_p_ord[j].Codigo_reserva, chave_codigo_reserva) > 0){
            strcpy(reservas_p_ord[j + 1].Codigo_reserva, reservas_p_ord[j].Codigo_reserva);
            reservas_p_ord[j + 1].Num_passageiros_reserva = reservas_p_ord[j].Num_passageiros_reserva;
            j--;
        }
        strcpy(reservas_p_ord[j + 1].Codigo_reserva, chave_codigo_reserva);
        reservas_p_ord[j + 1].Num_passageiros_reserva = chave_Num_Passageiros;

    }
}


/*Transforma a linked list numa array*/
reserva* linked_list_to_array(struct nohs* linked_list, int Num_reservas){
    int i = 0;
    struct nohs* iterador = linked_list;
    reserva* array = (reserva*)malloc(sizeof(reserva)*Num_reservas);
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
        if(strcmp(Codigo_voo, voos.Codigo_voo[i]) == 0 && Dia == voos.Data[i].Dia && Mes == voos.Data[i].Mes && Ano == voos.Data[i].Ano){
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
        aux.Codigo_reserva = (char*) malloc(sizeof(char)*(Length_codigo_reserva+1));
        strcpy(aux.Codigo_reserva, Codigo_reserva);
        aux.Num_passageiros_reserva = Numero_de_pessoas;
        for(i = 0; i < Length_codigo_reserva; i++){
            if(Length_codigo_reserva < 10 || (isdigit(Codigo_reserva[i]) == 0 && isupper(Codigo_reserva[i]) == 0)){
                printf("invalid reservation code\n");
                return 0;
            }
        }
        for(i = 0; i < contador_voos; i++){
            for(iterador = voos.reservas_list[i];iterador != NULL; iterador = iterador->prox){
                if(strcmp(iterador->reservas.Codigo_reserva, Codigo_reserva) == 0){
                    printf("%s: flight reservation already used\n", Codigo_reserva);
                    return 0;
                }
                
            }
        }
        
        if(voos.Num_passageiros[ind_voo] + Numero_de_pessoas > voos.Capacidade[ind_voo]){
            printf("too many reservations\n");
            return 0;
        }
        else if(Numero_de_pessoas <= 0){
            printf("invalid passenger number\n");
            return 0;
        }
        voos.Num_passageiros[ind_voo] += Numero_de_pessoas;
        voos.reservas_list[ind_voo] = adiciona_noh_inicio(voos.reservas_list[ind_voo], aux);
        voos.Num_reservas[ind_voo]++;
            
    }
    else{/*Se nao houver input para alem da data/codigo_voo*/
        linkedlist_array = linked_list_to_array(voos.reservas_list[ind_voo], voos.Num_reservas[ind_voo]);
        insertionSort_Voos_reservas(linkedlist_array, voos.Num_reservas[ind_voo]);
        if(voos.Num_reservas[ind_voo] > 0){
            for(j = 0; j < voos.Num_reservas[ind_voo]; j++){
                printf("%s %d\n", linkedlist_array[j].Codigo_reserva, linkedlist_array[j].Num_passageiros_reserva);
            }
        }
        free(linkedlist_array);
    }
    return 0;
}

/*Funcao que print linked list*/
void print_linked_list(struct nohs* linkedlist){
    struct nohs* iterador;
    for(iterador = linkedlist; iterador != NULL; iterador = iterador->prox){
        printf("%s %d\n", iterador->reservas.Codigo_reserva, iterador->reservas.Num_passageiros_reserva);
    }
}

void apagar_voo(char Codigo[], char ID_partida[]){
    int i, j, k, Indice_voo = 0;
    struct nohs* iterador;
    for(i = 0; i < contador_voos; i++){
        if(strcmp(Codigo, voos.Codigo_voo[i]) == 0){
            Indice_voo = i;
            for(iterador = voos.reservas_list[Indice_voo];iterador != NULL; iterador = iterador->prox){
                apagar_noh(voos.reservas_list[Indice_voo], voos.Num_reservas[Indice_voo], iterador->reservas.Codigo_reserva);
                voos.Num_reservas[Indice_voo]--;
            }
            for(k = Indice_voo; k < contador_voos; k++){
                strcpy(voos.Codigo_voo[k] ,voos.Codigo_voo[k+1]);
                strcpy(voos.IDpartida[k] ,voos.IDpartida[k+1]);
                strcpy(voos.IDchegada[k] ,voos.IDchegada[k+1]);
                voos.Data[k] = voos.Data[k+1];
                voos.Horas[k] = voos.Horas[k+1];
                voos.Duracao[k] = voos.Duracao[k+1];
                voos.Capacidade[k] = voos.Capacidade[k+1];
                voos.reservas_list[k] = voos.reservas_list[k+1];
                voos.Num_passageiros[k] = voos.Num_passageiros[k+1];
                voos.Num_reservas[k] = voos.Num_reservas[k+1];
            }
            for(j = 0; j < contador_aeroportos; j++){
                if(strcmp(ID_partida, aeroportos.ID[j]) == 0){
                    aeroportos.N_voos[j]--;
                }
            }
            contador_voos--;
            i--;
        }
    }
    
}

/* Elimina voos ou uma reserva*/
int elimina_voos_ou_reserva(){
    char Codigo[MAXINPUT];
    int  Indice_voo = 0, i, verificador = 0;
    struct nohs* iterador;
    scanf("%s", Codigo);
    for(i = 0; i < contador_voos; i++){
        if((strcmp(Codigo, voos.Codigo_voo[i]) == 0)){
            verificador++;
        }
        if(voos.Num_reservas[i] > 0){
            for(iterador = voos.reservas_list[i];iterador != NULL; iterador = iterador->prox){
                if(strcmp(iterador->reservas.Codigo_reserva, Codigo) == 0){
                    verificador = -5;
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
        apagar_noh(voos.reservas_list[Indice_voo], voos.Num_reservas[Indice_voo], Codigo);
        voos.Num_reservas[Indice_voo]--;
        voos.Num_passageiros[Indice_voo] -= voos.reservas_list[Indice_voo]->reservas.Num_passageiros_reserva;
        return 0;
    }
    else{
        apagar_voo(Codigo, voos.IDpartida[Indice_voo]);
    }
    return 0;
}
