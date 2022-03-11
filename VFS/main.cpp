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
/*
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
*/

void display_block(const FSBlock& block){
	
	std::cout << "    \"Content\": " << block.content << ", " << std::endl;
	std::cout << "    \"ID\": \"" << block.id << "\"" << ", " << std::endl;
	std::cout << "    \"Name\": \"" << block.name << "\"" << ", " << std::endl;

	if (folder_raised(block.flag)){
		std::cout << "    \"Nb Files\": " << block.nb_files << ", " << std::endl;
		std::cout << "    \"Nb Folders\": " << block.nb_folders << ", " << std::endl;
		std::cout << "    \"Nb Versionables\": " << block.nb_versionables << ", " << std::endl;
	}
	else if(file_raised(block.flag)){
		std::cout << "    \"Extension\": \"" << block.extension << "\"" << ", " << std::endl;
	}

	std::cout << "    \"Parent\": " << block.parent<< std::endl;
	
}

// IMPROVE: [FileSystem] The ask_reader() method must be const.

void inspect_vfs(FileSystem& vfs){
	std::cout << "[" << std::endl;
	bool first = true;

	vfs.vfs_io().ask_reader().linear_browsing([&](const FSize& seg_size, const FSBlock* blocks, const FSPos& pos, const FSPos& next){
		if (first){
			std::cout << "  {" << std::endl;
			first = false;
		}
		else{
			std::cout << "  ,{" << std::endl;
		}
		std::cout << "  \"Blocks\":  [" << std::endl;
		for (size_t i = 0 ; i < seg_size ; i++){
			std::cout << "    {" << std::endl;
			display_block(blocks[i]);
			if (i < seg_size - 1){
				std::cout << "    }," << std::endl;
			}
			else{
				std::cout << "    }" << std::endl;
			}			
		}
		std::cout << "  ]," << std::endl;
		std::cout << "  \"Position\": " << pos << ", " << std::endl;
		std::cout << "  \"Next\": " << next << std::endl;
		std::cout << "  }" << std::endl;
	});
	std::cout << "]" << std::endl;
}


int main(int argc, char* argv[], char* env[]){
	
	srand(time(NULL));

	FileSystem vfs("/tmp", true, "carthage");

	/*ArgsNewFolder anF1;
	anF1.name = "F1";
	anF1.icon = 3;
	
	if (vfs.current()->create_folder(anF1)){
		std::cout << "FAILED" << std::endl;
	}

	ArgsNewFolder anF2;
	anF2.name = "F2";
	anF2.icon = 3;
	vfs.current()->create_folder(anF2);*/

	// inspect_vfs(vfs);


	return 0;
}


// DEBUG [General]: Control that we are always in the range of a FSize when we add an element in a Container.

