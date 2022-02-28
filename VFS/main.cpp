#include <iostream>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include "FileSystem.hpp"

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

// DEBUG: [General] Make a debug mode with preprocessor blocks.
// DEBUG: [General] Make a function inspecting the VFS linearly block by block.

void test_vfs_1(FileSystem& vfs){

	ArgsNewFolder anF1;
	anF1.name = "Mon Dossier";
	anF1.icon = 4;

	ArgsNewFolder anF2;
	anF2.name = "Autre Dossier";
	anF2.icon = 5;

	ArgsNewFile anf1;
	anf1.name = "Premier Fichier";
	anf1.extension = "txt";
	anf1.icon = 6;

	ArgsNewFile anf2;
	anf2.name = "Autre Fichier";
	anf2.extension = "txt";
	anf2.icon = 7;

	if (vfs.folder()){
		std::cerr << "The current obj is a Folder" << std::endl;
		if (vfs.folder()->accept_folders()){
			vfs.folder()->new_folder(anF1);
			vfs.open(vfs.folder()->at(0));

			vfs.folder()->new_folder(anF2);
			vfs.open(vfs.folder()->at(0));

			vfs.folder()->new_file(anf1);
			vfs.folder()->new_file(anf2);
			vfs.open(vfs.folder()->at(0));
		}
	}
	else if (vfs.versionable()){
		std::cerr << "The current obj is a Versionable" << std::endl;
	}
	else{
		std::cerr << "The current obj has an unknown type" << std::endl;
	}
}


void test_vfs_2(FileSystem& vfs){
	ArgsNewFolder F1;
	F1.name = "F1";
	F1.icon = 4;

	ArgsNewFolder F2;
	F2.name = "F2";
	F2.icon = 5;

	ArgsNewFolder F3;
	F3.name = "F3";
	F3.icon = 6;

	ArgsNewFile f1;
	f1.name = "f1";
	f1.extension = "txt";
	f1.icon = 7;

	ArgsNewFile f2;
	f2.name = "f2";
	f2.extension = "txt";
	f2.icon = 7;

	vfs.current()->new_folder(F2);
	vfs.open(vfs.current()->at(0));
	vfs.current()->new_folder(F3);
	vfs.current()->new_file(f2);
	vfs.previous();
	vfs.current()->new_folder(F1);
	vfs.open(vfs.current()->at(1));
	vfs.current()->new_file(f1);
}

// IMPROVE [FileSystem]: What happens in we try to call open with an index out of range, or 0 on an empty Container?

void test_vfs_3(FileSystem& vfs){

	for (size_t i = 0 ; i < 50000 ; i++){

		int c = rand() % 5;

		if (c == 0){ // Opening content
			if (vfs.current()->size() > 0){
				vfs.open(vfs.current()->at(rand() % vfs.current()->size()));
			}
		}
		else if (c == 1){ // previous
			vfs.previous();
		}
		else if (c == 2){ // next
			vfs.next();
		}
		else if (c == 3){ // new file
			ArgsNewFile fi;
			fi.icon = 0;
			fi.extension = ".txt";
			ID id(1);
			fi.name = id.c_str();
			vfs.current()->new_file(fi);
		}
		else if (c == 4){ // new folder
			ArgsNewFolder fo;
			fo.icon = 0;
			ID id(1);
			fo.name = id.c_str();
			vfs.current()->new_folder(fo);
		}
	}
}

void display_block(const FSBlock& block){
	std::cout << "{" << std::endl;
	std::cout << "  Content: " << block.content << std::endl;
	std::cout << "  ID: " << block.id << std::endl;
	std::cout << "  Name: \"" << block.name << "\"" << std::endl;

	if (folder_raised(block.flag)){
		std::cout << "  Nb Files: " << block.nb_files << std::endl;
		std::cout << "  Nb Folders: " << block.nb_folders << std::endl;
		std::cout << "  Nb Versionables: " << block.nb_versionables << std::endl;
	}
	else if(file_raised(block.flag)){
		std::cout << "  Extension: (" << block.extension << ")" << std::endl;
	}

	std::cout << "  Parent: " << block.parent << std::endl;
	std::cout << "}" << std::endl;
}


