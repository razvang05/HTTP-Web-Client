#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "parson.h"
#include "helpers.h"
#include "requests.h"
#include <ctype.h>


void register_user(int *num_cookies) {
    int sockfd;
    char *username, *password;
    if  (*num_cookies > 0) {
        puts("ERROR-You are logged in!");
        return;
    }

    username = (char*)malloc(sizeof(char) * 30);
    if (username == NULL) {
        error("malloc failed\n");
    }

    password = (char*)malloc(sizeof(char) * 30);
    if (password == NULL) {
        error("malloc failed\n");
    }
    memset(username, 0, 30);
    memset(password, 0, 30);

    printf("username=");
    fgets(username, LINELEN, stdin);
    remove_enter(username);
            
    printf("password=");
    fgets(password, LINELEN, stdin);
    remove_enter(password);


    if (is_valid_user(username) == 0) {
        puts("ERROR - Username is invalid!");
        return;
    }
    if (is_valid_user(password) ==0) {
        puts("ERROR - Password is invalid!");
        return;
    }
    // se creeaza obiectul json
    // si se pun obiectele username si password
    // il incarc in cererea POST http
    JSON_Value* val = json_value_init_object();
    JSON_Object* obj = json_value_get_object(val);
    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    json_object_set_string(obj, "username", username);
    json_object_set_string(obj, "password", password);
    char *body_data = NULL;
    body_data = json_serialize_to_string(val);
    
    char *message = compute_post_request(HOSTPORT, "/api/v1/tema/auth/register", "application/json", body_data, NULL, 0, NULL);

    send_to_server(sockfd, message);
    char *response = receive_from_server(sockfd);
    // verific daca mesajul e de succes sau eroare
    if (strstr(response, "HTTP/1.1 201 Created")) {
        puts("SUCCESS-Successfully registered user!");
    } else {
        printf("ERROR-409-The username %s is already taken.\n", username);
    }

    free(body_data);
    close_connection(sockfd);
    free(message);
    free(response);
    free(password);
    free(username);
    json_value_free(val);
}

void login(char **cookies, int *num_cookies) {
    char *message;
    char *response;
    int sockfd;

    if (*num_cookies > 0) {
        puts("ERROR - You are already logged in!");
        return;
    }

    char * username, *password;
    username = (char*)malloc(sizeof(char) * 30);
    if (username == NULL) {
        error("malloc failed\n");
    }
    password = (char*)malloc(sizeof(char) * 30);
    if (password == NULL) {
        error("malloc failed\n");
    }
    memset(username, 0, 30);
    memset(password, 0, 30);

    printf("username=");
    fgets(username, LINELEN, stdin);
    remove_enter(username);
    
    printf("password=");
    fgets(password, LINELEN, stdin);
    remove_enter(password);
    // verific daca sunt valide
    if (is_valid_user(username) == 0 || is_bun(username) == 0) {
        puts("ERROR - Username is invalid!");
        return;
    }

    if (is_valid_user(password) == 0 || is_bun(password) == 0) {
        puts("ERROR - Password is invalid!");
        return;
    }

    // creez obiecttul json unde adaug username sipassword
    // deschid conexiunea cu serverul si trimit cererea http POST
    JSON_Value* val = json_value_init_object();
    JSON_Object* obj = json_value_get_object(val);

    sockfd = open_connection(HOST,PORT, AF_INET, SOCK_STREAM, 0);
    json_object_set_string(obj, "username", username);
    json_object_set_string(obj, "password", password);
    char* body_data = NULL;
    body_data = json_serialize_to_string(val);

    message = compute_post_request(HOSTPORT, "/api/v1/tema/auth/login", "application/json", body_data, NULL, 0, NULL);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    //daca raspunsul contine ...OK 
    // voi extrage cookie ul de sesiune cu ajutorul lui strchr si strncpy
    // adica ce identific dupa Set-Cookie
    if (strstr(response, "HTTP/1.1 200 OK")) {
        printf("SUCCESS - 200 - Has been successfully logged in user %s.\n",username);
        char *cookie_start = strstr(response,"Set-Cookie: ");
        if (cookie_start) {
            cookie_start += 12;
            char *cookie_end = strchr(cookie_start, ';');
            if(cookie_end && (cookie_end != cookie_start)) {
                int cookie_lenght = cookie_end - cookie_start;
                strncpy(cookies[*num_cookies], cookie_start, cookie_lenght);
                cookies[*num_cookies][cookie_lenght] = '\0';
                // actualizez numarul de cookieuri
                (*num_cookies)++;
            }

        }
    } else {
        puts("ERROR - Login failed!");
    }

    free(body_data);
    close_connection(sockfd);
    free(message);
    free(response);
    free(username);
    free(password);
    json_value_free(val);
}

