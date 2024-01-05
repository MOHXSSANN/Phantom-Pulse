// Include defs.h
#include "defs.h"




/************************************************
 * Function: void initListOfGhosts(GhostEvidenceListType *ghostEvidenceList)
 * Description: Initializes a list of ghost evidence by setting both the head and tail pointers to NULL, indicating an empty list.
 * Parameters:
 *   - GhostEvidenceListType *ghostEvidenceList: Pointer to the structure representing the list of ghost evidence.
 * Returns: void
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ***********************************************/
void initListOfGhosts(GhostEvidenceListType *ghostEvidenceList) {
    ghostEvidenceList->head = ghostEvidenceList->tail = NULL;
}



/********************************************************************************
 * Function: RoomNodeType* getRandomRoom(RoomNodeType *head)
 * Description: Retrieves a randomly selected room node from the given list of rooms.
 * Parameters:
 *   - RoomNodeType *head: Pointer to the head of the list of rooms.
 * Returns: RoomNodeType* - Pointer to the randomly selected room node.
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 **********************************************************************/
RoomNodeType* getRandomRoom(RoomNodeType *head) {
    int roomInt = randInt(1, ALL_ROOMS);
    RoomNodeType *traverseRoomNode = head;

    int i = 1;
    while (i < roomInt && traverseRoomNode != NULL) {
        traverseRoomNode = traverseRoomNode->next;
        i++;
    }

    return traverseRoomNode;
}


/* *******************************************************************************************
 * Function: void *ghostThread(void *arg)
 * Description: This function represents the behavior of a ghost in a multi-threaded environment. The ghost
 *              thread sleeps for a certain duration specified by restDuration in the GhostType structure. 
 *              After waking up, it checks if the ghost is present at the current location. If so, it randomly
 *              decides to either perform a random move or create new random evidence. If the ghost is not
 *              present, it randomly decides to move or create new random evidence, while decrementing the
 *              boredomDuration in the GhostType structure.
 * Parameters:
 *      - void *arg: A pointer to a GhostType structure, representing the ghost's characteristics.
 * Return: NULL
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ********************************************************************************************/
void *ghostThread(void *arg) {
    srand(time(NULL));

    GhostType *ghostPointer = (GhostType*) arg;

    do {
        sleep(ghostPointer->restDuration);

        if (isGhostHere(ghostPointer)) {
            int pickMove = randInt(0, 2);

            ghostPointer->boredomDuration = BOREDOM_MAX;

            if (pickMove) {
                newRandomEvidence(ghostPointer);
            }

        } else {
            int pickMoveI = randInt(0, 3);

            ghostPointer->boredomDuration--;

            if (pickMoveI == 0) {
                moveGhost(ghostPointer);
            } else if (pickMoveI == 1) {

                newRandomEvidence(ghostPointer);
            }
        }
    } while (ghostPointer->boredomDuration > 0);

    return NULL;
}

/* *******************************************************************************************
 * Function: int randomGhostEvidence(GhostClassType ghostType)
 * Description: This function generates a random ghost evidence based on the specified ghost type.
 *              The function uses an array, ghostPointerArray, to map ghost types to possible evidence
 *              values. Depending on the ghost type, certain indices in the array are set to specific values.
 *              The function then returns a random value from the array using the randInt function.
 * Parameters:
 *      - GhostClassType ghostType: An enumeration representing the type of ghost.
 * Return: An integer representing the randomly generated ghost evidence.
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ********************************************************************************************/
int randomGhostEvidence(GhostClassType ghostType) {
    int ghostPointerArray[3] = {0};

    if (ghostType == BANSHEE) {
        ghostPointerArray[2] = 3;
    } else if (ghostType == POLTERGEIST) {
        ghostPointerArray[1] = 1;
        ghostPointerArray[2] = 2;
    } else if (ghostType == BULLIES) {
        ghostPointerArray[1] = 2;
    } else if (ghostType == PHANTOM) {
        ghostPointerArray[0] = 1;
    }

    return ghostPointerArray[randInt(0, 3)];
}




/* *******************************************************************************************
 * Function: void addRoomEvidence(GhostEvidenceListType *list, EvidenceNodeType* evidenceNode)
 * Description: This function adds a new piece of evidence represented by the given evidenceNode
 *              to the GhostEvidenceListType. It updates the linked list by either making the new
 *              evidenceNode the head and tail if the list is empty, or by appending it to the end
 *              of the list and updating the tail if the list already has elements.
 * Parameters:
 *      - GhostEvidenceListType *list: A pointer to the GhostEvidenceListType representing the list of evidence.
 *      - EvidenceNodeType* evidenceNode: A pointer to the EvidenceNodeType representing the new evidence.
 * Return: None
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ********************************************************************************************/
void addRoomEvidence(GhostEvidenceListType *list, EvidenceNodeType* evidenceNode) {
    evidenceNode->next = NULL;

    (list->head == NULL) 
        ? (list->head = list->tail = evidenceNode)  // If the list is currently empty
        : (list->tail->next = evidenceNode, list->tail = evidenceNode); // If the list already has elements
}


