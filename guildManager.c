#include "guildManager.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef enum {DISPONIBILE, IN_MISSIONE, FERITO, A_RIPOSO} statoAvventuriero;
typedef enum {IN_ATTESA, ASSEGNATA, COMPLETATA} statoMissione;
typedef enum {BASSA, MEDIA, ALTA, CRITICA} prioritaMissione;

// Funzione privata
node_id find_node_by_codice(indirect_graph _graph, char* _codice);

struct _guildManager {
    hashmap avventurieri;
    indirect_graph localita;

    queue alta;
    queue critica;
    queue media;
    queue bassa;

    missione missioneAssegnata;         // missione assegnata che deve essere completata
    avventuriero avventurieroAssegnato; // avventuriero assegnato alla missione qui sopra

    stack missioniCompletate;
};

struct _avventuriero {
    char codice[20];
    char nome[50];
    char classe[50];
    int livello;
    int energia;
    char localitaCorrente[50];

    int forza;
    int magia;
    int agilita;

    statoAvventuriero stato;
};

struct _missione {
    char codice[20];
    char nome[50];
    char localita[50];                 // località in cui si svolge la missione
    char caratteristicaRichiesta[20];  // caratteristica richiesta dalla missione (forza, agilita...)
    int valoreMinimo;                  // valore minimo della caratteristica richiesta per eseguire la missione

    int costoEnergia;                  // costo energia per completare la missione (verra sottratto all'avventuriero)

    prioritaMissione priorita;
    statoMissione stato;               // stato = completata, assegnata o in attesa
};

struct _operazione {
    missione missioneCompletata;
    avventuriero avventurieroCoinvolto;

    char vecchioLuogoAvventuriero[50];
    int vecchiaEnergiaAvventuriero;

    statoAvventuriero vecchioStatoAvventuriero;
    statoMissione vecchioStatoMissione;
};

struct _localita {
    char nome[50];
    char codice[5];
};

// F0
guildManager guildManagerCreate() {
    guildManager _manager = malloc(sizeof(struct _guildManager));
    if (_manager == NULL) return NULL;

    _manager->avventurieri = hashmap_create(20);
    _manager->localita = indirect_graph_create(20);

    _manager->alta = create_queue(20);
    _manager->critica = create_queue(20);
    _manager->media = create_queue(20);
    _manager->bassa = create_queue(20);

    _manager->missioneAssegnata = NULL;
    _manager->avventurieroAssegnato = NULL;

    _manager->missioniCompletate = create_stack(20);

    return _manager;
}

// F1
int aggiungiLocalita(guildManager _manager) {
    if (_manager == NULL) return GM_ERROR_NULL;

    localita nuovaLocalita = malloc(sizeof(struct _localita));
    if (nuovaLocalita == NULL) return GM_ERROR_NULL;

    printf("\nAggiungi nuova località:\nInserisci il nome della località: ");
    scanf(" %49[^\n]", nuovaLocalita->nome);

    if (find_node_by_codice(_manager->localita, nuovaLocalita->nome) != INDIRECT_GRAPH_ERROR_INVALID_ID) {
        printf("\nErrore! Località già presente.");
        free(nuovaLocalita);
        return GM_ERROR_LOCALITA;
    }

    printf("\nInserisci il codice della localita: ");
    scanf(" %4s", nuovaLocalita->codice);

    if (find_node_by_codice(_manager->localita, nuovaLocalita->codice) != INDIRECT_GRAPH_ERROR_INVALID_ID) {
        printf("\nErrore! Codice già presente.");
        free(nuovaLocalita);
        return GM_ERROR_LOCALITA;
    }

    // adding the new area to the graph
    add_node(_manager->localita, nuovaLocalita);

    printf("\nNuova località aggiunta con successo: %s %s", nuovaLocalita->codice, nuovaLocalita->nome);
    return GM_SUCCESS;
}

