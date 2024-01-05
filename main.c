#include "defs.h"

/***************************************************************************************
 * Function: void initializeGame(int argc, char *argv[])
 * Description: This function initializes the game by setting up the house, populating
 *              rooms, creating hunters and ghosts, and managing the game threads.
 * Parameters:
 *      - int argc: The number of command-line arguments.
 *      - char *argv[]: An array of command-line argument strings.
 * Return: None
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ***************************************************************************************/
void initializeGame(int argc, char *argv[]) {
    int hunterRestDuration = 0;
    int ghostRestDuration = 0;

    switch (argc) {
        case 3:
            hunterRestDuration = strtol(argv[1], NULL, 10);
            ghostRestDuration = strtol(argv[2], NULL, 10);
            break;
    }

    srand(time(NULL));

    pthread_t pThreadghost;

    HouseType house;
    initializeHouse(&house);
    populateRooms(&house);

    HunterListType hunterList;
    HunterListType *hunterListPointer = &hunterList;
    initListOfHunters(hunterListPointer);

    initializeGhost(randInt(0, 4), randomRoom(house.rooms->head)->data, ghostRestDuration, house.ghost);
    GhostType *ghostPointer = house.ghost;

    RoomType *vanRoom = house.rooms->head->data;

    int toolSize = MAX_HUNTERS;
    int toolArray[MAX_HUNTERS] = {0, 1, 2, 3};

    int i = 0;
    while (i < MAX_HUNTERS) {
        char name[MAX_STR];
        printf("%d. Hunter:\n", i + 1);
        scanf("%s", name);

        HunterType *currHunterPointer;
        initializeHunter(name, vanRoom, randomTool(toolArray, &toolSize), hunterRestDuration, &currHunterPointer);

        printf("[HUNTER INIT] [%s] is a [%s] hunter\n", currHunterPointer->name, evidenceTypeToString(currHunterPointer->evidence));

        assignHunterToRoom(vanRoom, currHunterPointer);
        appendHunterToList(hunterListPointer, currHunterPointer);
        appendHunterToList(house.hunters, currHunterPointer);

        i++;
    }

    pthread_t hunterThreadArray[MAX_HUNTERS];
    int j = 0;
    while (j < MAX_HUNTERS) {
        pthread_create(&hunterThreadArray[j], NULL, hunterThread, (void *)hunterListPointer->hunterList[j]);
        j++;
    }

    pthread_create(&pThreadghost, NULL, ghostThread, (void *)ghostPointer);

    int k = 0;
    while (k < MAX_HUNTERS) {
        pthread_join(hunterThreadArray[k], NULL);
        k++;
    }

    pthread_join(pThreadghost, NULL);

    int m = 0;
    while (m < hunterListPointer->size) {
        printHunter(hunterListPointer->hunterList[m]);
        m++;
    }

    printf("\n\nHunters with max fear:\n");
    int fearCounter = getFearLevel(hunterListPointer);
    getWinner(hunterListPointer, ghostPointer, fearCounter);

    int n = 0;
    while (n < hunterListPointer->size) {
        releaseHunterResources(hunterListPointer->hunterList[n]);
        n++;
    }

    releaseRoomList(house.rooms);
    free(house.rooms);
    free(house.hunters);
    free(house.ghost);
}

/***************************************************************************************
 * Function: int main(int argc, char *argv[])
 * Description: The main function of the program that initializes the game and returns
 *              the exit code.
 * Parameters:
 *      - int argc: The number of command-line arguments.
 *      - char *argv[]: An array of command-line argument strings.
 * Return: The exit code.
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ***************************************************************************************/
int main(int argc, char *argv[]) {
    initializeGame(argc, argv);

    return 0;
}


/***************************************************************************************
 * Function: int getFearLevel(HunterListType *hunterListPointer)
 * Description: This function calculates the number of hunters with fear levels exceeding
 *              a predefined threshold (FEAR_MAX). It prints information about the hunters
 *              with high fear levels and returns the fear count.
 * Parameters:
 *      - HunterListType *hunterListPointer: A pointer to the list of hunters.
 * Return: The count of hunters with fear levels exceeding FEAR_MAX.
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ***************************************************************************************/
int getFearLevel(HunterListType *hunterListPointer) {
    int fearCounter = 0;

    int i = 0;
    while (i < hunterListPointer->size) {
        HunterType *currHunter = hunterListPointer->hunterList[i];

        switch (currHunter->fear >= FEAR_MAX) {
            case 1:
                printHunter(currHunter);
                fearCounter++;
                break;
        }

        i++;
    }

    return fearCounter;
}

