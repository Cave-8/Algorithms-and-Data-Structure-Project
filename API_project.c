#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int const SIZE_BLOCK = 4096;
int const ALL_LENG = 7;
int index_blocco = 0;
int all_num = 0;
int ok = 0;

/**
 * Basic cell for 123 cells array containing minimum and exact number of occurrences and if the software knows exact number of occurrences
 */
typedef struct match                                                                                                    //Cella base per vettore da 123 celle che fungerà da vettore match per ogni carattere
{
    int minimum;                                                                                                        //Numero minimo dei caratteri nella parola
    int exact;                                                                                                          //Numero esatto dei caratteri nella parola
    bool number_known;                                                                                                  //True se so quanti caratteri esatti ci sono nella parola, false altrimenti
}match;

/**
 * Binary Search Tree node
 */
typedef struct node                                                                                                     //Node BST che contiene ogni parola inserita
{
    struct node *son_l;                                                                                                 //Puntatore al figlio sinistro
    struct node *son_r;                                                                                                 //Puntatore al figlio destro
    bool valid;                                                                                                         //True se il nodo rispetta i vincoli, false altrimenti
    char key[];                                                                                                         //Stringa associata al nodo
}node;

/**
 * Counter of characters for each character in the alphabet
 */
typedef struct counter_sym                                                                                              //Cella base per vettore da 123 celle che conta il numero di / e + per ogni carattere
{
    int n_i;                                                                                                            //Numero di / associati al carattere
    int c_i;                                                                                                            //Numero di + associati al carattere
    int z_i;
}counter_sym;

/**
 * If true than the characters does appear in the word
 */
typedef struct position
{
    bool presente[124];                                                                                                 //Tiene conto su 123 caratteri possibili dove possono/non possono esserci, la cella 123 è true se so già che lettera è obbligatorio ci sia; evita doppi controlli
}position;

/**
 * List node
 */
typedef struct list_node
{
    node *el;
    struct list_node *next;
} list_node;

/**
 * Used ASCII codes are only these 64
 */
int alphabet[64] = {45, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 95, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122};                                                                                                   //Utilizzato solo per scorrere gli ASCII code significativi
node *root = NULL;
node *node_block = NULL;
node *blocks[100000];
list_node *valid_head = NULL;
list_node *valid_tail = NULL;
list_node *not_valid_head = NULL;
list_node *not_valid_tail = NULL;

/**
 * Custom string compare
 * @param s1 is first string
 * @param s2 is second string
 * @param length is string length, equals for both strings
 * @return -1 if s1<s2, 1 if s1>s2, 0 if s1==s2
 */
int string_compare (char const s1[], char const s2[], int length)
{
    for (int i = 0; i < length; i++)
    {
        if (s1[i] < s2[i])
            return -1;
        if (s1[i] > s2[i])
            return 1;
    }
    return 0;
}

/**
 * Create block
 * @param length is word length for current game
 */
void create_block (int length)
{
    node_block = NULL;
    node_block = malloc((sizeof(node) + sizeof(char) * (length + 1)) * SIZE_BLOCK);
    blocks[all_num] = node_block;
    all_num++;
}

/**
 * Insert node in valid list
 * @param node is node to be inserted
 */
void insert_in_valid_list (node *node)
{
    list_node *handle = (list_node *)malloc(sizeof(list_node));
    handle->el = node;
    handle->next = NULL;

    if (valid_head == NULL)
    {
        valid_head = handle;
        valid_tail = handle;
        return;
    }
    else
    {
        valid_tail->next = handle;
        valid_tail = handle;
        return;
    }
}

/**
 * Insert node in not valid list
 * @param node is node to be inserted
 */
void not_valid_node (list_node *node)
{
    if (not_valid_head == NULL)
    {
        not_valid_head = node;
        not_valid_tail = node;
        return;
    }
    else
    {
        not_valid_tail->next = node;
        not_valid_tail = node;
        return;
    }
}

/**
 * Remove node from valid list
 * @param prev is previous node
 * @param node is current node
 */
void remove_from_valid_list (list_node *prev, list_node *node)
{
    if (valid_head == valid_tail)
    {
        valid_head = NULL;
        return;
    }

    if (node == valid_head)
    {
        valid_head = node->next;
        node->next = NULL;
        return;
    }

    if (node == valid_tail)
    {
        valid_tail = prev;
        prev->next = NULL;
        node->next = NULL;
        return;
    }

    prev->next = node->next;
    node->next = NULL;
}

/**
 * Reset lists
 */
void reset_lists ()
{
    list_node *handle = valid_head;
    list_node *pred = NULL;
    list_node *next = NULL;

    while (handle != NULL)
    {
        next = handle->next;
        remove_from_valid_list(pred, handle);
        not_valid_node(handle);
        pred = handle;
        handle = next;
    }

    valid_head = not_valid_head;
    valid_tail = not_valid_tail;
    not_valid_head = NULL;
    not_valid_tail = NULL;
}

/**
 * Create BST
 * @param key is word inserted in root
 * @param length is word length
 * @return root of BST
 */
