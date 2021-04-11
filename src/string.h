#ifndef __STRING_H__
	#define __STRING_H__
	char *substring(char *dest, char *source, int start, int end);
	int charArrCpy(char a[], char b[], int length);
	int charArrCpyIndex(char a[], char b[], unsigned int startIndex, unsigned int endIndex);
	int charArrCpyIndexOpp(char a[], char b[], unsigned int startIndex, unsigned int endIndex);
	int strlenSpace(const char *str);
	void charArrToUpper(char a[], int length);
#endif
