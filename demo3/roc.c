#include "roc.h"

int visit(Roc* roc);
int comm(unsigned int port);
int contact(Roc* roc, int sock,
        int (*type)(Roc*, FILE*, FILE*, char*, char*), char*);
int get_port(Roc* roc, int sock);
int check_error(bool err);
int airport(Roc* roc, FILE* out, FILE* in, char*, char*);
int mapper(Roc* roc, FILE* out, FILE* in, char*, char*);

/**
 * Creates the roc client and handles all inputted arguments.
 * @param argc The number of arguments.
 * @param argv The string values for all given arguments.
 * @return The return value after handling the arguments.
 */
int main(int argc, char** argv) {
    Roc* roc = malloc(sizeof(Roc));

    if (argc < 3) {
        fprintf(stderr, "Usage: roc2310 id mapper {airports}\n");
        return 1;
    }

    roc->id = calloc(strlen(argv[1]), sizeof(char));
    roc->id = argv[1];

    if (strcmp(argv[2], "-") != 0) {
        int mapper = atoi(argv[2]);
        if (!valid_port(mapper)) {
            fprintf(stderr, "Invalid mapper port\n");
            return 2;
        } else {
            roc->port = mapper;
        }
    }

    roc->numAirports = argc - 3;
    roc->airports = calloc(argc - 3, sizeof(char*));
    for (int i = 3; i < argc; ++i) {
        roc->airports[i - 3] = malloc(strlen(argv[i]));
        strcpy(roc->airports[i - 3], argv[i]);
    }
    return visit(roc);
}

/**
 * Processes the ports to determine if they are valid.
 * Communicates with the mapper to find the value for non-numeric
 * ports.
 * @param roc The data structure to add the port values corresponding
 * to the inputs.
 */
void get_ports(Roc* roc) {
    roc->ports = calloc(roc->numAirports, sizeof(int));
    for (int j = 0; j < roc->numAirports; ++j) {
        int port = atoi(roc->airports[j]);

        if (valid_port(port)) {
            roc->ports[j] = port;
            continue;
        } else if (roc->port == 0) {
            fprintf(stderr, "Mapper required\n");
            exit(3);
        }

        int mapSock = comm(roc->port);
        if (mapSock < 0) {
            fprintf(stderr, "Failed to connect to mapper\n");
            exit(4);
        }

        int newPort = contact(roc, mapSock, mapper, roc->airports[j]);
        if (newPort <= 0) {
            fprintf(stderr, "No map entry for destination\n");
            exit(5);
        } else {
            roc->ports[j] = newPort;
        }
    }
}

/**
 * Connects to all the stored ports and sends the id
 * of this roc.
 * @param roc The data structure.
 * @return The status after connecting to the ports.
 */
int visit(Roc* roc) {
    bool err = false;

    get_ports(roc);

    for (int i = 0; i < roc->numAirports; ++i) {
        int sock = comm(roc->ports[i]);
        if (sock > 0) {
            contact(roc, sock, airport, "");
            close(sock);
        } else {
            err = true;
        }
    }
    return check_error(err);
}

/**
 * A function that handles input and output communication
 * for connection to an airport.
 * @param roc The data structure.
 * @param out The send file stream.
 * @param in The receive file steam.
 * @param buffer The buffer to read to.
 * @param line The line to send.
 * @return The success value.
 */
int airport(Roc* roc, FILE* out, FILE* in, char* buffer, char* line) {
    fprintf(out, "%s\n", roc->id);
    fflush(out);

    fgets(buffer, BUFF_SIZE, in);
    fprintf(stdout, "%s", buffer);
    return 0;
}

/**
 * A function that handles input and output communication
 * for connection to a mapper.
 * @param roc The data structure.
 * @param out The send file stream.
 * @param in The receive file steam.
 * @param buffer The buffer to read to.
 * @param line The line to send.
 * @return The success value.
 */
int mapper(Roc* roc, FILE* out, FILE* in, char* buffer, char* line) {
    fprintf(out, "?%s\n", line);
    fflush(out);

    if (fgets(buffer, BUFF_SIZE, in) == NULL) {
        return -1;
    }

    if (strcmp(buffer, ";\n") == 0) {
        return -1;
    }
    int val = atoi(buffer);
    return val;
}

/**
 * Contacts the given socket to communicate the required informaiton.
 * @param roc The data structure.
 * @param sock The socket to communicate to.
 * @param type The type of communication required.
 * @param line The line to communicate.
 * @return The success value.
 */
int contact(Roc* roc, int sock,
        int (*type)(Roc*, FILE*, FILE*, char*, char*), char* line) {
    FILE* out = fdopen(sock, "w");
    FILE* in = fdopen(sock, "r");
    char buffer[BUFF_SIZE];

    int retVal = type(roc, out, in, buffer, line);

    fclose(in);
    fclose(out);
    return retVal;
}

/**
 * Creates a socket for an inputted port.
 * @param port The port to create the socket for.
 * @return The socket if successful. Else returns -1.
 */
int comm(unsigned int port) {
    int sock;
    struct sockaddr_in sai;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        return -1;
    }
    memset(&sai, 0, sizeof(struct sockaddr_in));
    sai.sin_family = AF_INET;
    sai.sin_port = htons(port);
    sai.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr*) &sai, sizeof(sai)) < 0) {
        return -1;
    }
    return sock;
}

/**
 * Checks if an error was encountered when communicating
 * with the given ports.
 * @param err If the error occurred.
 * @return The return value.
 */
int check_error(bool err) {
    if (err) {
        fprintf(stderr, "Failed to connect to at least one destination\n");
        return 6;
    }
    return 0;
}