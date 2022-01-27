#ifndef ACTION_BASE_HPP_INCLUDED
#define ACTION_BASE_HPP_INCLUDED

#include <functional>
#include <string>

enum ActionReturn{
	POLL_FAILED = -1,
	SUCCESS = 0
};

class Action{

protected:

	bool last_poll = false;
	int last_execution = 0;
	std::string log{};

public:

	virtual bool poll() = 0;
	virtual int execute() = 0;

};

class FileAction : public Action{

};

class FolderAction : public Action{

};

class VersionableAction : public FolderAction{

};


#endif //ACTION_BASE_HPP_INCLUDED