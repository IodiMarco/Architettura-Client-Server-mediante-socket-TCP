/* ------------------------------------------------------------------------------------------- */
									/* Inizio File Protocol.h */
/* ------------------------------------------------------------------------------------------- */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

/* ------------------------------------------------------------------------------------------- */
						/* Gestione include per rendere il codice portabile */
/* ------------------------------------------------------------------------------------------- */

#if defined WIN32
    // --- Windows ---
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    // --- Unix (Linux/Mac) ---
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netdb.h>

    // Definiamo i tipi Windows per compatibilità
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define closesocket close
#endif

/* ------------------------------------------------------------------------------------------- */
									/* Librerie Standard */
/* ------------------------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ------------------------------------------------------------------------------------------- */
							/* Definizione Costanti (come da traccia) */
/* ------------------------------------------------------------------------------------------- */

// Parametri di connessione
#define SERVER_PORT 56700     // Numero di porta di default
#define NUM_MAX_CONN 5		  // Numero massimo di connessioni al server
#define NO_ERROR 0            // Per WSAStartup

// Parametri di protocollo
#define MAX_CITY_LEN 64       // Lunghezza massima nome città
#define BUFFER_SIZE 512       // Dimensione buffer generica

// Codici di stato per la risposta
#define STATUS_OK 0
#define STATUS_CITY_NOT_FOUND 1
#define STATUS_INVALID_REQUEST 2

// Tipi di dati meteo validi
#define TYPE_TEMPERATURE 't'
#define TYPE_HUMIDITY    'h'
#define TYPE_WIND        'w'
#define TYPE_PRESSURE    'p'

/* ------------------------------------------------------------------------------------------- */
								/* Strutture dati (come da traccia)  */
/* ------------------------------------------------------------------------------------------- */

typedef struct {
    char type;              // 't', 'h', 'w', 'p'
    char city[MAX_CITY_LEN]; // Nome città null-terminated
} weather_request_t;

typedef struct {
    unsigned int status;  // Codice di stato
    char type;            // Echo del tipo richiesto
    float value;          // Valore del dato meteo
} weather_response_t;

/* ------------------------------------------------------------------------------------------- */
							/* Funzione clearwinsock() (per Windows) */
/* ------------------------------------------------------------------------------------------- */

static inline void clearwinsock()
{
#if defined WIN32
    WSACleanup();
#endif
}

/* ------------------------------------------------------------------------------------------- */
				/* Prototipi di funzioni per la generazione dati (come da traccia) */
/* ------------------------------------------------------------------------------------------- */

float get_temperature(void);    // -10.0 / 40.0 °C
float get_humidity(void);       // 20.0 / 100.0 %
float get_wind(void);           // 0.0 / 100.0 km/h
float get_pressure(void);       // 950.0 / 1050.0 hPa

/* ------------------------------------------------------------------------------------------- */
				/* Prototipo di funzione che verifica se la città è supportata */
/* ------------------------------------------------------------------------------------------- */

int is_supported_city(const char* city);

/* ------------------------------------------------------------------------------------------- */
									/* Fine File Protocol.h */
/* ------------------------------------------------------------------------------------------- */
#endif