// F2
int aggiungiCollegamentoLocalita(guildManager _manager) {
    if (_manager == NULL) return GM_ERROR_NULL;

    char choice;

    printf("\nScegli un opzione: ");
    printf("\nA. Aggiungi un nuovo collegamento,\nB. Modifica lo stato di un collegamento\n");
    scanf(" %c", &choice);

    if (choice == 'A') {
        char primaLocalita[50];
        char secondaLocalita[50];

        printf("\nAggiungi collegamento tra 2 località:\nInserisci il codice della prima località: ");
        scanf(" %49[^\n]", primaLocalita);

        printf("\nInserisci il codice della seconda località: ");
        scanf(" %49[^\n]", secondaLocalita);

        // seleziona la prima localita dal grafo
        node_id primaLocalitaId = find_node_by_codice(_manager->localita, primaLocalita);

        // seleziona la seconda localita dal grafo
        node_id secondaLocalitaId = find_node_by_codice(_manager->localita, secondaLocalita);

        if (primaLocalitaId == INDIRECT_GRAPH_ERROR_INVALID_ID || secondaLocalitaId == INDIRECT_GRAPH_ERROR_INVALID_ID) {
            printf("\nErrore! Località errate o inesistenti.");
            return GM_ERROR_LOCALITA;
        }

        // void* is a generic pointer, it can point to any data type, use this because the graph is also generic
        // prima_localita stores the address to the object memorized in the node
        // for the compiler prima_localita is only a void*
        // so (localita)prima_localita means that void* is a pointer to localita
        // so _primaLocalita is a variable of type struct localita
        // localita _primaLocalita is equivalent to struct _localita* _primaLocalita
        void* prima_localita;
        indirect_graph_get_value(_manager->localita, primaLocalitaId, &prima_localita);
        localita _primaLocalita = (localita)prima_localita;  // Specific pointer to struct localita

        void* seconda_localita;
        indirect_graph_get_value(_manager->localita, secondaLocalitaId, &seconda_localita);
        localita _secondaLocalita = (localita)seconda_localita;

        // aggiunge il collegamento
        add_edge(_manager->localita, primaLocalitaId, secondaLocalitaId);

        printf("\nCollegamento tra %s %s e %s %s attivato.", _primaLocalita->codice, _primaLocalita->nome, _secondaLocalita->codice, _secondaLocalita->nome);
    }

    else if (choice == 'B') {
        char primaLocalita[50];
        char secondaLocalita[50];

        printf("\nModifica collegamento tra 2 località:\nInserisci il codice della prima località: ");
        scanf(" %49[^\n]", primaLocalita);

        printf("\nInserisci il codice della seconda località: ");
        scanf(" %49[^\n]", secondaLocalita);

        node_id primaLocalitaId = find_node_by_codice(_manager->localita, primaLocalita);

        node_id secondaLocalitaId = find_node_by_codice(_manager->localita, secondaLocalita);

        if (primaLocalitaId == INDIRECT_GRAPH_ERROR_INVALID_ID || secondaLocalitaId == INDIRECT_GRAPH_ERROR_INVALID_ID) {
            printf("\nErrore! Località errate o inesistenti.");
            return GM_ERROR_LOCALITA;
        }

        void* prima_localita;
        indirect_graph_get_value(_manager->localita, primaLocalitaId, &prima_localita);
        localita _primaLocalita = (localita)prima_localita;

        void* seconda_localita;
        indirect_graph_get_value(_manager->localita, secondaLocalitaId, &seconda_localita);
        localita _secondaLocalita = (localita)seconda_localita;

        if (indirect_graph_path_exists(_manager->localita, primaLocalitaId, secondaLocalitaId)) {
            char risposta[10];
            printf("\nIl collegamento tra %s e %s è attivo, vuoi bloccarlo? ", _primaLocalita->codice, _secondaLocalita->codice);
            scanf(" %9[^\n]", risposta);
            if (strcmp(risposta, "Si") == 0 || strcmp(risposta, "si") == 0) {
                remove_edge(_manager->localita, primaLocalitaId, secondaLocalitaId);
                printf("\nCollegamento tra %s %s e %s %s bloccato con successo.", _primaLocalita->codice, _primaLocalita->nome, _secondaLocalita->codice, _secondaLocalita->nome);
            }
            else printf("\nCollegamento non modificato.");
        }
    }

    else {
        printf("\nOpzione non valida.");
        return GM_ERROR_NULL;
    }
    return GM_SUCCESS;
}

