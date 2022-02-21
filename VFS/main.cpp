#include <iostream>
#include "FileSystem.hpp"



enum class ArgType: uint8_t{
	FLOAT,
	INT,
	UINT,
	STRING,
	BOOL,
	CHAR
};

#define SIZE_OF_STRING 256
#define SIZE_OF_DESCRIPTION 256

class Description : public StackSegment<SIZE_OF_DESCRIPTION>{
public:

	Description(const char* c): StackSegment<SIZE_OF_DESCRIPTION>(c, strlen(c)){}

};

template <typename T>
class BaseArg{

protected:

	bool filled = false;
	T data;

public:

	const Name displayed_name;
	const ArgType arg_type;
	const Description description;
	const bool mandatory = false;
	

public:

	BaseArg(const Name& n, const ArgType& at, const bool& m, const char* d): displayed_name(n), arg_type(at), mandatory(m), description(d){}

	BaseArg(const T& t, const Name& n, const ArgType& at, const bool& m, const char* d): displayed_name(n), arg_type(at), mandatory(m), description(d){
		this->set(t);
	}

	inline void set(const T& t){
		this->data = t;
		this->filled = true;
	}

	inline BaseArg<T>& operator=(const T& t){
		this->set(t);
		return *this;
	}

	inline bool is_set_and_mandatory() const{ return (this->filled && this->mandatory); }
	inline bool is_set() const{ return this->filled; }
	inline const T& value() const{ return this->data; }
};



class FloatArg: public BaseArg<float>{

public:

	FloatArg(const Name& n, const bool& m, const char* c): BaseArg<float>(n, ArgType::FLOAT, m, c){}
	FloatArg(const float& f, const Name& n, const bool& m, const char* c): BaseArg<float>(f, n, ArgType::FLOAT, m, c){}

};



class IntArg: public BaseArg<int>{

public:

	IntArg(const Name& n, const bool& m, const char* c): BaseArg<int>(n, ArgType::INT, m, c){}
	IntArg(const int& i, const Name& n, const bool& m, const char* c): BaseArg<int>(i, n, ArgType::INT, m, c){}

};

// Ou alors simplement faire des maps avec des clés pré-remplies et empecher l'ajout de nouvelles clés ?
//  => Comment exporter ça vers JS ?
//  => Comment déterminer la méthode de set() ? Avec un template ?

// Try to use a simple vector instead, and make a predefined set of arguments ?
// -> Utiliser une classe "Usine" qui est capable d'instancier une collection de paramètres ?
// -> Une collection hétérogène va être complexe à former et à stocker ...
// -> Étant donné que nous sommes sur des petites structures, on pourrait faire le sacrifice d'un vector hétérogène.

// Retirer le nom de l'argument lui-même et le mettre dans la key de la map ?
// Utiliser une structure custom plutôt qu'une map

class BasicObjectArgs{

};

// - Créer un attribut spécial pour le nom
// - Les valeurs numériques devrait avoir des bornes possibles
// - Certains attributs doivent pouvoir prendre leur valeur parmi un panel (menu déroulant / enum)
// - Créer des attributs calculés ? Qui prennent leur valeur en fonction des autres d'un même bloc ?

/*

Admettons que nous sommes dans le cas où nous avons choisi la POO élégante côté C++.

	- Les séquences, shots, ... sont des classes qui héritent de Folder.
		=> Comment peut-on scripter de nouvelles classes ?
	- Les arguments pour passer des données au constructeur de chacune de ces structures sont
	  des classes qui sont des collections de dérivés de BaseArg.
	  	=> Comment construire une nouvelle collection pour un nouveau type ?
	- Les Actions sont des objets attachés par classe (Folder, Sequence, ...)
	  Un objet génère lui-même la liste d'actions à laquelle il pourrait être soumises

1. Je veux créer via Python un nouveau FSObject, ou quelque chose qui se comporte comme tel.
2. Je veux qu'il ait ses propres arguments.
3. Je veux qu'il ait ses propres actions.
4. Je veux que cet objet soit affichable par le client navigateur.

    = = Exemple = =

 => Je veux créer la catégorie "render" en Python.

- Cette catégorie (si elle était faite en C++) devrait hériter de Folder (ou Versionable).
- Cette catégorie devrait avoir un attribut propre disant si le rendu doit être fait avec Cycles ou Eevee.
- Quand on créé cet objet, la fenêtre de paramètres doit proposer de configurer le-dit paramètre.
- Seule une action dans shot devrait être capable d'instancier une unique instance d'un tel objet.
- Cet objet devrait posséder une action qui permet de lire tous les .png qu'il contient et utiliser ffmpeg ou tout
autre software pour les transformer en un .mp4.

*/

