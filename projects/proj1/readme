Spouštění klienta:
./ipk-client -h host -p port [-n|-f|-l] login
Z přepínačů -n, -f, -l je možné vybrat pouze jeden
-h  -   hostname, povinný, je možné zadat doménu i ip adresu
-p  -   číslo portu, povinné
-n  -   vypíše user info k zadanému loginu, je třeba uvést login
-f  -   vypíše cestu k domovskému adresáři k zadanému loginu, je třeba uvést login
-l  -   vypíše seznam všech uživatelů na zařízení, kde server běží, pokud je zadán login, 
        vypíšou se jen uživatelé začínající stejně jako login string

Spouštění serveru:
./ipk-server -p port
-p  -   číslo portu, povinné

Omezení projektu:
-   V případě interní chyby serveru (chyba alokace) se klientovi vždy odpoví, že je problém 
    v neexistujícím loginu
-   Při větším množství požadavků (3<) a pokud je server i klient v jiné síti, server hlavně při
    žádosti o list loginů nestíhá. Je to zřejmě kvůli mému rozhodnutí, že vzhledem k jednoduché
    komunikaci bude stačit čekání v queue tcp přenosu 
