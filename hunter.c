#include "defs.h"

/* *******************************************************************************************
 * Function: void initializeHunter(char* name, RoomType *room, int uniqueRandomTool, int restDuration, HunterType **hunter)
 * Description: This function initializes a HunterType structure with the provided parameters. It allocates
 *              memory for the hunter, copies the name, generates random evidence, assigns the room, allocates
 *              and initializes a ghostEvidence list, and initializes fear, boredom timer, and rest duration.
 *              The initialized hunter is assigned to the pointer passed as an argument.
 * Parameters:
 *      - char* name: A string representing the name of the hunter.
 *      - RoomType *room: A pointer to the RoomType representing the initial room of the hunter.
 *      - int uniqueRandomTool: An integer representing a unique random tool for the hunter.
 *      - int restDuration: An integer representing the rest duration of the hunter.
 *      - HunterType **hunter: A pointer to a pointer that will be assigned the initialized hunter.
 * Return: None
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ********************************************************************************************/
void initializeHunter(char* name, RoomType *room, int uniqueRandomTool, int restDuration, HunterType **hunter) {
    HunterType *hunterPointer = (HunterType*) malloc(sizeof(HunterType));

    strcpy(hunterPointer->name, name);

    hunterPointer->evidence = (EvidenceClassType) uniqueRandomTool;

    hunterPointer->room = room;

    
    GhostEvidenceListType *evidenceListPtr = (GhostEvidenceListType*) malloc(sizeof(GhostEvidenceListType));
    initializeEvidence(evidenceListPtr);
    hunterPointer->ghostEvidence = evidenceListPtr;
    
    hunterPointer->fear = 0;
    hunterPointer->timer = BOREDOM_MAX;
    hunterPointer->restDuration = restDuration;

    *hunter = hunterPointer; 
}  

/* *******************************************************************************************
 * Function: void initializeEvidence(GhostEvidenceListType* list)
 * Description: This function initializes a GhostEvidenceListType structure by setting both the head
 *              and tail pointers to NULL.
 * Parameters:
 *      - GhostEvidenceListType* list: A pointer to the GhostEvidenceListType structure to be initialized.
 * Return: None
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ********************************************************************************************/
void initializeEvidence(GhostEvidenceListType* list) {
    list->head = list->tail = NULL;
}


/* *******************************************************************************************
 * Function: void initListOfHunters(HunterListType *list)
 * Description: This function initializes a HunterListType structure by setting the size of the hunter
 *              list to 0.
 * Parameters:
 *      - HunterListType *list: A pointer to the HunterListType structure to be initialized.
 * Return: None
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ********************************************************************************************/
void initListOfHunters(HunterListType *list) {
    list->size = 0;
}

/* *******************************************************************************************
 * Function: bool appendHunterToList(HunterListType *hunters, HunterType *hunter)
 * Description: This function appends a hunter to a HunterListType structure if there is space.
 *              It returns true if the hunter is added successfully; otherwise, it returns false.
 * Parameters:
 *      - HunterListType *hunters: A pointer to the HunterListType structure.
 *      - HunterType *hunter: A pointer to the HunterType structure to be appended.
 * Return: true if the hunter is added successfully, false otherwise.
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ********************************************************************************************/

bool appendHunterToList(HunterListType *hunters, HunterType *hunter) {
    bool addedSuccessfully = (hunters->size < MAX_HUNTERS);
    if (addedSuccessfully) {
        hunters->hunterList[hunters->size++] = hunter;
    }
    return addedSuccessfully;
}


