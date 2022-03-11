<h1 style="text-align: center;">CARTHAGE</h1>



						0. WHAT DO WE NEED?
**1.  A way to manipulate the current_version of a Versionable through the Versionable, that doesn't block operations on the Versionable itself.**

> <u>Implement from the Container the methods:</u>
>
>  - `virtual Container& at() = 0;`
>  - `Folder& Folder::at() override { return *this; }`
> - `Folder& Versionable::at() override { return *(this->current_version); }`
>
> Like this, we can manipulate the "raw" object by calling methods on itself, and manipulate the "exposed" object by calling `at()`.

**2.  A safe way to copy FSObjects into a given Container.**

> <u>Why can't this method be member of `FileSystem`?</u>
>
> - Because if the source is a `Versionable`, we need to be able to call `accept_versionables()` on the destination, and if the source is a `Folder`, we need to be able to call `accept_folders()`. We could achieve that by inspecting the `FSBlock`, but it is cleaner if we can simply use polymorphism.

> <u>What is the problem with the paths?</u>
>
> - The path can be acquired either by requesting it to the `FileSystem` if the object is the `current_object`, or by using `deduce_path()`.
> - We have no way to know if the source or the destination is the `current_object`.
>   *Solution:* Don't watch if an object is the `current_object` and simply use `deduce_path()` on both objects. Copying is not supposed to be a fast operation.

**3.   A cleaner way to build the current_object's path.**

> - For a `File`, the full path is: [parent's path] / [system name] (the system name includes the extension)
> - For a `Container`, there is no such thing as a full path
> - For a `Folder`, the full path is: [parent's path] / [system name] (the extension is supposed to be empty)
> - For a `Versionable`, the `at()` method should enable to build the path of the current version, but it looks hard to do.

> - The solution could be to pass as system name of the `current_version`, the `system_name` of the `Versionable` concatenated with the one of the `Folder`.
> - Add to `FSObject` a method: `void set_root(const fs::path& r);` that modifies the system name ?
>   The root must be stored apart to restore it if the system name is refreshed.

**4.   Determine if we should keep the FSBlock into the FSObjects, or simply destroy it and generate it again when we need it.**

> Destroying it would avoid internal state problems. Keeping it just implies to use it as a struct of encapsulated attributes.

**5.   A cleaner way to interact with the VFS.**

> Started implementation in "carthage/experimental/read_file"

**6.   Think to a clean way to implement the NAS install and the NAS lock.**

> Implies to know what happens if two processes try to open a file and edit it at the same time ON ALL SYSTEMS.
> Also requires to know how opening a file on a NAS works. Is the file a local copy or really opened on the NAS?




						I. IMPLEMENTATION PRIORITIES
- [ ] Implement a way to copy a `FSObject` into another.
- [ ] Check that the new path system works properly.
- [ ] Implement the versioning.
- [ ] Create a batch of unit tests.
- [ ] Implement functions to interact with the VFS block-wise.
- [ ] Implement the `previous()` and `next()` methods.
- [ ] Transform `FileSystem` into a state machine.
- [ ] Handle endianness of `seg_size` & `next`.
- [ ] Implement `after_reading()` & `before_writing()`.
- [ ] Add a method for segments gathering.
- [ ] Methods interfacing the objects.
- [ ] Add the Table alongside the VFS, and a `global_open()` method.
- [x] Make the filter checking for a versionable object on the stack. Use it in `accept_versionable()`.

- [x] Replace `FSType` equality tests by functions testing if the flag is raised in an instance.
- [x] Turn the `FileSystem` pointer into a reference.




						II. PROTOTYPAGE DE LA MÉTHODE DE COPY DE FSOBJECT
On doit explicitement call new_versionable() ou new_folder() quand on veut copier pour créer la copie.
Il est donc plus facile d'utiliser le type dynamique des objets pour faire les copies
	> On peut facilement créer un Versionable depuis un Versionable, ou un Folder depuis un Folder.
La source est donc le summoner (this) --> La méthode sera donc copy_to() au lieu de copy_from()

Depuis la source donc, on doit pouvoir recover:
	- Le chemin de la source
	- Le chemin de la destination
	- Le FSObject dynamique qui représente la destination

La destination doit être passée par pointeur pour qu'on puisse déterminer son type dynamique.
On en a besoin pour pouvoir appeler les méthodes accepts_xxx() et new_xxx().




						III. MODIFICATIONS TO BRING TO THE FILESYSTEM
- We want to be able to call `size()` on a `FileSystem`.
- We want to call `new_file()`, `new_folder()` et `new_versionable()` on a `FileSystem`.
- We need a method `is_versionable()`, and then a method to cast the current object to a `Versionable*`.
- We can remove the `Folder*` cast.




						IV. DEFRAGMENTATION / GATHERING
- [ ] If a block is identified as empty after we inspected its removed content, its "Content" pointer must be passed to 0.




						V. INTERESTING DESIGN PATTERNS
  - Abstract Factory *(experimental solution for type deduction of current object)*
  - Adapter *(second experimental solution to handle specialization of current object)*
  - Command *(to implement Actions)*
  - Composite *(to process batches of folder as a single folder)*
  - Decorator *(adding the versionable behavior as a decorator of Folder rather than a specialization)*
  - Flyweight *(to handle the content of objects)*
  - Mediator *(interaction objects-actions / management of versionable behavior)*
  - Memento *(save)*
  - Visitor *(for display)*