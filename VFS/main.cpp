#include <iostream>
#include "FileSystem.hpp"

int main(int argc, char* argv[], char* env[]){

	/*FileSystem vfs("testing", true);
	vfs.current[0].open();
	vfs.current[1].open();*/

	return 0;
}

// Problème: Étant donné que le FileSystem n'est jamais dépilé, on peut perpetuellement le parcourir.
// Le file system ne devrait pas être capable d'utiliser les fonctions d'itération
// On fixe la racine à un unique dossier, on peut alors sortir FileSystem des FSObject ?
// Il faut que current pointe sur le bon objet au départ.

// [O] Container ne contiendra que des FSBlock et on ne fera un cast que lorsqu'on voudra ouvrir le dit contenu
//     Pour l'affichage, on fera des sort successifs par nom, type, ...

// [O] const std::unique_ptr<Container>& FileSystem::current() const; 
//     => Returns the current container, the last opened

// - const std::vector<FSBlock>& Container::content() const; 
//   => Non-opened elements are being stored as FSBlock, not precisely typed objects

// [?] How to handle specific operation with this point of view ? If we perform type tests, objects are useless.
//     There must be a cast at some point.
// ex: How to handle versioning from the FileSystem ?

// - Since the current object is held by a pointer, we can do a last minute cast to handle operations ?

/*
FileSystem fs{"some/path"};
for (const FSBlock& block : fs.current().content()){
	if (block.data().data.name.equals("Some Name")){
		fs.open(block.id());
	}
}
*/

// - Quand on est dans un dossier, on veut pouvoir faire un right-click sur n'importe quel élément et voir les actions qui y sont associées
//   on ne peut donc pas avoir tous les objets stockés sous la forme d'un type générique jusqu'à l'ouverture.
//   Le contenu doit être stocké sous leur vrai forme ? 
//   => On stocke des vectors des types voulus et on utilise le design pattern iterator pour parcourir tous les vectors comme s'il n'y en avait qu'un
//      avec la méthode content() citée ci-dessus.

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

// [X] Impossible to implement this way, we don't know what containers are able to propose files or folders, ...
//     The method must be in the static type
//     => We can return a pointer instead of a reference and check for the nullity of this pointer.

/*
FileSystem fs{"some/path"};
for (const File& file : fs.current().files()){
	if (block.data().data.name.equals("Some Name")){
		fs.open(block.id());
	}
}
*/

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

// - To prevent the user from providing the FS address himsef for each method, we can store it as variable in the object.
//   => The FS type is declared from the first lines of FSObject

// - current() could be a template method raising an exception if the template-type is different from the block type ?

// - The open() method implies very specific behaviors from the object, which imply to know the dynamic type of the object before we call open().
// 

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
//                ASSESSMENTS
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

// - Each FSObject will hold the address of the FSObject which created it.
//   It will require a safety system in case of deletion

// - Objects will be stored in a unique vector in the Container object.
//   The cast will be performed just before the open() method was called.

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
