#include "defs.h"

/************************************************************
 * Function: void printHunter(const HunterType *hunter)
 * Description: This function prints information about a hunter, including the hunter's name
 *              and the type of evidence they possess.
 * Parameters:
 *      - const HunterType *hunter: A pointer to the HunterType structure containing the hunter's
 *                                   information to be printed.
 * Return: None
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ************************************************************/
void printHunter(const HunterType *hunter) {
    printf("[HUNTER INIT] [%s] is a [%s] hunter\n", hunter->name, evidenceTypeToString(hunter->evidence));
}


/***************************************************************
 * Function: int verifyEvidence(HunterType *currHunter)
 * Description: This function verifies evidence between two hunters in the same room.
 *              It randomly selects another hunter in the same room, and the two hunters
 *              review their ghost evidence. If one hunter has evidence that the other does not,
 *              the evidence is communicated (printed), but not transferred.
 * Parameters:
 *      - HunterType *currHunter: A pointer to the HunterType structure representing the current hunter.
 * Return:
 *      - int: Returns C_TRUE (1) to indicate the verification process is complete.
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ***************************************************************/
int verifyEvidence(HunterType *currHunter) {
    RoomType *currRoom = currHunter->room;

    int randomHunter = randInt(0, currRoom->hunters->size);

    HunterType *hunterReview = currRoom->hunters->hunterList[randomHunter];

    HunterType *hunters[2] = {currHunter, hunterReview};

    for (int i = 0; i < 2; i++) {
        HunterType *startingH = hunters[i];
        HunterType *endH = hunters[(i + 1) % 2];

        EvidenceNodeType *node = startingH->ghostEvidence->head;

        while (node != NULL) {
            if (isEvidenceFromGhost(node->data) && !isDuplicate(endH->ghostEvidence, node)) {
                printf("[HUNTER REVIEW] [%s] reviewed evidence and found %s %f\n", startingH->name, evidenceTypeToString(node->data->evidenceType), node->data->readingInfo);
            }
            node = node->next;
        }
    }

    return C_TRUE;
}



/***************************************************************
 * Function: int repositionHunter(HunterType *currHunter)
 * Description: This function repositions a hunter to a random connected room,
 *              updating the hunter's current room, and decrementing the boredom timer.
 * Parameters:
 *      - HunterType *currHunter: A pointer to the HunterType structure representing the current hunter.
 * Return:
 *      - int: Returns C_TRUE (1) if the repositioning is successful, C_FALSE (0) otherwise.
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ***************************************************************/


int repositionHunter(HunterType* currHunter) {
    
    RoomNodeType *node = currHunter->room->connectedRooms->head;
    int sizeCounter;
    
    for (sizeCounter = 0; node != NULL; sizeCounter++, node = node->next);

    int roomInt = randInt(0, sizeCounter);

    RoomNodeType *roomNode = currHunter->room->connectedRooms->head;
    for(int i = 0; i < roomInt; i++) {
        roomNode = roomNode->next;
    }

    RoomType *oldRoom = currHunter->room;

    sem_wait(&(oldRoom->semaphore));

    int newRoomAvailable = 1; 
    do {
        if (sem_trywait(&(roomNode->data->semaphore)) != 0) {
            newRoomAvailable = 0;
            break; 
        }
    } while (0);

    if (!newRoomAvailable) {
        sem_post(&(oldRoom->semaphore));
        return C_FALSE;
    }

    rerepositionHunter(currHunter, C_FALSE);
    assignHunterToRoom(roomNode->data, currHunter);

    printf("[HUNTER MOVE] [%s] has moved into [%s]\n", currHunter->name, currHunter->room->name);

    currHunter->timer--;
  
    sem_post(&(oldRoom->semaphore));
    sem_post(&(roomNode->data->semaphore));

    return C_TRUE;
}


