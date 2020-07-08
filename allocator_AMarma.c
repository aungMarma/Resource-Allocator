#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
If the status is 0 and process is null, then there is no process,
It just represent the empty block of memory
empty block with start, end, next could be null or another block

If status is 0 and process is not null,
It represents a preocess that has not placed in memory yet
it has no start and end value yet, just name of the process and size of it

If the status is not 1, we have a block occupied by a process
*/
struct BlockNode
{
    int start;
    int end;
    int status; // 0 => free, 1=> occupied
    int processSize;
    struct BlockNode *next;
    char processName[];
};

// declartion of functions
struct BlockNode *findSmallest(struct BlockNode *blockNode);
void init(struct BlockNode *blockNode);
void removeFromList(struct BlockNode *blockNode);
void insert(struct BlockNode *newBlockNode, struct BlockNode *oldBlockNode);
void allocate(struct BlockNode *blockNode);
void showStat();
void releaseProcess(char processName[]);
void merge(struct BlockNode *prev, struct BlockNode *curr);
void compaction();
struct BlockNode *insertToHeadOccupiedNode(struct BlockNode *newHead, struct BlockNode *blockNode);

// max index in memory
// 1048576 => 1048576 - 1
int max = 0;
// head of the list, represents the allocation table
struct BlockNode *head = NULL;

// moves processes towards the low end of addresses and free space to the high
// end of memory addresses.Memory addresses start at 0.
void compaction()
{
    // 1. place all non-free block to the end of the memory
    // 2. place all free block to the beginning to the memory
    // 3. all all in step 2

    struct BlockNode *curr = head;
    struct BlockNode *newHead = NULL;

    while (curr != NULL)
    {
        struct BlockNode *temp = curr->next;

        // insert might modify the next pointer of curr.next
        if (curr->status == 1)
        {
            newHead = insertToHeadOccupiedNode(newHead, curr);
        }
        curr = temp;
    }

    int min = newHead->start - 1;
    if (min > 0)
    {
        // init the list with whole free block of memory
        struct BlockNode *newBlockNode = (struct BlockNode *)malloc(sizeof(struct BlockNode));
        newBlockNode->start = 0;
        newBlockNode->end = min;
        newBlockNode->status = 0;
        newBlockNode->processSize = 0;
        strcpy(newBlockNode->processName, "");

        // adjust pointers, e.g place the new node at the beginning of the list
        newBlockNode->next = newHead;
        newHead = newBlockNode;
    }
    head = newHead; // head points to brand new list
}

// place first blockNode to end of the memory
struct BlockNode *insertToHeadOccupiedNode(struct BlockNode *newHead, struct BlockNode *blockNode)
{
    blockNode->end = max;                                // max is global
    blockNode->start = max - blockNode->processSize + 1; // 3 - 4 + 1 => 0
    max = blockNode->start - 1;                          // update max for next iteration (e.g next function call)

    if (newHead == NULL)
    {
        blockNode->next = NULL;
        return blockNode;
    }
    else
    {
        blockNode->next = newHead;
        newHead = blockNode;
        return newHead;
    }
}

// release the process with the given process name
void releaseProcess(char processName[])
{
    struct BlockNode *curr = head;
    struct BlockNode *prev = NULL;
    while (curr != NULL)
    {
        if (strcmp(curr->processName, processName) == 0)
        {
            curr->status = 0; // 1 to 0
            strcpy(curr->processName, "");
            curr->processSize = 0;

            // merge contiguous free block
            merge(curr, curr->next);
            merge(prev, curr);
            return; // done
        }
        prev = curr;
        curr = curr->next;
    }
}

// merge two coniguous free holes, when a preocess is released
void merge(struct BlockNode *prev, struct BlockNode *curr)
{
    // If one of these is null, no merge needed
    if (prev == NULL || curr == NULL)
        return;

    if (prev->status == 0 && curr->status == 0)
    {
        // merge curr to prev

        // grab the smallest of two
        if (prev->start > curr->start)
        {
            prev->start = curr->start;
        }
        // grab the biggest of two
        if (prev->end < curr->end)
        {
            prev->end = curr->end;
        }

        // remove curr
        removeFromList(curr);
    }
}

// initialize the list
void init(struct BlockNode *blockNode)
{
    head = blockNode;
    max = blockNode->end;
}