/* *******************************************************************************************
 * Function: void *hunterThread(void *arg)
 * Description: This function represents the behavior of a hunter in a multi-threaded environment.
 *              The hunter thread sleeps for a specified rest duration, then performs a random action
 *              such as searching for evidence, roaming around, or communicating with other hunters.
 *              The thread continues these actions until it either finds evidence, reaches maximum fear,
 *              or the boredom timer reaches zero.
 * Parameters:
 *      - void *arg: A pointer to the HunterType structure representing the hunter.
 * Return: NULL
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ********************************************************************************************/
void *hunterThread(void *arg) {
    srand(time(NULL));

    HunterType *threadHunter = (HunterType*)arg;

    int action;
    do {
        struct timespec sleepTime;
        sleepTime.tv_sec = threadHunter->restDuration / 1000;
        sleepTime.tv_nsec = (threadHunter->restDuration % 1000) * 1000000;

        nanosleep(&sleepTime, NULL);

        action = randInt(0, 3);

        if (action == 0) {
            sem_wait(&(threadHunter->room->semaphore));
            grabEvidence(threadHunter);
            sem_post(&(threadHunter->room->semaphore));
        } else if (action == 1) {
            repositionHunter(threadHunter);
        } else if (action == 2) {
            sem_wait(&(threadHunter->room->semaphore));

            if (threadHunter->room->hunters->size > 1) {
                verifyEvidence(threadHunter);
            }

            sem_post(&(threadHunter->room->semaphore));
        }

        if (didHunterFindGhost(threadHunter)) {
            threadHunter->fear++;
            threadHunter->timer = BOREDOM_MAX;
        }

    } while (!(containsEvidence(threadHunter) || (threadHunter->fear >= 100) || (threadHunter->timer <= 0)));

    rerepositionHunter(threadHunter, true);
    return NULL;
}



/* *******************************************************************************************
 * Function: int didHunterFindGhost(HunterType *currHunter)
 * Description: This function checks if the hunter has found the ghost in the current room.
 *              It returns 1 (C_TRUE) if the hunter's room's ghost is not null, otherwise, it
 *              returns 0 (C_FALSE).
 * Parameters:
 *      - HunterType *currHunter: A pointer to the HunterType representing the current hunter.
 * Return: 1 (C_TRUE) if the hunter found the ghost, 0 (C_FALSE) otherwise.
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ********************************************************************************************/
int didHunterFindGhost(HunterType *currHunter) {
    return (currHunter->room->ghost != NULL) ? 1 : 0;
}


/* *******************************************************************************************
 * Function: int containsEvidence(HunterType *currHunter)
 * Description: This function checks if the hunter's ghost evidence list contains at least 3 ghostly elements.
 *              It iterates over the list, counting the number of ghostly elements, and returns 1 (C_TRUE) if
 *              the count is greater than or equal to 3, otherwise, it returns 0 (C_FALSE).
 * Parameters:
 *      - HunterType *currHunter: A pointer to the HunterType representing the current hunter.
 * Return: 1 (C_TRUE) if the evidence list contains at least 3 ghostly elements, 0 (C_FALSE) otherwise.
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ********************************************************************************************/
int containsEvidence(HunterType *currHunter) {
    int counter = 0;
    EvidenceNodeType *node;

    for (node = currHunter->ghostEvidence->head; node != NULL; node = node->next) {
        if (isEvidenceFromGhost(node->data)) {
            counter++;
        }
    }

    return (counter >= 3) ? 1 : 0;
}




/* *******************************************************************************************
 * Function: float createStandardValue(EvidenceClassType evidenceLevel)
 * Description: This function creates a standard value based on the given evidence level. It generates
 *              a random value within a specified range for each evidence level. If the evidence level is
 *              not within the expected range, it returns C_MISC_ERROR.
 * Parameters:
 *      - EvidenceClassType evidenceLevel: An enumeration representing the level of evidence.
 * Return: A float representing the created standard value or C_MISC_ERROR if an error occurs.
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ********************************************************************************************/
float createStandardValue(EvidenceClassType evidenceLevel) {
    float value;

    if (evidenceLevel == 0) {
        value = randFloat(0, 4.90);
    } else if (evidenceLevel == 1) {
        value = randFloat(0, 27.00);
    } else if (evidenceLevel == 2) {
        value = 0;
    } else if (evidenceLevel == 3) {
        value = randFloat(40.0, 70.0);
    } else {
        value = C_MISC_ERROR;
    }

    return value;
}


