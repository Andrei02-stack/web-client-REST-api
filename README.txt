PCOM - Tema 4 Client Web. Comunicaţie cu REST API.
Săcăluș Andrei - 325CA

Pentru rezolvarea temei am folosit scheletul de la laboratorul 10 . 
Cerintele temei au fost rezolvate in fisierul client.c,
care simuleaza comportamentul clientului, comenzile fiind preluate de la tastatura.
Pentru parsarea obiectelor json, am folosit de fisierele parson.c si parson.h
pentru a usura parsarea informatiilor despre user si carti in json.

client.c: 
    get_user_credentials(): preia numele de utilizator și parola 
    de la utilizator și le returnează în format JSON.

    handle_register(): gestionează comanda „register”. Stabilește o conexiune cu serverul, 
    preia acreditările utilizatorului, trimite o cerere POST către server pentru înregistrare și afișează 
    mesajul de succes sau eroare corespunzător.

    handle_login(): gestionează comanda „login”. Acesta verifică dacă utilizatorul 
    este deja conectat, preia acreditările utilizatorului, trimite o solicitare POST către server pentru 
    autentificare și afișează mesajul corespunzător de succes sau eroare. De asemenea, stochează cookie-urile 
    primite pentru solicitări viitoare.

    handle_logout(): se ocupă de comanda „logout”. Acesta verifică dacă utilizatorul 
    este conectat, trimite o cerere GET către server pentru deconectare și afișează mesajul de succes 
    corespunzător.

    handle_enter_library():  gestionează comanda „enter_library”. Verifică dacă utilizatorul 
    este autentificat, trimite o solicitare GET către server pentru accesarea bibliotecii și afișează mesajul 
    corespunzător de succes sau eroare. De asemenea, stochează tokenul JWT primit pentru solicitări viitoare.

    handle_add_book():  gestionează comanda „add_book”. Verifică dacă utilizatorul se află 
    în bibliotecă, preia detaliile cărții de la utilizator, trimite o solicitare POST către server pentru 
    adăugarea cărții și afișează mesajul de succes la adăugarea cu succes.

    handle_get_book(): se ocupă de comanda „get_book”. Verifică dacă utilizatorul se află 
    în bibliotecă, preia ID-ul cărții de la utilizator, trimite o solicitare GET către server pentru 
    preluarea detaliilor cărții și afișează informațiile cărții primite.

    handle_get_books(): se ocupă de comanda „get_books”. Verifică dacă utilizatorul 
    se află în bibliotecă, trimite o solicitare GET către server pentru a prelua toate cărțile din bibliotecă 
    și afișează informațiile primite despre carte.

    handle_delete_book(): se ocupă de comanda „delete_book”. Verifică dacă utilizatorul 
    se află în bibliotecă, preia ID-ul cărții de la utilizator, trimite o solicitare GET către server 
    pentru ștergerea cărții și afișează mesajul de succes la ștergere.

    main(): funcția principală a codului. Stabilește o conexiune cu serverul, citește 
    comenzile de la utilizator și apelează funcțiile corespunzătoare pe baza comenzii introduse. 
    Programul continuă să se execute până când utilizatorul introduce comanda „exit”.

requests.c:
    compute_post_request() & compute_get_request(): Acestea sunt funcțiile din scheletul laboratorului
    modificate pentru tema dată astfel am adăugat ca parametri in plus 
    -> jwt: tokenul JWT care urmează să fie inclus în anteturile cererii (opțional).
    -> cmd: tipul comenzii ("get" sau "delete").

helpers.c: 
    am adăugat rcv_get_req, si rcv_post_req pentru primirea datelor de la server din
    fiecare tip "GET" si "POST"
