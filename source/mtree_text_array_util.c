/*
 * contrib/mtree_gist/mtree_text_array_util.c
 */

#include "mtree_text_array_util.h"
#include "mtree_util.h"

float mtree_text_array_distance_internal(mtree_text_array* first, mtree_text_array* second, PG_FUNCTION_ARGS) {
	char* distanceFunctionName = mtree_text_array_distance_functions[0];

	if (PG_HAS_OPCLASS_OPTIONS()) {
		MtreeOptionsStruct* options = (MtreeOptionsStruct*)PG_GET_OPCLASS_OPTIONS();

		distanceFunctionName = GET_STRING_RELOPTION(options, distancestrategy);
	}

	if (strcmp(distanceFunctionName, "simple_text_array_distance") == 0) {
		return simple_text_array_distance(first, second);
	}

	return weighted_text_array_distance(first, second);
}

#define MIN_FLOAT(x, y) (((x) < (y)) ? (1.0 * x) : (1.0 * y))

/*
 * This distance function is used for song similarity queries with the
 * Million Songs Dataset.
 */
float weighted_text_array_distance(mtree_text_array* first, mtree_text_array* second) {
	unsigned char lengthOfFirstArray = first->arrayLength;
	unsigned char lengthOfSecondArray = second->arrayLength;
	unsigned char numberOfMatchingTags = 0;
	char* separator = "###";
	char* saveFirst;
	char* saveSecond;
	float sum = 0.0;

	for (unsigned char i = 0; i < lengthOfFirstArray; ++i) {
		char* firstData = calloc(strlen(first->data[i]) + 1, sizeof(char));
		char* firstDataStart = firstData;
		strcpy(firstData, first->data[i]);

		char* firstTagName = strtok_r(firstData, separator, &saveFirst);
		int firstTagRelevance = atoi(strtok_r(NULL, separator, &saveFirst));

		bool isMatchingTag = false;
		int secondTagRelevance;
		for (unsigned char j = 0; j < lengthOfSecondArray; ++j) {
			char* secondData = calloc(strlen(second->data[j]) + 1, sizeof(char));
			char* secondDataStart = secondData;
			strcpy(secondData, second->data[j]);

			char* secondTagName = strtok_r(secondData, separator, &saveSecond);
			secondTagRelevance = atoi(strtok_r(NULL, separator, &saveSecond));

			if (strcmp(firstTagName, secondTagName) == 0) {
				isMatchingTag = true;
				numberOfMatchingTags++;

				free(secondDataStart);
				continue;
			}

			free(secondDataStart);
		}

		if (isMatchingTag) {
			sum += MIN_FLOAT(firstTagRelevance, secondTagRelevance);
		}

		free(firstDataStart);
	}

	sum /= 1.0 * (lengthOfFirstArray + lengthOfSecondArray - numberOfMatchingTags);

	return 100.0 - sum;
}

int simple_text_array_distance(mtree_text_array* first, mtree_text_array* second) {
	int sum = 0;
	unsigned char length = first->arrayLength;

	if (second->arrayLength < length) {
		length = second->arrayLength;
	}

	for (unsigned char i = 0; i < length; ++i) {
		if (string_distance(first->data[i], second->data[i]) == 0) {
			++sum;
		}
	}

	return length - sum;
}

bool mtree_text_array_equals(mtree_text_array* first, mtree_text_array* second) {
	if (first->arrayLength != second->arrayLength) {
		return false;
	}

	for (unsigned char i = 0; i < first->arrayLength; ++i) {
		if (first->data[i] != second->data[i]) {
			return false;
		}
	}

	return true;
}

bool mtree_text_array_overlap_distance(mtree_text_array* first, mtree_text_array* second, float* distance) {
	return *distance <= first->coveringRadius + second->coveringRadius;
}

bool mtree_text_array_contains_distance(mtree_text_array* first, mtree_text_array* second, float* distance) {
	return first->coveringRadius >= *distance + second->coveringRadius;
}

bool mtree_text_array_contained_distance(mtree_text_array* first, mtree_text_array* second, float* distance) {
	return mtree_text_array_contains_distance(second, first, distance);
}

mtree_text_array* mtree_text_array_deep_copy(mtree_text_array* source) {
	mtree_text_array* destination = (mtree_text_array*)palloc(VARSIZE_ANY(source));
	memcpy(destination, source, VARSIZE_ANY(source));
	return destination;
}

int get_text_array_distance(int size, mtree_text_array* entries[size], int distances[size][size], int i, int j, PG_FUNCTION_ARGS) {
	if (distances[i][j] == -1) {
		distances[i][j] = mtree_text_array_distance_internal(entries[i], entries[j], fcinfo);
	}
	return distances[i][j];
}