/* *******************************************************************************************
 * Function: float createGhostType(EvidenceClassType evidenceType)
 * Description: This function creates a ghost type value based on the given evidence type.
 *              It uses predefined ranges for each evidence type to generate a random value within
 *              the specified range. The function returns the random value or C_MISC_ERROR if the
 *              evidenceType is out of bounds.
 * Parameters:
 *      - EvidenceClassType evidenceType: An enumeration representing the type of evidence.
 * Return: A float representing the created ghost type or C_MISC_ERROR if an error occurs.
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ********************************************************************************************/
float createGhostType(EvidenceClassType evidenceType) {
    struct {
        float min;
        float max;
    } ranges[] = {
        {4.7, 5.0},       
        {-10.0, 1.0},      
        {1, 1},              
        {65.0, 75.0}         
    };

    if (evidenceType < 0 || evidenceType >= sizeof(ranges) / sizeof(ranges[0])) {
        return C_MISC_ERROR;
    }

    return randFloat(ranges[evidenceType].min, ranges[evidenceType].max);
}



/* *******************************************************************************************
 * Function: int isGhostHere(GhostType *currGhost)
 * Description: This function checks if a ghost is present in the current room. It returns 1 if
 *              the number of hunters in the current room is greater than 0, indicating the presence
 *              of a ghost. Otherwise, it returns 0.
 * Parameters:
 *      - GhostType *currGhost: A pointer to the GhostType representing the current ghost.
 * Return: 1 if the ghost is present, 0 otherwise.
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ********************************************************************************************/
int isGhostHere(GhostType *currGhost) {
    return (currGhost->room->hunters->size > 0) ? 1 : 0;
}




/* *******************************************************************************************
 * Function: void printGhost(const GhostType *ghost)
 * Description: This function prints a report of the given ghost's details. It includes the type
 *              of specter, ghostly location, and the remaining boredom countdown. The report is
 *              formatted for easy readability.
 * Parameters:
 *      - const GhostType *ghost: A pointer to the GhostType representing the ghost to be printed.
 * Return: None
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ********************************************************************************************/
void printGhost(const GhostType *ghost) {
    puts("\n*** GHOST REPORT ***");
    printf("Type of Specter: %s\n", ghostTypeToString(ghost->ghostType));
    printf("Ghostly Location: %s\n", ghost->room->name);
    printf("Boredom Countdown: %d\n", ghost->boredomDuration);
    puts("*** END OF GHOST REPORT ***\n");
}

/* *******************************************************************************************
 * Function: void printGhostEvidenceList(const GhostEvidenceListType *ghostEvidenceList, const char* indents)
 * Description: This function prints a report of the given ghost's evidence list. It includes the
 *              type of evidence, reading information, and whether the evidence is from a ghost or not.
 *              The report is formatted with indentation for better readability.
 * Parameters:
 *      - const GhostEvidenceListType *ghostEvidenceList: A pointer to the GhostEvidenceListType representing
 *        the list of evidence.
 *      - const char* indents: A string representing the indentation for formatting.
 * Return: None
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ********************************************************************************************/
void printGhostEvidenceList(const GhostEvidenceListType *ghostEvidenceList, const char* indents) {
    const EvidenceNodeType *evidenceNode = ghostEvidenceList->head;

    if (evidenceNode != NULL) {
        do {
            printf("%sType: %s", indents, evidenceTypeToString(evidenceNode->data->evidenceType));
            printf("\t%f %s\n", evidenceNode->data->readingInfo,
                isEvidenceFromGhost(evidenceNode->data) ? "(Ghostly)" : "");
            evidenceNode = evidenceNode->next;
        } while (evidenceNode != NULL);
    }
}



/* *******************************************************************************************
 * Function: void freeGhost(GhostType *ghost)
 * Description: This function frees the memory allocated for the given ghost and its associated
 *              evidence list. It disconnects the evidence list from the room, releases the memory
 *              for the evidence list, and finally frees the memory for the ghost.
 * Parameters:
 *      - GhostType *ghost: A pointer to the GhostType representing the ghost to be freed.
 * Return: None
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ********************************************************************************************/
void freeGhost(GhostType *ghost) {
    GhostType *ghostToDelete = ghost;
    GhostEvidenceListType *evidenceListToDelete = ghost->room->evidenceList;

    ghostToDelete->room->evidenceList = NULL;  // Disconnect the evidence list from the room
    releaseEvidenceList(evidenceListToDelete);    // Free the evidence list
    free(ghostToDelete);                        // Free the ghost
}
