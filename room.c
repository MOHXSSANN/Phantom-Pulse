#include "defs.h"

/************************************************************************************************
 * Function: void initializeRoom(RoomType *room, const char *name)
 * Description: This function initializes a RoomType structure, setting up its semaphore, name,
 *              and allocating memory for connectedRooms, evidenceList, and hunters. It also
 *              initializes the Room's ghost to NULL.
 * Parameters:
 *      - RoomType *room: Pointer to the RoomType structure to be initialized.
 *      - const char *name: Name to be assigned to the room.
 * Return: None
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ************************************************************************************************/
void initializeRoom(RoomType *room, const char *name) {
    if (sem_init(&(room->semaphore), 0, 1) != 0) {
        perror("Failed to initialize semaphore");
        exit(EXIT_FAILURE); 
    }

    strncpy(room->name, name, sizeof(room->name) - 1);
    room->name[sizeof(room->name) - 1] = '\0'; 

    room->connectedRooms = malloc(sizeof(RoomListType));
    if (room->connectedRooms == NULL) {
        perror("Failed to allocate memory for connectedRooms");
        exit(EXIT_FAILURE); 
    }
    initListOfRooms(&(room->connectedRooms));

    room->evidenceList = malloc(sizeof(GhostEvidenceListType));
    if (room->evidenceList == NULL) {
        perror("Failed to allocate memory for evidenceList");
        exit(EXIT_FAILURE); 
    }
    initListOfGhosts(room->evidenceList);

    room->hunters = malloc(sizeof(HunterListType));
    if (room->hunters == NULL) {
        perror("Failed to allocate memory for hunters");
        exit(EXIT_FAILURE); 
    }
    initListOfHunters(room->hunters);


    room->ghost = NULL;
}

/************************************************************************************************
 * Function: void initListOfRooms(RoomListType **roomList)
 * Description: This function initializes a RoomListType structure by setting its head and tail
 *              pointers to NULL.
 * Parameters:
 *      - RoomListType **roomList: Pointer to a pointer to the RoomListType structure to be
 *                                 initialized.
 * Return: None
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ************************************************************************************************/
void initListOfRooms(RoomListType **roomList){
    RoomListType *list = *roomList;
    list->head = list->tail = NULL;
}


/************************************************************************************************
 * Function: void connectRooms(RoomType *room1, RoomType *room2)
 * Description: This function connects two rooms by creating nodes for each room and adding them
 *              to each other's connected rooms list.
 * Parameters:
 *      - RoomType *room1: Pointer to the first room to be connected.
 *      - RoomType *room2: Pointer to the second room to be connected.
 * Return: None
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ************************************************************************************************/
void connectRooms(RoomType *room1, RoomType *room2){
    RoomNodeType *roomNode1 = calloc(1, sizeof(RoomNodeType)), *roomNode2 = calloc(1, sizeof(RoomNodeType));

    addRoom((roomNode1->data = room1)->connectedRooms, roomNode2);
    addRoom((roomNode2->data = room2)->connectedRooms, roomNode1);
}


/************************************************************************************************
 * Function: void addRoom(RoomListType *roomList, RoomNodeType *room)
 * Description: This function adds a new room node to the given room list.
 * Parameters:
 *      - RoomListType *roomList: Pointer to the room list.
 *      - RoomNodeType *room: Pointer to the room node to be added.
 * Return: None
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ************************************************************************************************/
void addRoom(RoomListType *roomList, RoomNodeType *room) {
    room->next = NULL;
    roomList->head ? (roomList->tail->next = room) : (roomList->head = room);
    roomList->tail = room;
}

/************************************************************************************************
 * Function: int assignHunterToRoom(RoomType* room, HunterType* hunter)
 * Description: This function assigns a hunter to a room if there is available space.
 * Parameters:
 *      - RoomType* room: Pointer to the room.
 *      - HunterType* hunter: Pointer to the hunter to be assigned.
 * Return:
 *      - int: Returns C_TRUE if the hunter was assigned successfully, C_FALSE otherwise.
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ************************************************************************************************/
int assignHunterToRoom(RoomType* room, HunterType* hunter) {
    int roomAvailability = (room->hunters->size < MAX_HUNTERS) && (
        (room->hunters->hunterList[room->hunters->size] = hunter),
        (room->hunters->size++),
        (hunter->room = room),
        C_TRUE
    );

    return roomAvailability ? C_TRUE : C_FALSE;
}



/************************************************************************************************
 * Function: void displayRoomList(RoomListType *roomList)
 * Description: This function displays the names of rooms in a RoomListType.
 * Parameters:
 *      - RoomListType* roomList: Pointer to the RoomListType to be displayed.
 * Return: None
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ************************************************************************************************/

void displayRoomList(RoomListType *roomList){
    for(RoomNodeType *currNode = roomList->head; currNode; currNode = currNode->next){
        printf("\t%s\n", currNode->data->name);
    }
}


/************************************************************************************************
 * Function: void releaseConnectedRooms(RoomListType* list)
 * Description: This function releases the memory allocated for the connected rooms in a RoomListType.
 *              It iterates through the list, frees the memory for each node, and sets the head to NULL.
 * Parameters:
 *      - RoomListType* list: Pointer to the RoomListType whose connected rooms need to be released.
 * Return: None
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ************************************************************************************************/
void releaseConnectedRooms(RoomListType* list) {
    for (RoomNodeType *currNode = list->head; currNode; list->head = currNode) {
        RoomNodeType *nextNode = currNode->next;
        free(currNode);
        currNode = nextNode;
    }
}

/************************************************************************************************
 * Function: void releaseRoomList(RoomListType *list)
 * Description: This function releases the memory allocated for a RoomListType. It iterates through
 *              the list, frees the memory for each node along with its contents (semaphore, evidence
 *              list, connected rooms, hunters, and the room itself), and sets the head to NULL.
 * Parameters:
 *      - RoomListType *list: Pointer to the RoomListType whose rooms need to be released.
 * Return: None
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ************************************************************************************************/
void releaseRoomList(RoomListType *list){
    RoomNodeType *currentNode = list->head;
    while(currentNode){
        RoomNodeType *nextNode = currentNode->next;
        sem_destroy(&(currentNode->data->semaphore));
        releaseEvidenceList(currentNode->data->evidenceList);
        releaseConnectedRooms(currentNode->data->connectedRooms);
        free(currentNode->data->connectedRooms);
        free(currentNode->data->hunters);
        free(currentNode->data);
        free(currentNode);
        currentNode = nextNode;
    }
    list->head = NULL;
}

/************************************************************************************************
 * Function: RoomNodeType* randomRoom(RoomNodeType* head)
 * Description: This function returns a pointer to a randomly selected room from a linked list of
 *              rooms. It first counts the number of rooms, generates a random index, and then
 *              traverses to the selected room.
 * Parameters:
 *      - RoomNodeType* head: Pointer to the head of the linked list of rooms.
 * Return: RoomNodeType* - Pointer to the randomly selected room.
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ************************************************************************************************/
RoomNodeType* randomRoom(RoomNodeType* head) {
    int count = 0;
    RoomNodeType* current = head;
    while (current != NULL) {
        count++;
        current = current->next;
    }

    int randomIndex = rand() % count;

    current = head;
    for (int i = 0; i < randomIndex; i++) {
        current = current->next;
    }

    return current;
}