void enter_library(char **cookies, int cookies_num, char **jwt) {
    char *message;
    int sockfd;
    if (cookies_num == 0) {
        puts("ERROR - You aren't logged in!");
        return;
    }

    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request(HOSTPORT, "/api/v1/tema/library/access", "application/json", cookies, cookies_num, *jwt);
    send_to_server(sockfd, message);
    char *response = receive_from_server(sockfd);
    // extrag tokenul de la token....
    if (strstr(response, "HTTP/1.1 200 OK")) {
        puts("SUCCESS - 200 - You entered the library.");
        char *token = "\"token\":\"";
        char *start = strstr(response, token);
        if (start) {
            start = start + strlen(token);
            char *final = strchr(start, '\"');
            if (final != start) {
                int length = final - start;
                *jwt = calloc(length + 1, sizeof(char));
                // copiez valoarea tokenului
                strncpy(*jwt, start, length);
                (*jwt)[length] = '\0';
            }

        }
    } else {
        puts("ERROR - 401 - You are not logged in.");
    }


    close_connection(sockfd);
    free(message);
    free(response);
}

void get_books(char **cookies, int *cookies_num, char *jwt) {
    int sockfd;

    if (*cookies_num == 0) {
        puts("ERROR-You aren't logged in!");
        return;
    }

    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    char *message = compute_get_request(HOSTPORT, "/api/v1/tema/library/books", "application/json", cookies, *cookies_num, jwt);
    send_to_server(sockfd, message);
    char *response = receive_from_server(sockfd);
    // extrag datele despre carti de la careacterul [ in dreapta
    // in caz ca cererea s a executat cu succes
    if (strstr(response, "HTTP/1.1 200 OK")) {
        puts("SUCCESS - Books:");
        char* p = strstr(response, "[");
        puts(p);
        close_connection(sockfd);
        free(message);
        free(response);
        return;
    } else {
        puts("ERROR-You don't have access to the library!");
    }
    
    close_connection(sockfd);
    free(message);
    free(response);
}

void get_book(char **cookies, int *cookies_num, char **jwt) {
    char *message;
    char *response;
    int sockfd;
    if (*cookies_num == 0) {
        puts("ERROR-You aren't logged in!");
        return;
    }
    if (*jwt == NULL) {
        puts("ERROR - You don't have access to the library");
        return;
    }
    // verific daca id ul este corect
    char *id;
    id = (char*)malloc(sizeof(char) * 256);
    if (id == NULL) {
        error("malloc failed\n");
    }
    memset(id, 0, 256);

    printf("id=");
    scanf("%s", id);
    if (is_number(id) == 0) {
        puts("ERROR - This id is incorrect");
        return;
    }

    char *path;
    path = (char*)malloc(sizeof(char) * 256);
    if (path == NULL) {
        error("malloc failed\n");
    }
    memset(path, 0, 256);
    // creez adresa URL alaturi de id pentru a sti ce index al cartii sa afisez
    sprintf(path, "/api/v1/tema/library/books/%s", id);
    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request(HOSTPORT, path, "application/json", cookies, *cookies_num, *jwt);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    // extrag raspunsul cu ajutorul functie basic..json in format json
    if (strstr(response, "HTTP/1.1 200 OK")) {
        char* p = basic_extract_json_response(response);
        puts(p); 
    } else {
        puts("ERROR - Can't get this book");
    }

    close_connection(sockfd);
    free(message);
    free(response);
    free(id);
    free(path);

}