int main(int argc, char* argv[], char* env[]){

	IntArg ia(42, "New Seed", true, "Seed being used to generate new objects.");
	std::cout << ia.displayed_name << std::endl;
	std::cout << ia.description << std::endl;
	std::cout << ia.value() << std::endl;

	// FileSystem vfs("/tmp", true, "Testing Project");
	// std::cout << vfs.current()->data().name << std::endl;
	// vfs.current[0].open();
	// vfs.current[1].open();

	return 0;
}

// COMMENT AJOUTER DU CONTENU DANS UN FOLDER ?
/*

Sachant qu'il faut pouvoir:
	- Attacher à chaque objet de la meta-data (plus tard)
	- Attacher les caractéristiques basiques d'un FSObject

Quelle doit être la signature de la fonction pour ajouter du contenu dans un objet ?
	- Utiliser une struct dédiée qui représente un set d'arguments ?
	  => void add_folder(const FolderArgs& args, const Metadata& meta);
	  => void add_folder(const FolderArgs& args);

Comment savoir quels arguments ont été renseignés, et les quels ne l'ont pas été ?

Il faut un moyen d'ajouter plusieurs objets en même temps pour l'optimisation.

*/


// = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

// [ ] Intégrer un système de gestion de tickets

// [O] Quand on est dans un dossier, on veut pouvoir faire un right-click sur n'importe quel élément et voir les actions qui y sont associées
//     on ne peut donc pas avoir tous les objets stockés sous la forme d'un type générique jusqu'à l'ouverture.
//     Le contenu doit être stocké sous leur vrai forme ? 
//     => On stocke des vectors des types voulus et on utilise le design pattern iterator pour parcourir tous les vectors comme s'il n'y en avait qu'un
//        avec la méthode content() citée ci-dessus.


// = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
//                ASSESSMENTS
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = =


// - Each FSObject will hold the address of the FSObject which created it.
//   It will require a safety system in case of deletion

// - We need a way to determine the dynamic type of current once it is stored.
//   The best case would be to have a method returning a type, or an object which can represent a type.
//    [?] State machine (store a way to retrieve the type)
//    [?] std::variant & std::visit
//   If we can return a constant reference to a (std::variant / std::visit) we can use it to cast our current object

// - An object must have a method exposing its skills. This method is specialized along the FSObject hierarchy.

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

// - Abstract Factory (experimental solution for type deduction of current object)
// - Adapter (second experimental solution to handle specialization of current object)
// - Command (to implement Actions)
// - Composite (to process batches of folder as a single folder)
// - Decorator (adding the versionable behavior as a decorator of Folder rather than a specialization)
// - Flyweight (to handle the content of objects)
// - Mediator (interaction objects-actions / management of versionable behavior)
// - Memento (save)
// - Visitor (?)

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

/*

	- Les actions lors du right click
	- Le parcours des éléments contenus
	- Le comportement pour le versioning -> Le versioning est une action interne
	   => Faire en sorte que les actions spécifiques soient des actions internes ?
	   => Si on veut faire des actions en batch: (ex: appliquer l'action de rendu à tous les shots sélectionnés d'une séquence)
	   		- Pour la sélection, il faut que les éléments soient instanciés (selon l'implémentation actuelle)
	   		- Il faut pouvoir détecter que ce sont des shots (donc leur métadata doit être chargée)
	   		- Il faut détecter que ce sont des versionables (donc on a besoin de leur type dynamique)

*/

/*

	Comment ré-ouvrir un object à travers previous() et next() ?
	-> On ne peut pas utiliser les même méthodes que pour l'ouverture normale, sinon il va y avoir le stacking.
	-> Il faut passer au change_directory sans passer par le open() de l'object en question.

*/