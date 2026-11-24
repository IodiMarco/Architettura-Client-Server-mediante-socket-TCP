/* ------------------------------------------------------------------------------------------- */
								/* Modulo Gestione del Client */
/* ------------------------------------------------------------------------------------------- */

#include "protocol.h"

/* ------------------------------------------------------------------------------------------- */
							/* Main e inizializzazione della socket */
/* ------------------------------------------------------------------------------------------- */

int main(int argc, char* argv[]) {

	int server_port = SERVER_PORT; // inizializzata al valore di default
    char* ip_server = "127.0.0.1"; //indirizzo ip per la connessione del server
    char type = 0; //inizializzazione variabile per l'inserimento del tipo (es. t)
    char city[MAX_CITY_LEN] = ""; //inizializzazione variabile per l'inserimento della città (es. Bari)

/* ------------------------------------------------------------------------------------------- */
    					/* Gestione della ricerca del tipo per città */
/* ------------------------------------------------------------------------------------------- */

    // Parsing argomenti
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) ip_server = argv[++i];
        else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) server_port = atoi(argv[++i]);
        else if (strcmp(argv[i], "-r") == 0 && i + 1 < argc) {
            if (strlen(argv[i + 1]) < 2) { printf("Richiesta non valida\n"); return -1; }
            type = argv[i + 1][0];
            strncpy(city, argv[i + 1] + 2, MAX_CITY_LEN - 1);
            city[MAX_CITY_LEN - 1] = '\0';
        }
    }

    // Validazione argomenti
    if (!type || strlen(city) == 0) {
        printf("Uso corretto: -r \"type city\"\n");
        return -1;
    }


    // --- Inizializzazione socket su Windows ---
#if defined WIN32
    WSADATA wsa_data;
    int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (result != NO_ERROR) // NO_ERROR è definito in protocol.h
    {
        printf("Errore in WSAStartup()\n");
        return 0;
    }
#endif

//---------------------------------------------------------------------------------------------------------------------------------------//

    //Creazione della socket "socket_client"
    SOCKET socket_client;
    socket_client = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (socket_client == INVALID_SOCKET)
    {
        printf("Creazione della Socket fallita.\n");
        // N.B. non serve closesocket se la creazione fallisce
        clearwinsock(); // Pulizia
        return -1;
    }

//---------------------------------------------------------------------------------------------------------------------------------------//

    //Assegnazione Indirizzo Socket Server
    struct sockaddr_in indirizzo_socket;
    memset(&indirizzo_socket, 0, sizeof(indirizzo_socket));
    indirizzo_socket.sin_family = AF_INET;

    indirizzo_socket.sin_addr.s_addr = inet_addr(ip_server);
    indirizzo_socket.sin_port = htons(server_port);

    printf("Connessione al server %s sulla porta %d...\n", ip_server, server_port);


//---------------------------------------------------------------------------------------------------------------------------------------//

    // Connessione con il Server
    if (connect(socket_client, (struct sockaddr*)&indirizzo_socket, sizeof(indirizzo_socket)) < 0)
    {
        printf("Connessione fallita.\n");
        closesocket(socket_client);
        clearwinsock(); // Pulizia
        return -1;
    }

//---------------------------------------------------------------------------------------------------------------------------------------//

    // Invio di dati al Server (Logica Weather Client)
    weather_request_t request;
    request.type = type;
    strncpy(request.city, city, MAX_CITY_LEN - 1);
    request.city[MAX_CITY_LEN - 1] = '\0';

    if (send(socket_client, (char*)&request, sizeof(request), 0) != sizeof(request))
    {
        printf("Errore: messaggio (richiesta) non recapitato.\n");
        closesocket(socket_client);
        clearwinsock(); // Pulizia
        return -1;
    }

//---------------------------------------------------------------------------------------------------------------------------------------//

    // Ricezione dati dal Server (Logica Weather Client)
    weather_response_t response;
    int n = recv(socket_client, (char*)&response, sizeof(response), 0);
    if (n <= 0) {
        printf("Errore ricezione o connessione chiusa dal server\n");
        closesocket(socket_client);
        clearwinsock(); // Pulizia
        return -1;
    }

//---------------------------------------------------------------------------------------------------------------------------------------//

/* ------------------------------------------------------------------------------------------- */
    						/* Stampa il risultato mandato dal Server */
/* ------------------------------------------------------------------------------------------- */

    printf("Ricevuto risultato dal server ip %s. ", ip_server);
    if (response.status == STATUS_OK) {
        switch (response.type)
        {
        case TYPE_TEMPERATURE:
        	printf("%s: Temperatura = %.1f°C\n", city, response.value);
        break;

        case TYPE_HUMIDITY:
        	printf("%s: Umidità = %.1f%%\n", city, response.value);
        break;

        case TYPE_WIND:
        	printf("%s: Vento = %.1f km/h\n", city, response.value);
        break;

        case TYPE_PRESSURE:
        	printf("%s: Pressione = %.1f hPa\n", city, response.value);
        break;
        }
    }
    else if (response.status == STATUS_CITY_NOT_FOUND) printf("Città non disponibile\n");
    else if (response.status == STATUS_INVALID_REQUEST) printf("Richiesta non valida\n");


    // --- Chiusura socket su Unix/Mac ---
    closesocket(socket_client);

    // --- Chiusura socket su Windows ---
    clearwinsock();

    return 0;
}
