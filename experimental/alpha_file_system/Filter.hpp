#ifndef CONTENT_FILTER_HPP_INCLUDED
#define CONTENT_FILTER_HPP_INCLUDED

class Filter{

	Target tgt = Target::UNDEFINED;
	Attribute attr = Attribute::UNDEFINED;
	Type tp = Type::ALLOWED;
	bool recursive = false; // Can only be bound to a Folder

public:

	enum Target{
		UNDEFINED,
		FILE,
		FOLDER
	};

	enum Attribute{
		UNDEFINED,
		NAME,
		EXTENSION,
		SIZE,
	};

	enum Type{
		UNDEFINED,
		EXPECTED, // We must have that, otherwise it's an error
		ALLOWED, // If we find it we take it but it can be missing (default)
		REJECTED // Impossible to instanciate, deleted if already exists
	};
};

#endif //CONTENT_FILTER_HPP_INCLUDED