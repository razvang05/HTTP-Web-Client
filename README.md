Gheorghe Marius Razvan 324CA

Project 4 - Pcom

In this project, I based my implementation on Lab 9 (mostly focusing on requests). I implemented all the commands specified in the assignment.

To create the JSON files, I used the parson library, which provides all necessary functionalities for working with JSON, and is easy to integrate and understand.

In the main function, the program begins by reading commands, and depending on the verification, it executes the given command.

Commands:
"register" calls the register_user function: it checks if a user is already logged in; if not, it proceeds to allocate space and reads the username and password. If they are alphanumeric, it validates them and creates a JSON object. It opens a connection with the server, sends a request, receives the response, and checks if it executed with SUCCESS or an ERROR (if the username is already taken). Finally, it closes the connection and frees the memory.

"login" calls the login function, which checks if there is an active login. If not, it verifies if the username and password are valid, then creates a JSON object, opens a connection to the server, sends a POST request, and checks the response. If the HTTP response contains "OK," it extracts the session cookie and updates the cookie counter. At the end, it frees allocated resources and closes the connection.

"enter_library" calls the enter_library function, opens a connection to the server, and sends an HTTP GET request. Depending on the response, it extracts the JWT token to use in subsequent functions. In case of an error, it means there is no authorization to enter the library.

"get_books" calls the get_books function, starts a connection with the server, sends an HTTP GET request, and then examines the response to check if it is "OK." It searches for the beginning of the "[" character, indicating the JSON object that needs to be displayed. Otherwise, it shows the error case.

"get_book" calls the get_book function, where it checks if the user is registered in the library, reads and validates the ID, constructs the API path using the book’s ID to form the GET request endpoint, and sends the request to the server. It checks the response; if it contains "OK," it retrieves the JSON object content using the basic_extract_json_response() function. Otherwise, it displays an error case. Finally, it closes the server connection and frees resources.

"add_book" calls the add_book function, where it verifies access to the library via the JWT token, then reads the book’s specific data (title, author, etc.). It checks that all book data is valid (not empty strings) and confirms that page_count is a valid number. It creates a JSON object to add book details and calls the POST request with the authorization token. If the response contains "OK," it indicates SUCCESS; otherwise, it shows an ERROR.

"delete_book" calls the delete_book function, which deletes a book with a given ID. First, it verifies access to the library, checks the validity of the ID, and constructs the URL path for the API with the book’s ID for identification. It calls the DELETE request along with the authentication token and cookie. If the server response contains "OK," it indicates SUCCESS; otherwise, ERROR.

"logout" calls the logout function, which sends a GET request with the necessary endpoint along with the token and cookie. The response received through receive_from_server() is checked for the "OK" message, indicating SUCCESS, after which the cookie count is decreased, and the JWT is set to NULL. It then frees the used resources.

"exit" is executed by terminating the program.
