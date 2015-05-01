#include "helper.h"

void strcpy_b (char * destination, const char * source, uint8_t byte){
	for(int i = 0; i < byte; i++) {
		destination[i] = source[i];
	}
}

bool strmatch (const char * str1, const char * str2){
	while(!(*str1 == '\0' && *str2 == '\0')) {
		if(*str1 != *str2)
			return FALSE;
		str1++;
		str2++;
	}
	return TRUE;

}
