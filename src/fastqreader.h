#ifndef FASTQ_READER_H
#define FASTQ_READER_H

#include <stdio.h>
#include <stdlib.h>
#include "read.h"
#include "zlib/zlib.h"
#include "common.h"
#include <iostream>
#include <fstream>

class FastqReader{
public:
	FastqReader(string filename, bool hasQuality = true);
	~FastqReader();
	bool isZipped();

	//this function is not thread-safe
	//do not call read() of a same FastqReader object from different threads concurrently
	Read* read();
	void close();

public:
	static bool isZipFastq(string filename);
	static bool isFastq(string filename);
	static bool test();

private:
	void init();
	bool getLine(char* line, int maxLine);

private:
	string mFilename;
	gzFile mZipFile;
	ifstream mFile;
	bool mZipped;
	bool mHasQuality;

};

#endif