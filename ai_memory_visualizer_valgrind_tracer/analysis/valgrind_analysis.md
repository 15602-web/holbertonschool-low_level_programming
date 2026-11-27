Memory Maps (version condensée)

0. Environnement
Compilation : cd programs && make (-Wall -Wextra -Werror -pedantic -std=gnu89 -g) ; exécution : ./heap_example
x86-64, Linux, pile vers les adresses basses, ASLR actif. sizeof(Person) = 16 o (pointeur 8 + int 4 + padding 4).
Outil IA : Claude, pour générer les cartes, puis vérifié contre la sortie réelle.

1. heap_example.c
1.1 Déroulement
main : alice et bob sont des pointeurs sur la pile, à NULL.
Ligne 51, person_new("Alice", 30) : malloc struct (A1, 16 o, ligne 14) et malloc nom (A2, 6 o, ligne 21). alice reçoit
A1.
Ligne 52, person_new("Bob", 41) : B1 (16 o) et B2 (4 o).
Lignes 66-67 : free(bob->name) (B2) puis free(bob) (B1).
Ligne 69 : person_free_partial(alice) libère seulement A1 (free(p), ligne 41). A2 n’est jamais libéré.
Fin : A2 (6 o) reste alloué, sans aucun pointeur vers lui.

1.2 Cartes mémoire
(a) Après la ligne 52

PILE                          TAS
alice = [0xA1] ──────────►  A1 {name=[0xA2], age=30} ──► A2 "Alice\0" (6 o)
bob   = [0xB1] ──────────►  B1 {name=[0xB2], age=41} ──► B2 "Bob\0"   (4 o)

Les variables locales de person_new ont disparu ; seuls les blocs du tas survivent.

(b) Après la ligne 67

alice ──► A1 (vivant) ──► A2 (vivant)
bob   ──► B1 LIBÉRÉ, B2 LIBÉRÉ   (bob est pendant)

L’ordre est correct : B2 avant B1. Sinon bob->name serait illisible (use-after-free) et B2 fuirait.

(c) Après la ligne 69

alice ──► A1 LIBÉRÉ (pendant)
bob   ──► B1 LIBÉRÉ (pendant)
A2 (6 o, "Alice\0") : TOUJOURS ALLOUÉ, inatteignable : FUITE

Le seul pointeur vers A2 était name, stocké dans A1, désormais libéré.

1.3 Durées de vie
Objet	Stockage	Mort
alice, bob	pile (main)	fin de main
p, len, i, name	pile	retour de person_new
A1	tas	free(p), ligne 41
A2	tas	jamais libéré
B1 / B2	tas	lignes 67 / 66
littéraux, chaînes des printf	.rodata	fin du programme

Un bloc du tas vit jusqu’au free, pas jusqu’au retour de la fonction qui l’a créé.

1.4 Aliasing et propriété
p et alice contiennent la même adresse (A1) ; après le retour, alice en est propriétaire.
Chaque Person possède son nom : il faut libérer le nom, puis la struct.
name (paramètre) pointe vers le littéral en .rodata ; p->name pointe vers une copie sur le tas, pas un alias.
Dans person_free_partial, p est un alias temporaire de alice ; après free, alice est pendant (mettre à NULL serait
plus sûr).
Aucun alias entre alice et bob.

2. Synthèse

Chaque Person utilise deux blocs du tas (struct de 16 o + copie du nom) ; alice et bob sont de simples pointeurs sur
la pile. Les blocs survivent au retour de person_new, ses variables locales non.

bob est libéré correctement (nom, puis struct). Pour alice, person_free_partial ne libère que la struct : le bloc de 6
octets devient inatteignable, c’est une fuite par perte de propriété (definitely lost). Les pointeurs pendants ne sont
jamais utilisés, donc ni use-after-free ni double free.

Pour l’IA, je vérifie ses affirmations avec le code (grep -n) et la sortie réelle de Valgrind ; ici, ses chiffres et
numéros de ligne étaient approximatifs, j'ai utilisé python tutor pour comprendre comment marchait le code, le programme ne marche pas à cause du fait que le pointeur pointe sur bob qui a été libéré entre temps, puis la variable "p" est libéré ce qui fait quel'entièreté du programme ne fonctionne plus correctement.