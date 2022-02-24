#include "SystemName.hpp"

SystemName::SystemName(const ID& id, const Extension& ext){
	this->nullify();
	size_t length_id = id.length();
	memcpy(this->buffer, id.buffer, length_id);
	if (ext.length()){
		this->buffer[length_id] = '.';
		memcpy(this->buffer + length_id + 1, ext.buffer, ext.length());
	}
}

SystemName::SystemName(const ID& id){
	this->nullify();
	size_t length_id = id.length();
	memcpy(this->buffer, id.buffer, length_id);
}