/***********************************************************************
 * Function: void rerepositionHunter(HunterType *hunter, int locked)
 * Description: This function removes a hunter from its current room's list
 *              of hunters, updating the list and decreasing the size.
 * Parameters:
 *      - HunterType *hunter: A pointer to the HunterType structure representing the hunter to be removed.
 *      - int locked: An integer indicating whether the old room's semaphore is already locked (C_TRUE) or not (C_FALSE).
 * Return: None
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ***********************************************************************/
void rerepositionHunter(HunterType *hunter, int locked) {
    RoomType *oldRoom = hunter->room;

    if (locked) {
        sem_wait(&(oldRoom->semaphore));
    }

    HunterListType *roomHunters = oldRoom->hunters;

    int indexToRemove = -1;
    for (int i = 0; i < roomHunters->size; i++) {
        if (roomHunters->hunterList[i] == hunter) {
            indexToRemove = i;
            break;
        }
    }

    if (indexToRemove != -1) {
        for (int j = indexToRemove + 1; j < roomHunters->size; j++) {
            roomHunters->hunterList[j - 1] = roomHunters->hunterList[j];
        }

        roomHunters->size--;
    }

    if (locked) {
        sem_post(&(oldRoom->semaphore));
    }
}


/***********************************************************************
 * Function: void addHunterEvidence(GhostEvidenceListType *list, EvidenceNodeType *evidence)
 * Description: This function adds a new evidence node to the GhostEvidenceListType structure.
 * Parameters:
 *      - GhostEvidenceListType *list: A pointer to the GhostEvidenceListType structure where the evidence will be added.
 *      - EvidenceNodeType *evidence: A pointer to the EvidenceNodeType structure representing the evidence to be added.
 * Return: None
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ***********************************************************************/
void addHunterEvidence(GhostEvidenceListType *list, EvidenceNodeType *evidence) {
    // Set the next to NULL for safety
    evidence->next = NULL;

    // Use a ternary operator to handle the empty list case
    (list->head == NULL) ? (list->head = list->tail = evidence) : (list->tail->next = evidence, list->tail = evidence);
}


/***********************************************************************
 * Function: void moveGhost(GhostType *currGhost)
 * Description: This function moves the ghost to a random connected room.
 * Parameters:
 *      - GhostType *currGhost: A pointer to the GhostType structure representing the ghost to be moved.
 * Return: None
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ***********************************************************************/
void moveGhost(GhostType *currGhost) {
    if (rand() % 100 < 45) {
        int size = 0;
        RoomNodeType *node = currGhost->room->connectedRooms->head;

        do {
            size++;
            node = node->next;
        } while (node != NULL);

        int nodeInt = randInt(0, size);
        RoomNodeType *tempRoom = currGhost->room->connectedRooms->head;

        int i = 0;
        do {
            tempRoom = tempRoom->next;
            i++;
        } while (i < nodeInt && tempRoom != NULL);

        if (tempRoom != NULL) {
            printf("[GHOST MOVE] Ghost has moved into [%s]\n", tempRoom->data->name);

            currGhost->room->ghost = NULL;

            currGhost->room = tempRoom->data;

            currGhost->room->ghost = currGhost;
        }
    }
}


/***********************************************************************
 * Function: void initializeGhost(GhostClassType ghostType, RoomType *room, int restDuration, GhostType *ghost)
 * Description: This function initializes a GhostType structure with the provided ghost type, room, and rest duration.
 * Parameters:
 *      - GhostClassType ghostType: The type of the ghost.
 *      - RoomType *room: A pointer to the RoomType structure representing the initial room of the ghost.
 *      - int restDuration: The rest duration of the ghost.
 *      - GhostType *ghost: A pointer to the GhostType structure to be initialized.
 * Return: None
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ***********************************************************************/
void initializeGhost(GhostClassType ghostType, RoomType *room, int restDuration, GhostType *ghost) {
    ghost->ghostType = ghostType;
    ghost->restDuration = restDuration;
    ghost->boredomDuration  = BOREDOM_MAX;
    ghost->room = room;
    
}


