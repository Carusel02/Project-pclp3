#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

typedef struct info
{   unsigned int varsta;
    unsigned int len; 
} info;

typedef struct data_structure
{   info *header;
    void *data; 
} data_structure;

// adaugare
int add_last(void **baza, int *len, data_structure *data)
{   // alocare vector baza fix cu cat avem nevoie
    if (*baza == NULL)
        *baza = malloc(sizeof(info) + data->header->len); 
    else
        *baza = realloc(*baza, *len + sizeof(info) + data->header->len);
     
    // salvare adresa inceput
    void *pointer = *baza;
    // adaugare spre final
    *baza = *baza + *len;
    // copiere header
    memcpy(*baza, data->header, sizeof(info));
    (*baza) = ((char *)*baza + 4); // trecere catre zona len
    size_t size = *(int *)(*baza); // extragere len
    (*baza) = ((int *)*baza + 1); // ajungere zona data
    memcpy(*baza, data->data, size); // copiere bytes din data
    *baza = pointer; // adresa de start

    return 0;
}

// functie delete
int delete_at(void **baza, int *len, int index)
{   // salvare adresa initiala
    void *pointer = *baza;
    // index element de sters
    int delete = 0;
    
    // parcurgere pana la index
    int lungime_data = *(int *)(*baza + 4);
    int suma = 0;
    while (delete != index) {
        // aflam cat din lungimea vectorului am parcurs
        suma = suma + sizeof(info) + lungime_data;
        // trecem la urmatorul element
        *baza = *baza + sizeof(info) + lungime_data;
        // aflam cat are zona data a urmatorului element
        lungime_data = *(int *)(*baza + 4);
        // incrementam indexul delete
        delete++;
    }
     
    // modificare date
    int pas = sizeof(info) + lungime_data;
    // adresa de dupa locul ocupat de element
    suma = suma + pas;
    // zona ramasa dupa stergerea elementului
    size_t size = *len - suma;
    // copiere bytes din zona de dupa element in zona elementului 
    memcpy(*baza, *baza + pas, size); // un fel de suprascriere a datelor
    *baza = pointer; // adresa initiala pointer
    *baza = realloc(*baza, *len - pas); // realocare cu dimensiune - element sters
    *len = *len - pas;

    return 0;
}


void write_data(void **baza,data_structure *elem, int *len) {    
    printf("Introduceti varsta:\n");
    unsigned int varsta;
    scanf("%d", &varsta);
    elem->header = (info *)malloc(sizeof(info));
    elem->header->varsta = varsta;

    char *nume = malloc(256 * sizeof(char));
    char *parola = malloc(256 * sizeof(char));
    
    printf("Introduceti nume:\n");
    scanf("%s", nume);
    printf("Introduceti parola:\n");
    scanf("%s", parola);
    
    printf("Introduceti cnp:\n");
    int64_t cnp;
    scanf("%"SCNd64"", &cnp);
    
    printf("Introduceti vot:\n");
    int8_t vot;
    scanf("%"SCNd8"", &vot);

    size_t size_nume = strlen(nume) + 1;
    size_t size_parola = strlen(parola) + 1;
    size_t size_cnp = sizeof(int64_t);
    size_t size_vot = sizeof(int8_t);
    size_t total_size = size_nume + size_parola + size_vot + size_cnp;
    elem->header->len = (int)total_size;

    elem->data = malloc(total_size);
    memcpy(elem->data, nume, size_nume);
    elem->data = ((char *)elem->data + size_nume);
    memcpy(elem->data, parola, size_parola);
    elem->data = ((char *)elem->data + size_parola);
    memcpy(elem->data, &cnp, size_cnp);
    elem->data = ((int64_t *)elem->data + 1);
    memcpy(elem->data, &vot, size_vot);
    elem->data = ((int8_t *)elem->data + 1);
    elem->data = (char *)elem->data - total_size;
    
    
    if(add_last(baza, len, elem) != 0)
    exit(-1);

    *len = *len + elem->header->len + (int)sizeof(info);

    free(elem->data);
    free(nume);
    free(parola);
    free(elem->header);
    free(elem);
}

