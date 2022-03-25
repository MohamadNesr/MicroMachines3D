## But du projet

Le but du projet est de proposer une version du jeu [Micro Machines](https://online.oldgames.sk/play/genesis/micro-machines-2-turbo-tournament/9876) en mode tête à tête en 3D. Dans ce jeu, plusieurs véhicules font la course sur un circuit. Une seule caméra suit tous les joueurs à la fois. Pour marquer un point, un joueur doit faire en sorte que les autres joueurs sortent du champ de la caméra, tout en étant en tête de la course.

Pour ce projet, une base de code vous est fournie permettant de faire circuler
des véhicules sur un terrain. Sur cette base, vous avez trois tâches
indépendantes à réaliser : mettre en place la caméra et déterminer les joueurs
visibles, gérer le tracé course et déterminer le joueur en tête, et enfin
ajouter des obstacles pour rendre la course plus intéressante. Vous pouvez
réaliser ces tâches dans l'ordre de votre choix.

Si vous avez besoin d'éléments de décor supplémentaires, vous pouvez aller en
récupérer par exemple des éléments chez [Quaternius](https://quaternius.com/).
Vous y trouverez des véhicules, des arbres, des grenouilles, ...

### Mise en place

Placez vous dans le répertoire de gKit à la racine et clonez ce dépôt :

```
git clone https://forge.univ-lyon1.fr/m1if27/projet-mmachine.git
```

Pour versionner votre code, commencez plutôt par réaliser un fork et clonez
votre propre version. Éditez ensuite le fichier `premake4.lua` et ajoutez une
ligne à la fin du fichier pour intégrer ce dossier à l'ensemble :

```
include projet-mmachine
```

Vous pouvez ensuite régénérer la configuration pour votre système de compilation

```
premake5 gmake
```

pour linux avec g++ ou windows avec mingw

```
premake5 vs2019
```

pour visual studio 2019, et ainsi de suite, comme spécifié dans la documentation
de gKit. Le code de base contient :

* `src` : le dossier contenant les fichiers de base pour vous aider
* `app` : le dossier contenant les petites applications démo associées
* `assets` : le dossier contenant le maillage du véhicule et l'image du circuit

Pour rajouter votre propre application, inspirez-vous de celles existantes, et
éditez le fichier `premake4.lua` à la racine du projet pour spécifier le nom de
l'application créée.


### Caméra

Le travail minimal dans cette section consiste à avoir une caméra qui suit les
joueurs, et de pouvoir déterminer quels joueurs sont visibles ou non.

Dans le jeu, une seule caméra fait son possible pour suivre tous les joueurs à
la fois. Dans une application 3D, le placement de la caméra est réalisé en
spécifiant les matrices de vue et la matrice de projection. Pour générer ces
matrices, vous disposez dans `mat.h` de plusieurs fonctions utiles.

#### Matrice vue

Une matrice vue peut être générée en utilisant la fonction `Lookat` dans
`mat.h`. Cette fonction nécessite trois paramètres : 

* `from` : le point dans l'espace où est placée la caméra
* `to` : le point dans l'espace que reagrde la caméra
* `up` : un vecteur vertical vers le ciel

avec `from` et `to`, la caméra est contrainte sur la droite reliant les deux
points, mais peut encore tourner autour de cette droite tout en continuant à
regarder le point `to`. Le vecteur `up` sert donc à fixer l'angle de la caméra
autour de cet axe. C'est comme quand vous prenez une photo, il faut tenir son
appareil droit, sinon le paysage semblera en pente.

Dans votre cas, vous devrez regarder un point calculé à partir des positions des
joueurs, et placer la caméra à un endroit suffisamment éloigné et élevé pour
voir tous les joueurs. La position de votre caméra devra évoluer pour suivre les
joueurs pendant leur course, mais attention à maîtriser la trajectoire de la
caméra pour ne pas donner le mal des transports.

#### Matrice projection

La matrice de projection encode les paramètres de votre caméra. Sa construction
se fait via la fonction `Perspective` et nécessite quatre paramètres :

* `fov` : l'angle d'ouverture de la caméra. 45 degrés, c'est bien.
* `aspect` : le ratio d'aspect : 3/2, 4/3, 16/9, ...
* `znear` : la distance à partir de laquelle les objets sont visibles
* `zfar` : la distance à partir de la quelle les objets sont invisibles

On aurait tendance à vouloir placer `znear` à zéro et `zfar` à l'infini, mais
c'est une mauvaise idée car l'écart entre ces deux valeur détermine la précision
du test de profondeur réalisé pour ordonner les fragments de triangle sur chaque
pixel. Si l'écart est trop grand, les fragments ne pourront plus être
correctement ordonnés, et le triangle affiché ne sera pas nécessairement le bon.
Il faut donc s'assurer que les deux valeurs choisies permettent d'englober de la
scène sans trop d'excès.

#### Déterminer si un point est visible

Étant donné un point dans le repère du monde, multiplier ses coordonnées
successivement par la matrice de vue puis la matrice de projection place ce
point dans le repère de la caméra. Dans ce repère, tous les points visibles sont
situés dans le cube de diagonale `(-1,-1,-1)` -- `(1,1,1)`. Pour déterminer si
un objet est visible ou non, une méthode simple consiste par exemple à calculer
sa boîte englobante et détemriner si les sommets de cette boîte sont tous
visibles ou non. Vous pourrez ainsi déterminer si l'objet est totalement visible
et la plupart des situations où il est totalement invisible.

### Collisions

Le travail minimal dans cette section consiste à gérer des obstacles simples.
Grossièrement, il doit être possible de placer des arbres sur le parcours, et
que le véhicule s'arrête lorsqu'il s'en prend un.

#### Affecter le véhicule

Pour gérer les effets des collisions, tous les paramètres des joueurs dans
`player.hpp` sont publics pour faire simple. Vous pouvez donc ajuster la
position le vecteur vitesse et la directino du joueur en fonction des
collisions. La position est l'endroit où se trouve le joueur. La direction est
un vecteur qui indique vers où pointe l'avant du véhicule. La vitesse est un
vecteur qui indique dans quelle direction le véhicule se déplace. Notez que
lorsque le véhicule dérape, sa direction et sa vitesse ne vont pas dans le même
sens. Vous pouvez aussi jouer sur le paramètre friction pour ajuster la friction
latérale (à quel point le véhicule glisse quand il tourne) et la friction
linéaire (à quel point le véhicule freine bien).

#### Englobants

Comme vu lors du [TP intersection de boîtes
englobantes](https://forge.univ-lyon1.fr/box-intersection-etu), il est facile de
tester l'intersection de deux boîtes alignées sur les axes, et faisable de
tester l'intersection de boîtes non alignées. Le plus simple pour gérer vos
collisions consiste donc à utiliser des boîtes englobantes pour tous vos objets
constituant des obstacles. Vos collisions ne seront pas exactes, mais elles
seront suffisamment réalistes pour les besoin de ce jeu.

Pour les éléments fixes du décor, vu qu'ils ne bougnet pas, il est simple de
garder des boîtes englobantes alignées sur les axes. Pour les véhicules et
obstacles qui se déplacent par contre, si vous calculer l'englobant initial, il
ne restera pas aligné sur les axes. Il faudra donc gérer les collisions de
boîtes englobantes non alignées sur les axes.

Une autre solution consiste à utiliser des sphères plutôt que des boîtes. Dans
ce cas, le test d'intersection est plus simple et les sphères restent des
sphères même si les objets qu'elles contiennent se déplacent. Pour calculer une
sphère englobante, il est nécessaire de déterminer un point central, puis de
calculer le sommet le plus éloigné de ce centre pour obtenir un rayon. Deux
sphères s'intersectent lorsque la distance entre les deux centres est plus
faible que la somme de leur deux rayons.

#### Gestion du terrain

Vous pouvez également utiliser des collisions pour définir des zones du circuit
dans lesquelles les paramètres du véhicule changent (flaques d'huiles, perte
d'adhérence, zones ralenties, boost) Le terrain en lui-même affecte le joueur
via la fonction `project`. Vous pouvez créer vos propres terrains en dérivant de
la classe `Terrain`. La méthode project prent trois paramètres :

* `from` : l'endroit où se trouve le véhicule
* `to` : l'endroit où le véhicule souhaiterait se rendre
* `normal` : un vecteur que la fonction remplira

Le principe de cette fonction est de considérer la trajectoire que souhaiterait
avoir le véhicule et la contraindre par le terrain en modifiant le point de
destination. Par exemple dans `ImgTerrain`, la fonction commence par s'assurer
que le véhicule ne décolle pas du terrain en ajustant la coordonnée z de la
destination pour replacer le véhicule sur le terrain. Ensuite, le test empêche
de monter des pentes trop raides. Lorsque c'est le cas, la destination est
réinitialisée au point de départ et le véhicule n'avance pas. Enfin la normale
du véhicule est systématiquement le vecteur vertical pour le garder horizontal.

### Circuit

Le travail minimal dans cette section consiste à pouvoir déterminer à tout
instant le joueur en tête de la course. Cette partie peut être traitée de
plusieurs façons.


#### Tracé du circuit par une courbe

Vous avez réalisé des courbes dans le [TP
courbes](https://forge.univ-lyon1.fr/m1if27/courbes-etu). Pas besoin d'avoir
totalement fini le TP ici, mais simplement d'avoir implémenté la subdivision.
Une méthode simple consiste alors à implémenter le circuit en utilisant une
courbe : placez des poitns de contrôle pour suivre le tracé du circuit, et
subdivisez la. Ensuite, pour déterminer l'avancement d'un véhicule, il suffit de
calculer le point de la courbe le plus proche. L'indice de ce point dans le
tableau de l'ensemble des points de la courbe sera alors une bonne approximation
de l'avancement du véhicule le long du tracé. Le véhicule pour lequel le point
de la courbe a l'indice le plus fort est en tête. Attention cependant si votre
course fait plusieurs tours.

Pour placer les points de cette courbe, vous pouvez par exemple faire en sorte
qu'à l'appui d'une touche le joueur affiche sa position dans la console ou la
stocke dans un tableau. Vous pouvez ainsi déplacer le véhicule sur le circuit et régulièrement récupérer un point.

#### Tracé du circuit par une image

Vous pouvez également étendre la classe `ImgTerrain` pour charger une seconde
image de la même taille que le terrain, et qui contient dans chaque pixel une
valeur correspondant à l'avancement d'un joueur qui se trouverait à cet endroit.
Pour déterminer l'avancement d'un joueur il vous suffit alors d'aller chercher
dans cette image le pixel correspondant à sa position.

Générer une telle image manuellement risque néanmoins d'être compliqué, et le
faire automatiquement ressemblera à la méthode précédente : commencer par
colorer les pixels du tracé de la course, avec une teinre de gris correspondant
à l'avancement de la course : 0 au départ et 1 à l'arrivée. Ensuite, pour chaque
pixel, déterminez le pixel de la courbe le plus proche et attribuez lui sa
couleur.