void inspect_vfs(const FileSystem& vfs){

	std::ifstream str(vfs.vfs_path(), std::ios::in | std::ios::binary);
	if (!str.is_open()){
		std::cerr << "Failed to open the VFS" << std::endl;
		return;
	}

	FSize seg_size = 0;
	FSPos start_blocks = 0;
	FSPos next = 0;
	FSPos pos = 0;

	while(str.peek() != EOF){
		str.read((char*)&seg_size, sizeof(FSize));
		if (seg_size){
			FSBlock blocks[seg_size];
			start_blocks = str.tellg();
			str.read((char*)blocks, seg_size * sizeof(FSBlock));

			for(size_t i = 0 ; i < seg_size ; i++){
				blocks[i].after_reading();
				std::cout << "Position: " << start_blocks << std::endl;
				display_block(blocks[i]);
				start_blocks += sizeof(FSBlock);
			}

			str.read((char*)&next, sizeof(FSPos));
		}
		else{
			FSPos p = str.tellg();
			p -= sizeof(FSize);
			std::cerr << "Empty segment detected at position: " << p << std::endl;
		}
	}

	str.close();
}


int main(int argc, char* argv[], char* env[]){
	
	srand(time(NULL));

	FileSystem vfs("/tmp");

	// test_vfs_3(vfs);
	inspect_vfs(vfs);
	vfs.make_hierarchy("/home/clement/Desktop/vfs.html");

	return 0;
}

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

// 		= = = IMPLEMENTATION PRIORITIES = = =

// - [ ] Implement the versioning.
// - [ ] Create a batch of unit tests.
// - [ ] Implement functions to interact with the VFS block-wise.
// - [ ] Transform FileSystem into a state machine.
// - [ ] Handle endianness of seg_size & next.
// - [ ] Implement after_reading() & before_writing().
// - [ ] Add a method for defragmentation.
// - [ ] Methods interfacing the objects.
// - [ ] Add the Table alongside the VFS, and a global_open() method.
// - [X] Make the filter checking for a versionable object on the stack. Use it in accept_versionable().
// - [X] Implement the previous() and next() methods.
// - [X] Replace FSType equality tests by functions testing if the flag is raised in an instance.
// - [X] Turn the FileSystem pointer into a reference.

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

// When we create a version, do we do not want to modify the content of the current folder.
// We just want to create a new folder and recursively copy the content of current in this new folder.
// The function used could also be used for the defragmentation.
// For this function, we would like the FileSystem to be locked in writing.
// The Project must be responsible for delegating the fstream objects. 

// Prototype:
// On a besoin d'un path dans ce prototype
// La fonction de copie doit aussi copier l'élément sur lequel elle est summoned.
// Le contenu doit être copié en parallèle de la copie sur le VFS.

// Le current_path ne devrait pas être stocké mais calculé à la volée ?

// La fonction récursive copie le contenu du block passé en argument, pas le block lui-même.
// Le block passé en argument a déjà été copié par la fonction de lancement si c'est la racine, ou par le précédent appel dans le cas normal.
// We maybe need to instanciate Files and Folders and call a copy(const Path& p) method on them.
void Container::recursive_copy(const FSBlock& obj, std::ifstream& vfs_in, std::ofstream& vfs_out, const FSPos& parent, Path start){

	std::vector<FSBlock> blocks;
	blocks.reserve(obj.nb_files + obj.nb_folders);

	FSPos c_pos = obj.content;
	FSize seg_size = 0;

	while (c_pos){
		vfs_in.seekg(c_pos);
		vfs_in.read((char*)&seg_size, sizeof(FSize));
		FSBlock raw_blocks[seg_size];
		vfs_in.read((char*)raw_blocks, seg_size*sizeof(FSBlock));

		for (size_t i = 0 ; i < seg_size ; i++){
			raw_blocks[i].after_reading()
			blocks.push_back(raw_blocks[i]);
		}
	}

}

