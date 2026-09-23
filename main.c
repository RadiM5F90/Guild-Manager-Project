#include <stdio.h>
#include "guildManager.h"
#include <string.h>

int main(void) {
    guildManager _manager = guildManagerCreate();
    int choice;

    do {
        printf("\nInizializzazione del programma Guild Manager...\nScegli una delle seguenti opzioni: \n");
        printf("0. Esci dal programma;\n");
        printf("1. Aggiungi una localita o gestisci un collegamento;\n");
        printf("2. Aggiungi un nuovo avventuriero;\n");
        printf("3. Registra una missione;\n");
        printf("4. Verifica un percorso;\n");
        printf("5. Gestisci e completa una missione;\n");
        printf("6. Annulla l'ultima missione completata;\n");
        printf("7. Stampa tutte le missioni disponibili e completate;\n");
        scanf("%d", &choice);

        switch (choice) {
            case 0:
                printf("\nUscendo dal programma...");
                break;
            case 1:
                char risposta[20];
                printf("\nVuoi gestire un collegamento o aggiungere una nuova località? ");
                scanf("%19s", risposta);

                if (strcmp(risposta, "collegamento") == 0 || strcmp(risposta, "Collegamento") == 0) aggiungiCollegamentoLocalita(_manager);
                else if (strcmp(risposta, "localita") == 0 || strcmp(risposta, "località") == 0) aggiungiLocalita(_manager);
                else printf("\nOpzione non valida.");
                break;
            case 2:
                int varAvventuriero = aggiungiAvventuriero(_manager);
                if (varAvventuriero == GM_ERROR_LOCALITA) printf("\nErrore! Località errata o inesistente.");
                break;
            case 3:
                int varRegistraMissione = registraMissione(_manager);
                if (varRegistraMissione == GM_ERROR_LOCALITA) printf("\nErrore! Località errata o inesistente.");
                else if (varRegistraMissione == GM_ERROR_VALUE) printf("\nErrore, valore minimo non valido.");
                else if (varRegistraMissione == GM_ERROR_ENERGY) printf("\nErrore, costo energia non valido.");
                break;
            case 4:
                if (verificaPercorso(_manager) == GM_ERROR_LOCALITA) printf("\nErrore! Località errate o inesistenti.");;
                break;
            case 5:
                if (gestisciProssimaMissione(_manager) == GM_ERROR_MISSION) {
                    printf("\nErrore. Non ci sono missioni da gestire.");
                    break;
                }
                completaMissione(_manager);
                break;
            case 6:
                if (annullaUltimaMissione(_manager) == GM_ERROR_MISSION) printf("\nErrore! Non ci sono missioni da annullare.");
                break;
            case 7:
                if (stampaMissioni(_manager) == GM_ERROR_MISSION) printf("\nErrore! Non ci sono missioni da stampare.");
                break;
            default:
                printf("\nOpzione non valida.");
        }
    } while (choice != 0);

    return 0;
}
