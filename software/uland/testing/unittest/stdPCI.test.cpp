#include "stdPCI.h"

// tester (stub) class for PCI tester module

#define CARD_COUNT 3
#define BUFFER_SIZE (256*1024)


// yes, it is bad to declare global variables.
// but, to keep the header file unchanged, it is necessary
// plus, it'll simulate having one piece of hardware with common data
static unsigned char cardbuffer[CARD_COUNT][BUFFER_SIZE];
static int card_in_use[CARD_COUNT];
static bool stdpci_init = false;

unsigned int buffer_pos;

//todo: implement this so it supports multiple interfaces opening 
int stdPCI::createHandles(char* dev) {
	buffer_pos = 0;

	if (stdpci_init == false) {
		for(int i=0; i < CARD_COUNT; i++)
			card_in_use[i] = 0;
	}
	
	num_cards = CARD_COUNT;
	card_id = -1;

	return ALTIX_OK;
}

int stdPCI::freeHandles() {
	return 0;
}

int stdPCI::readData(char* buffer, int size, int id) {
	if ((buffer_pos + size) > BUFFER_SIZE)
		size = BUFFER_SIZE - buffer_pos;

	for(int i=0; i < size; i++)
		buffer[i] = cardbuffer[card_id][buffer_pos + i];

	return size;
}

int stdPCI::sendData(const char* buffer, int size, int id) {
	if ((buffer_pos + size) > BUFFER_SIZE)
		size = BUFFER_SIZE - buffer_pos;

	for(int i=0; i < size; i++)
		cardbuffer[card_id][buffer_pos + i] = buffer[i] ;

	return size;
}

int stdPCI::setPosition(int pos, int id) {
	buffer_pos = pos;
	return 0;
}

int stdPCI::rewind() {
	return setPosition(0);
	
}

int stdPCI::setCard(int id) {
	return lockCard(id);
}

int stdPCI::lockCard(int id) {
	if (card_id != -1)
		return ALTIX_CARD_NOT_VALID;

	card_id = id;
	return ALTIX_OK;
}

int stdPCI::unlockCard(int id) {
	int cid = id;

	if(card_id == cid) {
		card_id = -1;
		return ALTIX_OK;
	}

	return ALTIX_CARD_NOT_VALID;
}

int stdPCI::getCardCount() { 
	return num_cards; 
}

int stdPCI::getMemoryLength(int id) {
	return BUFFER_SIZE;
}

vector<int> stdPCI::getIDList() {
	vector<int> result;

	for(unsigned int i=0; i < num_cards; i++)
		result.push_back(i);
		
	return result;
}