// F3
int verificaPercorso(guildManager _manager) {
    if (_manager == NULL) return GM_ERROR_NULL;

    char primaLocalita[50];
    char secondaLocalita[50];

    printf("\nInserisci il codice della prima località: ");
    scanf(" %49[^\n]", primaLocalita);

    printf("\nInserisci il codice della seconda località: ");
    scanf(" %49[^\n]", secondaLocalita);

    node_id primaLocalitaId = find_node_by_codice(_manager->localita, primaLocalita);

    node_id secondaLocalitaId = find_node_by_codice(_manager->localita, secondaLocalita);

    if (primaLocalitaId == INDIRECT_GRAPH_ERROR_INVALID_ID || secondaLocalitaId == INDIRECT_GRAPH_ERROR_INVALID_ID) return GM_ERROR_LOCALITA;

    // Recupera le localita (opzionale)
    void* prima_localita;
    indirect_graph_get_value(_manager->localita, primaLocalitaId, &prima_localita);
    localita _primaLocalita = (localita)prima_localita;

    // indirect_graph_get_value() serve se dopo aver trovato il nodo, si volesse accedere ai dati della localita (_secondaLocalita->nome)
    void* seconda_localita;
    indirect_graph_get_value(_manager->localita, secondaLocalitaId, &seconda_localita);
    localita _secondaLocalita = (localita)seconda_localita;

    // Verifica il percorso tra le 2 localita
    if(indirect_graph_path_exists(_manager->localita, primaLocalitaId, secondaLocalitaId))
        printf("\nIl percorso tra %s e %s esiste (attivo).", primaLocalita, secondaLocalita);

    else printf("\nIl percorso tra %s e %s non esiste (non attivo).", primaLocalita, secondaLocalita);

    return GM_SUCCESS;
}

// F4
int aggiungiAvventuriero(guildManager _manager) {
    if (_manager == NULL) return GM_ERROR_NULL;

    char _stato[20];

    avventuriero nuovoAvventuriero = malloc(sizeof(struct _avventuriero));
    if (nuovoAvventuriero == NULL) return GM_ERROR_ALLOC;

    printf("\nAggiungi nuovo avventuriero:\nInserisci codice avventuriero: ");
    scanf("%19s", nuovoAvventuriero->codice);

    if (hashmap_has_key(_manager->avventurieri, nuovoAvventuriero->codice)) {
        printf("\nErrore, avventuriero già presente.");
        free(nuovoAvventuriero);
        return GM_ERROR_NULL;
    }

    printf("\nInserisci il nome: ");
    scanf(" %49[^\n]", nuovoAvventuriero->nome);

    printf("\nInserisci la classe: ");
    scanf(" %49[^\n]", nuovoAvventuriero->classe);

    printf("\nInserisci località corrente: ");
    scanf(" %49[^\n]", nuovoAvventuriero->localitaCorrente);

    if (find_node_by_codice(_manager->localita, nuovoAvventuriero->localitaCorrente) == INDIRECT_GRAPH_ERROR_INVALID_ID) {
        free(nuovoAvventuriero);
        return GM_ERROR_LOCALITA;
    }

    printf("\nInserisci il livello: ");
    scanf("%d", &nuovoAvventuriero->livello);

    printf("\nInserisci l'energia: ");
    scanf("%d", &nuovoAvventuriero->energia);

    printf("\nInserisci la forza: ");
    scanf("%d", &nuovoAvventuriero->forza);

    printf("\nInserisci la magia: ");
    scanf("%d", &nuovoAvventuriero->magia);

    printf("\nInserisci l'agilità: ");
    scanf("%d", &nuovoAvventuriero->agilita);

    // disponibile, in missione, ferito, a riposo
    printf("\nInserisci lo stato dell'avventuriero: ");
    scanf(" %19[^\n]", _stato);

    if (strcmp(_stato, "disponibile") == 0) nuovoAvventuriero->stato = DISPONIBILE;
    else if (strcmp(_stato, "in missione") == 0) nuovoAvventuriero->stato = IN_MISSIONE;
    else if (strcmp(_stato, "ferito") == 0) nuovoAvventuriero->stato = FERITO;
    else if (strcmp(_stato, "a riposo") == 0) nuovoAvventuriero->stato = A_RIPOSO;

    // Aggiunge l'avventuriero alla hashmap
    hashmap_set(_manager->avventurieri, nuovoAvventuriero->codice, nuovoAvventuriero);

    printf("\nNuovo avventuriero aggiunto correttamente: %s\n", nuovoAvventuriero->nome);

    return GM_SUCCESS;
}