void add_book(char **cookies, int cookies_num, char *jwt) {
    char *message;
    char *response;
    int sockfd;
    if (cookies_num == 0) {
        puts("ERROR - You aren't logged in");
        return;
    }
    if (jwt == NULL) {
        puts("ERROR-Don't have access to the library!");
        return;
    }

    char *title = (char*)malloc(LINELEN * sizeof(char));
    if (title == NULL) {
        error("malloc failed\n");
    }
    memset(title, 0, LINELEN);
    char *author = (char*)malloc(LINELEN * sizeof(char));
    if (author == NULL) {
        error("malloc failed\n");
    }
    memset(author, 0, LINELEN);
    char *genre = (char*)malloc(LINELEN * sizeof(char));
    if (genre == NULL) {
        error("malloc failed\n");
    }
    memset(genre, 0, LINELEN);
    char *publisher = (char*)malloc(LINELEN * sizeof(char));
    if (publisher == NULL) {
        error("malloc failed\n");
    }
    memset(publisher, 0, LINELEN);
    char *page_count = (char*)malloc(LINELEN * sizeof(char));
    if (page_count == NULL) {
        error("malloc failed\n");
    }
    memset(page_count, 0, LINELEN);

    printf("title=");
    fgets(title, LINELEN, stdin);
    remove_enter(title);

    printf("author=");
    fgets(author, LINELEN, stdin);
    remove_enter(author);

    printf("genre=");
    fgets(genre, LINELEN, stdin);
    remove_enter(genre);

    printf("publisher=");
    fgets(publisher, LINELEN, stdin);
    remove_enter(publisher);

    printf("page_count=");
    fgets(page_count, LINELEN, stdin);
    remove_enter(page_count);

    if (is_bun(title) == 0) {
        puts("ERROR-Invalid title.");
        return;
    }
    if (is_bun(author) == 0) {
        puts("ERROR-Invalid author.");
        return;
    }
    if (is_bun(genre) == 0) {
        puts("ERROR-Invalid genre.");
        return;
    }
    if (is_bun(publisher) == 0) {
        puts("ERROR-Invalid publisher.");
        return;
    }
     if (is_bun(page_count) == 0) {
        puts("ERROR-Invalid page_count.");
        return;
    }


    if (is_number(page_count) == 0) {
        puts("ERROR-Invalid page count format.");
        return;
    }
    int nr_page_count = atoi(page_count);
    // creez obiectul json 
    // si adaug datele necesare despre carte la el
    JSON_Value* val = json_value_init_object();
    JSON_Object* obj = json_value_get_object(val);
    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    json_object_set_string(obj, "title", title);
    json_object_set_string(obj, "author", author);
    json_object_set_string(obj, "genre", genre);
    json_object_set_number(obj, "page_count", nr_page_count);
    json_object_set_string(obj, "publisher", publisher);
    char* body_data = NULL;
    body_data = json_serialize_to_string(val);

    message = compute_post_request(HOSTPORT, "/api/v1/tema/library/books", "application/json", body_data, NULL, 0, jwt);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);

    if (strstr(response, "HTTP/1.1 200 OK")) {
        puts("SUCCESS - OK - Book added to the library.");
    } else {
        puts("ERROR - You can't add book");
    }

    free(body_data);
    json_value_free(val);
    close_connection(sockfd);
    free(message);
    free(response);
    free(title);
    free(genre);
    free(author);
    free(publisher);
    free(page_count);
}

