# Servizio meteo client/server TCP

Applicazione in C composta da un **server TCP** e da un **client TCP**. Il client richiede un dato meteorologico per una città; il server controlla la richiesta, genera un valore casuale realistico e restituisce la risposta.

Il progetto è pensato per Windows, Linux e macOS ed è organizzato in due progetti Eclipse CDT separati.

## Funzionamento

1. Il server si mette in ascolto sulla porta `56700` (o su quella indicata con `-p`).
2. Il client si connette al server e invia una richiesta contenente il tipo di dato e la città.
3. Il server verifica che il tipo richiesto e la città siano validi.
4. Per le richieste valide, il server genera un valore casuale e lo invia al client.
5. Il client stampa il risultato e chiude la connessione. Il server rimane attivo e può gestire altre richieste.

## Dati disponibili

| Codice | Dato | Intervallo generato |
| --- | --- | --- |
| `t` | Temperatura | da -10.0 a 40.0 °C |
| `h` | Umidità | da 20.0 a 100.0 % |
| `w` | Velocità del vento | da 0.0 a 100.0 km/h |
| `p` | Pressione atmosferica | da 950.0 a 1050.0 hPa |

Le città riconosciute non distinguono maiuscole e minuscole:

`Bari`, `Roma`, `Milano`, `Napoli`, `Torino`, `Palermo`, `Genova`, `Bologna`, `Firenze` e `Venezia`.

## Struttura del progetto

```text
.
├── client-project/
│   └── src/
│       ├── main.c       # Connessione al server, invio richiesta e stampa risposta
│       └── protocol.h   # Protocollo e definizioni condivise
├── server-project/
│   └── src/
│       ├── main.c       # Server, validazione e generazione dei dati meteo
│       └── protocol.h   # Protocollo e definizioni condivise
└── Assegnazione.md      # Traccia originale dell'esercitazione
```

## Compilazione da terminale

Su Linux e macOS, dalla cartella principale del repository:

```bash
gcc -Wall -Wextra -o server server-project/src/main.c
gcc -Wall -Wextra -o client client-project/src/main.c
```

Su Windows con MinGW, aggiungere la libreria Winsock:

```bash
gcc -Wall -Wextra -o server.exe server-project/src/main.c -lws2_32
gcc -Wall -Wextra -o client.exe client-project/src/main.c -lws2_32
```

In alternativa, i due progetti possono essere importati in Eclipse CDT come progetti esistenti.

## Avvio

Avviare prima il server:

```bash
./server
```

Per usare una porta diversa:

```bash
./server -p 56701
```

Poi eseguire il client in un altro terminale:

```bash
./client -r "t Bari"
```

Sintassi completa del client:

```text
./client [-s indirizzo_server] [-p porta] -r "tipo città"
```

- `-s`: indirizzo IPv4 del server; predefinito `127.0.0.1`.
- `-p`: porta del server; predefinita `56700`.
- `-r`: richiesta obbligatoria; il primo carattere è il tipo e il testo successivo è il nome della città.

Esempi:

```bash
./client -r "h Roma"
./client -r "w Milano"
./client -s 192.168.1.10 -p 56701 -r "p Napoli"
```

L'output contiene un valore casuale, quindi il numero varia a ogni richiesta:

```text
Ricevuto risultato dal server ip 127.0.0.1. Bari: Temperatura = 22.5°C
```

## Gestione degli errori

- Una città diversa da quelle supportate produce `Città non disponibile`.
- Un tipo diverso da `t`, `h`, `w` o `p` produce `Richiesta non valida`.
- Il client segnala eventuali problemi di connessione o di ricezione della risposta.

## Protocollo applicativo

Client e server inviano direttamente tramite TCP le seguenti strutture C, definite in entrambi i file `protocol.h`:

```c
typedef struct {
    char type;
    char city[64];
} weather_request_t;

typedef struct {
    unsigned int status;
    char type;
    float value;
} weather_response_t;
```

I codici di stato sono:

- `0`: richiesta completata;
- `1`: città non disponibile;
- `2`: richiesta non valida.

> Il protocollo invia strutture binarie native. Client e server devono quindi essere compilati con rappresentazioni compatibili delle strutture e dei tipi numerici.

## Licenza

Vedere [LICENSE](LICENSE).
