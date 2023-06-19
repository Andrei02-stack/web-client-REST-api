#include <stdio.h>		/* printf, sprintf */
#include <stdlib.h>		/* exit, atoi, malloc, free */
#include <unistd.h>		/* read, write, close */
#include <string.h>		/* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>		/* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <stdbool.h>
#include <ctype.h>

#include "helpers.h"
#include "requests.h"
#include "parson.h"

#define MAX_LENGTH 100

char *rcv_post_req(int socket, char host[16], char *cmd, char *user[1], char *jwt)
{
	char *post_message = compute_post_request(host, cmd, "application/json", user, 1, NULL, 0, jwt);
	send_to_server(socket, post_message);

	return receive_from_server(socket);
}

char *rcv_get_req(int socket, char host[16], char *cmd, char *jwt, char *cookies[1], char *get_delete)
{
	char *get_message = compute_get_request(host, cmd, NULL, cookies, 1, jwt, get_delete);
	send_to_server(socket, get_message);

	return receive_from_server(socket);
}

// Functie care preia datele utilizatorului
char *get_user_credentials()
{
	char *username = calloc(BUFLEN, sizeof(char));
	char *password = calloc(BUFLEN, sizeof(char));
	printf("username=");
	scanf("%s", username);
	printf("password=");
	scanf("%s", password);

	JSON_Value *value = json_value_init_object();
	JSON_Object *object = json_value_get_object(value);
	json_object_set_string(object, "username", username);
	json_object_set_string(object, "password", password);

	return json_serialize_to_string(value);
}

// Functie care se ocupa de comanda "register"
void handle_register(char *user[1], char host[16], int port)
{
	int socket = open_connection(host, port, AF_INET, SOCK_STREAM, 0);
	// Primim datele utilizatorului
	user[0] = get_user_credentials();

	// Verifica daca username-ul este deja folosit
	if (strstr(rcv_post_req(socket, host, REGISTER, user, NULL), "is taken") != NULL)
	{
		printf("USERNAME ALREADY IN USE! Try something else.\n");
	}
	else
	{
		printf("SUCCESSFULLY REGISTERED! Log in using your credentials.\n");
	}
}

// Functie care se ocupa de comanda "login"
void handle_login(char *user[1], char host[16], int port, int *connected,
				  int in_library, char *cookies[1], char cookie[MAX_LENGTH])
{
	int socket = open_connection(host, port, AF_INET, SOCK_STREAM, 0);
	// Verificam daca suntem deja logati
	if (*connected == 1)
	{
		printf("You are already logged in!\n");
	}
	else
	{	
		// Primim datele utilizatorului
		user[0] = get_user_credentials();
		// Trimitere cerere de tip POST
		char *response = rcv_post_req(socket, host, LOGIN, user, NULL);
		
		// Verificam daca am primit un raspuns de tipul "OK"
		if (strstr(response, "OK") != NULL)
		{
			*connected = 1;
			printf("SUCCESSFULLY LOGGED IN!\n");
			// Get the cookies
			char *cookie_tok = strstr(response, "Set-Cookie:");

			strtok(cookie_tok, ";");
			cookie_tok += 12;
			//memset(cookie, 0, MAX_LENGTH);
			strcpy(cookie, cookie_tok);
			//cookie[strlen(cookie) - 2] = '\0';
			cookies[0] = cookie;
		}
		else
		{
			printf("Wrong credentials! Please try again.\n");
			*connected = 0;
			in_library = 0;
		}
	}
}

// Functie care se ocupa de comanda "logout"
void handle_logout(char host[16], int port, int *connected,
				   int in_library, char *cookies[1], char jwt[BUFLEN])
{

	int socket = open_connection(host, port, AF_INET, SOCK_STREAM, 0);
	// Verificam daca suntem logati
	if (*connected == 1)
	{
		// trimitem un request de logout
		rcv_get_req(socket, host, LOGOUT, jwt, cookies, "get");
		printf("SUCCESSFULLY LOGGED OUT!\n");
	}
	else
	{
		error("You are not logged in!\n");
	}
	*connected = 0;
	in_library = 0;
}

