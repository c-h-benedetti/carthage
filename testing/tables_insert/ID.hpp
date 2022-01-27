#ifndef BASIC_ID_HPP_INCLUDED
#define BASIC_ID_HPP_INCLUDED

#include <cstdint>

// 32
#define SIZE_ID 0b100000

#define ascii_0 48
#define ascii_9 57
#define ascii_A 65
#define ascii_Z 90
#define ascii_a 97
#define ascii_z 122

void build_name(char* buffer);

class ID{

	char id[SIZE_ID] = {};

	ID(int i);

public:

	ID();
	ID(const char* c);
	ID(const ID& i);

	ID& operator=(const ID& i);
	ID& operator=(const char* i);

	void tell() const;
	void get(char* c) const;

	friend int idcmp(const ID& id1, const ID& id2);

	static ID null_id();
};


int idcmp(const ID& id1, const ID& id2);

bool operator<(const ID& id1, const ID& id2);
bool operator>(const ID& id1, const ID& id2);
bool operator==(const ID& id1, const ID& id2);
bool operator<=(const ID& id1, const ID& id2);
bool operator>=(const ID& id1, const ID& id2);


#endif // BASIC_ID_HPP_INCLUDED