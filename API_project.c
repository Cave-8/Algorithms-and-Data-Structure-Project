#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int const SIZE_BLOCK = 4096;
int const ALL_LENG = 7;
int index_blocco = 0;
int all_num = 0;
int ok = 0;
typedef struct partita                                                                                                  //Cella base per vettore da 123 celle che fungerà da vettore partita per ogni carattere
{
    int minimum;                                                                                                        //Numero minimo dei caratteri nella parola
    int exact;                                                                                                          //Numero esatto dei caratteri nella parola
    bool number_known;                                                                                                  //True se so quanti caratteri esatti ci sono nella parola, false altrimenti
}partita;

typedef struct node                                                                                                     //Node BST che contiene ogni parola inserita
{
    struct node *son_l;                                                                                                 //Puntatore al figlio sinistro
    struct node *son_r;                                                                                                 //Puntatore al figlio destro
    bool valid;                                                                                                         //True se il nodo rispetta i vincoli, false altrimenti
    char key[];                                                                                                         //Stringa associata al nodo
}node;

typedef struct counter_sym                                                                                              //Cella base per vettore da 123 celle che conta il numero di / e + per ogni carattere
{
    int n_i;                                                                                                            //Numero di / associati al carattere
    int c_i;                                                                                                            //Numero di + associati al carattere
    int z_i;
}counter_sym;

typedef struct posizione
{
    bool presente[124];                                                                                                 //Tiene conto su 123 caratteri possibili dove possono/non possono esserci, la cella 123 è true se so già che lettera è obbligatorio ci sia; evita doppi controlli
}posizione;

typedef struct list_node
{
    node *el;
    struct list_node *next;
} list_node;

int scorrimento[64] = {45,48,49,50,51,52,53,54,55,56,57,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,95,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122};                                                                                                   //Utilizzato solo per scorrere gli ASCII code significativi
node *root = NULL;
node *blocco_node = NULL;
node *blocchi[100000];
list_node *testa_validi = NULL;
list_node *coda_validi = NULL;
list_node *testa_non_validi = NULL;
list_node *coda_non_validi = NULL;

//Esce al primo carattere diverso
int compara_stringhe (char const s1[], char const s2[], int lunghezza)
{
    for (int i = 0; i < lunghezza; i++)
    {
        if (s1[i] < s2[i])
            return -1;
        if (s1[i] > s2[i])
            return 1;
    }
    return 0;
}

//Prealloca SIZE_BLOCK nodi
void crea_blocco (int lunghezza)
{
    blocco_node = NULL;
    blocco_node = malloc((sizeof(node) + sizeof(char) * (lunghezza + 1)) * SIZE_BLOCK);
    blocchi[all_num] = blocco_node;
    all_num++;
}

void inserisci_lista_validi (node *nodo)
{
    list_node *handle = (list_node *)malloc(sizeof(list_node));
    handle->el = nodo;
    handle->next = NULL;

    if (testa_validi == NULL)
    {
        testa_validi = handle;
        coda_validi = handle;
        return;
    }
    else
    {
        coda_validi->next = handle;
        coda_validi = handle;
        return;
    }
}

void inserisci_lista_non_validi (list_node *nodo)
{
    if (testa_non_validi == NULL)
    {
        testa_non_validi = nodo;
        coda_non_validi = nodo;
        return;
    }
    else
    {
        coda_non_validi->next = nodo;
        coda_non_validi = nodo;
        return;
    }
}

void rimuovi_lista_validi (list_node *pred, list_node *nodo)
{
    if (testa_validi == coda_validi)
    {
        testa_validi = NULL;
        return;
    }

    if (nodo == testa_validi)
    {
        testa_validi = nodo->next;
        nodo->next = NULL;
        return;
    }

    if (nodo == coda_validi)
    {
        coda_validi = pred;
        pred->next = NULL;
        nodo->next = NULL;
        return;
    }

    pred->next = nodo->next;
    nodo->next = NULL;
}

void reset_liste ()
{
    list_node *handle = testa_validi;
    list_node *pred = NULL;
    list_node *next = NULL;

    while (handle != NULL)
    {
        next = handle->next;
        rimuovi_lista_validi(pred, handle);
        inserisci_lista_non_validi(handle);
        pred = handle;
        handle = next;
    }

    testa_validi = testa_non_validi;
    coda_validi = coda_non_validi;
    testa_non_validi = NULL;
    coda_non_validi = NULL;
}