// F5
int registraMissione(guildManager _manager) {
    if (_manager == NULL) return GM_ERROR_NULL;

    char priorita[20];

    missione nuovaMissione = malloc(sizeof(struct _missione));
    if (nuovaMissione == NULL) return GM_ERROR_ALLOC;

    printf("\nInserisci codice missione: ");
    scanf(" %19[^\n]", nuovaMissione->codice);

    printf("\nInserisci nome missione: ");
    scanf(" %49[^\n]", nuovaMissione->nome);

    printf("\nInserisci località missione: ");
    scanf(" %49[^\n]", nuovaMissione->localita);

    // Se non trova la localita
    if (find_node_by_codice(_manager->localita, nuovaMissione->localita) == INDIRECT_GRAPH_ERROR_INVALID_ID) {
        free(nuovaMissione);
        return GM_ERROR_LOCALITA;
    }

    printf("\nInserisci caratteristica richiesta: ");
    scanf(" %19[^\n]", nuovaMissione->caratteristicaRichiesta);

    printf("\nInserisci valore minimo: ");
    scanf("%d", &nuovaMissione->valoreMinimo);

    if (nuovaMissione->valoreMinimo <= 0) {
        free(nuovaMissione);
        return GM_ERROR_VALUE;
    }

    printf("\nInserisci costo energia: ");
    scanf("%d", &nuovaMissione->costoEnergia);

    if (nuovaMissione->costoEnergia <= 0) {
        free(nuovaMissione);
        return GM_ERROR_ENERGY;
    }

    printf("\nInserisci priorità: ");
    scanf(" %19s", priorita);

    if (strcmp(priorita, "critica") == 0) {
        nuovaMissione->priorita = CRITICA;
        enqueue(_manager->critica, nuovaMissione);
    }
    else if (strcmp(priorita, "alta") == 0) {
        nuovaMissione->priorita = ALTA;
        enqueue(_manager->alta, nuovaMissione);
    }
    else if (strcmp(priorita, "media") == 0) {
        nuovaMissione->priorita = MEDIA;
        enqueue(_manager->media, nuovaMissione);
    }
    else if (strcmp(priorita, "bassa") == 0) {
        nuovaMissione->priorita = BASSA;
        enqueue(_manager->bassa, nuovaMissione);
    }

    nuovaMissione->stato = IN_ATTESA;

    printf("\nNuova missione registrata correttamente: %s\n", nuovaMissione->nome);
    return GM_SUCCESS;
}