// functia de printare
void print(void *baza, int len) {
    // daca vectorul are lungimea 0 nu mai afisam nimic
    if (len == 0)
    return;

    void *pointer = baza;
    int ok = 0; // variabila contor
    int lungime_finala = 0;
    int add = 0;
    
    // extragem tipul si dupa afisam informatia pana cand ajungem la len
    while (ok == 0) {
    printf("Varsta %d\n", *(int *)(baza));
    

    int lungime_nume = strlen((char *)(baza + 8)) + 1;
    int lungime_parola = strlen((char *)(baza + 8 + lungime_nume)) + 1;
    // facem cast la tipul de informatie pe care vrem sa il citim, dupa sarim peste x pasi
    
    // aici e baza + sizeof(head) = 8 + lungime la sir1 + 2 bytes ( 2 bancnote de 1 byte)
    printf("Nume : %s Parola : %s\n", (char *)(baza + 8), (char *)(baza + 8 + lungime_nume)); 
    // prima bancnota se afla la baza + sizeof(head) = 8 + lungime primul sir
    printf("Cnp :%" PRId64 "\n", *(int64_t *)(baza + 8 + lungime_nume + lungime_parola));
    // a doua se afla dupa prima + 1 byte
    printf("Vot :%" PRId8 "\n", *(int8_t *)(baza + 16 + lungime_nume + lungime_parola));
    printf("\n");
    
    // crestem add cu dimensiunea unui struct de element
    lungime_finala = 8 + 9 + lungime_nume + lungime_parola;
    add = add + lungime_finala;
    
    // verificare daca am ajuns pana la finalul vectorului
    if (add < len)
        baza = baza + lungime_finala;
    else
        ok = 1; // iesire daca este cazul

    }

    baza = pointer; // baza ia adresa intiala pentru alta posibila afisare
}

void print_secret(void *baza, int len) {
    // daca vectorul are lungimea 0 nu mai afisam nimic
    if (len == 0)
    return;

    void *pointer = baza;
    int ok = 0; // variabila contor
    int lungime_finala = 0;
    int add = 0;
    
    // extragem tipul si dupa afisam informatia pana cand ajungem la len
    while (ok == 0) {
    printf("Varsta %d\n", *(int *)(baza));
    

    int lungime_nume = strlen((char *)(baza + 8)) + 1;
    int lungime_parola = strlen((char *)(baza + 8 + lungime_nume)) + 1;
    

    printf("Nume : %s Parola : ", (char *)(baza + 8)); 
    for(int i = 0 ; i < lungime_parola - 1 ; i++) {
        printf("*");
    }
    printf("\n");

    printf("Cnp : [hidden]\n");

    printf("Vot : [hidden]\n");
    
    // crestem add cu dimensiunea unui struct de element
    lungime_finala = 8 + 9 + lungime_nume + lungime_parola;
    add = add + lungime_finala;
    
    // verificare daca am ajuns pana la finalul vectorului
    if (add < len)
        baza = baza + lungime_finala;
    else
        ok = 1; // iesire daca este cazul

    }

    baza = pointer; // baza ia adresa intiala pentru alta posibila afisare
}


// functie find
void find(void *data_block, int len, int index) {
    if (index < 0)
        return;
    // salvare adresa initiala
    void *pointer = data_block;
    int find = 0;
    
    // parcurgere vector ( similar cu functiile de mai sus )
    int lungime_data = *(int *)(data_block + 4);
    while (find != index)
    {
        data_block = data_block + sizeof(info) + lungime_data;
        lungime_data = *(int *)(data_block + 4);
        find++;
    }
    
    // dupa ce am gasit indexul afisam ( similar cu functia print )
    printf("Varsta %d\n", *(int *)(data_block));


    int lungime = strlen((char *)(data_block + 8)) + 1;
    printf("%s pentru %s\n", (char *)(data_block + 8), (char *)(data_block + 8 + lungime + 2));
    printf("%" PRId8 "\n", *(int8_t *)(data_block + 8 + lungime));
    printf("%" PRId8 "\n", *(int8_t *)(data_block + 9 + lungime));
    printf("\n");

    data_block = pointer; // returnare adresa initiala
}



int main() {
    // 
    void *baza = NULL;
    int len = 0;
    int lungime_vector = 0;
    
    // primire comanda
    char buffer[256];
    // rulare pana la exit
    int run = 1;
    

    
    do
    {   // scanare buffer
        scanf("%s", buffer);
        
        // comanda insert care creeaza un element, il aloca + scrie in el
        if(strcmp(buffer, "insert") == 0) {
            data_structure *elem = (data_structure *)malloc(sizeof(data_structure));
             // dupa ce scrie aloca in vectorul generic apeland o subfunctie din subprogram
            write_data(&baza, elem, &len);
            // crestem lungimea vectorului ( nu ca nr de bytes, ca nr de elem)
            lungime_vector++;
        }

        if(strcmp(buffer, "print") == 0) {
            // apelam functia de printare
            print(baza,len);
        }

        if(strcmp(buffer, "confidential") == 0) {
            // apelam functia de printare
            print_secret(baza,len);
        }

        if(strcmp(buffer, "exit") == 0) {
            // oprim programul din a mai rula
            run = 0;
        }

         if (strcmp(buffer, "find") == 0) {
            // scanam un index si apelam functia de gasire
            int index;
            scanf("%d", &index);
            find(baza, len, index);
        }

         if (strcmp(buffer, "delete_at") == 0) {
            // scanam un index si apelam functia de stergere
            int index;
            scanf("%d", &index);
            // daca nu a mers stergerea iesim din program cu exit status -2
            if (delete_at(&baza, &len, index) != 0)
            exit(-2);
            // scadem lungimea vectorului
            lungime_vector--;
        }

    } while ( run == 1 );
    
    // free la vectorul generic
    free(baza);




    return 0;
}