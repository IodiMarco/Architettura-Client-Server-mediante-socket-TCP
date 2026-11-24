/* ------------------------------------------------------------------------------------------- */
								/* Modulo Gestione del Server */
/* ------------------------------------------------------------------------------------------- */

#include "protocol.h"
#include <time.h> //necessario per la funzione srand()

/* ------------------------------------------------------------------------------------------- */
									/* Funzioni Meteo */
/* ------------------------------------------------------------------------------------------- */

float get_temperature(void)
{
	return -10.0f + (rand() / (float)RAND_MAX) * 50.0f;
}
float get_humidity(void)
{
	return 20.0f + (rand() / (float)RAND_MAX) * 80.0f;
}
float get_wind(void)
{
	return (rand() / (float)RAND_MAX) * 100.0f;
}
float get_pressure(void)
{
	return 950.0f + (rand() / (float)RAND_MAX) * 100.0f;
}

/* ------------------------------------------------------------------------------------------- */
										/* Ricerca Città */
/* ------------------------------------------------------------------------------------------- */

const char* supported_cities[] =
{
    "Bari","Roma","Milano","Napoli","Torino",
    "Palermo","Genova","Bologna","Firenze","Venezia"
};

/* ------------------------------------------------------------------------------------------------------- */
/* Verifica se la città inserita è presente nell’array, ignorando la differenza tra maiuscole e minuscole. */
/* ------------------------------------------------------------------------------------------------------- */

int is_supported_city(const char* city) {
    for (int i = 0; i < 10; i++) {
        const char* c = supported_cities[i];
        int match = 1;

        size_t len_c = strlen(c);
        size_t len_city = strlen(city);
        if (len_c != len_city) continue;

        for (size_t j = 0; j < len_c; j++) {
            if (tolower(c[j]) != tolower(city[j])) {
                match = 0;
                break;
            }
        }
        if (match) return 1;
    }
    return 0;
}

/* ------------------------------------------------------------------------------------------- */
							/* Main e inizializzazione della socket */
/* ------------------------------------------------------------------------------------------- */

int main(int argc, char* argv[]) {

    srand(time(NULL)); 		/* serve per la generazione dei dati  */
    int port = SERVER_PORT; /* numero di porta inserito in protocol.h */

    /* Permette di usare una porta specifica (invece di quella di default 56700) */
    if (argc == 3 && strcmp(argv[1], "-p") == 0)
        port = atoi(argv[2]);

    // --- Inizializzazione socket su Windows ---
#if defined WIN32
    WSADATA wsa_data;
    int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (result != NO_ERROR) {
        printf("Errore in WSAStartup()\n");
        return 0;
    }
#endif

//---------------------------------------------------------------------------------------------------------------------------------------//

    //Creazione della socket "socket_server"
    SOCKET socket_server;
    socket_server = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (socket_server == INVALID_SOCKET) {
        printf("Creazione della Socket fallita.\n");
        clearwinsock();
        return -1;
    }

//---------------------------------------------------------------------------------------------------------------------------------------//

    //Assegnazione Indirizzo Socket Server
    struct sockaddr_in indirizzo_socket;
    memset(&indirizzo_socket, 0, sizeof(indirizzo_socket));
    indirizzo_socket.sin_family = AF_INET;
    // CORREZIONE BUG: Usa la variabile 'port' (default o da riga di comando)
    indirizzo_socket.sin_port = htons(port);
    indirizzo_socket.sin_addr.s_addr = INADDR_ANY;

//---------------------------------------------------------------------------------------------------------------------------------------//

    // Associazione (bind) della socket all'indirizzo
    if (bind(socket_server, (struct sockaddr*)&indirizzo_socket, sizeof(indirizzo_socket)) < 0) {
        printf("Bind fallito\n");
        closesocket(socket_server);
        clearwinsock();
        return -1;
    }

//---------------------------------------------------------------------------------------------------------------------------------------//

    // Messa in ascolto della socket
    if (listen(socket_server, NUM_MAX_CONN) < 0) {
        printf("Listen fallita\n");
        closesocket(socket_server);
        clearwinsock();
        return -1;
    }

    // Stampa la porta effettivamente usata
    printf("Server in ascolto sulla porta %d...\n", port);

//---------------------------------------------------------------------------------------------------------------------------------------//

    // Loop principale del server
    while (1) {
        struct sockaddr_in addr_client;

    // ---- Gestione portabile di socklen_t ----
		#ifdef _WIN32
				int len = sizeof(addr_client);
		#else
				socklen_t len = sizeof(addr_client);
		#endif
   // -------------------------------------------

        // Accettazione connessione dal client
        SOCKET socket_client = accept(socket_server, (struct sockaddr*)&addr_client, &len);

        if (socket_client == INVALID_SOCKET) {
            printf("Accept fallita\n");
            continue;
        }

        // Ricezione dati dal client
        weather_request_t request;
        int n = recv(socket_client, (char*)&request, sizeof(request), 0);
        if (n <= 0) {
            printf("Errore in recv() o connessione chiusa dal client\n");
            closesocket(socket_client);
            continue;
        }

        printf("Richiesta '%c %s' dal client ip %s\n",
            request.type, request.city, inet_ntoa(addr_client.sin_addr));

        // Preparazione ed elaborazione risposta
        weather_response_t response;
        memset(&response, 0, sizeof(response));

        if (request.type != TYPE_TEMPERATURE &&
            request.type != TYPE_HUMIDITY &&
            request.type != TYPE_WIND &&
            request.type != TYPE_PRESSURE) {
            response.status = STATUS_INVALID_REQUEST;
        }
        else if (!is_supported_city(request.city)) {
            response.status = STATUS_CITY_NOT_FOUND;
        }
        else {
            response.status = STATUS_OK;
            response.type = request.type;
            switch (request.type) {
            case TYPE_TEMPERATURE: response.value = get_temperature(); break;
            case TYPE_HUMIDITY:    response.value = get_humidity();    break;
            case TYPE_WIND:        response.value = get_wind();        break;
            case TYPE_PRESSURE:    response.value = get_pressure();    break;
            }
        }

        // Invio di dati al Client (la risposta)
        if (send(socket_client, (char*)&response, sizeof(response), 0) != sizeof(response))
        {
            printf("Errore: messaggio di risposta non recapitato.\n");
        }

        // Chiusura socket client
        closesocket(socket_client);

    }

//---------------------------------------------------------------------------------------------------------------------------------------//

    // --- Chiusura socket ---
    closesocket(socket_server);

    // --- Chiusura socket su Windows ---
    clearwinsock();

    return 0;
}