//Crea l'albero
node *crea_albero (char key[], int lunghezza)
{
    if (lunghezza < ALL_LENG)
    {
        root = blocco_node;
        index_blocco++;
        blocco_node++;
    }
    else
        root = malloc(sizeof(node) + sizeof(char) * (lunghezza + 1));

    memcpy(root->key, key, lunghezza + 1);

    root->valid = true;
    root->son_r = NULL;
    root->son_l = NULL;

    return root;
}

//theta(log k) -> k = numero elementi
node *amplia_albero (char key[], int lunghezza)
{
    node *handle_node = NULL;

    if (lunghezza <= ALL_LENG)
    {
        if (index_blocco == SIZE_BLOCK || index_blocco == 0)
        {
            crea_blocco(lunghezza);
            index_blocco = 0;
        }
        handle_node = blocco_node;
        index_blocco++;
        blocco_node++;
    }
    else
        handle_node = malloc(sizeof(node) + sizeof(char) * (lunghezza + 1));

    memcpy(handle_node->key, key, lunghezza + 1);
    handle_node->valid = true;

    node *pred = NULL;
    node *curr = root;

    while (curr != NULL)
    {
        if (compara_stringhe(handle_node->key, curr->key, lunghezza) < 0)
        {
            pred = curr;
            curr = curr->son_l;
        }
        else
        {
            pred = curr;
            curr = curr->son_r;
        }
    }

    if (pred == NULL)
        root = handle_node;
    else if (compara_stringhe(handle_node->key, pred->key, lunghezza) < 0)
        pred->son_l = handle_node;
    else
        pred->son_r = handle_node;

    handle_node->son_l = NULL;
    handle_node->son_r = NULL;

    return handle_node;
}

void reset_dizionario (int *num)
{
    list_node *handle = testa_validi;

    while (handle != NULL)
    {
        handle->el->valid = true;
        (*num)++;
        handle = handle->next;
    }
}

void inserisci_parole(int lunghezza, char buffer[])
{
    int begin = 0;
    int err_scanf;

    while (begin != 1)
    {
        err_scanf = scanf("%s", buffer);                                                                          //Ricezione stringa (compresi i comandi)
        if (err_scanf == 0)
            return;
        if (buffer[0] != '+' || buffer[1] != 'n')
        {
            if (buffer[0] != '+' || buffer[1] != 'i')
                amplia_albero(buffer, lunghezza);                                                                   //Amplia l'albero con la chiave creata
        }
        else
            begin = 1;
    }
}

void crea_lista (node *nodo)
{
    if (nodo->son_l != NULL)
        crea_lista(nodo->son_l);
    inserisci_lista_validi(nodo);
    if (nodo->son_r != NULL)
        crea_lista(nodo->son_r);
}

int ricerca_albero (char key[], node *nodo, int lunghezza)
{
    node *temp = nodo;
    while (temp != NULL)
    {
        if (compara_stringhe(temp->key, key, lunghezza) > 0)
            temp = temp->son_l;
        else if (compara_stringhe(temp->key, key, lunghezza) < 0)
            temp = temp->son_r;
        else
            return 1;
    }
    return 0;
}

void filtra_parola(node *nodo, int lunghezza, int *num, bool appartiene[], partita vettore_partita[], char reference[], char learning[], posizione locazione[], int counter_informazioni, int informazioni_caratteri[])
{
    int caratteri[123] = {0};
    int accesso = 0;

    for (int i = 0; i < lunghezza; i++)
    {
        if (learning[i] != '?')                                                                                         //So che carattere c'è in posizione i
        {
            if (nodo->key[i] != reference[i])
            {
                (*num)--;
                nodo->valid = false;
                return;
            }
        }
        else
        {
            if ((locazione[i].presente[(int) nodo->key[i]] == false || appartiene[(int) nodo->key[i]] == false))        //Verifica se il carattere di key in posizione i è compatibile con ciò che è stato appreso (vincoli 1/2/3)
            {
                (*num)--;
                nodo->valid = false;
                return;
            }
        }
        caratteri[(int)nodo->key[i]]++;
    }

    for (int i = 0; i < counter_informazioni; i++)
    {
        accesso = informazioni_caratteri[i];
        if (vettore_partita[accesso].number_known == true && caratteri[accesso] != vettore_partita[accesso].exact)                  //Ho un numero esatto di caratteri definito (vincolo 5)
        {
            (*num)--;
            nodo->valid = false;
            return;
        }
        else if (vettore_partita[accesso].number_known == false && caratteri[accesso] < vettore_partita[accesso].minimum)           //So solo i caratteri minimi (vincolo 4)
        {
            (*num)--;
            nodo->valid = false;
            return;
        }
    }
}

