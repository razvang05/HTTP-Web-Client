# Project 4 - Pcom

## Gheorghe Marius Razvan  
### Date: 20.05.2024
### Email: rzvrazvan03@gmail.com



### Overview

In this project, I based my implementation on Lab 9 (mostly focusing on requests). I implemented all the commands specified in the assignment using the parson library for working with JSON. The library provides all necessary functionalities for handling JSON files, and it’s easy to integrate and understand.

### Commands Implemented:

1. **register**
   - The `register_user` function is called.
   - It checks if a user is already logged in; if not, it allocates space and reads the username and password.
   - If the username and password are alphanumeric, it validates them and creates a JSON object.
   - A connection with the server is established, and the registration request is sent.
   - The response is received and checked for "SUCCESS" or "ERROR" (if the username is already taken).
   - The connection is closed and the memory is freed.

2. **login**
   - The `login` function checks if there is an active login.
   - If there isn't, it verifies if the username and password are valid.
   - A JSON object is created, and a connection is made to the server.
   - A POST request is sent, and the response is examined.
   - If the response contains "OK", the session cookie is extracted and the cookie counter is updated.
   - Finally, allocated resources are freed and the connection is closed.

3. **enter_library**
   - The `enter_library` function opens a connection to the server and sends an HTTP GET request.
   - Depending on the response, it extracts the JWT token for future requests.
   - If there is an error, it indicates no authorization to enter the library.

4. **get_books**
   - The `get_books` function initiates a connection to the server and sends an HTTP GET request.
   - The response is checked for "OK".
   - The function searches for the beginning of the "[" character to locate the JSON object to display.
   - If an error occurs, the error message is displayed.

5. **get_book**
   - The `get_book` function checks if the user is registered in the library.
   - It reads and validates the book ID, constructs the API path using the book’s ID to form the GET request endpoint, and sends the request to the server.
   - If the response contains "OK", it retrieves the JSON object content using the `basic_extract_json_response()` function.
   - Otherwise, it displays an error message.
   - The server connection is closed, and resources are freed.

6. **add_book**
   - The `add_book` function verifies access to the library via the JWT token.
   - It reads the book’s specific data (title, author, etc.) and checks the validity of the data (e.g., ensuring page_count is a valid number).
   - A JSON object is created with the book details, and a POST request is made with the authorization token.
   - If the response contains "OK", the operation is marked as successful; otherwise, an error message is displayed.

7. **delete_book**
   - The `delete_book` function deletes a book with a given ID.
   - It first verifies access to the library, checks the validity of the ID, and constructs the URL path for the API with the book’s ID.
   - A DELETE request is sent along with the authentication token and cookie.
   - If the server response contains "OK", the operation is successful; otherwise, an error message is displayed.

8. **logout**
   - The `logout` function sends a GET request with the necessary endpoint, token, and cookie.
   - The response received from the server is checked for "OK", indicating the success of the operation.
   - The cookie count is decreased, and the JWT is set to `NULL`.
   - The used resources are then freed.

9. **exit**
   - This command terminates the program.

### Libraries Used

- **parson**: A simple JSON library that provides functionalities to parse, create, and manipulate JSON data. This library was used throughout the project for creating and parsing the JSON objects.

### How to Run

1. Make sure you have the necessary dependencies installed (e.g., `parson` library).
2. To run the program, use the following command:

```bash
./pcom_program <command>