// Functie care se ocupa de comanda "enter_library"
void handle_enter_library(char host[16], int port, int *connected,
						  int *in_library, char *cookies[1], char jwt[BUFLEN])
{
	int socket = open_connection(host, port, AF_INET, SOCK_STREAM, 0);
	if (connected)
	{
		*in_library = 1;
		// retinem jwt-ul care ne va ajuta sa verificam accesul catre librarie

		char *jwt_tok = strstr(rcv_get_req(socket, host, ACCESS, jwt, cookies, "get"), "token");
		if (jwt_tok == NULL)
		{
			printf("YOU HAVE NO ACCESS!\n");
			*in_library = 0;
		}
		else
		{
			jwt_tok += 8;
			memset(jwt, 0, BUFLEN);
			strcpy(jwt, jwt_tok);
			jwt[strlen(jwt) - 2] = '\0';
			printf("SUCCESSFULLY ENTERED THE LIBRARY!\n");
		}
	}
	else
	{
		printf("Please log in first.\n");
	}
}

// Functie care se ocupa de comanda "add_book"
void handle_add_book(char host[16], int port, int *in_library, char *addbook[1],
					 char jwt[BUFLEN])
{
	int socket = open_connection(host, port, AF_INET, SOCK_STREAM, 0);
	if (*in_library == 1)
	{
		char title[MAX_LENGTH], author[MAX_LENGTH], genre[MAX_LENGTH], publisher[MAX_LENGTH];
		int pages;

		do
		{
			printf("title=");
			scanf("%s", title);
			if (strlen(title) < 3 || strlen(title) > MAX_LENGTH)
			{
				printf("WRONG FORMAT! Please try again.\n");
			}
		} while (strlen(title) < 3 || strlen(title) > MAX_LENGTH);

		do
		{
			printf("author=");
			scanf("%s", author);
			if (strlen(author) < 3 || strlen(author) > MAX_LENGTH)
			{
				printf("WRONG FORMAT! Please try again.\n");
			}
		} while (strlen(author) < 3 || strlen(author) > MAX_LENGTH);

		do
		{
			printf("genre=");
			scanf("%s", genre);
			if (strlen(genre) < 3 || strlen(genre) > MAX_LENGTH)
			{
				printf("WRONG FORMAT! Please try again.\n");
			}
		} while (strlen(genre) < 3 || strlen(genre) > MAX_LENGTH);

		do
		{
			printf("publisher=");
			scanf("%s", publisher);
			if (strlen(publisher) < 3 || strlen(publisher) > MAX_LENGTH)
			{
				printf("WRONG FORMAT! Please try again.\n");
			}
		} while (strlen(publisher) < 3 || strlen(publisher) > MAX_LENGTH);

		printf("pages_count=");
		scanf("%d", &pages);
		if (pages < 1)
		{
			printf("WRONG FORMAT! Please try again.\n");
			printf("pages_count=");
			scanf("%d", &pages);
		}

		char pages_string[BUFLEN];
		sprintf(pages_string, "%d", pages);

		JSON_Value *value = json_value_init_object();
		JSON_Object *object = json_value_get_object(value);
		json_object_set_string(object, "title", title);
		json_object_set_string(object, "author", author);
		json_object_set_string(object, "genre", genre);
		json_object_set_string(object, "page_count", pages_string);
		json_object_set_string(object, "publisher", publisher);
		addbook[0] = json_serialize_to_string(value);

		rcv_post_req(socket, host, BOOKS, addbook, jwt);
		printf("The book was successfully added to library!\n");
	}
	else
	{
		printf("Please enter the library first!\n");
	}
}