/* *******************************************************************************************
 * Function: int removeEvidence(GhostEvidenceListType *list, EvidenceNodeType *evidence)
 * Description: This function removes a specific evidence node from a GhostEvidenceListType structure.
 *              It returns C_TRUE if the evidence is successfully removed, C_FALSE otherwise.
 * Parameters:
 *      - GhostEvidenceListType *list: A pointer to the GhostEvidenceListType structure.
 *      - EvidenceNodeType *evidence: A pointer to the EvidenceNodeType structure representing the evidence to be removed.
 * Return: C_TRUE if the evidence is successfully removed, C_FALSE otherwise.
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ********************************************************************************************/
int removeEvidence(GhostEvidenceListType *list, EvidenceNodeType *evidence) {
    if (evidence == NULL || list == NULL || list->head == NULL) {
        return C_FALSE;
    }

    if (evidence == list->head) {
        list->head = evidence->next;
    } else {
        EvidenceNodeType *evidence = list->head;
        while (evidence->next != NULL && evidence->next != evidence) {
            evidence = evidence->next;
        }

        if (evidence->next != NULL) {
            evidence->next = evidence->next;

            if (evidence == list->tail) {
                list->tail = evidence;
            }
        } else {
            return C_FALSE;
        }
    }

    free(evidence->data);
    free(evidence);

    return C_TRUE;
}







/* *******************************************************************************************
 * Function: int isEvidenceFromGhost(EvidenceType *evidence)
 * Description: This function checks if the given evidence is ghostly based on its category and reading data.
 *              It returns C_TRUE if the evidence is ghostly, C_FALSE otherwise.
 * Parameters:
 *      - EvidenceType *evidence: A pointer to the EvidenceType structure representing the evidence.
 * Return: C_TRUE if the evidence is ghostly, C_FALSE otherwise.
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ********************************************************************************************/
int isEvidenceFromGhost(EvidenceType *evidence) {
    switch (evidence->evidenceType) {
        case 0: return (evidence->readingInfo > 4.90 && evidence->readingInfo <= 5.00);
        case 1: return (evidence->readingInfo >= -10 && evidence->readingInfo < 0);
        case 2: return (evidence->readingInfo == 1.00);
        case 3: return (evidence->readingInfo > 70.00 && evidence->readingInfo <= 75.00);
        default: return C_FALSE; // Considered not ghostly for unknown categories
    }
}

/* *******************************************************************************************
 * Function: GhostEvidenceListType* copyEvidence(GhostEvidenceListType *copyList)
 * Description: This function creates a copy of a GhostEvidenceListType structure by iterating through
 *              the original list, allocating memory for each evidence and node, and copying the data
 *              to the new list.
 * Parameters:
 *      - GhostEvidenceListType *copyList: A pointer to the GhostEvidenceListType structure to be copied.
 * Return: A pointer to the newly created copied list.
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ********************************************************************************************/
GhostEvidenceListType* copyEvidence(GhostEvidenceListType *copyList) {
    GhostEvidenceListType *copyListPointer = (GhostEvidenceListType*)malloc(sizeof(GhostEvidenceListType));
    initListOfGhosts(copyListPointer);

    for (EvidenceNodeType *node = copyList->head; node != NULL; node = node->next) {
        EvidenceNodeType *tempEvNode = (EvidenceNodeType*)malloc(sizeof(EvidenceNodeType));
        EvidenceType *tempEvidence = (EvidenceType*)malloc(sizeof(EvidenceType));

        tempEvidence->readingInfo = node->data->readingInfo;
        tempEvidence->evidenceType = node->data->evidenceType;

        tempEvNode->data = tempEvidence;

        addHunterEvidence(copyListPointer, tempEvNode);
    }

    return copyListPointer;
}