void filter_algo(node *nodo, int lunghezza, int *num, bool appartiene[], partita vettore_partita[], char reference[], char learning[], char buffer[], int informazioni[], posizione locazione[], int counter_informazioni, int informazioni_caratteri[])
{
    bool del = 0;

    if (*num == 1)
        return;

    list_node *handle = testa_validi;
    list_node *pred = NULL;
    list_node *next = NULL;

    while (handle != NULL)
    {
        if ((handle->el)->valid == true)
            filtra_parola(handle->el, lunghezza, num, appartiene, vettore_partita, reference, learning, locazione, counter_informazioni, informazioni_caratteri);

        next = handle->next;

        if ((handle->el)->valid == false)
        {
            rimuovi_lista_validi(pred, handle);
            inserisci_lista_non_validi(handle);
            del = 1;
        }

        if (del == 0)
            pred = handle;
        handle = next;
        del = 0;
    }
}

void learning_algo(char key[], int lunghezza, int *num, bool appartiene[], partita vettore_partita[123], char reference[], char learning[], char buffer[], char symbols[], int informazioni[], posizione locazione[], bool info_gia_presenti[], int *counter_informazioni, int informazioni_caratteri[])
{
    int num_info = 0;
    int accesso = 0;
    ok = 0;
    bool modifiche = false;
    int temp_caratteri[123] = {0};
    counter_sym counter[123] = {0};
    partita temp_char[123] = {0};

    for (int i = 0; i < lunghezza; i++)
    {
        temp_caratteri[(unsigned int) key[i]]++;                                                                        //Aumenta il contatore temporaneo dei caratteri
        symbols[i] = '?';

        if (key[i] == reference[i])
        {
            symbols[i] = '+';
            learning[i] = key[i];
            ok++;
            locazione[i].presente[123] = true;
        }
        else
            locazione[i].presente[(unsigned int) key[i]] = false;                                                       //Setup vincolo 2

        counter[(unsigned int)reference[i]].n_i++;                                                                      //Occorrenze della lettera nella stringa (n_i)
        if (symbols[i] == '+')
        {
            counter[(unsigned int) key[i]].c_i++;                                                                       //Occorrenze della lettera nella stringa in posizione corretta (c_i)
            temp_char[(unsigned int) key[i]].exact++;                                                                   //Aggiorna il numero di caratteri esatti
            temp_char[(unsigned int) key[i]].minimum++;                                                                 //Aggiorna il numero di caratteri minimi
        }
        informazioni[num_info] = (int)key[i];                                                                           //Inserisco nel vettore informazioni l'ASCII code relativo al carattere con informazioni aggiornate
        num_info++;
    }

    for (int i = 0; i < lunghezza; i++)                                                                                 //Stampa wordle
    {
        if (symbols[i] != '+')
            counter[(unsigned int) key[i]].z_i++;

        if ((counter[(unsigned int) key[i]].z_i > (counter[(unsigned int) key[i]].n_i - counter[(unsigned int) key[i]].c_i)) && symbols[i] != '+')
            symbols[i] = '/';

        else if (symbols[i] != '+')
        {
            symbols[i] = '|';
            temp_char[(unsigned int) key[i]].minimum++;                                                                 //Aggiorna il numero di caratteri minimi
            temp_char[(unsigned int) key[i]].exact++;
        }
    }

    for (int i = 0; i < num_info; i++)
    {
        accesso = informazioni[i];
        if (info_gia_presenti[accesso] == false)                                                                        //Aggiorno il vettore che mi dice se conosco già informazioni relative a quel carattere
        {
            info_gia_presenti[accesso] = true;
            informazioni_caratteri[(*counter_informazioni)] = accesso;
            (*counter_informazioni)++;
        }

        if (temp_char[accesso].minimum > vettore_partita[accesso].minimum)
        {
            vettore_partita[accesso].minimum = temp_char[accesso].minimum;                                              //Aggiorna eventualmente il contatore minimo
            modifiche = true;
        }

        if (temp_char[accesso].exact > vettore_partita[accesso].exact)                                                  //Aggiorna eventualmente il contatore esatto
        {
            vettore_partita[accesso].exact = temp_char[accesso].exact;
            modifiche = true;
        }

        if (temp_caratteri[accesso] > vettore_partita[accesso].exact)                                                   //Setup vincolo 5
        {
            vettore_partita[accesso].number_known = true;
            modifiche = true;
        }

        if (temp_caratteri[accesso] > 1 && vettore_partita[accesso].exact == 0)
        {
            appartiene[accesso] = false;
            modifiche = true;
        }
    }

    if (ok != lunghezza)
        printf("%.*s\n", lunghezza, symbols);
    if (modifiche == true)                                                                                              //Se non ci sono modifiche non filtra
        filter_algo(root, lunghezza, num, appartiene, vettore_partita, reference, learning, buffer, informazioni, locazione, *counter_informazioni, informazioni_caratteri);
    modifiche = false;
}