// Functie care se ocupa de comanda "get_book"
void handle_get_book(char host[16], int port, int *in_library, char jwt[BUFLEN],
					 char *cookies[1])
{
	int socket = open_connection(host, port, AF_INET, SOCK_STREAM, 0);
	if (*in_library == 1)
	{
		char id[MAX_LENGTH];
		printf("id=");
		scanf("%s", id);
		if (strlen(id) < 1 || strlen(id) > MAX_LENGTH)
		{
			printf("WRONG FORMAT! Please try again.\n");
			printf("id=");
			scanf("%s", id);
		}
		char get_book[BUFLEN];
		strcpy(get_book, BOOKS);
		strcat(get_book, "/");
		strcat(get_book, id);
		char *res = rcv_get_req(socket, host, get_book, jwt, cookies, "get");
		printf("%s\n", strstr(res, "{"));
	}
	else
	{
		printf("Please enter the library first!\n");
	}
}

// Functie care se ocupa de comanda "get_books"
void handle_get_books(char host[16], int port, int *in_library, char jwt[BUFLEN],
					  char *cookies[1])
{
	int socket = open_connection(host, port, AF_INET, SOCK_STREAM, 0);
	if (*in_library == 1)
	{
		// Daca suntem in librarie, se cauta cartile retinute
		char *res = rcv_get_req(socket, host, BOOKS, jwt, cookies, "get");
		printf("%s\n", strstr(res, "["));
	}
	else
		error("Please enter the library first.\n");
}

// Functie care se ocupa de comanda "delete_book"
void handle_delete_book(char host[16], int port, int *in_library, char jwt[BUFLEN],
						char *cookies[1])
{
	int socket = open_connection(host, port, AF_INET, SOCK_STREAM, 0);
	if (*in_library == 1)
	{
		// Daca suntem in librarie, se cauta cartile retinute
		char route[BUFLEN], id[BUFLEN];
		int book_id = 0;

		do
		{
			printf("id=");
			scanf("%s", id);
			if (sscanf(id, "%d", &book_id) != 1 || book_id <= 0)
			{
				printf("WRONG FORMAT! Try again!\n");
			}
		} while (book_id <= 0);

		sprintf(route, "%s/%d", BOOKS, book_id);

		// Se trimite request-ul de delete
		char *delete = strstr(rcv_get_req(socket, host, route, jwt, cookies, "delete"), "No book was deleted!");
		if (delete != NULL)
			error("NO BOOK WAS DELETED! The id you entered is not valid!\n");
		else
			printf("The book was successfully deleted!\n");
	}
	else
		error("Please enter the library first.\n");
}

// programul principal unde se citesc comenzile de la tastatura
int main(int argc, char *argv[])
{
	int connected = 0, in_library = 0, exit = 0;
	char host[16] = "34.254.242.81";
	int port = 8080;
	int socket = open_connection(host, port, AF_INET, SOCK_STREAM, 0);

	char *user[1], *cookies[1], cookie[BUFLEN], jwt[BUFLEN], *addbook[1];
	char cmd[BUFLEN];

	// se citeste comanda de la tastatura
	while (!exit)
	{
		fgets(cmd, BUFLEN, stdin);

		if (strncmp(cmd, "register", 8) == 0)
		{
			handle_register(user, host, port);
		}

		else if (strncmp(cmd, "login", 5) == 0)
		{
			handle_login(user, host, port, &connected, in_library, cookies, cookie);
		}

		else if (strncmp(cmd, "logout", 6) == 0)
		{
			handle_logout(host, port, &connected, in_library, cookies, jwt);
		}

		else if (strncmp(cmd, "enter_library", 13) == 0)
		{
			handle_enter_library(host, port, &connected, &in_library, cookies, jwt);
		}

		else if (strncmp(cmd, "add_book", 8) == 0)
		{
			handle_add_book(host, port, &in_library, addbook, jwt);
		}

		else if (strncmp(cmd, "get_books", 9) == 0)
		{
			handle_get_books(host, port, &in_library, jwt, cookies);
		}
		else if (strncmp(cmd, "get_book", 8) == 0)
		{
			handle_get_book(host, port, &in_library, jwt, cookies);
		}

		else if (strncmp(cmd, "delete_book", 11) == 0)
		{
			handle_delete_book(host, port, &in_library, jwt, cookies);
		}

		else if (strncmp(cmd, "exit", 4) == 0)
		{
			exit = 1;
		}
		close_connection(socket);
	}
}