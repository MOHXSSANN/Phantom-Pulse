#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

#define INVALID_EVIDENCE_TOOL -1

#define MAX_STR            64
#define MAX_RUNS           50
#define BOREDOM_MAX        100
#define C_TRUE              1
#define C_FALSE             0
#define MAX_HUNTERS         4
#define HUNTER_WAIT        5000
#define GHOST_WAIT         600
#define FEAR_MAX        10
#define LOGGING         C_TRUE
#define MAX_NAME_LENGTH 256



//#define FEAR_RATE           1
#define USLEEP_TIME     50000
#define ALL_ROOMS        13
#define C_MISC_ERROR       -1
#define C_NO_ROOM_ERROR    -2
#define C_ARR_ERROR        -3
#define UNKNOWN_GHOST      -4

typedef enum { EMF, TEMPERATURE, FINGERPRINTS, SOUND } EvidenceClassType;
typedef enum { POLTERGEIST, BANSHEE, BULLIES, PHANTOM } GhostClassType;
enum LoggerDetails { LOG_FEAR, LOG_BORED, LOG_EVIDENCE, LOG_SUFFICIENT, LOG_INSUFFICIENT, LOG_UNKNOWN };


int randInt(int, int);
float randFloat(float, float);

typedef struct EvidenceNode {
    struct EvidenceType* data;
    struct EvidenceNode* next;
} EvidenceNodeType;

typedef struct RoomNode {
    struct RoomType* data;
    struct RoomNode* next;
} RoomNodeType;

typedef struct GhostEvidenceList {
    EvidenceNodeType* head;
    EvidenceNodeType* tail;
} GhostEvidenceListType;

typedef struct RoomList {
    RoomNodeType* head;
    RoomNodeType* tail;
    int size;
} RoomListType;

typedef struct HunterType {
    struct RoomType *room;
    EvidenceClassType evidence;
    GhostEvidenceListType *ghostEvidence;
    char name[MAX_STR];
    int fear;
    int timer;
    int restDuration;
    int evidenceCollected;

} HunterType;

typedef struct GhostType {
    GhostClassType ghostType;
    struct RoomType *room;
    int boredomDuration;
    int restDuration;
} GhostType;

typedef struct RoomType {
    sem_t semaphore;
    char name[MAX_STR];
    RoomListType* connectedRooms;
    GhostEvidenceListType* evidenceList;
    struct HunterListType *hunters;
    struct GhostType *ghost;
} RoomType;

typedef struct HunterListType {
    int size;
    HunterType *hunterList[MAX_HUNTERS];
    
} HunterListType;

typedef struct EvidenceType {
    EvidenceClassType evidenceType;
    float readingInfo;
} EvidenceType;

typedef struct {
    GhostType* ghost;
    HunterListType *hunters;
    RoomListType* rooms;
} HouseType; 

void *ghostThread(void*);
void *hunterThread(void*);

void populateRooms(HouseType*);

void initListOfHunters(HunterListType*); //g
void initializeHouse(HouseType*); //g
void initializeRoom(RoomType *room, const char *name);//g
void initListOfRooms(RoomListType**);//g
void initListOfGhosts(GhostEvidenceListType *);
void initializeGhost(GhostClassType, RoomType*, int, GhostType *);
void initializeEvidence(GhostEvidenceListType *);
void initializeHunter(char* , RoomType *, int, int, HunterType **);
void connectRooms(RoomType*, RoomType*);
void addRoom(RoomListType*, RoomNodeType*);
extern int totalEvidenceCollected;
bool appendHunterToList(HunterListType *hunters, HunterType *hunter);
int assignHunterToRoom(RoomType*, HunterType*);
RoomNodeType* randomRoom(RoomNodeType* head);
int randomTool(int * , int *);
int findingGhost(HunterListType*);
int getFearLevel(HunterListType *);
void getWinner(HunterListType *, GhostType*, int);
void addHunterEvidence(GhostEvidenceListType*, EvidenceNodeType*);
void newRandomEvidence(GhostType*);
float createGhostType(EvidenceClassType);
void moveGhost(GhostType*);
int isGhostHere(GhostType*);
int randomGhostEvidence(GhostClassType);
int didHunterFindGhost(HunterType*);
int containsEvidence(HunterType*);
int verifyEvidence(HunterType*);
int grabEvidence(HunterType*);
int repositionHunter(HunterType*);
int removeEvidence(GhostEvidenceListType *, EvidenceNodeType *);
int isEvidenceFromGhost(EvidenceType*);
float createStandardValue(EvidenceClassType);
const char* evidenceTypeToString(EvidenceClassType evidence);
const char* ghostTypeToString(GhostClassType ghost);
GhostEvidenceListType* copyEvidence(GhostEvidenceListType *);
int isDuplicate(GhostEvidenceListType *, EvidenceNodeType*);
void addRoomEvidence(GhostEvidenceListType *, EvidenceNodeType*);
void rerepositionHunter(HunterType *, int);
void printHunter(const HunterType *hunter);
void printGhost(const GhostType *ghost);
void printGhostEvidenceList(const GhostEvidenceListType *ghostEvidenceList, const char* indents);
void displayRoomList(RoomListType*);
void releaseHunterResources(HunterType *); 
void freeGhost(GhostType *);
void freeRoom(RoomType *);
void cleanUpEvidenceData(GhostEvidenceListType *);
void releaseEvidenceNodes(GhostEvidenceListType *);
void releaseEvidenceList(GhostEvidenceListType *);
void releaseRoomList(RoomListType *);
void releaseConnectedRooms(RoomListType*);



