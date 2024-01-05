#include "defs.h"

/************************************************************************************************
 * Function: const char* evidenceTypeToString(EvidenceClassType evidence)
 * Description: This function converts an EvidenceClassType enum value to a corresponding string.
 *              It uses a static array of strings and checks the validity of the input before
 *              returning the corresponding string.
 * Parameters:
 *      - EvidenceClassType evidence: The evidence type to be converted to a string.
 * Return: const char* - Pointer to the string representation of the evidence type.
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ************************************************************************************************/
const char* evidenceTypeToString(EvidenceClassType evidence) {
    const char* evidenceStrings[] = {"EMF", "TEMPERATURE", "FINGERPRINTS", "SOUND", "INVALID"};
    
    if (evidence >= 0 && evidence < sizeof(evidenceStrings) / sizeof(evidenceStrings[0])) {
        return evidenceStrings[evidence];
    }

    return evidenceStrings[sizeof(evidenceStrings) / sizeof(evidenceStrings[0]) - 1];  // Default to "INVALID"
}

/************************************************************************************************
 * Function: void evidenceToString(EvidenceClassType type, char* str)
 * Description: This function converts an EvidenceClassType enum value to a corresponding string
 *              and copies the result to the provided character array. It uses a static array
 *              of strings and checks the validity of the input before copying.
 * Parameters:
 *      - EvidenceClassType type: The evidence type to be converted to a string.
 *      - char* str: The character array where the resulting string will be copied.
 * Return: None
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ************************************************************************************************/
void evidenceToString(EvidenceClassType type, char* str) {
    const char* evidenceStrings[] = {"EMF", "TEMPERATURE", "FINGERPRINTS", "SOUND"};

    if (type >= 0 && type < sizeof(evidenceStrings) / sizeof(evidenceStrings[0])) {
        strcpy(str, evidenceStrings[type]);
    } else {
        strcpy(str, "UNKNOWN");
    }
}


/************************************************************************************************
 * Function: const char* ghostTypeToString(GhostClassType ghost)
 * Description: This function converts a GhostClassType enum value to a corresponding string
 *              and returns the result. It uses a static array of strings and checks the
 *              validity of the input before returning the string.
 * Parameters:
 *      - GhostClassType ghost: The ghost type to be converted to a string.
 * Return: const char*: The string representation of the ghost type.
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ************************************************************************************************/
const char* ghostTypeToString(GhostClassType ghost) {
    const char* ghostStrings[] = {"POLTERGEIST", "BANSHEE", "BULLIES", "PHANTOM"};

    if (ghost >= 0 && ghost < sizeof(ghostStrings) / sizeof(ghostStrings[0])) {
        return ghostStrings[ghost];
    } else {
        return "UNKNOWN";
    }
}


/************************************************************************************************
 * Function: int randInt(int min, int max)
 * Description: This function generates a random integer in the range [min, max).
 * Parameters:
 *      - int min: The lower bound of the range (inclusive).
 *      - int max: The upper bound of the range (exclusive).
 * Return: int: The generated random integer.
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ************************************************************************************************/
int randInt(int min, int max) {
    return rand() % (max - min) + min;
}


/************************************************************************************************
 * Function: float randFloat(float a, float b)
 * Description: This function generates a random floating-point number in the range [a, b).
 * Parameters:
 *      - float a: The lower bound of the range (inclusive).
 *      - float b: The upper bound of the range (exclusive).
 * Return: float: The generated random floating-point number.
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ************************************************************************************************/
float randFloat(float a, float b) {
    return ((float)rand()) / (float)RAND_MAX * (b - a) + a;
}