node *create_tree (char key[], int length)
{
    if (length < ALL_LENG)
    {
        root = node_block;
        index_blocco++;
        node_block++;
    }
    else
        root = malloc(sizeof(node) + sizeof(char) * (length + 1));

    memcpy(root->key, key, length + 1);

    root->valid = true;
    root->son_r = NULL;
    root->son_l = NULL;

    return root;
}

/**
 * Insert word in BST (θ(logK) where K is number of words in BST)
 * @param key is word inserted in tree
 * @param length is word length
 * @return inserted node
 */
node *add_node_BST (char key[], int length)
{
    node *handle_node = NULL;

    if (length <= ALL_LENG)
    {
        if (index_blocco == SIZE_BLOCK || index_blocco == 0)
        {
            create_block(length);
            index_blocco = 0;
        }
        handle_node = node_block;
        index_blocco++;
        node_block++;
    }
    else
        handle_node = malloc(sizeof(node) + sizeof(char) * (length + 1));

    memcpy(handle_node->key, key, length + 1);
    handle_node->valid = true;

    node *pred = NULL;
    node *curr = root;

    while (curr != NULL)
    {
        if (string_compare(handle_node->key, curr->key, length) < 0)
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
    else if (string_compare(handle_node->key, pred->key, length) < 0)
        pred->son_l = handle_node;
    else
        pred->son_r = handle_node;

    handle_node->son_l = NULL;
    handle_node->son_r = NULL;

    return handle_node;
}

/**
 * Reset dictionary of current game
 * @param num is number of elements in dictionary
 */
void reset_dictionary (int *num)
{
    list_node *handle = valid_head;

    while (handle != NULL)
    {
        handle->el->valid = true;
        (*num)++;
        handle = handle->next;
    }
}

/**
 * Insert word {@link add_node_BST}
 * @param length is word length
 * @param buffer contains command
 */
void insert_word(int lunghezza, char buffer[])
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
                add_node_BST(buffer, lunghezza);                                                                   //Amplia l'albero con la chiave creata
        }
        else
            begin = 1;
    }
}

/**
 * Create list
 * @param node is valid node
 */
void create_list (node *nodo)
{
    if (nodo->son_l != NULL)
        create_list(nodo->son_l);
    insert_in_valid_list(nodo);
    if (nodo->son_r != NULL)
        create_list(nodo->son_r);
}

/**
 * Search desired word in BST
 * @param key is searched word
 * @param curr_node is current node
 * @param length is word length
 * @return 1 if word is in BST, otherwise returns 0
 */
int BST_search (char key[], node *curr_node, int length)
{
    node *temp = curr_node;
    while (temp != NULL)
    {
        if (string_compare(temp->key, key, length) > 0)
            temp = temp->son_l;
        else if (string_compare(temp->key, key, length) < 0)
            temp = temp->son_r;
        else
            return 1;
    }
    return 0;
}

/**
 * Filter word according to information collected
 */
void filtra_parola(node *nodo, int lunghezza, int *num, bool appartiene[], match vettore_partita[], char reference[], char learning[], position locazione[], int counter_informazioni, int informazioni_caratteri[])
{
    int caratteri[123] = {0};
    int accesso = 0;

    for (int i = 0; i < lunghezza; i++)
    {
        if (learning[i] != '?')                                                                                         //So che carattere c'è in position i
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
            if ((locazione[i].presente[(int) nodo->key[i]] == false || appartiene[(int) nodo->key[i]] == false))        //Verifica se il carattere di key in position i è compatibile con ciò che è stato appreso (vincoli 1/2/3)
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

/**
 * Pruning algorithm according to information collected
 */
void filter_algo(node *nodo, int lunghezza, int *num, bool appartiene[], match vettore_partita[], char reference[], char learning[], char buffer[], int informazioni[], position locazione[], int counter_informazioni, int informazioni_caratteri[])
{
    bool del = 0;

    if (*num == 1)
        return;

    list_node *handle = valid_head;
    list_node *pred = NULL;
    list_node *next = NULL;

    while (handle != NULL)
    {
        if ((handle->el)->valid == true)
            filtra_parola(handle->el, lunghezza, num, appartiene, vettore_partita, reference, learning, locazione, counter_informazioni, informazioni_caratteri);

        next = handle->next;

        if ((handle->el)->valid == false)
        {
            remove_from_valid_list(pred, handle);
            not_valid_node(handle);
            del = 1;
        }

        if (del == 0)
            pred = handle;
        handle = next;
        del = 0;
    }
}

/**
 * Collect information for word comparison
 */
void learning_algo(char key[], int lunghezza, int *num, bool appartiene[], match vettore_partita[123], char reference[], char learning[], char buffer[], char symbols[], int informazioni[], position locazione[], bool info_gia_presenti[], int *counter_informazioni, int informazioni_caratteri[])
{
    int num_info = 0;
    int accesso = 0;
    ok = 0;
    bool modifiche = false;
    int temp_caratteri[123] = {0};
    counter_sym counter[123] = {0};
    match temp_char[123] = {0};

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
            counter[(unsigned int) key[i]].c_i++;                                                                       //Occorrenze della lettera nella stringa in position corretta (c_i)
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

/**
 * Procedure for inserting word
 */
void inserisci_inizio(int lunghezza, int *num, bool appartiene[], match vettore_partita[], char reference[], char buffer[], char learning[], position locazione[], int counter_informazioni, int informazioni_caratteri[])
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
                temp = create_tree(buffer, lunghezza);
                (*num)++;
                filtra_parola(temp, lunghezza, num, appartiene, vettore_partita, reference, learning, locazione, counter_informazioni, informazioni_caratteri);
            }
            else
            {
                temp = add_node_BST(buffer, lunghezza);
                (*num)++;
                filtra_parola(temp, lunghezza, num, appartiene, vettore_partita, reference, learning, locazione, counter_informazioni, informazioni_caratteri);

                if (temp->valid == true)                                                                                //Inserisci in coda lista validi
                    insert_in_valid_list(temp);
                else                                                                                                    //Inserisce in coda lista non validi
                {
                    list_node *nuovo_nodo = (list_node *)malloc(sizeof(list_node));
                    nuovo_nodo->el = temp;
                    nuovo_nodo->next = NULL;
                    not_valid_node(nuovo_nodo);
                }

            }
        }
        else
            inserisci_fine = true;
    }
}