void delete_book(char **cookies, int cookies_num, char *jwt) {
    char *message;
    char *response;
    int sockfd;

    if (cookies_num == 0) {
        puts("ERROR-You aren't logged in");
        return;
    }

    if (jwt == NULL) {
        puts("ERROR-Don't have access to the library");
        return;
    }


    char *id;
    id = (char*)malloc(sizeof(char) * 256);
    if(id == NULL) {
        error("malloc failed\n");
    }
    memset(id, 0, 256);

    printf("id=");
    fgets(id, 10, stdin);
    remove_enter(id);

    if (is_number(id) == 0) {
        puts("ERROR - Id is incorrect");
        return;
    }
    
    char *path;
    path = (char*)malloc(sizeof(char) * 256);
    if (path == NULL) {
        error("malloc failed\n");
    }
    memset(path, 0, 256);
    // formez calea URL pentru api alaturi de id
    sprintf(path, "/api/v1/tema/library/books/%s", id);
    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    message = compute_delete_request(HOSTPORT, path, NULL, cookies, cookies_num, jwt);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    // prelucrez raspunsul dat de server
    if (strstr(response, "HTTP/1.1 200 OK")) {
        printf("SUCCESS - Deleted book with id %s.\n", id); 
    } else {
        puts("ERROR - You can't delete.");
    }

    close_connection(sockfd);
    free(message);
    free(response);
    free(id);
    free(path);
}

void logout(char **cookies, int *cookies_num, char **jwt) {
    int sockfd; 
    // deschid conexiunea si trimit cererea la server
    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    char *message = compute_get_request(HOSTPORT, "/api/v1/tema/auth/logout", NULL, cookies, *cookies_num, *jwt);
    send_to_server(sockfd, message);
    char *response = receive_from_server(sockfd);

    // extrag continutul din response si daca este bun
    // scad numarul de cookies si pun token pe NULL
    // eliberez resursele si inchid conexiunea
    if(strstr(response, "HTTP/1.1 200 OK")) {
        puts("SUCCESS - Successfully logged out!");
        (*cookies_num)--;
        *jwt = NULL;
        close_connection(sockfd);
        free(message);
        free(response);
        return;
    } else {
        puts("ERORR - You aren't logged in.");
    }
    
    close_connection(sockfd);
    free(message);
    free(response);
}


int main(int argc, char *argv[])
{
    //aloc memorie pentru jwt si cookies
    char *command, *jwt;
    command = (char*)malloc(sizeof(char) * 20);
    if(command == NULL) {
        error("malloc failed\n");
    }
    memset(command, 0, 20);
    jwt = (char*)malloc(sizeof(char) * 2000);
    if(jwt == NULL) {
        error("malloc failed\n");
    }
    memset(jwt, 0, 2000);
    jwt = NULL;

    char **cookies;
    cookies = (char**)malloc(sizeof(char*) * 20);
    if(cookies == NULL) {
        error("malloc failed\n");
    }
    for(int i = 0; i < 20; i++) {
        cookies[i] = (char*)malloc(sizeof(char) * 2000);
        if(cookies[i] == NULL) {
            for(int j = 0; j < i; j++) {
                free(cookies[j]);
            }
            free(cookies);
            error("malloc failed\n");
        }
        memset(cookies[i], 0, 2000);
    }
    
    int num_cookies = 0;
    // executia comenzilor
    while(1) {
        fgets(command, 20, stdin);
        remove_enter(command);

        fflush(stdin);

        if(strncmp(command, "register", 8) == 0) {
            register_user(&num_cookies);
        } else
        if(strncmp(command, "login", 5) == 0) {
            login(cookies, &num_cookies);

        } else
        if (strncmp(command, "enter_library", 13) == 0) {
           enter_library(cookies, num_cookies, &jwt);
        } else
        if(strncmp(command, "get_books", 9) == 0) {
            get_books(cookies, &num_cookies, jwt);
        } else 
        if(strncmp(command, "get_book", 8) == 0) {
                get_book(cookies, &num_cookies, &jwt);
        } else 
        if(strncmp(command, "add_book", 8) == 0) {

            add_book(cookies, num_cookies, jwt);
        } else 
        if(strncmp(command, "delete_book", 11) == 0) {
                delete_book(cookies, num_cookies, jwt);
        } else 
        if(strncmp(command, "logout", 6) == 0) {
            logout(cookies, &num_cookies, &jwt);

        } else 
        if(strcmp(command, "exit") == 0) {
            free(command);
            free(jwt);
            for(int i = 0; i < 20; i++) {
                free(cookies[i]);
            }
            free(cookies);
            break;
        } else if(strcmp(command, "") != 0) {
            printf("Invalid command %s.\n", command);
        }
        fflush(stdin);
    }
   
    return 0;
}
