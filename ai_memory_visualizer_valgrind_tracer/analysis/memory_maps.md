# Memory Maps

## 0. Méthode et environnement
- Compilation : `cd programs && make` (-Wall -Wextra -Werror -pedantic -std=gnu89 -g)
- Exécution : `./stack_example` (sans l'utilisation de Valgrind)
- Architecture : x86-64, Linux. La pile croît vers les adresses basses.
- ASLR actif : les adresses changent entre deux exécutions, seuls les écarts sont comparables.
- Outil IA : Claude. Prompts détaillés en 1.5 et corrigé pour que ce soit plus lisible

## 1. stack_example.c
### 1.1 Points d'exécution clés
Le programme n'alloue rien sur le tas : tout est sur la pile (ou en .rodata pour les littéraux).

1. `main` appelle `walk_stack(0,3)` : création de la frame depth=0, `marker=0`.
2. `dump_frame("enter",0)` : la frame est créée puis détruite au retour.
3. `walk_stack(1,3)` : nouvelle frame sous la précédente, `marker=10`.
4. Idem pour depth=2 (`marker=20`) et depth=3 (`marker=30`).
5. depth=3 : `depth < max_depth` est faux, pas de récursion. `dump_frame("exit",3)`, puis retour.
6. Les frames sont détruites dans l'ordre inverse : 3, 2, 1, 0, chacune faisant son `dump_frame("exit",n)` avant de
retourner.

Sortie attendue : `enter` 0,1,2,3 puis `exit` 3,2,1,0.

### 1.2 Cartes mémoire pas à pas
**(a) Dans dump_frame("enter", 1)**
Adresses hautes
┌──────────────────────────────┐
│ main (pas de locale)         │
├──────────────────────────────┤
│ walk_stack depth=0           │
│   depth=0, max_depth=3       │
│   marker=0   @0x...A         │
├──────────────────────────────┤
│ walk_stack depth=1           │
│   depth=1, max_depth=3       │
│   marker=10  @0x...B         │
├──────────────────────────────┤
│ dump_frame ("enter",1)       │
│   label -> .rodata ("enter") │
│   local_int=101 @0x...C      │
│   local_buf="B\0..." @0x...D │
│   p_local = 0x...C (= &local_int)
└──────────────────────────────┘
Adresses basses

**(b) Au point le plus profond : dump_frame("enter", 3)**
 → 4 frames walk_stack empilées (depth 0 à 3) + 1 frame dump_frame
   marker valant 0, 10, 20, 30, tous vivants.

**(c) Pendant dump_frame("exit", 3)**
 → mêmes frames qu'en (b) : dump_frame("enter",3) est déjà détruite,
   c'est la même zone mémoire qui est réutilisée.

### 1.3 Durées de vie des variables
| Variable | Stockage | Naissance | Mort |
|---|---|---|---|
| `depth`, `max_depth` (walk_stack n) | pile | entrée de walk_stack(n) | retour de walk_stack(n) |
| `marker` (walk_stack n) | pile | entrée de walk_stack(n) | retour de walk_stack(n), donc après tous les appels plus
profonds |
| `local_int`, `local_buf`, `p_local` | pile | entrée de dump_frame | retour de dump_frame |
| `label` (paramètre) | pile (le pointeur) | entrée de dump_frame | retour de dump_frame |
| `"enter"`, `"exit"` | .rodata | tout le programme | tout le programme |

### 1.4 Aliasing
- `p_local` est un alias de `local_int` : `p_local == &local_int` (même adresse dans la sortie).
  Valide uniquement pendant dump_frame. Il n'est jamais retourné ni stocké ailleurs, donc pas de pointeur pendant
  (dangling).
- `local_buf` : dans `(void *)local_buf`, le tableau se convertit en `&local_buf[0]`. L'adresse affichée est celle du
premier octet.
- `label` pointe vers une chaîne littérale en .rodata : ce n'est ni la pile ni le tas, et on ne doit pas écrire dedans
(const).
- Aucun aliasing entre frames : chaque frame a ses propres variables.

### 1.5 Erreur de l'IA et correction
pas d'erreur, le programme compile correctement.

## 2. Synthèse
Dans `stack_example.c`, toute la mémoire utilisée est sur la pile, sauf les chaînes `"enter"` et `"exit"` qui sont en
lecture seule dans .rodata. Il n'y a aucune allocation sur le tas, donc rien à libérer.

Chaque appel de fonction crée une frame sur la pile, qui contient ses variables locales. La pile grandit vers les
adresses basses : à chaque niveau de récursion, la nouvelle frame de `walk_stack` se place sous la précédente. Une
variable reste valide jusqu'au retour de sa fonction. C'est pourquoi `marker` garde la même adresse et la même valeur
à `enter` et à `exit` : sa frame n'est détruite qu'une fois tous les appels plus profonds terminés.

Les frames de `dump_frame` sont différentes : elles sont créées puis détruites à chaque appel, donc elles n'existent
jamais en même temps. Leur emplacement peut être réutilisé d'un appel à l'autre, mais ce ne sont pas les mêmes
variables.

Le pointeur `p_local` est un alias de `local_int` (même adresse). Il reste valide uniquement pendant `dump_frame` et
n'est jamais utilisé après, donc il n'y a pas de pointeur pendant. Le programme est correct : aucune erreur mémoire.

Pour l'IA, je compare ses explications à la sortie réelle du programme (adresses, valeurs) et à la durée de vie de
chaque variable. Ici, ses explications étaient correctes, donc je n'ai pas eu d'erreur à corriger.