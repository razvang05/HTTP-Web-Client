Gheorghe Marius Razvan 324CA

Tema4 Pcom

Am folosit 2 sleepdays.
In implementarea temei am plecat de la laboratorul 9(in mare parte requests).
Am implementat toate comenzile specificate in tema.

Pentru a crea fisierele JSON am folosit bibilioteca parson
care ofera toate functionalitatile pentru lucru cu JSON,
dar este si usor de integrat si de inteles.


In functia main incep programul,prin citirea comenzilor iar in functie 
de verificare execut comanda care a fost data.

In comanda "register" este apelata functia register_user:
verifica daca nu cumva utilizatorul este deja logat,daca nu este nimeni
merg mai departe aloc spatiu si citesc username ul si parola,
daca sunt caractere alfanumerice le validez si creez un obiect JSON
Deschid o legatura cu serverul iar apoi trimit o cerere la server apoi
iau raspunsul primit si verific daca s a executat cu SUCCES sau Eroare(
in caz ca username ul a mai fost folosit) apoi inchid conexiunea si 
eliberez memoria.

Comanda "login" apeleaza functia login unde verifica daca nu cumva
este o logare activa in caz ca nu verific daca username ul si parola
sunt valide apoi creez un obiect JSON ,deschid conexiunea cu serverul.
timit o cerere de tip POST pe care o trimit iar raspunsul primit
il verific in caz ca raspunsul HTTP contine OK voi extrage cookie ul de sesiune
si actualizez contorul de cookie uri.La final eliberez resursele alocate si
inchid conexiunea.

Comanda "enter_library" apeleaza functia enter_library deschid conexiunea
cu serverul,trimit cererea http de tip get_request iar in functie de raspuns
extrag token ul in jwt pentru a l utiliza in functiile urmatoare iar in caz
de eroare inseamna ca nu am autorizatia de a intra in library.

Comanda "get_books" apeleaza functia get_books,se incepe o conexiune
cu serverul trimite o cerere http get_request la server iar apoi extrage
raspunsul si vede daca este "OK" caut inceputul caracterului "[" ce indica
obiectul JSON pe care trebuie sa il afisez altfel afisez cazul de eroare.

Comanda "get_book" apeleaza functia get_book unde verific daca sunt inregistrat
in library apoi citesc si validez id ul,construiesc path ul catre API folosind
id ul cartii formand endpoint ul pentru cererea GET.Trimit cererea la server
iar apoi verific raspunsul in caz ca raspunsul contine "OK" preiau
continutul obiectului JSON cu functia "basic_extract_json_response()" altfel
cazul de eroare.La final inchid conexiunea cu serverul si eliberez resursele.

Comanda "add_book" apeleaza functia add_book unde verific prin tokenul jwt
daca am acces la library apoi citesc datele specifice cartii(titlu,autor etc)
Verific validitatea tututor datelor despre carte sa nu fie "" iar la page_count
verific sa fie numar valid,creez obiectul JSON pentru a adauga detaliile despre 
carte apelez cererea POST cu tokenul pentru autorizatie.Daca raspunsul 
contine "OK" inseamna ca este cu SUCCESS daca nu EROARE.

Comanda "delete_book" apeleaza functia delete_book unde sterg o carte cu un 
id dat, inainte verific daca am acces la library apoi verifica 
validitatea id ului si construiesc calea URL pentru API alaturi de 
id ul cartii pentru identificare apelez cererea de DELETE impreuna cu 
tokenul pentru autentificare si cookie iar daca raspunsul de la server 
contine "OK" este cu SUCCESS daca nu EROARE.

Comanda "logout" apeleaza functia logout unde se trimite
o cerere get_request cu endpointul necesar alaturi de token si cookie.
Raspunsul primit prin receive_from_server() este verificat daca contine mesajul
"OK" care indica un SUCCES si se scade numarul de cookieuri si setez jwt la NULL.
eliberez resursele folosite.

Comanda "exit" se executa prin finalizarea executiei programului.