// F6
int gestisciProssimaMissione(guildManager _manager) {
    if (_manager == NULL) return GM_ERROR_NULL;
    // Puntatore a struct missione, inizializzato a NULL
    // Serve ad accedere ai dati della missione (_missione->codice ecc...)
    missione _missione = NULL;

    // Legge (senza rimuovere) la prossima missione da eseguire da una delle queue
    if (!queue_is_empty(_manager->critica)) queue_peek(_manager->critica, (void**)&_missione);
    else if (!queue_is_empty(_manager->alta)) queue_peek(_manager->alta, (void**)&_missione);
    else if (!queue_is_empty(_manager->media)) queue_peek(_manager->media, (void**)&_missione);
    else if (!queue_is_empty(_manager->bassa)) queue_peek(_manager->bassa, (void**)&_missione);
    else return GM_ERROR_MISSION;


    node_id localitaMissioneID = find_node_by_codice(_manager->localita, _missione->localita);

    printf("\nProssima Missione: %s", _missione->nome);

    // Variabile che conterra' l'avventuriero migliore selezionato
    avventuriero avventurieroSelezionato = NULL;

    // Variabile che serve a ricordare il valore della caratteristica dell'avventuriero migliore trovato
    // se la missione richiede "forza", la forza dell'avventuriero migliore viene memorizzata qui
    // inizializzato a -1 perche' non e' stato trovato ancora nessun avventuriero, e' solo un valore iniziale
    int valoreMaggiore = -1;

    // serve per l'avventuriero che avra' il livello maggiore tra quelli idonei
    int livelloMaggiore = -1;


    list keys = hashmap_get_keys(_manager->avventurieri);
    iterator it = list_iterator_create(keys);

    char caratteristica[20];
    strcpy(caratteristica, _missione->caratteristicaRichiesta);

    // Controlla tutti gli avventurieri
    // Per ogni codice avventuriero recupera l'avventuriero con hashmap_get
    // Scorre tutti i codici
    while (iterator_has_next(it)) {
        // significa "tratta questo void* come un puntatore a char"
        char* codice = (char*)iterator_next(it);
        void* value = NULL;

        int idoneo = 1;

        // -1 e' solo un valore iniziale, prima di calcolare realmente il risultato
        // conterra' il valore della caratteristica richiesta dalla missione per l'avventuriero corrente
        int valoreCaratteristica = -1;

        node_id localitaCorrenteID = INDIRECT_GRAPH_ERROR_INVALID_ID;

        // Avventuriero che si sta controllando in questo momento
        // All'inizio non abbiamo ancora recuperato l'avventuriero, quindi viene inizializzato a NULL
        avventuriero currentAvventuriero = NULL;

        // Recupera l'avventuriero dalla hashmap
        // codice e' la chiave appena presa dalla lista
        if (hashmap_get(_manager->avventurieri, codice, &value) == HASHMAP_SUCCESS) {
            // Trasforma il void* generico ottenuto dalla hashmap in un puntatore del tipo avventuriero
            // Peche' bisogna trasformare il void*? perche' con void* non si puo' fare currentAvventuriero->nome ecc...
            currentAvventuriero = (avventuriero)value;

            printf("\nControllo avventuriero: %s", currentAvventuriero->nome);

            if (currentAvventuriero->stato != DISPONIBILE) {
                printf(" -> non disponibile");
                idoneo = 0;
            }

            if (idoneo == 1) {
                if (currentAvventuriero->energia < _missione->costoEnergia) {
                    printf(" -> energia insufficiente");
                    idoneo = 0;
                }
            }

            if (idoneo == 1) {
                if (strcmp(caratteristica, "forza") == 0) valoreCaratteristica = currentAvventuriero->forza;
                else if (strcmp(caratteristica, "magia") == 0) valoreCaratteristica = currentAvventuriero->magia;
                else if (strcmp(caratteristica, "agilita") == 0 || strcmp(caratteristica, "agilità") == 0) valoreCaratteristica = currentAvventuriero->agilita;
                else {
                    printf(" -> caratteristica non riconosciuta: %s", caratteristica);
                    idoneo = 0;
                }
            }

            // Controlla il valore minimo
            if (idoneo == 1) {
                if (valoreCaratteristica < _missione->valoreMinimo) {
                    printf(" -> caratteristica insufficiente");
                    idoneo = 0;
                }
            }

            // Individua la localita corrente
            if (idoneo == 1) {
                localitaCorrenteID = find_node_by_codice(_manager->localita, currentAvventuriero->localitaCorrente);

                if (localitaCorrenteID == INDIRECT_GRAPH_ERROR_INVALID_ID) {
                    printf(" -> Localita corrente non trovata: %s", currentAvventuriero->localitaCorrente);
                    idoneo = 0;
                }
            }

            // Controlla il percorso attivo
            if (idoneo == 1) {
                if (localitaCorrenteID != localitaMissioneID) {
                    if (!indirect_graph_path_exists(_manager->localita, localitaCorrenteID, localitaMissioneID)) {
                        printf(" -> Nessun percorso attivo");
                        idoneo = 0;
                    }
                }
            }

            // Avventuriero idoneo
             if (idoneo == 1) {
                 printf(" -> idoneo");

                 // seleziona l'avventuriero migliore tra quelli idonei
                 if ((valoreCaratteristica > valoreMaggiore) || ((valoreCaratteristica == valoreMaggiore) && (currentAvventuriero->livello > livelloMaggiore))) {
                     avventurieroSelezionato = currentAvventuriero;
                     valoreMaggiore = valoreCaratteristica;
                     livelloMaggiore = currentAvventuriero->livello;
                 }
             }
        }
    }
    iterator_destroy(&it);
    list_destroy(&keys);

    // Se nessun avventuriero è idoneo
    if (avventurieroSelezionato == NULL) {
        printf("\nErrore! Nessun avventuriero è idoneo per questa missione.");
        return GM_ERROR_MISSION;
    }

    // Una volta trovato l'avventuriero adatto, la missione può essere estratta dalla queue
    if (_missione->priorita == CRITICA) dequeue(_manager->critica, (void**)&_missione);
    else if (_missione->priorita == ALTA) dequeue(_manager->alta, (void**)&_missione);
    else if (_missione->priorita == MEDIA) dequeue(_manager->media, (void**)&_missione);
    else if (_missione->priorita == BASSA) dequeue(_manager->bassa, (void**)&_missione);

    _missione->stato = ASSEGNATA;
    avventurieroSelezionato->stato = IN_MISSIONE;

    // salva missione e avventuriero nel guildManager
    _manager->missioneAssegnata = _missione;
    _manager->avventurieroAssegnato = avventurieroSelezionato;

    printf("\nMissione assegnata con successo.");
    printf("\nMissione: %s | Avventuriero: %s", _missione->nome, avventurieroSelezionato->nome);

    return GM_SUCCESS;
}

