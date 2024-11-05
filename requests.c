#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

char *compute_get_request(char *host, char *url, char *query_params,
                            char **cookies, int cookies_count, char* jwt)
{
    char *message = calloc(BUFLEN, sizeof(char));
    if(message == NULL) {
        error("calloc failed\n");
    }
    char *line = calloc(LINELEN, sizeof(char));
    if(line == NULL) {
        error("calloc failed\n");
    }

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    if (cookies_count != 0) {
        memset(line, 0, LINELEN);
        sprintf(line, "Cookie: %s", cookies[cookies_count - 1]);
        compute_message(message, line);
    }

    if(jwt != NULL ) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", jwt);
        compute_message(message, line);
    }
    // Step 4: add final new line
    compute_message(message, "");
    free(line);
    return message;
}

char *compute_post_request(char *host, char *url, char* content_type, char *body_data, char **cookies, int cookies_count, char *jwt)
{
    char *message = calloc(BUFLEN, sizeof(char));
    if(message == NULL) {
        error("calloc failed\n");
    }

    char *line = calloc(LINELEN, sizeof(char));
    if(line == NULL) {
        error("calloc failed\n");
    }

    char *body_data_buffer = calloc(LINELEN, sizeof(char));
    if(body_data_buffer == NULL) {
        error("calloc failed\n");
    }

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // Step 2: add the host
    sprintf(line, "HOST %s", host);
    compute_message(message, line);
    /* Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
            in order to write Content-Length you must first compute the message size
    */

    strcpy(body_data_buffer, body_data);

    memset(line, 0, LINELEN);
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);
        
    memset(line, 0, LINELEN);
    sprintf(line, "Content-Length: %ld", strlen(body_data_buffer));
    compute_message(message, line);

    // Step 4 (optional): add cookies
    if(jwt != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", jwt);
        compute_message(message, line);
    }
    if(cookies != NULL && cookies_count > 0) {
        for(int i = 0;i<cookies_count;i++) {
            if(cookies[i]) {
                sprintf(line, "Cookie: %s", cookies[i]);
                compute_message(message, line);
            }
        }
    }
    // Step 5: add new line at end of header
    compute_message(message, "");
    // Step 6: add the actual payload data
    memset(line, 0, LINELEN);
    if(body_data != NULL) {
        strcat(message, body_data_buffer);
    }

    free(line);
    return message;
}

char *compute_delete_request(char *host, char *url, char *query_params,
                            char **cookies, int cookies_count, char* jwt)
{
    char *message = calloc(BUFLEN, sizeof(char));
    if(message == NULL) {
        error("calloc failed\n");
    }

    char *line = calloc(LINELEN, sizeof(char));
    if(line == NULL) {
        error("calloc failed\n");
    }

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    if (cookies_count != 0) {
        memset(line, 0, LINELEN);
        sprintf(line, "Cookie: %s", cookies[cookies_count - 1]);
        compute_message(message, line);
    }

    if(jwt != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", jwt);
        compute_message(message, line);
    }
    // Step 4: add final new line
    compute_message(message, "");
    free(line);
    return message;
}