void inserisci_inizio(int lunghezza, int *num, bool appartiene[], partita vettore_partita[], char reference[], char buffer[], char learning[], posizione locazione[], int counter_informazioni, int informazioni_caratteri[])
{
    bool inserisci_fine = false;
    int err_scanf = 0;
    node *temp = NULL;

    while (inserisci_fine == false)
    {
        err_scanf = scanf("%s", buffer);
        if (err_scanf != 1)
            return;

        if (buffer[0] != '+' || buffer[1] != 'i')
        {
            if (root == NULL)
            {
                temp = crea_albero(buffer, lunghezza);
                (*num)++;
                filtra_parola(temp, lunghezza, num, appartiene, vettore_partita, reference, learning, locazione, counter_informazioni, informazioni_caratteri);
            }
            else
            {
                temp = amplia_albero(buffer, lunghezza);
                (*num)++;
                filtra_parola(temp, lunghezza, num, appartiene, vettore_partita, reference, learning, locazione, counter_informazioni, informazioni_caratteri);

                if (temp->valid == true)                                                                                //Inserisci in coda lista validi
                    inserisci_lista_validi(temp);
                else                                                                                                    //Inserisce in coda lista non validi
                {
                    list_node *nuovo_nodo = (list_node *)malloc(sizeof(list_node));
                    nuovo_nodo->el = temp;
                    nuovo_nodo->next = NULL;
                    inserisci_lista_non_validi(nuovo_nodo);
                }

            }
        }
        else
            inserisci_fine = true;
    }
}

void stampa_filtrate(node *nodo, int lunghezza)
{
    if (nodo->son_l != NULL)
        stampa_filtrate(nodo->son_l, lunghezza);
    if (nodo->valid == true)
        printf("%.*s\n", lunghezza, nodo->key);
    if (nodo->son_r != NULL)
        stampa_filtrate(nodo->son_r, lunghezza);
}

void libera_liste()
{
    list_node *handle = testa_validi;
    list_node *prec = NULL;

    while (handle != NULL)
    {
        prec = handle;
        handle = handle->next;

        prec->el = NULL;
        prec->next = NULL;
        free(prec);
    }

    handle = testa_non_validi;
    prec = NULL;

    while (handle != NULL)
    {
        prec = handle;
        handle = handle->next;

        prec->el = NULL;
        prec->next = NULL;
        free(prec);
    }
}

void libera_albero(node *nodo)
{
    if (nodo->son_l != NULL)
        libera_albero(nodo->son_l);
    if (nodo->son_r != NULL)
        libera_albero(nodo->son_r);

    nodo->son_l = NULL;
    nodo->son_r = NULL;
    free(nodo);
}

void safe_cancellazione_albero(node *nodo)
{
    if (nodo->son_l != NULL)
        safe_cancellazione_albero(nodo->son_l);
    if (nodo->son_r != NULL)
        safe_cancellazione_albero(nodo->son_r);

    nodo->son_l = NULL;
    nodo->son_r = NULL;
}