// F7
int completaMissione(guildManager _manager) {
    if (_manager == NULL || _manager->missioneAssegnata == NULL || _manager->avventurieroAssegnato == NULL) return GM_ERROR_NULL;

    // Recupera missione e avventuriero
    missione _missione = _manager->missioneAssegnata;
    avventuriero _avventuriero = _manager->avventurieroAssegnato;

    operazione nuovaOperazione = malloc(sizeof(struct _operazione));
    if (nuovaOperazione == NULL) return GM_ERROR_ALLOC;

    // Salva lo stato precedente prima di completare la missione
    nuovaOperazione->missioneCompletata = _missione;
    nuovaOperazione->avventurieroCoinvolto = _avventuriero;

    strcpy(nuovaOperazione->vecchioLuogoAvventuriero, _avventuriero->localitaCorrente);

    nuovaOperazione->vecchiaEnergiaAvventuriero = _avventuriero->energia;
    nuovaOperazione->vecchioStatoAvventuriero = _avventuriero->stato;
    nuovaOperazione->vecchioStatoMissione = _missione->stato;

    // Aggiorna i dati
    strcpy(_avventuriero->localitaCorrente, _missione->localita); // Sposta l'avventuriero nella nuova località (se necessario)
    printf("\n%s spostato dalla località %s a %s", _avventuriero->nome, nuovaOperazione->vecchioLuogoAvventuriero, _avventuriero->localitaCorrente);
    _avventuriero->energia -= _missione->costoEnergia;

    if (_avventuriero->energia < 2) _avventuriero->stato = A_RIPOSO;
    else _avventuriero->stato = DISPONIBILE;

    _missione->stato = COMPLETATA;

    // Inserisce l'operazione nello stack
    stack_push(_manager->missioniCompletate, nuovaOperazione);

    printf("\nMissione %s completata con successo da %s.",_missione->nome, nuovaOperazione->avventurieroCoinvolto->nome);

    _manager->missioneAssegnata = NULL;
    _manager->avventurieroAssegnato = NULL;

    return GM_SUCCESS;
}

