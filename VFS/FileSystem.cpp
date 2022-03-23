#include "FileSystem.hpp"
#include "File.hpp"
#include "Versionable.hpp"
#include <fstream>
#include <string>
#include <utility>

// IMPROVE: [FileSystem] Update the content of this block along the implementation of new features

/**
 * Function returning the block every VFS starts with.
 */
static FSBlock file_system_default(const char* n){
	FSBlock b;

	b.content = 0;
	b.id.randomize();
	b.name = (n) ? (n) : ("Data");
	b.flag = pair_flags(FSType::ROOT, FSType::FOLDER);

	return b;
}

/**
 * Creates a new empty VFS from scratch.
 * Only the Carthage's hierarchy and a root are added (which is the minimal)
 */
int FileSystem::new_vfs(const char* n){

	// = = = 1. Creating the root of the VFS. The first block, in the first segment. = = =
	FSBlock block = file_system_default(n);

	// = = = 2. Creating the data's folder on the system, according to the newly created block. = = =
	SystemName name(block.id);
	Path data = this->io_manager.user_root / name.c_str();

	if (!fs::create_directory(data)){
		return 2;
	}

	block.time = fs::last_write_time(data);

	// = = = 3. Inserting a segment in the VFS file. The VFSWriter constructor will create it if it doesn't exist. = = =
	return this->io_manager.ask_writer(true).sequence(0).add<FSize>(1).add<FSBlock>(block).add<FSPos>(0).status();
	// IMPROVE: [FileSystem] Error handling on output in new_vfs()
}



// DEBUG: [FSBlock] Check why calling constructor Name{0} or Extension{0} makes a segmentation fault.

/**
 * Method used to load the very first segment of a VFS.
 * The first segment is constant in term of size.
 * Its size is always of 1, and its next pointer is to 0.
 */
int FileSystem::open_root(){
	
	// = = = 1. Creating a new Folder object and filling it with the first block of the VFS. = = =
	this->stack.push_back(
		new Folder{
			*this,
			FIRST_BLOCK_POSITION
		}
	);
	
	// Checking that the flag is correct.
	if (removed_raised(this->current()->get_block().flag)){
		BasicBuffer<128> msg(0);
		msg.copy_from("The root block of the VFS seems to be corrupted, its REMOVED flag is raised.");
		throw std::logic_error(msg.c_str());
	}
	
	// = = = 2. Opening the root as a regular Folder object. = = =
	return this->current()->load();
}


static Path join(const std::vector<Path>& paths){
	Path aggregate = "";
	for (const Path& item : paths){
		aggregate /= item;
	}

	return aggregate;
}


void FileSystem::create(const FSObject& src, const Path& src_path, const Path& dst_path){

	if (file_raised(src.get_block().flag)){
		fs::copy_file(src_path, dst_path);
	}
	else if(folder_raised(src.get_block().flag)){
		fs::create_directory(dst_path);
	}
	else if(versionable_raised(src.get_block().flag)){
		fs::create_directory(dst_path);
	}
}

/* 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	==> COPIE D'ELEMENTS D'UN OBJET VERS UN AUTRE

Phase 1: Copie du niveau 0 en un seul segment:
  - On recoit le tableau d'éléments à copier, la destination & possiblement des blocks de substitution.
  - À partir du moment où on accepte les éléments à copier, il faudrait que le write-lock soit fermé.
    On ne veut pas qu'un élément soit accepté et removed par un autre thread la microseconde qui suit.

Phase 2: Copie récursive de chaque élément:
  - Chaque appel de copie récursive devrait se faire avec son propre write-lock
  - À chaque nouvel appel, check que l'original n'a pas été delete ?
  - Cette fonction devrait peut-être devenir block-wise et bouger au niveau du VFSWriter. Ca serait plus facile pour contrôler le lock.
  - La différence avec la première version est que là, la source et la destination sont censées exister. On devrait donc cloner le contenu et pas le current à chaque itération.

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/

/**
 * This function copies all blocks in #source to #dest.
 * A test is performed to verify that #dest accepts each block.
 * The recursive copy is launched after the segment was created.
 */
