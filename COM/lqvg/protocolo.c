#include "protocolo.h"

int enviarValueSize(int size,int sock){
	send(sock,size,sizeof(size),NULL);
}
