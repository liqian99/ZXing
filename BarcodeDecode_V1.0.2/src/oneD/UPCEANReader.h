#ifndef __UPCEANREADER_H__
#define __UPCEANREADER_H__
#include "../BitArray.h"
#include "OneDReader.h"


#define LEN(v) ((int)(sizeof(v)/sizeof(v[0])))
#define LEN0(v) ((int)(sizeof(v[0])))

/**
* Start/end guard pattern.
*/
static const int START_END_PATTERN_[] = {1, 1, 1};
#define START_END_PATTERN_LEN (LEN(START_END_PATTERN_))

/**
* Pattern marking the middle of a UPC/EAN pattern, separating the two halves.
*/
static const int MIDDLE_PATTERN_[] = {1, 1, 1, 1, 1};
#define MIDDLE_PATTERN_LEN (LEN(MIDDLE_PATTERN_))

/**
* "Odd", or "L" patterns used to encode UPC/EAN digits.
*/
static const int L_PATTERNS_[][4] = {
	{3, 2, 1, 1}, // 0
	{2, 2, 2, 1}, // 1
	{2, 1, 2, 2}, // 2
	{1, 4, 1, 1}, // 3
	{1, 1, 3, 2}, // 4
	{1, 2, 3, 1}, // 5
	{1, 1, 1, 4}, // 6
	{1, 3, 1, 2}, // 7
	{1, 2, 1, 3}, // 8
	{3, 1, 1, 2}  // 9
};
#define L_PATTERNS_LEN  (LEN(L_PATTERNS_))
#define L_PATTERNS_LEN0  (LEN0(L_PATTERNS_)/sizeof(int))

/**
* As above but also including the "even", or "G" patterns used to encode UPC/EAN digits.
*/
static const int L_AND_G_PATTERNS_[][4] = {
	{3, 2, 1, 1}, // 0
	{2, 2, 2, 1}, // 1
	{2, 1, 2, 2}, // 2
	{1, 4, 1, 1}, // 3
	{1, 1, 3, 2}, // 4
	{1, 2, 3, 1}, // 5
	{1, 1, 1, 4}, // 6
	{1, 3, 1, 2}, // 7
	{1, 2, 1, 3}, // 8
	{3, 1, 1, 2}, // 9
	{1, 1, 2, 3}, // 10 reversed 0
	{1, 2, 2, 2}, // 11 reversed 1
	{2, 2, 1, 2}, // 12 reversed 2
	{1, 1, 4, 1}, // 13 reversed 3
	{2, 3, 1, 1}, // 14 reversed 4
	{1, 3, 2, 1}, // 15 reversed 5
	{4, 1, 1, 1}, // 16 reversed 6
	{2, 1, 3, 1}, // 17 reversed 7
	{3, 1, 2, 1}, // 18 reversed 8
	{2, 1, 1, 3}  // 19 reversed 9
};
#define L_AND_G_PATTERNS_LEN (LEN(L_AND_G_PATTERNS_))
#define MAX_AVG_VARIANCE ((int)(PATTERN_MATCH_RESULT_SCALE_FACTOR * 0.48f))
#define MAX_INDIVIDUAL_VARIANCE ((int)(PATTERN_MATCH_RESULT_SCALE_FACTOR * 0.7f))
#define VECTOR_INIT(v) v, v + sizeof(v)/sizeof(v[0])

// int START_END_PATTERN[] = {1, 1, 1};//(VECTOR_INIT(START_END_PATTERN_));
// int MIDDLE_PATTERN[] = {1, 1, 1, 1, 1};//(VECTOR_INIT(MIDDLE_PATTERN_));
// const vector<int const*>
// UPCEANReader::L_PATTERNS (VECTOR_INIT(L_PATTERNS_));
// const vector<int const*>
// UPCEANReader::L_AND_G_PATTERNS (VECTOR_INIT(L_AND_G_PATTERNS_));



int UPCEANReaderFindGuardPatternB(BitArray *row,
								  int rowOffset,
								  BOOL whiteFirst,
								  int *pattern,int patternSize,
								  int *counters,int countersSize,
								  OneDRange *pRange);

int UPCEANReaderFindGuardPattern(BitArray *row,
								 int rowOffset,
								 BOOL whiteFirst,
								 int *pattern,int patternSize,OneDRange *pRange);

int UPCEANReaderDecodeDigit(BitArray *row,
                            int *counters,int countersSize,
                            int rowOffset,
                            int **patterns,int patternsSize);

BOOL UPCEANReaderCheckChecksum(char *s_,int *sSize_);


int EAN13ReaderDecodeMiddle(BitArray *row,
                            OneDRange *startRange,
                            char *pResult,int *pResultLen);


int EAN8ReaderDecodeMiddle(BitArray *row,
						   OneDRange *startRange,
						   char *result,int *resultLen);

int UPCEReaderDecodeEnd(BitArray *row, int endStart,OneDRange *pRange); 

int UPCEReaderDecodeMiddle(BitArray *row, OneDRange *startRange, char *result,int *resultLen);

BOOL UPCEReaderCheckChecksum(char *s,int *sLen);

int UPCEANReaderDecodeRow(int rowNumber, BitArray *row,char *pResult,int *pResultLen,int *pType);

#endif