void FileSystem::copy_to(FSObject* sources, Container* dest, const size_t& nb_items, FSBlock* as){
	
	if (!sources || !dest || dest->reload_from_vfs() || !dest->is_valid()){
		return;
	}

	std::vector<std::pair<FSObject, FSObject>> orig_and_copies;

	// Filtering objects that the destination could refuse.
	for (size_t i = 0 ; i < nb_items ; i++){
		
		// IMPROVE: [FileSystem] Maybe that creating a function reloading all sources with only one descriptor would save some time.
		FSObject& orig = sources[i];
		orig.reload_from_vfs();

		if (orig.is_valid() && dest->accepts(orig.get_block().flag)){
			FSBlock copy_data;

			if (as){
				copy_data = as[i];
			}
			else{
				copy_data = orig.get_block();
			}
			
			copy_data.id.randomize();
			copy_data.parent = dest->block_pos;
			copy_data.content = 0;

			FSObject copy(*this, copy_data, 0, dest); // path?

			orig_and_copies.push_back({orig, copy});
			this->create(orig, orig.get_system_path(), copy.get_system_path());
		}
	}

	FSPos head = 0;
	this->io_manager.ask_writer().blank_segment(orig_and_copies.size(), head);

	if (head > 0){
		OutputBuffer buffer;

		// Creating the new segment, and writing it to the VFS.
		buffer.add<FSize>(orig_and_copies.size());
		for (size_t i = 0 ; i < orig_and_copies.size() ; i++){
			std::pair<FSObject, FSObject>& obj = orig_and_copies[i];
			obj.second.set_block_position(head + sizeof(FSize) + i * sizeof(FSBlock));
			buffer.add<FSBlock>(obj.second.get_block());
		}
		buffer.add<FSPos>(0);
		this->io_manager.ask_writer().sequence(head).write_to_vfs(buffer);
	}

	FSBlock editions;

	for (size_t i = 0 ; i < orig_and_copies.size() ; i++){

		std::pair<FSObject, FSObject>& objs = orig_and_copies[i];

		if (folder_raised(objs.second.get_block().flag)){
			editions.nb_folders++;
		}
		else if (versionable_raised(objs.second.get_block().flag)){
			editions.nb_versionables++;
		}
		else{
			editions.nb_files++;
		}

		// this->replicate_to(objs.first, objs.second);
		this->io_manager.ask_writer().replicate_to(objs.first.get_block(), objs.second.get_block(), objs.first.get_system_path(), objs.second.get_system_path(), head + sizeof(FSize) + i * sizeof(FSBlock));
	}

	this->io_manager.ask_writer().link_segment(dest->get_block_position(), dest->editable_block(), head, editions);
}


/**
 * Builds a FileSystem object.
 * p: Root path of Carthage's target. Should point to a directory.
 * reset: If a Carthage setup already exists, should it be overriden?
 * n: Name to give to the project. Can be nullptr.
 */
FileSystem::FileSystem(const Path& p, bool reset, const char* n): io_manager(p){
	
	if (reset || !fs::exists(this->io_manager.vfs_path)){
		// Creating a basic Carthage hierarchy and a minimal VFS file containing the root segment.
		if (this->new_vfs(n)){
			// IMPROVE: [FileSystem] Should throw a more suitable type of exception.
			throw std::invalid_argument("FileSystem::FileSystem: Failed to create a new VFS.");
		}
	}
	
	// Opening the first segment contained in the VFS file.
	if (this->open_root()){
		throw std::invalid_argument("FileSystem::FileSystem: Failed to open the VFS.");
	}
}


