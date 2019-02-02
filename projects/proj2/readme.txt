Program realizující jednoduchou formou dhcp starvation attack

Spouštění:
./ipk-dhcpstarve -i interface
-i  -   interface, povinný parametr, dostupné možnosti lze získat např. pomocí ip addr

Omezení projektu:
-   Na freeBSD nelze nastavit interface, protože proto chybí podpora ve fci setsockopt(), kterou jsem použil

