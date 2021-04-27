#ifndef PARSE_H
	#define PARSE_H
	#define NUMARGS 50
	#define ARGLENGTH 100
	void initParseArguments();
	void bufferToArgs(char buffer[]);
	unsigned int numberArguments(char *buffer);
	unsigned int longestBufferLength(char *buffer);
	void inputToBuffer(char *buffer, unsigned int length);
#endif