// remove from the list, not release a process
void removeFromList(struct BlockNode *blockNode)
{
    struct BlockNode *prev = NULL;
    struct BlockNode *curr = head;

    while (curr != NULL)
    {
        if (curr == blockNode)
        {
            if (curr == head)
            {
                head = curr->next;
            }
            else
            {
                prev->next = curr->next;
            }
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

// insert the newnode before oldnode
void insert(struct BlockNode *newBlockNode, struct BlockNode *oldBlockNode)
{
    struct BlockNode *prev = NULL;
    struct BlockNode *curr = head;

    while (curr != NULL)
    {
        if (curr == oldBlockNode)
        {
            if (curr == head)
            {
                newBlockNode->next = curr;
                head = newBlockNode;
            }
            else
            {
                prev->next = newBlockNode;
                newBlockNode->next = curr;
            }
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

// apply the best-fit strategy
struct BlockNode *findSmallest(struct BlockNode *blockNode)
{
    struct BlockNode *b = head;
    struct BlockNode *smallestBlockNode = NULL;
    while (b != NULL)
    {
        // possible block to allocate, free and big enough
        // example, b => status 0 and 0-3, blockNode process size 4
        if (b->status == 0 && b->end - b->start + 1 >= blockNode->processSize)
        {
            if (smallestBlockNode == NULL)
            {
                smallestBlockNode = b;
            }
            else if (smallestBlockNode->end - smallestBlockNode->start + 1 > b->end - b->start + 1)
            {
                // found smaller
                smallestBlockNode = b;
            }
        }
        b = b->next;
    }
    return smallestBlockNode;
}

// allocate memory, respond to a request
void allocate(struct BlockNode *blockNode)
{
    struct BlockNode *smallestBlockNode = findSmallest(blockNode);

    // OR throw an error
    if (smallestBlockNode == NULL)
    {
        puts("Not enough memory available!!");
        return;
    }

    // start and end are zero based
    blockNode->start = smallestBlockNode->start;
    blockNode->end = smallestBlockNode->start + blockNode->processSize - 1;
    blockNode->status = 1; // occupy

    // allocate memory, fit the new blocknode to the list
    insert(blockNode, smallestBlockNode);

    smallestBlockNode->start = blockNode->end + 1;
    if (smallestBlockNode->end < smallestBlockNode->start)
    {
        // remove smallestBlockNode from the list
        removeFromList(smallestBlockNode);
    }
}

// show stat like in text
void showStat()
{
    // Addresses [0 : 315000] Process P1
    // Addresses [0 : 315000] Free
    struct BlockNode *curr = head;
    while (curr != NULL)
    {
        if (curr->status == 1)
        {
            printf("Addresses [%d : %d] Process %s\n", curr->start, curr->end, curr->processName);
        }
        else
        {
            printf("Addresses [%d : %d] Free\n", curr->start, curr->end);
        }
        curr = curr->next;
    }
}

int main(int argc, char *argv[])
{

    // get the max value
    if (argc <= 1)
    {
        puts("A max is required to allocate.");
        return 0; // exit
    }
    max = atoi(argv[1]) - 1;

    // init the list with whole free block of memory
    struct BlockNode *newBlockNode = (struct BlockNode *)malloc(sizeof(struct BlockNode));
    newBlockNode->start = 0;
    newBlockNode->end = max;
    newBlockNode->status = 0;
    newBlockNode->processSize = 0;
    strcpy(newBlockNode->processName, "");
    newBlockNode->next = NULL;

    init(newBlockNode); // init call
    // showStat();         // check initial set up

    // check user command against these cases
    char requestCommand[] = "RQ";
    char releaseCommand[] = "RL";
    char compactCommand[] = "C";
    char statCommand[] = "STAT";
    char quitCommand[] = "QUIT";

    // command cases, whatever is 1, apply corresponding command
    // e.g request = 1, RQ
    int request = 0;
    int release = 0;
    int compact = 0;
    int stat = 0;
    int quit = 0;

    // place holder for user input
    char inputStr[100];

    while (1)
    {
        // prompt and get the input
        printf("allocator>");
        fgets(inputStr, 100, stdin);
        // puts(inputStr);  // has a new line tailing

        // get rid of the tailing new line
        int inputStrLen = strlen(inputStr);
        if (inputStrLen > 0 && inputStr[inputStrLen - 1] == '\n')
        {
            inputStr[inputStrLen - 1] = '\0';
            inputStrLen--;
        }

        // for process name and its size
        char processName[10];
        int processSize = 0;

        // tokenize the inputStr
        char *token = strtok(inputStr, " ");
        int tokenNumber = 1;
        while (token != NULL)
        {
            if (tokenNumber == 1)
            {
                // Only one command will have 1
                if (strcmp(token, requestCommand) == 0)
                {
                    request = 1;
                    release = 0;
                    compact = 0;
                    stat = 0;
                    quit = 0;
                }
                else if (strcmp(token, releaseCommand) == 0)
                {
                    request = 0;
                    release = 1;
                    compact = 0;
                    stat = 0;
                    quit = 0;
                }
                else if (strcmp(token, compactCommand) == 0)
                {
                    request = 0;
                    release = 0;
                    compact = 1;
                    stat = 0;
                    quit = 0;
                }
                else if (strcmp(token, statCommand) == 0)
                {
                    request = 0;
                    release = 0;
                    compact = 0;
                    stat = 1;
                    quit = 0;
                }
                else if (strcmp(token, quitCommand) == 0)
                {
                    request = 0;
                    release = 0;
                    compact = 0;
                    stat = 0;
                    quit = 1;
                    return 0;
                }
                else
                {
                    request = 0;
                    release = 0;
                    compact = 0;
                    stat = 0;
                    quit = 0;
                    // do nothing, the user asked for a service that this software doesn't provide
                }
            }
            else if (tokenNumber == 2)
            {
                strcpy(processName, token);
            }
            else if (tokenNumber == 3)
            {
                processSize = atoi(token);
            }

            // get next token
            token = strtok(NULL, " ");
            tokenNumber++;
        }

        // execute one of four commands
        if (request == 1)
        {
            // init the list with whole free block of memory
            struct BlockNode *newBlock = (struct BlockNode *)malloc(sizeof(struct BlockNode));
            newBlock->start = 0;
            newBlock->end = 0;
            newBlock->status = 0; // if the request can be put in memory, status will be 1, in  allocate func
            newBlock->processSize = processSize;
            strcpy(newBlock->processName, processName);
            newBlock->next = NULL;

            // try to allocate it if there is enough space
            allocate(newBlock);
        }
        else if (release == 1)
        {
            releaseProcess(processName);
        }
        else if (compact == 1)
        {
            compaction();
        }
        else if (stat == 1)
        {

            showStat();
        }
        else if (quit == 1)
        {
            return 0; // quit
        }
    }

    return 0;
}