// DEBUG [General]: Control that we are always in the range of a FSize when we add an element in a Container.
// Cette fonction fait une copie de l'objet appelant à la fin du VFS.
// Le linking est la responsabilité de l'utilisateur de cette fonction.
// This function should also be able to copy Versionable objects.
// -> Since copy_to() and recursive_copy() are methods of Container, the caller is necessarily represented by a folder on the system.
// -> To make a copy, we also need the path of the original, so the path passed to the function must be the path of the current object ?
// path_of_original is the path of the element to copy.



// path_of_original: Path of the current object on the system.
// parent: Container that will own the copy
// destination: Destination on the system of the copy.

// Le nouvel élément créé doit être du type de l'appelant

void Container::init_copy_to(const Container& parent, const Path& path_of_original, const Path& destination){
	// VFS ouvert en ajout à la fin.
	std::ofstream vfs_out(this->refer_to.vfs_path(), std::ios::out | std::ios::app | std::ios::binary);
	// VFS ouvert en lecture pure.
	std::ifstream vfs_in(this->refer_to.vfs_path(), std::ios::in | std::ios::binary);

	// Copie du block de l'objet appelant dans un segment dédié de taille 1.
	BasicBuffer<sizeof(FSize) + sizeof(FSBlock) + sizeof(FSPos)> buffer;
	FSBlock b = this->block;



	b.refore_writting();
	buffer.add<FSize>(1).add<FSBlock>(b).add<FSPos>(0);
	buffer.write_to(this->vfs_out);

	// Lancement de copie récursive du contenu depuis la racine qu'est l'appelant.
	this->recursive_copy(
		this->block,
		vfs_in,
		vfs_out,
		this->block_pos,
		path_of_original,
		destination
	);

	// Fermeture des 2 instances de VFS.
	vfs_out.close();
	vfs_in.close();
}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

// IMPROVE [General]: The VFS must ABSOLUTELY be locked when we write inside.

virtual void Container::copy_to(Container* parent){}

// We must create a new Folder and link it to parent
// The new_folder function can not be used because the path loaded in refer_to is not the good one.
// We must overload the function creating a Folder with one taking a path.
void Folder::copy_to(Container* parent) override{
	if (parent && parent->accept_folders()){
		ArgsNewFolder anF;
		anF.name = this->block.name;
		anF.icon = this->block.icon;
		parent->new_folder(anF);

		// Implement a better way to get the last created element.
		Container* fdr = &(parent->at(parent->size() - 1));
		this->init_copy_to(fdr, this->fs.current_syspath());
	}
}

// We must create a new Versionable and link it to parent
void Versionable::copy_to(Container* parent) override{
	if (parent && parent->accept_folders()){
		ArgsNewFolder anF;
		anF.name = this->block.name;
		anF.icon = this->block.icon;
		parent->new_folder(anF);

		// Implement a better way to get the last created element.
		Container* fdr = &(parent->at(parent->size() - 1));
		this->init_copy_to(parent, this->fs.current_syspath().parent_path());
	}
}

// The recursive_copy() method can be common to every container, but the initializer must be virtual because a new element must be created.
// Where to copy things in term of VFS? Who owns the copy? The function of recursive copy must also take a block as argument, which gonna be the owner.


// Dans la phase de defragmentation, détecter si un Container est vide pour passer son pointeur de contenu à 0.
// Faire un truc qui détecte les modifications d'une version à l'autre.

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

// We want to be able to call size() sur un FileSystem.
// We want to call new_file(), new_folder() et new_versionable() sur un FileSystem.
// We need a method is_versionable(), and then a method to cast to a Versionable*.
// We can remove the Folder* cast.

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

/*

Pour implémenter previous() et next(), on a besoin d'un index sur l'élément qu'on est actuellement entrain de visiter.
Ces méthodes vont avoir une incidence sur le stack.

Quand on fait open() sur un élément, la tête pointe toujours sur le dernier élément du stack.

Quand on fait previous() (si le stack contient quelque chose):
	- Le stack n'est pas modifié, la tête est simplement renvoyée une position avant
	- Attention au cas où le stack ne contient qu'un seul élément
	- Peut-être que la racine devrait être insérée dans le stack pour ne pas qu'elle puisse être détruite, et qu'on pose une condition sur la valeur de la tête.
	  => Parce-que là, l'objet racine est détruit à l'ouverture du premier fichier, il est donc impossible de le retrouver.

*/