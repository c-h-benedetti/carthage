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


// DEBUG: [General] Make a debug mode with preprocessor blocks.


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

	std::cout << "    \"Parent\": " << block.parent << ", " << std::endl;
	
}

// IMPROVE: [FileSystem] The ask_reader() method must be const.

void vfs_to_json(FileSystem& vfs){
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
		std::cout << "  \"Segment Size\": " << seg_size << ", " << std::endl;
		std::cout << "  \"Blocks\":  [" << std::endl;
		for (size_t i = 0 ; i < seg_size ; i++){
			std::cout << "    {" << std::endl;
			display_block(blocks[i]);
			std::cout << "    \"Position\": " << pos + i * sizeof(FSBlock) << std::endl;
			if (i < seg_size - 1){
				std::cout << "    }," << std::endl;
			}
			else{
				std::cout << "    }" << std::endl;
			}			
		}
		std::cout << "  ]," << std::endl;
		std::cout << "  \"Next\": " << next << std::endl;
		std::cout << "  }" << std::endl;
	});
	std::cout << "]" << std::endl;
}


/*
- Comment déterminer si un objet est local ou non ? Les objets du VFS ne sont pas garantis d'exister sur le système.

- On veut rajouter FSType::LINK pour pointer vers un autre objet. Toutes les actions faites sur cet objet doivent être
répercutées sur l'original.

*/


// DEBUG: [General] Check that the letters' distribution for IDs is uniform.
// DEBUG: [General] Some functions should systematically reload their argument from the VFS before performing anything.
// DEBUG: [VFS] The VFS should only allow one modification at a time. How to make sure that the read data won't be discarded by the next writing?

int some_test(){
	FileSystem vfs("/tmp", true, "Carthage");

	ArgsNewFolder anF1;
	anF1.name = "AAA";
	anF1.icon = 3;

	Container* dest = vfs.current();
	
	if (vfs.current()->create_folder(anF1)){
		std::cout << "FAILED 1" << std::endl;
	}

	ArgsNewFolder anF2;
	anF2.name = "BBB";
	anF2.icon = 3;
	
	if (vfs.current()->create_folder(anF2)){
		std::cout << "FAILED 2" << std::endl;
	}


	ArgsNewFolder anF3;
	anF3.name = "CCC";
	anF3.icon = 3;
	
	if (vfs.current()->create_folder(anF3)){
		std::cout << "FAILED 3.1" << std::endl;
	}


	vfs.current()->at(0).open();

	anF1.name = "DDD";
	anF2.name = "EEE";
	anF3.name = "FFF";

	if (vfs.current()->create_folder(anF1)){
		std::cout << "FAILED 1" << std::endl;
	}

	if (vfs.current()->create_folder(anF2)){
		std::cout << "FAILED 2" << std::endl;
	}

	if (vfs.current()->create_folder(anF3)){
		std::cout << "FAILED 3.1" << std::endl;
	}

	vfs.current()->at(1).open();
	std::vector<FSObject> sources = {
		*(vfs.current())
	};

	anF1.name = "GGG";
	anF2.name = "HHH";
	anF3.name = "III";

	ArgsNewFile anf1;
	anf1.name = "file1";
	anf1.extension = "blend";

	if (vfs.current()->create_file(anf1)){
		std::cout << "FAILED 0" << std::endl;
	}

	if (vfs.current()->create_folder(anF1)){
		std::cout << "FAILED 1" << std::endl;
	}

	if (vfs.current()->create_folder(anF2)){
		std::cout << "FAILED 2" << std::endl;
	}

	if (vfs.current()->create_folder(anF3)){
		std::cout << "FAILED 3.1" << std::endl;
	}

	vfs.copy_to(sources.data(), dest, 1, nullptr);

	// vfs_to_json(vfs);

	vfs.make_hierarchy("/home/clement/Desktop/carthage/experimental/vfs_hierarchy/vfs.html");

	return 0;
}

void test_versioning(){
	FileSystem vfs("/tmp", true, "Carthage");

	ArgsNewVersionable anV;
	anV.name = "Some Stuff";
	anV.icon = 8;

	ArgsNewFile anf1;
	anf1.name = "file1";
	anf1.extension = "blend";

	ArgsNewFolder anF3;
	anF3.name = "CCC";
	anF3.icon = 3;

	ArgsNewFolder anF2;
	anF2.name = "BBB";
	anF2.icon = 3;

	vfs.current()->create_versionable(anV);
	vfs.current()->at(0).open();
	vfs.current()->create_folder(anF3);
	vfs.current()->versionable().commit();
	vfs.current()->create_file(anf1);
	vfs.current()->versionable().commit();
	vfs.current()->create_folder(anF2);

	vfs.make_hierarchy("/home/clement/Desktop/carthage/experimental/vfs_hierarchy/vfs.html");
}

int main(int argc, char* argv[], char* env[]){
	
	srand(time(NULL));

	test_versioning();

	return 0;	
}


// IMPROVE: [Versionable] Zip the content once the VFS is written (after a version was done). Unpacking a version will fetch the only zip in the Folder
// IMPROVE: [General] Add the metadata file (useful for commit messages) and the dependencies file.

// IMPROVE: [General] Try to make safer the building of path and the reference to parents. Maybe stock a representation of the parent?





// DEBUG [General]: Control that we are always in the range of a FSize when we add an element in a Container.

/*

IMPLEMENT:

- [ ] The copy operation
- [ ] The prev_stack and next_stack operations
- [ ] Operations specific to Versionable
- [ ] Refresh the implementation of the "hierarchy to HTML" method.
- [ ] The defragmentation method.

*/



