# Aplikace typu Pac-Man

## Duben 2023

## Autoři
- xkrato67
- ![@lasjdhu](https://github.com/lasjdhu)

## Screenshot

![Screenshot](game.png)

## Implementace:

Výsledkem je hra vytvořená pomocí prostředí Qt v C++.
Uživatel používá GUI k interakci s hrou.
Implementace využívá návrhového vzoru Model View Controller, pro oddělení modelu hry a GUI.
Hra obsahuje tři interaktivní prvky, kterými jsou:
1) Počítání kroků hráče.
2) Zobrazování aktuálních životů.
3) Počet pokusů hráče na konci kola.

## Funkčnost

1) Při spuštění aplikace budete vyzváni k výběru mapy. Poté je možné vybrat mapu pomocí tlačítka Game -> 'Load', v horní liště
nebo spuštěním stejné mapy pomocí 'Restart' po skončení hry.
2) Stav hry lze uložit pomocí tlačítka Game -> 'Save'. Poté je možné ji načíst jako běžnou mapu.
3) Po skončení hry lze gameplay uložit pomocí tlačítka 'Save this gameplay' a poté ji znovu přehrát pomocí tlačítka Game -> 'Replay'.

## Omezení implementace

1) Pohyby Pac-Mana se ovládají pouze pomocí WASD, šipky nefungují.
2) Byl implementován pouze jeden mód přehrávání – postupné krokování.

## Známé chyby
1) Okénko 'Save this gameplay' se objeví tolikrát, kolikrát jste klikli na 'Restart'.
