#include "SystemName.hpp"

/*SystemName::SystemName(const ID& id, const Extension& ext){
	this->nullify();
	memcpy(this->buffer, id.buffer, SIZE_OF_ID);

	if (ext.length()){
		this->buffer[SIZE_OF_ID - 1] = '.';
		memcpy(this->buffer + SIZE_OF_ID, ext.buffer, ext.length());
	}
}*/

SystemName::SystemName(const ID& id){
	this->nullify();
	memcpy(this->buffer, id.buffer, SIZE_OF_ID);
}