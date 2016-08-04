#include "fastqreader.h"
#include "util.h"
#include <string.h>

FastqReader::FastqReader(string filename, bool hasQuality){
	mFilename = filename;
	mZipFile = NULL;
	mZipped = false;
	mHasQuality = hasQuality;
	init();
}

FastqReader::~FastqReader(){
	close();
}

void FastqReader::init(){
	if (isZipFastq(mFilename)){
		mZipFile = gzopen(mFilename.c_str(), "r");
		mZipped = true;
		Read* r = read();

		//test if it has quality line or not (fastq/fasta)
		if (r->mQuality[0] == '@')
			mHasQuality = false;
		delete r;
		gzrewind(mZipFile);
	}
	else if (isFastq(mFilename)){
		mFile.open(mFilename.c_str(), ifstream::in);
		mZipped = false;
	}
}

bool FastqReader::getLine(char* line, int maxLine){
	bool status = true;
	if(mZipped)
		status = gzgets(mZipFile, line, maxLine);
	else
		status = mFile.getline(line, maxLine);

	// trim \n, \r or \r\n in the tail
	int readed = strlen(line);
	if(readed >=2 ){
		if(line[readed-1] == '\n' || line[readed-1] == '\r'){
			line[readed-1] = '\0';
			if(line[readed-2] == '\r')
				line[readed-2] = '\0';
		}
	}

	return status;
}

Read* FastqReader::read(){
	const int maxLine = 1000;
	char line[maxLine];
	if (mZipped){
		if (mZipFile == NULL)
			return NULL;
	}

	if(!getLine(line, maxLine))return NULL;
	string name(line);

	if (!getLine(line, maxLine))return NULL;
	string sequence(line);

	if (!getLine(line, maxLine))return NULL;
	string strand(line);

	if (mHasQuality){
		if (!getLine(line, maxLine))return NULL;
		string quality(line);
		Read* read = new Read(name, sequence, strand, quality);
		return read;
	}
	else {
		Read* read = new Read(name, sequence, strand);
		return read;
	}

	return NULL;
}

void FastqReader::close(){
	if (mZipped){
		if (mZipFile){
			gzclose(mZipFile);
			mZipFile = NULL;
		}
	}
	else {
		if (mFile.is_open()){
			mFile.close();
		}
	}
}

bool FastqReader::isZipFastq(string filename) {
	if (ends_with(filename, ".fastq.gz"))
		return true;
	else if (ends_with(filename, ".fq.gz"))
		return true;
	else if (ends_with(filename, ".fasta.gz"))
		return true;
	else if (ends_with(filename, ".fa.gz"))
		return true;
	else
		return false;
}

bool FastqReader::isFastq(string filename) {
	if (ends_with(filename, ".fastq"))
		return true;
	else if (ends_with(filename, ".fq"))
		return true;
	else if (ends_with(filename, ".fasta"))
		return true;
	else if (ends_with(filename, ".fa"))
		return true;
	else
		return false;
}

bool FastqReader::isZipped(){
	return mZipped;
}

bool FastqReader::test(){
	FastqReader reader1("testdata/R1.fq");
	FastqReader reader2("testdata/R1.fq.gz");
	Read* r1 = NULL;
	Read* r2 = NULL;
	while(true){
		r1=reader1.read();
		r2=reader2.read();
		if(r1 == NULL || r2 == NULL)
			break;
		if(r1->mSeq.mStr != r2->mSeq.mStr){
			return false;
		}
		delete r1;
		delete r2;
	}
	return true;
}
