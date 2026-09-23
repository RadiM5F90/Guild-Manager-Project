
#include "undirected_graph.h"
#include "list_queue.h"
#include "list_stack.h"
#include "hashmap.h"

#define GM_SUCCESS 0
#define GM_ERROR_NULL -1
#define GM_ERROR_LOCALITA -2
#define GM_ERROR_VALUE -3
#define GM_ERROR_ENERGY -4
#define GM_ERROR_MISSION -5
#define GM_ERROR_ALLOC -6

typedef struct _guildManager* guildManager;
typedef struct _avventuriero* avventuriero;
typedef struct _missione* missione;
typedef struct _operazione* operazione;
typedef struct _localita* localita;


/* F0
 * Crea un nuovo gestore della Gilda.
 * @return puntatore al nuovo gestore creato.
 *         o NULL se il nuovo gestore creato è NULL.
 */
guildManager guildManagerCreate();

/* F1
 * Aggiunge una località al guildManager
 * @param _manager Manager in cui aggiungere la nuova località.
 * @return GM_ERROR_NULL se _manager è NULL,
 *         GM_ERROR_LOCALITA se la località è già presente,
 *         GM_SUCCESS se l'operazione riesce.
 */
int aggiungiLocalita(guildManager _manager);


/* F2
 * Aggiunge un collegamento tra 2 località esistenti.
 * @param _manager Manager in cui eseguire l'operazione.
 * @return GM_ERROR_NULL se _manager è NULL,
 *         GM_ERROR_LOCALITA se una delle località non esiste,
 *         GM_SUCCESS se l'operazione riesce.
 */
int aggiungiCollegamentoLocalita(guildManager _manager);

/* F3
 * Verifica un percorso composto da collegamenti attivi tra 2 località.
 * @param _manager Manager in cui eseguire l'operazione.
 * @return GM_ERROR_NULL se _manager è NULL,
 *         GM_ERROR_LOCALITA se una delle località non esiste,
 *         GM_SUCCESS se l'operazione riesce.
 */
int verificaPercorso(guildManager _manager);

/* F4
 * Aggiunge un nuovo avventuriero specificandone tutte le caratteristiche.
 * @param _manager Manager in cui eseguire l'operazione.
 * @return GM_ERROR_NULL se _manager è NULL,
 *         GM_ERROR_LOCALITA se la località iniziale non esiste,
 *         GM_SUCCESS se l'operazione riesce.
 */
int aggiungiAvventuriero(guildManager _manager);

/* F5
 * Registra una nuova missione specificandone le caratteristiche.
 * @param _manager Manager in cui eseguire l'operazione.
 * @return GM_ERROR_NULL se _manager è NULL,
 *         GM_ERROR_LOCALITA se la localita non esiste,
 *         GM_ERROR_VALUE se il valore minimo è < 0,
 *         GM_ERROR_ENERGY se il costo energetico è < 0,
 *         GM_SUCCESS se l'operazione riesce.
 */
int registraMissione(guildManager _manager);

/* F6
 * Individua la prossima missione da eseguire e l'avventuriero adatto.
 * @param _manager Manager in cui eseguire l'operazione.
 * @return GM_ERROR_NULL se _manager è NULL,
 *         GM_ERROR_MISSION se non ci sono missioni da eseguire
 *         GM_SUCCESS se l'operazione riesce.
 */
int gestisciProssimaMissione(guildManager _manager);

/* F7
 * Completa una missione aggiornando i suoi dati e quelli dell'avventuriero.
 * @param _manager Manager in cui eseguire l'operazione.
 * @return GM_ERROR_NULL se _manager è NULL,
 *         GM_SUCCESS se l'operazione riesce.
 */
int completaMissione(guildManager _manager);

/* F8
 * Annulla l'ultima missione effettuata.
 * @param _manager Manager in cui eseguire l'operazione.
 * @return GM_ERROR_NULL se _manager è NULL,
 *         GM_SUCCESS se l'operazione riesce.
 */
int annullaUltimaMissione(guildManager _manager);

/* F9 Funzionalità personalizzata
 * Stampa tutte le missioni da eseguire e quelle già eseguite.
 * @param _manager Manager in cui eseguire l'operazione.
 * @return GM_ERROR_NULL se _manager è NULL,
 *         GM_ERROR_MISSION se non ci sono missioni da stampare,
 *         GM_SUCCESS se l'operazione riesce.
 */
int stampaMissioni(guildManager _manager);