// F8
int annullaUltimaMissione(guildManager _manager) {
    if (_manager == NULL) return GM_ERROR_NULL;

    if (stack_is_empty(_manager->missioniCompletate)) return GM_ERROR_MISSION;

    void* vecchiaOperazione = NULL;
    // void** perche' lo stack_pop() non vuole solo il valore da restituire, ma anche l'indirizzo di una variabile in cui poter
    // scrivere il valore estratto dallo stack
    // anche perche' stack_pop usa void** _value_out
    // il doppio puntatore serve perche' stiamo passando l'indirizzo di una variabile che anche lei e' un puntatore
    stack_pop(_manager->missioniCompletate, (void**)&vecchiaOperazione);

    operazione daAnnullare = (operazione)vecchiaOperazione;

    strcpy(daAnnullare->avventurieroCoinvolto->localitaCorrente, daAnnullare->vecchioLuogoAvventuriero);
    daAnnullare->avventurieroCoinvolto->energia = daAnnullare->vecchiaEnergiaAvventuriero;
    daAnnullare->avventurieroCoinvolto->stato = daAnnullare->vecchioStatoAvventuriero;
    daAnnullare->missioneCompletata->stato = daAnnullare->vecchioStatoMissione;

    printf("\nUltima missione annullata con successo.");
    free(daAnnullare);
    return GM_SUCCESS;
}

