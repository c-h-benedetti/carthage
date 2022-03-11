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

void cheat_inspect_vfs(FileSystem& vfs){

	std::ifstream str(vfs.vfs_io().vfs_path);

	FSize seg_size = 0;
	FSBlock block;
	FSPos next = 0;

	while (str.peek() != EOF){
		str.read((char*)&seg_size, sizeof(FSize));
		str.read((char*)&block, sizeof(FSBlock));
		str.read((char*)&next, sizeof(FSPos));

		std::cout << "Size: " << seg_size << std::endl;
		std::cout << "Name: " << block.name << ", Parent: " << block.parent << ", Content: " << block.content << std::endl;
		std::cout << "Next: " << next << std::endl;
		std::cout << "========" << std::endl;
	}

	str.close();
}


int main(int argc, char* argv[], char* env[]){
	
	srand(time(NULL));

	FileSystem vfs("/tmp", true, "Carthage");

	vfs.current()->override_vfs();

	ArgsNewFolder anF1;
	anF1.name = "F1";
	anF1.icon = 3;
	
	if (vfs.current()->create_folder(anF1)){
		std::cout << "FAILED" << std::endl;
	}

	ArgsNewFolder anF2;
	anF2.name = "F2";
	anF2.icon = 3;
	vfs.current()->create_folder(anF2);

	inspect_vfs(vfs);


	return 0;
}


// DEBUG [General]: Control that we are always in the range of a FSize when we add an element in a Container.

