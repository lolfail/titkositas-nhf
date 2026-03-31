---
title: "**A programozás alapjai 2:** *Nagy házi*"
subtitle: "Specifikáció"
author: "Bencsik Gergő (`I48U0D`)"
date: \today
geometry: "margin=3cm"
header-includes:
  - \usepackage{float}
  - \floatplacement{figure}{H}
  - \usepackage[bottom]{footmisc}
  - \usepackage[utf8]{inputenc}
---

\tableofcontents
\newpage

# Feladat megnevezése

A választott feladat a [*Titkosítás*](https://infocpp.iit.bme.hu/hf_otlet#3).

> Készítsen titkosító osztályt! Az osztály legyen képes tetszőleges hosszúságú szöveg kódolt formátumú tárolására. A kódoláshoz olyan egyszerű műveletet használjon, ami nem függ a kódolandó szöveg hosszától (pl. kizáró vagy). Valósítsa meg a szokásos string műveleteket! Az osztályt úgy tervezze meg, hogy az örökléssel könnyen felhasználható legyen, az algoritmus könnyen cserélhető legyen! Valósítsa meg az összes értelmes műveletet operátor átdefiniálással (overload), de nem kell ragaszkodni az összes operátor átdefiniálásához! Legyen az osztálynak iterátora is!
Specifikáljon egy egyszerű tesztfeladatot, amiben fel tudja használni az elkészített adatszerkezetet! A tesztprogramot külön modulként fordított programmal oldja meg! A megoldáshoz **ne** használjon STL tárolót!
>  [`https://infocpp.iit.bme.hu/hf_otlet`](https://infocpp.iit.bme.hu/hf_otlet#3)-ről

# Feladatspecifikáció

A program célja, egy olyan *"programozói segédeszköz"* készítése, egy olyan osztály, amellyel egyszerű titkosításokat használó *string*-eket lehet létrehozni, kezelni, méghozzá úgy, hogy a titkosítási algoritmus cserélhető legyen.

## Tárolt szövegre vonatkozó megkötések

A tárolt, kódolt szövegre hosszúsági megkötés nem szabad hogy legyen. Dinamikusan kell kezelnie a memóriát a szöveg tárolásához, ez természetesen magával vonja azt is, hogy a végén fel is kell szabadítania mindazt.

Ezen felül a program csak **ASCII** enkódolású szöveggel képes dolgozni. Minden betű, karakter, csupán egyetlen bájtot foglalhat el.

## Titkosító algoritmusok megkötése

A programnak nem célja minden lehetséges titkosító algoritmust támogatni.

Olyan titkosításokat kell csupán támogatnia, amelyek:

* Nem függnek a kódolandó szöveg hosszától
* Nem függnek a már kódolt szövegtől (vagyis nem függ a kódolt szöveg tartalmától)

Ilyenek például az *XOR* kódolás, illetve a *Caesar-kódolás*.

## Definálandó műveletek

Az osztály maga és *C string*-ek között képes bizonyos műveletek *"elvégzésére"*, ezek:

| **operátor** | **végzendő művelet**                                 |
| ---          | ---                                                  | 
| `==`         | Kettő (dekódolt) tartalma *azonos-e*?                |
| `!=`         | Kettő (dekódolt) tartalma *különbözik*?              |
| `+`          | A kettő *összefűzése* egy *új* objektummá.           |
| `+=`         | Bal oldali *operandushoz fűzése* a jobb oldalinak.   |
| `=`          | Bal oldali operandusnak *értékadás*.                 |

Továbbá képes legyen kiírni egy *stream*-be a dekódolt szöveget a `<<` művelet. Illetve lehessen az eredeti szöveg egy bizonyos *index*-én lévő karaktert visszakérni (az indexelő operátor: `[]`-al).

Ha két különböző algoritmussal kódolt és tárolt objektumot fűzünk egymáshoz (`+` operátor), a programnak a bal oldali érték típusát kell, hogy figyelembe vegye.

## Iterátor

Megkötés még, hogy a kódolt szöveg bejárható legyen *iterátorral*. Ezért a következő műveleteket, metódusokat kell definiálni az osztály iterátorán, hogy szokványos módon tudjunk dolgozni velük:

| **operátor/metódus** | **célja**                                                             |
| ---                  | ---                                                                   |
| `.begin()`           | Visszaad egy *iterátort a kezdetre*.                                  |
| `.end()`             | *Záró iterátort* ad vissza.                                           |
| `*` (dereferálás)    | Iterátor pozícióján lévő eredeti karaktert *kinyerése*, *olvasásra*.  |
| `++` és `--`         | Iterátor *jobbra*/*balra* *léptetése eggyel*, módosítva, visszaadása. |
| `+` és `-`           | Iterátor *jobbra*/*balra* *léptetése* jobb oldali operandusnyival.    |
| `==` és `!=`         | Két iterátor *azonos*/*eltérő* helyzetben van?                        |
| `<`, `<=`, `>`, `>=` | Két iterátor *helyzetének összevetése*.                               |

Az iterátora *random access iterator-szerű*. Táblázatból látható, hogy összehasonlíthatók, tetszőleges távval mozgathatók, ezen felül még indexelhetők is legyenek. Azonban fontos megjegyzés, hogy a dereferálásuk **értéket** ad vissza, nem referenciát. Illetve, a táblázatban nem szerepel, de legyen képezhető a *távolságuk*, egymásból kivonással.

Nincs értelme két eltérő objektum iterátorának esetén több műveletnek is velük, azonban erre figyelni a felhasználó (programozó) feladata.

## Hibakezelés

Olyan esetekben, amikor a hiba a felhasználótól (ami a program jellege miatt egy programozótól) származik, és ezek jelzése az osztály felelőssége, akkor az dobjon hibát. Ilyen például a *túlindexelés*.

Ha hibát dob, azok legyenek szabványosak (utóbbi az esetben az `std::out_of_range`).

Emellett hibákat az osztály *ne nyeljen el*, hagyja felbuborékozni például a *memóriahiányból* eredő hibákat, hogy azt a hívó kezelje. Azonbon fontos, hogy ilyenek miatt *inkonzisztens* állapotba **nem** kerülhet az osztály. Vagyis, ha kivétel keletkezik például egy hozzáfűzés során, akkor legyen olyan a végén, mintha az összefűzés meg se történt volna.

## Program tesztelése

Az alaposztály teszteléséhez szükség lesz konkrétumokra, az egyes kódoló algoritmusok implementációjára. Így az alaposztály felhasználásával készült, már titkosításra is képes osztályokkal a főprogram a *standard input*-ról jövő szövegeken fog különféle műveleteket végezni, tesztelési célból, majd pedig ellenőrzni azok kimenetének helyességét.

A teszteléshez egyrészt a `gtest_lite` könyvtárra, másrészt pedig valamely memóriaszivárgást ellenőrző programra lesz szükség.
