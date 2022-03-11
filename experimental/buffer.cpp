
// To copy a Folder we need to create a new Folder, to copy a Versionable, we have to create a new Versionable. It leaves us two possibilities:
//   1. We make a method copy_to() using the dynamic type of the summoner (this) to create a new object.
//   2. We inspect the FSBlock of the source to know which kind of object must be created.
// We will go with the first solution for now. The second solution would be suitable for a global function copy_in().

// Difficulty: The source can be the current object or not, and so can the destination.
// We can hardly retreive a path if we don't know what is loaded or not.

// source:      summoner (object on the one the method is called)
// destination: 
// src path:
// dst path:

// A Versionable or a Folder in a Versionable cannot accept to contain a Versionable.
// The method to know if an object can contain a Versionable is only available for the current_obj.
// => The destination has to be the current_object.
// Then, the method could be member of FileSystem rather than Container.
// Only the recursive method should be left in Container.

// Comment connaitre le type de la source maintenant? Il faut re-migrer la fonction vers la source!
// Ajouter une variable aux FSObject qui permet de savoir s'ils sont l'objet actif d'un FileSystem.
//  -> Ne pas oublier de neutraliser cette variable dans le constructeur par copie (s'il existe encore).

// Ajouter dans les FSObject des méthodes:
//  - Container& Container::at() const = 0
//  - Folder& Folder::at() const -> *this
//  - Folder& Versionable::at() const -> *(this->current->version)
// Ces fonctions pourraient être summon par operator().
// Ca résoud le problème de la transmission d'infos à la current_version.
// Il faudrait peut-être une classe intermédiaire pour l'assurer que l'user passe bien par cette méthode pour certaines méthodes?

// La construction du path doit absolument être déléguée aux objets eux-mêmes.
// Le current_syspath fonction de façon trop imprévisible.
// Comment gérer la construction du chemin avec le at() ?

virtual void Container::copy_to(Container* destination) = 0;

void Folder::copy_to(Container* destination) override{
	if (destination && destination->is_active() && destination->accept_folders()){
		Path dest_path = destination->deduce_path();
		Path src_path = this->fs.current_syspath();
	}
}


// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

void FileSystem::copy_to_current(const FSObject* source){ // (1)
	if (source){

		if ()
		Path dest_path = this->current_path / this->current_obj->get_system_name(); // (2)
		Path source_path = source->deduce_path(); // (3)

		source->copy_to();
	}
}


//  (1)
// We need a pointer to a FSObject to be able to copy a File like a Container, which are two different inheritance branches of FSObject.
// The source can be constant.

//  (2)
// In the case of a Versionable:
//    - The attribute FileSystem::current_path contains the parent of the Versionable.
//    - The method FileSystem::current_syspath() returns the address of the current_version held by the Versionable.
// So, in order to have the path of the "raw" object, we build it from: FileSystem::current_path / FileSystem::current_obj->get_system_name();
//                                                                                 ^                          ^                    ^
//                                                                              parent                   "raw" object         its actual name

//  (3)
// The call to deduce_path() implies to pass a valid source, holding a valid reference to a FileSystem.

//  (*)
// The raw Versionable object must not accept Files or anything that doesn't come from itself.
// Maybe that we should stop doing the abstraction between the Versionable and its current version in some contexts?
//  Ex: Methods inherited from Container can be transmitted to the current_version, and some new methods specific to the raw object
//      can be added.

// !! Write an applications that enables to document the code outside of the code.
// -> The final result would be a fusion between this extra doc, and the one included in the code.
// -> Could be an extension of Doxygen. See Notebooks: A notebook attached to actual code, that updates itself when code is updated.

