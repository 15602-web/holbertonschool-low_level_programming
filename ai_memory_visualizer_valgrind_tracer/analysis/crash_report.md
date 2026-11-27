## 0. Méthode et environnement

Compilation avec `cd programs && make` (`-Wall -Wextra -Werror -pedantic -std=gnu89 -g`), exécution avec `./crash_example`, sans Valgrind. L'architecture est x86-64 sous Linux, où la pile croît vers les adresses basses. L'ASLR est actif, mais il n'a aucun effet sur le crash, qui est déterministe puisque `n` vaut 0 dans le code. Le programme se termine par `SIGSEGV` (code 139). L'outil IA utilisé est Claude, et les prompts sont détaillés en 1.5.

## 1. crash_example.c

### 1.1 Points d'exécution clés

`main` initialise `nums = NULL` (ligne 24) et `n = 0` (ligne 25) sur la pile, puis les deux `printf` (lignes 27-28) s'exécutent normalement. Ligne 30, `allocate_numbers(0)` empile un frame (`n = 0`, `arr = NULL`, `i = 0`). Aux lignes 9-10, `n <= 0` est vrai et la fonction retourne `NULL` : `malloc` (ligne 12) et la boucle (ligne 16) ne sont jamais atteints. Le frame est dépilé et `nums` reçoit `NULL`. Ligne 32, `nums[0] = 42` écrit 4 octets à l'adresse 0x0 sans aucune vérification, ce qui provoque le `SIGSEGV`. Les lignes 34 à 37 ne sont jamais exécutées.

### 1.2 Cartes mémoire pas à pas

Pendant `allocate_numbers(0)`, la pile contient le frame de `main` (`nums = NULL`, `n = 0`) et celui de la fonction (`n = 0`, `arr = NULL`, `i = 0`). Le tas est vide, car aucun `malloc` n'a été appelé. Après le retour, le frame de la fonction a disparu : `nums = NULL` pointe vers l'adresse 0x0, qui ne désigne aucun objet. À la ligne 32, l'accès à 0x0 touche une page non mappée : le MMU lève un défaut de page, le noyau ne trouve aucune région valide et envoie `SIGSEGV`.

### 1.3 Durées de vie des variables

`nums` et `n` (dans `main`) vivent sur la pile jusqu'à la fin de `main`, qui n'est jamais atteinte. `n`, `arr` et `i` (dans `allocate_numbers`) naissent à l'appel de la ligne 30 et meurent au retour de la ligne 10. Aucun bloc du tas n'est créé. Les chaînes des `printf` sont en .rodata pendant toute l'exécution.

### 1.4 Aliasing

Il n'y a aucun alias : `arr` et `nums` contiennent tous deux `NULL`, mais `NULL` n'est pas un objet, donc ils ne désignent pas la même mémoire. Le paramètre `n` est une copie, pas un alias du `n` de `main`. `free(nums)` (ligne 36) serait légal avec `NULL`, mais cette ligne n'est jamais atteinte.

### 1.5 Erreur de l'IA et correction

L'IA propose typiquement plusieurs causes. Sont correctes : la valeur de retour `NULL` n'est pas vérifiée avant la ligne 32, et `n = 0` la produit par la ligne 10. Sont incorrectes : un échec de `malloc` (jamais appelé), un dépassement de tampon dans la boucle (jamais exécutée), un pointeur non initialisé (`nums` est initialisé à `NULL`), un double free (`free(NULL)` est défini et la ligne n'est pas atteinte) et une dépendance à l'ASLR (`n` est constant). Côté correctifs, tester `nums == NULL` avant l'écriture est nécessaire, alors que changer `n` ne fait que masquer ce cas. Correction : chaque cause est rattachée à une ligne réellement exécutée, et celles dont la ligne n'est pas atteinte sont écartées.

après un tour sur python tutor, il y a surtout une "ERROR: Invalid write of size 4" basé sur la ligne 32 du code "nums[0] = 42;" deterministic NULL dereference (segmentation fault) requesting n=0

## 2. Synthèse

Le crash est un déréférencement de pointeur nul, donc un comportement indéfini, à la ligne 32. `allocate_numbers(0)` retourne `NULL` à la ligne 10 sans appeler `malloc`, puis `main` écrit à l'adresse 0 sans vérifier le retour. Le défaut est dans l'appelant, qui ignore le contrat d'erreur de la fonction (`NULL` pour `n <= 0` ou échec d'allocation). Le tas n'est pas concerné, la pile ne contient que des variables valides, et l'accès fautif vise une adresse non mappée. Pour l'IA, je rattache chaque cause proposée à une ligne exécutée : plusieurs causes plausibles étaient fausses parce que leurs lignes ne s'exécutent jamais. Correctif suggéré (non appliqué) : `if (nums == NULL)` avant la ligne 32.