/**
 * Print filtered words
 * @param node is current node
 * @param length is word length
 */
void stampa_filtrate(node *node, int length)
{
    if (node->son_l != NULL)
        stampa_filtrate(node->son_l, length);
    if (node->valid == true)
        printf("%.*s\n", length, node->key);
    if (node->son_r != NULL)
        stampa_filtrate(node->son_r, length);
}

/**
 * Free memory occupied by list
 */
void free_list()
{
    list_node *handle = valid_head;
    list_node *prec = NULL;

    while (handle != NULL)
    {
        prec = handle;
        handle = handle->next;

        prec->el = NULL;
        prec->next = NULL;
        free(prec);
    }

    handle = not_valid_head;
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

/**
 * Free memory occupied by BST
 * @param node is BST root
 */
void free_BST(node *node)
{
    if (node->son_l != NULL)
        free_BST(node->son_l);
    if (node->son_r != NULL)
        free_BST(node->son_r);

    node->son_l = NULL;
    node->son_r = NULL;
    free(node);
}

/**
 * Avoid memory access errors
 * @param node is BST root
 */
void free_safe_BST(node *node)
{
    if (node->son_l != NULL)
        free_safe_BST(node->son_l);
    if (node->son_r != NULL)
        free_safe_BST(node->son_r);

    node->son_l = NULL;
    node->son_r = NULL;
}

/**
 * Main function
 */
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
    position locazione[300];                                                                                           //Ogni cella di locazione ha 123 bool, vero se il carattere può appartenere, falso altrimenti
    char reference[300];
    char learning[300];
    char buffer[300];
    char symbols[300];
    int informazioni[300];

    err_scanf = scanf("%d", &word_length);

    insert_word(word_length, buffer);
    create_list(root);                                                                                             //Crea lista a partire dal dizionario

    do
    {
        win = false;
        nuova_partita = false;
        end = false;
        match vettore_partita[123] = {0};
        bool info_gia_presenti[123] = {0};                                                                              //Di default false, true se ho aggiunto la lettera nel vettore delle informazioni
        int informazioni_caratteri[64] = {0};                                                                           //Contiene gli ASCII code delle lettere per cui si conoscono informazioni
        int counter_informazioni = 0;                                                                                   //Tiene conto di quanti caratteri so informazioni

        err_scanf = scanf("%s", reference);                                                                       //Inserimento di parola di riferimento

        for (int i = 0; i < 64; i++)                                                                                    //Inizializzazione vettore match, vettore appartenenza, contatori per appartenenza, vettore di locazione
        {
            accesso = alphabet[i];
            appartiene[accesso] = true;
        }

        for (int i = 0; i < word_length; i++)
        {
            for (int u = 0; u < 64; u++)
                locazione[i].presente[alphabet[u]] = true;
        }

        strcpy(learning, "??????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????");

        num_elem_valid = 0;
        reset_dictionary(&num_elem_valid);

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
                found = BST_search(buffer, root, word_length);

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
        }                                                                                                               //A meno che non sia stato diminuito la match termina

        if (win == false)
            puts("ko");

        while (end == false)
        {
            err_scanf = scanf("%s", buffer);

            if (err_scanf != 1)                                                                                         //Partite terminate, inizio deallocazione strutture
            {
                if (word_length <= ALL_LENG)
                {
                    free_safe_BST(root);
                    for (int i = 0; i < all_num; i++)
                    {
                        free(blocks[i]);
                        blocks[i] = NULL;
                    }
                }
                else
                    free_BST(root);
                free_list();
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
        reset_lists();
    } while (nuova_partita == true);
}
