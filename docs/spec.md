---
title: "**A programozás alapjai 2:** *Nagy házi*"
subtitle: "Specifikáció és tervezet"
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

Megkötés még, hogy a kódolt szöveg bejárható legyen *iterátorral*. Ezért a következő műveleteket, metódusokat kell definiálni az osztályon/iterátorán, hogy szokványos módon tudjunk dolgozni velük:

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

Nincs értelme két eltérő objektum iterátorának esetén az összehasonlításuknak, sem a távolságuk meghatározásának (`-` operátor), azonban erre figyelni a felhasználó (programozó) feladata.

## Hibakezelés

Olyan esetekben, amikor a hiba a felhasználótól (ami a program jellege miatt egy programozótól) származik, és ezek jelzése az osztály felelőssége, akkor az dobjon hibát. Ilyen például a *túlindexelés*.

Ha hibát dob, azok legyenek szabványosak (utóbbi az esetben az `std::out_of_range`).

Emellett hibákat az osztály *ne nyeljen el*, hagyja felbuborékozni például a *memóriahiányból* eredő hibákat, hogy azt a hívó kezelje. Azonbon fontos, hogy ilyenek miatt *inkonzisztens* állapotba **nem** kerülhet az osztály. Vagyis, ha kivétel keletkezik például egy hozzáfűzés során, akkor legyen olyan a végén, mintha az összefűzés meg se történt volna.

## Program tesztelése

Az alaposztály teszteléséhez szükség lesz konkrétumokra, az egyes kódoló algoritmusok implementációjára. Így az alaposztályba ilyeneket *injektálva*, a főprogram a *standard input*-ról jövő szövegeken fog különféle műveleteket végezni, tesztelési célból, majd pedig ellenőrzni azok kimenetének helyességét.

A teszteléshez egyrészt a `gtest_lite` könyvtárra, másrészt pedig valamely memóriaszivárgást ellenőrző programra lesz szükség.

# Terv

![A feladat tervezett osztálydiagramja.](./docs/diagram.svg)

## Szabványostól eltérő jelölések értelmezése

Az egyes objektumok bal felső sarkában látható színes karika, benne egy betűvel az objektum típusát írja le:

| **Ikon/Karakter** | **Jelentése**  |
| ---               | ---            |
| `C`               | `class`        |
| `S`               | `struct`       |
| `E`               | `enum`         |
| `I`               | `interface`    |

Illetve a bekeretezése az egyes objektumoknak nem feltétlenül hordoz a kódban is megnyilvánuló jelentést, csak az osztálydiagram (és egyben a terv) értelmezéséhez nyújt segítséget.
\newpage

## Az interfészről

Kiemelendő, hogy külön interfész mint nyelvi eszköz, nem létezik C++11-ben.
Így ez csupán annyit jelent, hogy az `Algorithm` egy olyan "összefogó" *absztrakt osztály*, melynek csak metódusai vannak, adattagjai nem, és amiből az egyes titkosító algoritmus implementációk(osztályok) öröklenek (*realizálva* az interfészt), a *kompatibilitást* lehetővé téve.

Erre azért van szükség, mivel a `Cipher` osztály (melyet a felhasználó programozó majd használ) *dependency injection*-el kapja meg az általa használandó titkosító algoritmust, melyektől elvárt, hogy azonos módon lehessen használni azokat.

## A `View` struct-okról

Ezek olyan struct-ok melyek tárolnak egy *pointert* a titkosított(vagy éppen titkosítatlan) szöveg kezdetére, és annak hosszát.

Nincs destruktoruk, *nem "birtokolják"* az általuk tárolt memóriát, így nem is szabadíthatják azt fel.
Pont ezen okból nem is `class`-ok, hanem `struct`-ok. Azt próbálja sugallni, hogy annál egy egyszerűbb adatszerkezetről van szó.

A `ConstCipherView` és `CipherView` között pedig a különbség, hogy az első által tárolt memóriaterület nem módosítható. Erre a akkor van szükség amikor csak olvasásra szeretnénk átadni a szöveget.

## Titkosító algoritmusok

A feladat nem tartalmazza az egyes titkosító algoritmusok elkészítését, viszont teszteléshez nyilvánvalóan szükségesek. Így jelenleg erre kettő van csak definiálva, amik az osztálydiagramon is láthatók.
A belső működésük lényegtelen az azokat használó osztály számára, elvárt, hogy azonos módon lehessen őket használni (az interfész által definiált módon).

### `XORCipher`

Szokásos *kizáró vagyot* használó titkosítási algoritmus.

$$
c_e = c_d \oplus k
$$

Ahol $c_e$ a titkosított, $c_d$ a titkosítatlan karakter (`char`), $k$ pedig a titkosításhoz használt kulcs.
A dekódoláshoz ugyanezt kell alkalmazni ismét, csak ezúttal a titkosított karakterre.

### `CaesarCipher`

*Caesar-kódolás*, amely megenged karakter értéke szerinti, visszafelé történő eltolást is.

$$
c_e = c_d + s
$$

$c_e$ a titkosított, $c_d$ a titkosítatlan karakter, az $s$ pedig az *eltolás* mértéke (mely lehet negatív is ezen megvalósításban).
A dekódoláshoz az eltolás mértékét pedig éppen negált előjellel kell venni.
