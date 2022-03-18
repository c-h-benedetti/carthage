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


/*

- Après qu'on ait construit notre buffer avec replicate, est-il possible d'aller chercher les bons chemins pour faire
les copies de fichiers? On a besoin de faire une copie que pour les fichiers, les dossiers peuvent être créés à la volée.


- Comment déterminer si un objet est local ou non ? Les objets du VFS ne sont pas garantis d'exister sur le système.
- On veut rajouter FSType::LINK pour pointer vers un autre objet. Toutes les actions faites sur cet objet doivent être
répercutées sur l'original.

*/

/*

The replicate_to() function works only for one element, but it must be completed by another function.
The first function must copy in a single segment the level 0 of what's copied.
Then, on each newly created element, the replicate_to() function must be called to make a recursive copy.
With the path deduction, the method could be moved to Container instead of FileSystem.

*/

// DEBUG: [General] Check that the letters' distribution for IDs is uniform.

int main(int argc, char* argv[], char* env[]){
	
	srand(time(NULL));

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

	if (vfs.current()->create_folder(anF1)){
		std::cout << "FAILED 1" << std::endl;
	}

	if (vfs.current()->create_folder(anF2)){
		std::cout << "FAILED 2" << std::endl;
	}

	if (vfs.current()->create_folder(anF3)){
		std::cout << "FAILED 3.1" << std::endl;
	}

	vfs.copy_to(sources, dest);

	vfs_to_json(vfs);

	return 0;
}


/*

Comment va -t- on gérer la copie d'éléments ? Est-il autorisé d'avoir des éléments qui ne sont pas instanciés et valides ?
Ne devrait-on pas faire l'implémentation en n'utilisant jamais deduce_path() ? Tout objet devrait être généré légalement.

=> Si on fait cette supposition, les objets devraient stocker leur chemin entier (sans la racine) au cas où il y ait destruction d'un parent.
Dans l'état actuel, on ne peut faire une copie que sur un objet dont on est sur qu'aucun parent n'est détruit.

*/


/*
The first block must be copied in an individual way because it can be part of another segment, if several elements
have to be copied (not from a common root).

Il faut check que la source n'est pas préfixe de la destination.

Si un versionable est trouvé dans la hiérarchie, vérifier qu'il peut être copié.
=> Pour annuler l'opération, retirer le pointeur de contenu est assez. Si rien ne pointe sur les blocks corrompus
ils seront laissés où ils sont.
*/




// DEBUG [General]: Control that we are always in the range of a FSize when we add an element in a Container.

/*

IMPLEMENT:

- [ ] The copy operation
- [ ] The prev_stack and next_stack operations
- [ ] Operations specific to Versionable
- [ ] Refresh the implementation of the "hierarchy to HTML" method.
- [ ] The defragmentation method.

*/