// F9
int stampaMissioni(guildManager _manager) {
    if (_manager == NULL) return GM_ERROR_NULL;

    // Numero totale di missioni che F9 ha trovato nelle queue e nello stack
    int numeroMissioni = 0;

    printf("\nStampa missioni assegnate:");

    // Priorità critica
    if (!queue_is_empty(_manager->critica)) {
        int dimensione = queue_size(_manager->critica);
        queue temporanea = create_queue(dimensione);

        printf("\nMissioni con priorità critica: ");

        for (int i = 0; i < dimensione; i++) {
            // Toglie la missione dalla queue
            void* value = NULL;
            dequeue(_manager->critica, &value);

            missione _missione = (missione)value;
            printf("%s %s, ", _missione->codice, _missione->nome);

            // Inserisce la missione nella queue temporanea, per mantenere l'ordine
            enqueue(temporanea, value);
        }
        while (!queue_is_empty(temporanea)) {
            void* value = NULL;
            dequeue(temporanea, &value);
            enqueue(_manager->critica, value);
        }
        destroy_queue(&temporanea);
        numeroMissioni += dimensione;
    }

    // Priorità alta
    if (!queue_is_empty(_manager->alta)) {
        int dimensione = queue_size(_manager->alta);
        queue temporanea = create_queue(dimensione);

        printf("\nMissioni con priorità alta: ");

        for (int i = 0; i < dimensione; i++) {
            void* value = NULL;
            dequeue(_manager->alta, &value);

            missione _missione = (missione)value;
            printf("%s %s, ", _missione->codice, _missione->nome);

            enqueue(temporanea, value);
        }
        while (!queue_is_empty(temporanea)) {
            void* value = NULL;
            dequeue(temporanea, &value);
            enqueue(_manager->alta, value);
        }
        destroy_queue(&temporanea);
        numeroMissioni += dimensione;
    }

    // Priorità media
    if (!queue_is_empty(_manager->media)) {
        printf("\nMissioni con priorità media: ");

        int dimensione = queue_size(_manager->media);
        queue temporanea = create_queue(dimensione);

        for (int i = 0; i < dimensione; i++) {
            void* value = NULL;
            dequeue(_manager->media, &value);

            missione _missione = (missione)value;
            printf("%s %s, ", _missione->codice, _missione->nome);

            enqueue(temporanea, value);
        }
        while (!queue_is_empty(temporanea)) {
            void* value = NULL;
            dequeue(temporanea, &value);
            enqueue(_manager->media, value);
        }
        destroy_queue(&temporanea);
        numeroMissioni += dimensione;
    }

    // Priorità bassa
    if (!queue_is_empty(_manager->bassa)) {
        int dimensione = queue_size(_manager->bassa);
        queue temporanea = create_queue(dimensione);

        printf("\nMissioni con priorità bassa: ");

        for (int i = 0; i < dimensione; i++) {
            void* value = NULL;
            dequeue(_manager->bassa, &value);

            missione _missione = (missione)value;
            printf("%s %s, ", _missione->codice, _missione->nome);

            enqueue(temporanea, value);
        }
        while (!queue_is_empty(temporanea)) {
            void* value = NULL;
            dequeue(temporanea, &value);
            enqueue(_manager->bassa, value);
        }
        destroy_queue(&temporanea);
        numeroMissioni += dimensione;
    }

    // Missioni completate
    if (!stack_is_empty(_manager->missioniCompletate)) {
        int dimensione = stack_size(_manager->missioniCompletate);

        stack temp1 = create_stack(dimensione);
        stack temp2 = create_stack(dimensione);

        printf("\nMissioni completate: ");

        // Estrae tutte le operazioni dallo stack originale, stampa la missione completata e la salva in temp1
        while (!stack_is_empty(_manager->missioniCompletate)) {
            void* value = NULL;
            stack_pop(_manager->missioniCompletate, &value);

            // operazione perché nello stack viene salvata un'operazione e non direttamente una missione
            operazione _operazione = (operazione)value;
            missione _missione = _operazione->missioneCompletata;
            printf("%s %s, ", _missione->codice, _missione->nome);

            stack_push(temp1, value);
        }

        // Sposta da temp1 a temp2
        while (!stack_is_empty(temp1)) {
            void* value = NULL;

            stack_pop(temp1, &value);
            stack_push(temp2, value);
        }

        // Sposta da temp2 allo stack iniziale, per riottenere l'ordine originale delle missioni completate
        while (!stack_is_empty(temp2)) {
            void* value = NULL;

            stack_pop(temp2, &value);
            stack_push(_manager->missioniCompletate, value);
        }
        destroy_stack(&temp1);
        destroy_stack(&temp2);

        numeroMissioni += dimensione;
    }

    if (numeroMissioni == 0) return GM_ERROR_MISSION;

    return GM_SUCCESS;
}

// Funzione privata
// La funzione restituisce l'ID del nodo, non la localita'
node_id find_node_by_codice(indirect_graph _graph, char* _codice) {
    if (_graph == NULL || _codice == NULL) return INDIRECT_GRAPH_ERROR_INVALID_ID;

    int dimensione = indirect_graph_size(_graph);

    // scorre tutti i nodi del grafo, partendo dal primo nodo
    for (int i = 0; i < dimensione; i++) {
        // value andra' a contenere l'indirizzo di una localita
        // = NULL perche' inizialmente non abbiamo trovato ancora nessun valore
        void* value = NULL;

        // recupera il valore del nodo i
        // continue -> se il nodo non puo' essere usato, smette di lavorare su questo giro e passa al prossimo i
        // quindi se restituisce errore passa al prossimo
        // se restituisce INDIRECT_GRAPH_SUCCESS significa che ha trovato il nodo e ha recuperato il valore
        if (indirect_graph_get_value(_graph, i, &value) != INDIRECT_GRAPH_SUCCESS) continue;

        // converte value in tipo struct localita
        // tratta il void* come un puntatore a localita, cosi' si puo' accedere ai dati della localita (corrente->codice)
        localita corrente = (localita)value;

        // confronta il codice della localita trovata con il codice passato come parametro
        // restituisce i perche i e' l'ID del nodo che si sta controllando
        // l'ID del nodo che contiene quella localita
        if (strcmp(corrente->codice, _codice) == 0) return i;
    }
    return INDIRECT_GRAPH_ERROR_INVALID_ID;
}