/***************************************************************************************
 * Function: int randomTool(int *arr, int *size)
 * Description: This function selects a random tool from an array and removes it from
 *              the array to prevent duplicate selections. It returns the selected tool
 *              or an indicator for an invalid tool if the array is empty.
 * Parameters:
 *      - int *arr: Pointer to the array of available tools.
 *      - int *size: Pointer to the size of the array.
 * Return: The selected tool or INVALID_EVIDENCE_TOOL if the array is empty.
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ***************************************************************************************/
int randomTool(int *arr, int *size) {
    int derefSize = *size;

    switch (derefSize) {
        case 0:
            return INVALID_EVIDENCE_TOOL;
    }

    int randomIndex = randInt(0, derefSize);
    int valToReturn = arr[randomIndex];

    int i = randomIndex;
    while (i < derefSize - 1) {
        arr[i] = arr[i + 1];
        i++;
    }

    (*size)--;

    return valToReturn;
}



/*****************************************************************************************
 * Function: int findingGhost(HunterListType *hunters)
 * Description: This function analyzes the collected evidence from hunters to determine
 *              the speculated ghost type. It checks for the presence of specific evidence
 *              types and returns the corresponding ghost type.
 * Parameters:
 *      - HunterListType *hunters: Pointer to the list of hunters with their evidence.
 * Return: The speculated ghost type based on evidence, or UNKNOWN_GHOST if insufficient.
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 *****************************************************************************************/
int findingGhost(HunterListType *hunters) {
    int hasEvidence[4] = {C_FALSE, C_FALSE, C_FALSE, C_FALSE};

    for (int i = 0; i < hunters->size; i++) {
        EvidenceNodeType *tempEvidenceNode = hunters->hunterList[i]->ghostEvidence->head;

        while (tempEvidenceNode != NULL) {
            if (isEvidenceFromGhost(tempEvidenceNode->data) && hunters->hunterList[i]->fear < 100) {
                int evidenceType = tempEvidenceNode->data->evidenceType;
                hasEvidence[evidenceType] = C_TRUE;
            }
            tempEvidenceNode = tempEvidenceNode->next;
        }
    }

    if (hasEvidence[0] && hasEvidence[1] && hasEvidence[2]) {
        return POLTERGEIST;
    } else if (hasEvidence[0] && hasEvidence[1] && hasEvidence[3]) {
        return BANSHEE;
    } else if (hasEvidence[0] && hasEvidence[2] && hasEvidence[3]) {
        return BULLIES;
    } else if (hasEvidence[1] && hasEvidence[2] && hasEvidence[3]) {
        return PHANTOM;
    } else {
        return UNKNOWN_GHOST;
    }
}




/*****************************************************************************************
 * Function: void getWinner(HunterListType *list, GhostType *ghost, int fear)
 * Description: This function determines the winner based on the fear level and collected
 *              evidence. It prints the result, including the speculated and actual ghost
 *              types, or a message if there is insufficient evidence.
 * Parameters:
 *      - HunterListType *list: Pointer to the list of hunters with their evidence.
 *      - GhostType *ghost: Pointer to the ghost entity.
 *      - int fear: The overall fear level calculated from hunters' fear values.
 * Return: None
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 *
 *****************************************************************************************/
int totalEvidenceCollected = 0;

void getWinner(HunterListType *list, GhostType *ghost, int fear) {
    int ghostWon = (fear >= 4);

    if (totalEvidenceCollected >= 3) {
        printf("Hunters win! They have collected enough evidence to identify the ghost.\n");
        printf("\nEvidence collected by hunters:\n");
        for (int i = 0; i < list->size; ++i) {
            HunterType *hunter = list->hunterList[i];
            EvidenceNodeType *evidenceNode = hunter->ghostEvidence->head;
            while (evidenceNode != NULL) {
                EvidenceType *evidence = evidenceNode->data;
                printf("%s found [%s] in [%s]\n", hunter->name, evidenceTypeToString(evidence->evidenceType), hunter->room->name);
                evidenceNode = evidenceNode->next;
            }
        }
        exit(EXIT_SUCCESS);
    }

    printf("%s\n", ghostWon ? "The ghost won" : (
        printf("Speculated Ghost Type: %s\n", ghostTypeToString((GhostClassType)findingGhost(list))),
        printf("Actual Ghost Type: %s\n", ghostTypeToString(ghost->ghostType)),
        ((GhostClassType)findingGhost(list) == (GhostClassType)UNKNOWN_GHOST) ? "There was not enough ghostly evidence collected to determine the ghost" : 
        ((list->hunterList[0]->evidenceCollected == 4) ? "Hunters win! They have collected enough evidence to identify the ghost.\n" : "The ghost won")
    ));
}