/* *******************************************************************************************
 * Function: int isDuplicate(GhostEvidenceListType *list, EvidenceNodeType *nodeToCheck)
 * Description: This function checks if there is a duplicate evidence node in a GhostEvidenceListType
 *              structure by comparing the evidence type and reading info of the given nodeToCheck with
 *              each node in the list.
 * Parameters:
 *      - GhostEvidenceListType *list: A pointer to the GhostEvidenceListType structure.
 *      - EvidenceNodeType *nodeToCheck: A pointer to the EvidenceNodeType structure to check for duplicates.
 * Return: C_TRUE if a duplicate is found, C_FALSE otherwise.
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ********************************************************************************************/
int isDuplicate(GhostEvidenceListType *list, EvidenceNodeType *nodeToCheck) {
    for (EvidenceNodeType *currentNode = list->head; currentNode != NULL; currentNode = currentNode->next) {
        if (currentNode->data->evidenceType == nodeToCheck->data->evidenceType &&
            currentNode->data->readingInfo == nodeToCheck->data->readingInfo) {
            return C_TRUE;
        }
    }

    return C_FALSE;
}

/* *******************************************************************************************
 * Function: void releaseHunterResources(HunterType *hunter)
 * Description: This function releases resources associated with a HunterType structure by cleaning up
 *              the evidence data and freeing the memory allocated for the hunter.
 * Parameters:
 *      - HunterType *hunter: A pointer to the HunterType structure whose resources need to be released.
 * Return: None
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ********************************************************************************************/
void releaseHunterResources(HunterType *hunter) {
    cleanUpEvidenceData(hunter->ghostEvidence);
    free(hunter);
}

/* *******************************************************************************************
 * Function: void cleanUpEvidenceData(GhostEvidenceListType *list)
 * Description: This function iterates through a GhostEvidenceListType structure and frees the memory
 *              associated with the data of each evidence node.
 * Parameters:
 *      - GhostEvidenceListType *list: A pointer to the GhostEvidenceListType structure whose evidence data
 *                                      needs to be cleaned up.
 * Return: None
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ********************************************************************************************/
void cleanUpEvidenceData(GhostEvidenceListType *list){
    // Use a for loop to iterate through the list of evidence nodes
    for(EvidenceNodeType *node = list->head; node != NULL; node = node->next){
        // Free the data of each node
        free(node->data);
    }
}

/* *******************************************************************************************
 * Function: void releaseEvidenceNodes(GhostEvidenceListType *list)
 * Description: This function releases all evidence nodes in a GhostEvidenceListType structure by
 *              iterating through the list and freeing the memory for each node.
 * Parameters:
 *      - GhostEvidenceListType *list: A pointer to the GhostEvidenceListType structure whose evidence
 *                                      nodes need to be released.
 * Return: None
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ********************************************************************************************/
void releaseEvidenceNodes(GhostEvidenceListType *list) {
    for (EvidenceNodeType *currNode = list->head; currNode != NULL;) {
        EvidenceNodeType *nextNode = currNode->next;
        free(currNode);
        currNode = nextNode;
    }

    // After freeing all nodes, set the list's head to NULL
    list->head = NULL;
}


/* *******************************************************************************************
 * Function: void releaseEvidenceList(GhostEvidenceListType *list)
 * Description: This function releases all resources associated with a GhostEvidenceListType structure,
 *              including cleaning up evidence data, releasing evidence nodes, and freeing the memory
 *              allocated for the list.
 * Parameters:
 *      - GhostEvidenceListType *list: A pointer to the GhostEvidenceListType structure whose resources
 *                                      need to be released.
 * Return: None
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ********************************************************************************************/
void releaseEvidenceList(GhostEvidenceListType *list){
    cleanUpEvidenceData(list);
    releaseEvidenceNodes(list);
    free(list);
}
