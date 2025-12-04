# Filtrující DNS resolver

## Dmitrii Ivanushkin (xivanu00)

### 17.11.2025

### Popis

Program `dns` filtruje příchozí dotazy typu A podle předem definovaného seznamu
domén a jejich poddomén. Dotazy na blokované domény jsou odmítnuty. Všechny
ostatní dotazy jsou přeposílány na určený resolver a získané odpovědi vráceny
původnímu tazateli.

### Omezení

1. Server vyžaduje, aby parametr `-s` byl platná IP adresa. Hostname bohužel
   není podporován.
2. Spuštění s defaultním portem vyžaduje použití sudo

### Použití

- Spuštění programu
  ```sh
  make
  ```
  ```sh
  dns -s server [-p port] -f filter_file [-v]
  ```

  - `-s`: IP adresa serveru DNS resolveru
  - `-p`: port: Port pro naslouchání příchozím DNS dotazům
  - `-f`: filter_file: Cesta k souboru obsahujícímu blokované domény
  - `-v`: Režim s podrobným výpisem

- Spuštění testů
  ```sh
  make
  ```

### Seznam odevzdaných souborů

- examples/1.txt
- src/dns.c
- src/dns.h
- src/dns_utils.c
- src/dns_utils.h
- src/helpers.c
- src/helpers.h
- src/main.c
- Makefile
- README.md
- manual.pdf
- run_tests.sh
