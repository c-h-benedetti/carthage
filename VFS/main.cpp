#include <iostream>
#include "FileSystem.hpp"
#include <fstream>

/*

			= = = SCRIPTING & CLIENTS = = =

- Les séquences, shots, ... sont des classes qui héritent de Folder.
	=> Les classes hard-codées héritent de Folder
	=> Les classes scriptées sont des instances dont les valeurs sont remplies à la main.
	=> Il faut donc un moyen de stocker la métadata associée à un type d'instance (scripté ou non).

- Les arguments pour passer des données au constructeur de chacune de ces structures sont
  des classes qui sont des collections de dérivés de BaseArg.
  	=> Les arguments des nouvelles classes sont des instances de BaseArg<T> remplies à la main.
  	=> Il faut quand même mettre à disposition des fonctions de contrôle.

- Les Actions sont des objets attachés par classe (Folder, Sequence, ...)
  Un objet génère lui-même la liste d'actions à laquelle il pourrait être soumises
  	=> Le poll() d'une action doit donc aussi analyser la métadata pour déterminer si on peut trigger execute().

1. Je veux créer via Python un nouveau FSObject, ou quelque chose qui se comporte comme tel.
2. Je veux qu'il ait ses propres arguments.
3. Je veux qu'il ait ses propres actions.
4. Je veux que cet objet soit affichable par le client navigateur.

    = = Exemple = =

 => Je veux créer la catégorie "Render" en Python.

- Cette catégorie (si elle était faite en C++) devrait hériter de Folder (ou de Versionable).
- Cette catégorie devrait avoir un attribut propre disant si le rendu doit être fait avec Cycles ou Eevee.
- Quand on créé cet objet, la fenêtre de paramètres doit proposer de configurer le-dit paramètre.
- Seule une action dans shot devrait être capable d'instancier une unique instance d'un tel objet.
- Cet objet devrait posséder une action qui permet de lire tous les .png qu'il contient et utiliser ffmpeg ou tout
autre software pour les transformer en un .mp4.

*/


// IMPROVE: [General] Instead of sending errors to std::cerr, we should use a global stream for logs.
// IMPROVE: [General] Make functions to manipulate the VFS instead of manipulation des std::fstream all the time. These functions could at least handle the endianness of the size of the segment and the next pointer.
// IMPROVE: [General] Make a debug mode with preprocessor blocks.

void test_vfs_1(){

	FileSystem vfs("/tmp", true, "Project");

	ArgsNewFolder anF;
	anF.name = "MyFolder";
	anF.icon = 5;

	ArgsNewFolder anF2;
	anF2.name = "Other";
	anF2.icon = 5;

	ArgsNewFile anfi;
	anfi.extension = "TXT";
	anfi.name = "Document";
	anfi.icon = 5;

	ArgsNewFile anfe;
	anfe.extension = "TXT";
	anfe.name = "Some File";
	anfe.icon = 5;

	vfs.get_folder()->new_folder(anF);
	vfs.get_folder()->new_folder(anF2);
	vfs.current()->new_file(anfi);
	vfs.current()->new_file(anfe);

	vfs.get_folder()->get_folders()[0].open();

	vfs.current()->new_folder(anF);

	vfs.get_folder()->get_folders()[0].open();

	vfs.current()->new_file(anfi);
	vfs.current()->new_file(anfe);

	vfs.make_hierarchy("/home/clement/Desktop/vfs.html");

}

void inspect_vfs(){

	std::ifstream str("/tmp/.carthage/vfs.ctg");
	FSize size;
	FSBlock block;
	FSPos next_b;

	for (size_t i = 0 ; i < 4 ; i++){
		std::cout << "Reading at: " << str.tellg() << std::endl;

		BasicBuffer<sizeof(FSize)+sizeof(FSBlock)+sizeof(FSPos)> buffer;
		buffer.read_from(str);
		BufferReader head(buffer);
		head.get<FSize>(size).get<FSBlock>(block).get<FSPos>(next_b);

		std::cout << "Seg size: " << size << ", Next Block: " << next_b << std::endl;
		std::cout << "Name: " << block.name << ", Content: " << block.content << ", Parent: " << block.parent << std::endl;

		switch(block.flag){
			case(FSType::FOLDER):
				std::cout << "Type: (FDR) " << std::endl;
				break;
			case(FSType::FILE):
				std::cout << "Type: (FIL) " << std::endl;
				break;
			case(FSType::VERSIONABLE):
				std::cout << "Type: (VER) " << std::endl;
				break;
			case(FSType::ROOT):
				std::cout << "Type: (ROT) " << std::endl;
				break;
			case(FSType::REMOVED):
				std::cout << "Type: (XXX) " << std::endl;
				break;
			default:
				std::cout << "Type: (   ) " << std::endl;
		};

		std::cout << "=======" << std::endl;
	}

	str.close();
}

int main(int argc, char* argv[], char* env[]){

	test_vfs_1();
	inspect_vfs();

	return 0;
}

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

// 			= = = IMPLEMENTATION PRIORITIES = = =

// - [X] Replace FSType equality tests by functions testing if the flag is raised in an instance.
// - [ ] Implement the Versionable type.
// - [ ] Implement the previous() and next() methods.
// - [ ] Create a batch of unit tests.
// - [ ] Implement functions to interact with the VFS block-wise.
// - [ ] Transform FileSystem into a state machine.
// - [ ] Turn the FileSystem pointer into a reference.

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

// [>] Quand on est dans un dossier, on veut pouvoir faire un right-click sur n'importe quel élément et voir les actions qui y sont associées
//     on ne peut donc pas avoir tous les objets stockés sous la forme d'un type générique jusqu'à l'ouverture.
//     Le contenu doit être stocké sous leur vrai forme ? 
//     => On stocke des vectors des types voulus et on utilise le design pattern iterator pour parcourir tous les vectors comme s'il n'y en avait qu'un
//        avec la méthode content() citée ci-dessus.


// = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
// =                 ASSESSMENTS                           =
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

// - We should iterate through IDs by files, by folders and by versionables instead of
//   iterating directly on the objects. We don't want the user to call directly methods on FSObjects.
//   (The user can call const methods anyway)
//   Only the FileSystem should be able to call methods on FSObjects.

// - Each FSObject will hold the address of the FileSystem that created it.
//   It will require a safety system in case of deletion, or the usage of reference.

// - We need a way to determine the dynamic type of the current_obj once it is stored.
//   Using a state-machine implementation eases this control.
//   [?] Try to use std::variant & std::visit

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

// 			= = = INTERESTING DESIGN PATTERNS = = =

// - Abstract Factory (experimental solution for type deduction of current object)
// - Adapter (second experimental solution to handle specialization of current object)
// - Command (to implement Actions)
// - Composite (to process batches of folder as a single folder)
// - Decorator (adding the versionable behavior as a decorator of Folder rather than a specialization)
// - Flyweight (to handle the content of objects)
// - Mediator (interaction objects-actions / management of versionable behavior)
// - Memento (save)
// - Visitor (for display)

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