void FileSystem::make_hierarchy(const Path& out){
	std::ofstream str(out, std::ios::out | std::ios::binary);

	BasicBuffer<4096> buffer(0);
	BasicBuffer<4096> buffer2(0);

	buffer.copy_from("<!doctype HTML>\n");
	buffer.copy_from("<html>\n");
	buffer.copy_from("<head>\n");
	buffer.copy_from("    <link rel='stylesheet' type='text/css' href='vfs.css'>\n");
	buffer.copy_from("</head>\n");
	buffer.copy_from("<body>\n");
	buffer.write_to(str);

	bool version_on = false;
	size_t d_version = 0;

	this->io_manager.ask_reader().iterative_dfs_hierarchy(FIRST_BLOCK_POSITION, [&](const FSBlock& block, const size_t& depth){
		BasicBuffer<2048> buffer(0);

		if (depth == d_version){
			version_on = false;
		}

		// Opening the first tag containing the whole line.
		buffer.copy_from("<p class='line");

		if (root_raised(block.flag)){
			buffer.copy_from(" ROOT");
		}
		if (removed_raised(block.flag)){
			buffer.copy_from(" REMOVED");
		}
		if (file_raised(block.flag)){
			buffer.copy_from(" FILE");
		}
		if (folder_raised(block.flag)){
			buffer.copy_from(" FOLDER");
		}
		if (versionable_raised(block.flag)){
			buffer.copy_from(" VERSIONABLE");
		}
		if (current_raised(block.flag)){
			buffer.copy_from(" CURRENT");
		}
		if (version_raised(block.flag)){
			d_version = depth;
			version_on = true;
			buffer.copy_from(" VERSION");
		}
		
		buffer.copy_from("'>");

		// Characters used to represent the hierarchy in theur own span
		buffer.copy_from("<span class='shift'>");
		if (depth >= 1){
			for (size_t i = 0 ; i < depth - 1 ; i++){
				buffer.copy_from("&#x2502;&nbsp;&nbsp;&nbsp;");
			}
		}
		if (depth > 0){
			buffer.copy_from("&#x251C;&#x2500;&#x2500; ");
		}
		buffer.copy_from("</span>");

		// Data representing the block
		buffer.copy_from("<span class='data "); 
		if (version_on){
			buffer.copy_from(" VER_MEMBER");
		}
		buffer.copy_from("'");

		buffer.copy_from(" title='");
		buffer.add<ID>(block.id);
		buffer.copy_from("'>");

		buffer.copy_from(block.name.c_str());

		if (file_raised(block.flag)){
			buffer.copy_from(" (").copy_from(block.extension.c_str()).copy_from(")");
		}
		if (folder_raised(block.flag) || versionable_raised(block.flag)){
			// Insert the formated number of elements.
		}
		buffer.copy_from("</span>");

		// Flag representing the type of the item.
		buffer.copy_from("<span class='hint'>");
		buffer.copy_from("  [");

		if (root_raised(block.flag)){
			buffer.copy_from("R");
		}
		else{
			buffer.copy_from(".");
		}
		if (removed_raised(block.flag)){
			buffer.copy_from("X");
		}
		else{
			buffer.copy_from(".");
		}
		if (file_raised(block.flag)){
			buffer.copy_from("f");
		}
		else{
			buffer.copy_from(".");
		}
		if (folder_raised(block.flag)){
			buffer.copy_from("F");
		}
		else{
			buffer.copy_from(".");
		}
		if (versionable_raised(block.flag)){
			buffer.copy_from("V");
		}
		else{
			buffer.copy_from(".");
		}
		if (current_raised(block.flag)){
			buffer.copy_from("C");
		}
		else{
			buffer.copy_from(".");
		}
		if (version_raised(block.flag)){
			buffer.copy_from("v");
		}
		else{
			buffer.copy_from(".");
		}

		buffer.copy_from("]");
		buffer.copy_from("</span>");

		buffer.copy_from("</span>");

		buffer.copy_from("</p>");

		buffer.copy_from("\n");

		str.write(buffer.c_str(), strlen(buffer.c_str()));
	});

	buffer2.copy_from("</body>\n");
	buffer2.copy_from("</html>\n");
	buffer2.write_to(str);

	str.close();
}