int main() {
    int found = 0;
    int word_length = 0;
    int max_conf = 0;
    int num_elem_valid = 0;
    int accesso = 0;
    int err_scanf = 0;
    bool win = false;
    bool nuova_partita = false;
    bool end = false;
    bool appartiene[123];                                                                                               //Per ogni carattere è true se appartiene, false altrimenti (DEFAULT: true)
    posizione locazione[300];                                                                                           //Ogni cella di locazione ha 123 bool, vero se il carattere può appartenere, falso altrimenti
    char reference[300];
    char learning[300];
    char buffer[300];
    char symbols[300];
    int informazioni[300];

    err_scanf = scanf("%d", &word_length);

    inserisci_parole(word_length, buffer);
    crea_lista(root);                                                                                             //Crea lista a partire dal dizionario

    do
    {
        win = false;
        nuova_partita = false;
        end = false;
        partita vettore_partita[123] = {0};
        bool info_gia_presenti[123] = {0};                                                                              //Di default false, true se ho aggiunto la lettera nel vettore delle informazioni
        int informazioni_caratteri[64] = {0};                                                                           //Contiene gli ASCII code delle lettere per cui si conoscono informazioni
        int counter_informazioni = 0;                                                                                   //Tiene conto di quanti caratteri so informazioni

        err_scanf = scanf("%s", reference);                                                                       //Inserimento di parola di riferimento

        for (int i = 0; i < 64; i++)                                                                                    //Inizializzazione vettore partita, vettore appartenenza, contatori per appartenenza, vettore di locazione
        {
            accesso = scorrimento[i];
            appartiene[accesso] = true;
        }

        for (int i = 0; i < word_length; i++)
        {
            for (int u = 0; u < 64; u++)
                locazione[i].presente[scorrimento[u]] = true;
        }

        strcpy(learning, "??????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????");

        num_elem_valid = 0;
        reset_dizionario(&num_elem_valid);

        err_scanf = scanf("%d", &max_conf);

        for (int i = 0; i < max_conf; i++)
        {
            err_scanf = scanf("%s", buffer);

            if (buffer[0] == '+')                                                                                       //Si sta per inserire un comando
            {
                if (buffer[1] == 'i')
                    inserisci_inizio(word_length, &num_elem_valid, appartiene, vettore_partita, reference, buffer, learning, locazione, counter_informazioni, informazioni_caratteri);
                if (buffer[1] == 's')
                    stampa_filtrate(root, word_length);
                if (buffer[1] == 'n')
                {
                    nuova_partita = true;
                    i = max_conf;
                }
                else
                    i--;
            }
            else
            {
                found = ricerca_albero(buffer, root, word_length);

                if (found == 0)
                {
                    puts("not_exists");                                                                              //La stringa non esiste
                    i--;
                }

                if (found == 1)
                {
                    learning_algo(buffer, word_length, &num_elem_valid, appartiene, vettore_partita, reference, learning, buffer, symbols, informazioni, locazione, info_gia_presenti, &counter_informazioni, informazioni_caratteri);
                    if (ok != word_length)
                        printf("%d\n", num_elem_valid);
                    else
                    {
                        puts("ok");
                        i = max_conf;
                        win = true;
                    }
                }
            }
        }                                                                                                               //A meno che non sia stato diminuito la partita termina

        if (win == false)
            puts("ko");

        while (end == false)
        {
            err_scanf = scanf("%s", buffer);

            if (err_scanf != 1)                                                                                         //Partite terminate, inizio deallocazione strutture
            {
                if (word_length <= ALL_LENG)
                {
                    safe_cancellazione_albero(root);
                    for (int i = 0; i < all_num; i++)
                    {
                        free(blocchi[i]);
                        blocchi[i] = NULL;
                    }
                }
                else
                    libera_albero(root);
                libera_liste();
                return 0;
            }

            if (buffer[0] == '+' && buffer[1] == 'i')                                                                   //Se nuova_partita == true deve già iniziarne una nuova
                inserisci_inizio(word_length, &num_elem_valid, appartiene, vettore_partita, reference, buffer, learning, locazione, counter_informazioni, informazioni_caratteri);
            if (buffer[0] == '+' && buffer[1] == 's')                                                                   //Se nuova_partita == true deve già iniziarne una nuova
                stampa_filtrate(root, word_length);
            if (buffer[0] == '+' && buffer[1] == 'n')
            {
                nuova_partita = true;
                end = true;
            }
        }
        reset_liste();
    } while (nuova_partita == true);
}
