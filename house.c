#include "defs.h"

/* *******************************************************************************************
 * Function: void populateRooms(HouseType* house)
 * Description: This function populates the given house with rooms. It creates an array of room names,
 *              initializes RoomType structures for each room, creates RoomNodeType structures, assigns
 *              the RoomType structures to the RoomNodeType structures, and adds the rooms to the house.
 * Parameters:
 *      - HouseType* house: A pointer to the HouseType representing the house to be populated.
 * Return: None
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ********************************************************************************************/
void populateRooms(HouseType* house) {
    const char* roomNames[] = {"Van", "Hallway", "Master Bedroom", "Boy's Bedroom", "Bathroom", "Basement", "Basement Hallway", "Right Storage Room", "Left Storage Room", "Kitchen", "Living Room", "Garage", "Utility Room"};
    int numRooms = sizeof(roomNames) / sizeof(roomNames[0]);
    RoomType* rooms[numRooms];
    RoomNodeType* roomNodes[numRooms];

    for (int i = 0; i < numRooms; i++) {
        rooms[i] = calloc(1, sizeof(RoomType));
        initializeRoom(rooms[i], roomNames[i]);
        roomNodes[i] = calloc(1, sizeof(RoomNodeType));
        roomNodes[i]->data = rooms[i];
        addRoom(house->rooms, roomNodes[i]);
    }

    connectRooms(rooms[0], rooms[1]);
    connectRooms(rooms[1], rooms[2]);
    connectRooms(rooms[1], rooms[3]);
    connectRooms(rooms[1], rooms[4]);
    connectRooms(rooms[1], rooms[9]);
    connectRooms(rooms[1], rooms[5]);
    connectRooms(rooms[5], rooms[6]);
    connectRooms(rooms[6], rooms[7]);
    connectRooms(rooms[6], rooms[8]);
    connectRooms(rooms[9], rooms[10]);
    connectRooms(rooms[9], rooms[11]);
    connectRooms(rooms[11], rooms[12]);
}

/* *******************************************************************************************
 * Function: void initializeHouse(HouseType *house)
 * Description: This function initializes a HouseType structure by allocating memory for the ghost,
 *              rooms, and hunters. It also initializes the list of hunters if the allocation is
 *              successful.
 * Parameters:
 *      - HouseType *house: A pointer to the HouseType structure to be initialized.
 * Return: None
 * CHATGPT ACCESSED DECEMBER 2ND 2023
 ********************************************************************************************/
void initializeHouse(HouseType *house) {
    house->ghost = (GhostType*)calloc(1, sizeof(GhostType));
    house->rooms = (RoomListType*)calloc(1, sizeof(RoomListType));
    house->hunters = (HunterListType*)calloc(1, sizeof(HunterListType));
    
    if (house->hunters != NULL) {
        initListOfHunters(house->hunters);
    }
}