/************************************************************************************
 * Function: int grabEvidence(HunterType *currHunter)
 * Description: This function allows the hunter to grab evidence from the current room's
 *              evidence list. If the evidence matches the hunter's assigned evidence type,
 *              it is collected and added to the hunter's ghost evidence list.
 * Parameters:
 *      - HunterType *currHunter: A pointer to the HunterType structure representing the current hunter.
 * Return:
 *      - int: C_TRUE if evidence is successfully grabbed and collected, C_FALSE otherwise.
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ************************************************************************************/
int grabEvidence(HunterType *currHunter) {
    EvidenceNodeType *tempEvidence = currHunter->room->evidenceList->head;
    EvidenceType *newEvidence;
    EvidenceNodeType *newNode;

    if (tempEvidence == NULL) {
        // No evidence in the room
        return C_FALSE;
    }

    for (; tempEvidence != NULL; tempEvidence = tempEvidence->next) {
        if (tempEvidence->data->evidenceType != currHunter->evidence) {
            continue;
        }

        // Introduce a probability check (80% chance of collecting evidence)
        int shouldCollect = rand() % 100 < 60;

        if (shouldCollect) {
            newEvidence = (EvidenceType*) malloc(sizeof(EvidenceType));

            if (newEvidence == NULL) {
                perror("Failed to allocate memory for EvidenceType");
                exit(EXIT_FAILURE);
            }

            newEvidence->readingInfo = tempEvidence->data->readingInfo;
            newEvidence->evidenceType = tempEvidence->data->evidenceType;

            newNode = (EvidenceNodeType*) malloc(sizeof(EvidenceNodeType));

            if (newNode == NULL) {
                perror("Failed to allocate memory for EvidenceNodeType");
                exit(EXIT_FAILURE);
            }

            newNode->data = newEvidence;
            newNode->next = NULL;

            addHunterEvidence(currHunter->ghostEvidence, newNode);
            printf("[HUNTER EVIDENCE] [%s] found [%s] in [%s] and [COLLECTED]\n", currHunter->name, evidenceTypeToString(newEvidence->evidenceType), currHunter->room->name);
            removeEvidence(currHunter->room->evidenceList, tempEvidence);
            currHunter->timer = isEvidenceFromGhost(newEvidence) ? BOREDOM_MAX : currHunter->timer;

            // Increment evidenceCollected
            if (++currHunter->evidenceCollected >= 3) {
                printf("[HUNTER EVIDENCE] [%s] has collected the maximum allowed evidence\n", currHunter->name);

                // Increment totalEvidenceCollected
                if (++totalEvidenceCollected >= 3) {
                    printf("Hunters win! They have collected enough evidence to identify the ghost.\n");
                    exit(EXIT_SUCCESS);
                }

                return C_FALSE;  // Stop collecting evidence for this hunter
            }

            return C_TRUE;  // Successfully collected evidence
        }

        return C_FALSE;  // Hunter decided not to collect evidence this time
    }

    return C_FALSE;  // No evidence found for this hunter
}

/************************************************************************************
 * Function: void newRandomEvidence(GhostType *currGhost)
 * Description: This function generates new random evidence and adds it to the current
 *              room's evidence list. The evidence type and reading info are determined
 *              based on the ghost's ghostType.
 * Parameters:
 *      - GhostType *currGhost: A pointer to the GhostType structure representing the current ghost.
 * Return: None
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ************************************************************************************/
void newRandomEvidence(GhostType *currGhost) {
    sem_wait(&(currGhost->room->semaphore));

    EvidenceNodeType *node = malloc(sizeof(EvidenceNodeType));
    EvidenceType *newEvidencePointer = malloc(sizeof(EvidenceType));

    int randomEvidence = randomGhostEvidence(currGhost->ghostType);

    newEvidencePointer->evidenceType = (EvidenceClassType)randomEvidence;
    newEvidencePointer->readingInfo = createGhostType(newEvidencePointer->evidenceType);

    node->data = newEvidencePointer;
    node->next = NULL;

    addRoomEvidence(currGhost->room->evidenceList, node);

    printf("[GHOST EVIDENCE] Ghost left [%s] in [%s]\n", evidenceTypeToString(node->data->evidenceType),
           currGhost->room->name);

    sem_post(&(currGhost->room->semaphore));
}
