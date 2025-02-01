#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "parse.h"
//item location and person structs
struct Item {
    int quantity;
    char *name;
};

struct Location {
    struct Person *people;
    char *name;
    int personNum;
};

struct Person {
    char *name;
    struct Location *location;
    struct Item *items;
    int itemNum;
    bool locdefined;
};
//function prototypes
int isLocationExist(char name[]);
int isItemExist(struct Person person, char itemName[]);
int isPersonExist(char name[]);
//global variables
int initialSize = 100;
struct Person *people;
struct Location *locations;
int personSize = 100;
int locationSize = 100;
int itemSize = 100;
int personNum = 0;
int locationNum = 0;

void personMaker(struct Person *person) {
    person->itemNum = 0;
    person->items = malloc(initialSize * sizeof(struct Item));
}

void locationMaker(struct Location *location) {
   location->people = malloc(initialSize * sizeof(struct Person));
   location->personNum = 0;
}

/*
 * Adds a new person to the people array.
 * If the number of people exceeds the current capacity, the array is resized.
 * Initializes the new person with default values.
 */

void addPerson(char name[]) {
    if (personNum >= personSize) {
        personSize *= 2;
        people = realloc(people, personSize * sizeof(struct Person));
    }
    struct Person new;
    personMaker(&new);
    new.locdefined = false;
    new.name = strdup(name);
    new.items = malloc(itemSize*sizeof(struct Item));
    people[personNum++] = new;
}
/*
 * Adds a new location to the locations array.
 * If the number of locations exceeds the current capacity, the array is resized.
 * Initializes the new location with default values.
 */

void addLocation(char name2[]) {
    if (locationNum >= locationSize) {
        locationSize *= 2;
        locations = realloc(locations, locationSize * sizeof(struct Location));
    }
    struct Location new;
    locationMaker(&new);
    new.name = strdup(name2);
    locations[locationNum++] = new;
}
/*
 * Moves a person to a specified location.
 * If the person does not exist in the people array, adds the person.
 * If the location does not exist in the locations array, adds the location.
 * Checks if the person is already at the specified location.
 * Updates the person's location and the location's people array accordingly.
 */

void goto_location(char person[], char location[]) {
    int personIndex = isPersonExist(person);
    int locIndex = isLocationExist(location);
    if (personIndex == personNum) {
        addPerson(person);
    }

    if (locIndex == locationNum) {
        addLocation(location);
    }
    if(people[personIndex].locdefined){
        if(strcmp(people[personIndex].location->name,locations[locIndex].name) == 0){
             return;
             }

    }
    people[personIndex].locdefined = true;
    people[personIndex].location = &locations[locIndex];
    locations[locIndex].people[locations[locIndex].personNum++] = people[personIndex];
}
/*
 * Checks if an item exists in a person's inventory.
 * Returns the index of the item in the person's items array if found and its quantity is nonzero.
 * Otherwise, returns the total number of items in the person's inventory.
 */
int isItemExist(struct Person person, char itemName[]) {
    for (int i = 0; i < person.itemNum; i++) {
        if (strcmp(person.items[i].name, itemName) == 0 && person.items[i].quantity != 0) {
            return i;
        }
    }
    return person.itemNum;
}
/*
 * Checks if a person exists in the people array.
 * Returns the index of the person if found.
 * Otherwise, returns the total number of persons in the array.
 */
int isPersonExist(char name[]) {
    for (int i = 0; i < personNum; i++) {
        if (strcmp(name, people[i].name) == 0) {
            return i;
        }
    }
    return personNum;
}
/*
 * Checks if a person has a specific quantity of an item in their inventory.
 * Returns true if the person exists and has exactly the specified quantity of the item.
 * Returns false otherwise, including cases where the person does not exist, the item is not found,
 * or the quantity of the item in the person's inventory does not match the specified quantity.
 */

bool has(char person[],char itemName[],int quantity){
    int personIndex = isPersonExist(person);
    if(personIndex == personNum && quantity == 0)return true;
    if(personIndex == personNum)return false;
    int itemIndex = isItemExist(people[personIndex],itemName);
    if(itemIndex == people[personIndex].itemNum && quantity == 0) return true;
    if(itemIndex == people[personIndex].itemNum) return false;
    if(people[personIndex].items[itemIndex].quantity != quantity)return false;
    return true;
}
/*
 * Checks if a person has less than a specified quantity of an item in their inventory.
 * Returns true if the person does not exist and the specified quantity is greater than 0.
 * Returns false if the person does not exist or the item is not found in their inventory.
 * Returns true if the item is found but its quantity is less than the specified quantity.
 * Returns false otherwise.
 */

bool hasless(char person[],char itemName[],int quantity){
    int personIndex = isPersonExist(person);
    int itemIndex = isItemExist(people[personIndex],itemName);
    if(personIndex == personNum && quantity > 0) return true;
    if(personIndex == personNum) return false; 
    if(itemIndex == people[personIndex].itemNum) return true;
    if(people[personIndex].items[itemIndex].quantity >= quantity)return false;
    return true;
}
/*
 * Checks if a person has more than a specified quantity of an item in their inventory.
 * Returns false if the person does not exist or the item is not found in their inventory.
 * Returns false if the item is found but its quantity is less than or equal to the specified quantity.
 * Returns true if the item is found and its quantity is greater than the specified quantity.
 */

bool hasmore(char person[],char itemName[],int quantity){
    int personIndex = isPersonExist(person);
    int itemIndex = isItemExist(people[personIndex],itemName);
    if(personIndex == personNum) return false;
    if(itemIndex == people[personIndex].itemNum) return false;
    if(people[personIndex].items[itemIndex].quantity <= quantity)return false;
    return true;
}
/*
 * Checks if a person is at a specified location.
 * Returns false if the person does not exist or if their location is not defined.
 * Returns false if the person's location does not match the specified location.
 * Returns true if the person exists and is at the specified location.
 */

bool isPersonatLocation(char person[],char location[]){
    int personIndex = isPersonExist(person);
    if(personNum == personIndex)return false;
    if(people[personNum].location == NULL) return false;
    if(strcmp(people[personNum].location->name,location)!= 0) return false;
    return true;
}
/*
 * Prints the names of people who are at a specified location.
 * If the location does not exist, prints "NOBODY".
 * If there are no people at the location, prints "NOBODY".
 * Otherwise, prints the names of people at the location separated by "and",
 * and ends with a newline character.
 */

void whoisThere(char location[]){
    int locIndex = isLocationExist(location);
    if(locIndex == locationNum){
        printf("NOBODY\n");
        return;
    }
    struct Location temp = locations[locIndex];
    if(temp.personNum == 0){
        printf("NOBODY\n");
        return;
    }
    bool printedYet = true;
    for(int i = 0; i < temp.personNum -1;i++){
        if(strcmp(people[i].location->name,temp.name) == 0){
            if(!printedYet){
                printf(" ");
            }
            printedYet = false;
            printf("%s and",temp.people[i].name);
        }
    }
    bool lastCheck = true;
    if(strcmp(people[temp.personNum -1].location->name,temp.name) == 0){
        if(!printedYet){
                printf(" ");
            }
        lastCheck = false;
        printedYet = false;
        printf("%s\n",temp.people[temp.personNum -1].name);
    }
    if(lastCheck){
        if(printedYet){
            printf("NOBODY\n");
        }else{
            printf("\n");
        }
    }    
}

/*
 * Checks if a location exists in the locations array.
 * Returns the index of the location if found.
 * Otherwise, returns the total number of locations in the array.
 */

int isLocationExist(char name[]) {
    for (int i = 0; i < locationNum; i++) {
        if (strcmp(name, locations[i].name) == 0) {
            return i;
        }
    }
    return locationNum;
}
/*
 * Prints the location of a specified person.
 * If the person does not exist, prints "NOWHERE".
 * If the person's location is not defined, prints "NOWHERE".
 * Otherwise, prints the name of the location where the person is currently located.
 */

void where(char person[]) {
    int personIndex = isPersonExist(person);
    if (personIndex == personNum) {
        printf("NOWHERE\n");
        return;
    }

    if (personIndex < 0 || personIndex >= personNum) {
        printf("Invalid personIndex: %d\n", personIndex);
        return;
    }
    if (!people[personIndex].locdefined) {
        printf("NOWHERE\n");
        return;
    }

    printf("%s\n", people[personIndex].location->name);
}
/*
 * Checks if a person is at a specified location.
 * Returns false if the person does not exist or if their location is not defined.
 * Returns true if the person is at the specified location.
 */

bool locCheck(char person[],char location[]){
    int personIndex = isPersonExist(person);
    if(personIndex == personNum)return false;
    if(people[personIndex].location == NULL)return false;
    return (strcmp(people[personIndex].location->name,location) == 0);
    
}
/*
 * Adds the quantity of a specified item from a person's inventory to a current total.
 * If the person does not exist or the item is not found in their inventory, does nothing.
 * Otherwise, adds the quantity of the item to the current total.
 */

void adder(int *current, char *person,char itemName[]){
    int personThere = isPersonExist(person);
    if(personThere == personNum) return;
    int itemIndex = isItemExist(people[personThere],itemName);
    if(itemIndex == people[personThere].itemNum) return;
    *current += people[personThere].items[itemIndex].quantity;
}
/*
 * Calculates the total quantity of a specified item across multiple persons' inventories.
 * Iterates through an array of person names, adding the quantity of the specified item from each person's inventory to a total.
 * Prints the total quantity of the item.
 */

void total(char persons[][MAX_LINE_LENGTH], int size, char itemName[]) {
    int total = 0;
    for(int i = 0; i < size; i++) {
        adder(&total, persons[i], itemName);
    }
    printf("%d\n",total);
    return;
}
/*
 * Prints the inventory of a specified person.
 * If the person does not exist, prints "NOTHING".
 * If the person's inventory is empty, prints "NOTHING".
 * Otherwise, prints the quantities and names of items in the person's inventory, separated by "and".
 */

void envantoryAsk(char person[]) {
    int personIndex = isPersonExist(person);
    if (personIndex == personNum) {
        printf("NOTHING\n");
        return;
    }
    struct Person temp = people[personIndex];
    if (temp.itemNum == 0) {
        printf("NOTHING\n");
        return;
    }
    bool printedYet = false;
    for (int i = 0; i < temp.itemNum; i++) {
        if (temp.items[i].quantity != 0) {
            if (printedYet) {
                printf(" and ");
            }
            printf("%d %s", temp.items[i].quantity, temp.items[i].name);
            printedYet = true;
        }
    }
    if (!printedYet) {
        printf("NOTHING\n");
    } else {
        printf("\n");
    }
}
/*
 * Buys a specified quantity of an item for a person.
 * If the person does not exist, creates a new person with the specified name and adds the item to their inventory.
 * If the person exists but does not have the specified item, adds the item to their inventory.
 * If the person exists and already has the specified item, increases the quantity of the item in their inventory.
 */

void buy(char name[], char itemName[],int quantity){
    int personIndex = isPersonExist(name);
    if(personIndex == personNum){
        addPerson(name);
        struct Item item;
        item.name = itemName;
        item.quantity = quantity;
        people[personIndex].itemNum++;
        people[personIndex].items[0] = item;
        return;
    }
    int itemIndex = isItemExist(people[personIndex],itemName);
    if(itemIndex == people[personIndex].itemNum){
    struct Item item;
    item.name = itemName;
    item.quantity = quantity;
    people[personIndex].items[itemIndex] = item;
    people[personIndex].itemNum++;
    }
    else{
        people[personIndex].items[itemIndex].quantity+= quantity;
    }

}
/*
 * Checks if it is possible to sell a specified quantity of an item for a person.
 * Returns true if the sale is possible, false otherwise.
 * If the person does not exist, or if the person does not have the specified item, or if the quantity of the item is less than the specified quantity,
 * the function returns false.
 * Otherwise, it returns true.
 */

bool sellcheck(char name[], char itemName[],int quantity){
    int personIndex = isPersonExist(name);
    if(personIndex == personNum) {
        return false;}
    int itemIndex = isItemExist(people[personIndex],itemName);
    if(itemIndex == people[personIndex].itemNum)  {
        return false;}
    if(people[personIndex].items[itemIndex].quantity < quantity)  {
        return false;}
    return true;
}
/*
 * Sells a specified quantity of an item for a person.
 * Returns true if the sale is successful, false otherwise.
 * If the person does not exist, or if the person does not 
 * have the specified item, or if the quantity of the item is less than the specified quantity,
 * the function returns false.
 * Otherwise, it decreases the quantity of the item in the person's inventory by the specified quantity and returns true.
 */

bool sell(char name[],char itemName[],int quantity){
    int personIndex = isPersonExist(name);
    if(personIndex == personNum) {
        return false;}
    int itemIndex = isItemExist(people[personIndex],itemName);
    if(itemIndex == people[personIndex].itemNum)  {
        return false;}
    if(people[personIndex].items[itemIndex].quantity < quantity)  {
        return false;}
    people[personIndex].items[itemIndex].quantity -= quantity;
    return true;
}
/*
 * Performs various actions based on the provided action code and corresponding lists of subjects, objects, and others.
 * Parameters:
 * - action: An integer representing the action to be performed.
 * - subjectList: A pointer to an ArrayList containing the subjects involved in the action.
 * - objectList: A pointer to an ArrayList containing the objects involved in the action.
 * - othersList: A pointer to an ArrayList containing additional objects involved in the action.
 * Action Codes:
 * - INVENTORY: Prints the current location of the subject.
 * - BUY: Buys items for the subjects from the objects list.
 * - SELL: Sells items for the subjects from the objects list.
 * - BUY_FROM: Buys items for the subjects from the objects list and sells them to others from the others list.
 * - WHO: Prints the people present at the current location.
 * - SELL_TO: Sells items from the subjects to the objects and buys items for others.
 * - GO: Moves the subjects to the specified location.
 * - TOTAL: Prints the total quantity of a specified item across all subjects.
 * - WHERE: Prints the current location of the subject.
 */

void action(int action,struct ArrayList *subjectList,struct ArrayList *objectList,struct ArrayList *othersList){
   switch (action)
   {
   case INVENTORY:
        where(subjectList[0].list[0]);
        break;
   case BUY:
      for(int i = 0; i < subjectList->count;i++){
        for(int j = 0; j < objectList->count; j = j+2){
            buy(subjectList[0].list[i],objectList->list[j+1],atoi(objectList->list[j]));        
            }
      }
      break;
    case SELL:
        bool stillGo = true;
        for(int i = 0; i < subjectList->count;i++){
            for(int j = 0; j < objectList->count; j = j+2){
            stillGo = stillGo && sellcheck(subjectList[0].list[i],objectList->list[j+1],atoi(objectList->list[j]));        
            }
        }
        if(stillGo){
            for(int i = 0; i < subjectList->count;i++){
                for(int j = 0; j < objectList->count; j = j+2){
                    sell(subjectList[0].list[i],objectList->list[j+1],atoi(objectList->list[j]));        
                }
            }
        }
        break;
   case BUY_FROM:
        bool check = true;
        for(int k = 0; k < objectList->count; k = k+2){
            int totalNum = atoi(objectList->list[k]) * subjectList->count;
            check = check && sellcheck(othersList[0].list[0],objectList->list[k+1],totalNum);
        }
        if(check){
            for(int i = 0; i < subjectList->count;i++){
                for(int j = 0; j < objectList->count; j = j+2){
                    buy(subjectList[0].list[i],objectList->list[j+1],atoi(objectList->list[j]));        
                }
            }
            for(int i = 0; i < subjectList->count;i++){
                for(int j = 0; j < objectList->count; j = j+2){
                    sell(othersList[0].list[0],objectList->list[j+1],atoi(objectList->list[j]));        
                }
            }
        }
      break;
   case WHO:
    whoisThere(subjectList[0].list[0]);
    break;
   case SELL_TO:
      bool stillgo = true;
      for(int i = 0; i < subjectList->count;i++){
            for(int j = 0; j < objectList->count; j = j+2){
                stillgo = stillgo && sellcheck(subjectList[0].list[i],objectList->list[j+1],atoi(objectList->list[j]));
            }
        }
        if(stillgo){
            for(int i = 0; i < subjectList->count;i++){
                for(int j = 0; j < objectList->count; j = j+2){
                    sell(subjectList[0].list[i],objectList->list[j+1],atoi(objectList->list[j]));
                    buy(othersList[0].list[0],objectList->list[j+1],atoi(objectList->list[j]));        
                }
            }
            }

      break;
   case GO:
      for(int i = 0; i < subjectList[0].count;i++){
        goto_location(subjectList[0].list[i],objectList[0].list[0]);
      }
      break;
    case TOTAL:
        if(objectList == NULL){
            envantoryAsk(subjectList[0].list[0]);
            }
        else{
            total(subjectList->list,subjectList->count,objectList->list[0]);
        }
        break;
    case WHERE:
        where(subjectList[0].list[0]);
        break;
  }

}
/*
 * Checks various conditions based on the provided question code and corresponding lists of subjects and objects.
 * Parameters:
 * - question: An integer representing the question to be checked.
 * - subjectlist: A pointer to an ArrayList containing the subjects involved in the question.
 * - objectlist: A pointer to an ArrayList containing the objects involved in the question.
 * - otherlist: A pointer to an ArrayList containing additional objects involved in the question.
 * Question Codes:
 * - AT: Checks if the subjects are at the specified location (objectlist[0]).
 * - HAS: Checks if the subjects have the specified quantity of the specified item from the objectlist.
 * - MORE: Checks if the subjects have more than the specified quantity of the specified item from the objectlist.
 * - LESS: Checks if the subjects have less than the specified quantity of the specified item from the objectlist.
 * Returns:
 * - true if the condition holds true for all subjects, otherwise false.
 */

bool ifCheck(int question, struct ArrayList *subjectlist, struct ArrayList *objectlist, struct ArrayList *otherlist) {
    switch (question) {
        case AT:
            bool retboolean = true;
            for(int i = 0; i < subjectlist->count; i++) {
                retboolean = retboolean && locCheck(subjectlist[0].list[i], objectlist[0].list[0]);
            }
            return retboolean;
            break;

        case HAS:
            bool retbool = true;
            for(int i = 0; i < subjectlist->count; i++) {
                for(int j = 0; j < objectlist->count; j = j+2) {
                    retbool = retbool && has(subjectlist[0].list[i], objectlist->list[j+1], atoi(objectlist->list[j]));

                }
            }
            return retbool;
            break;

        case MORE:
            bool retbool2 = true;
            for(int i = 0; i < subjectlist->count; i++) {
                for(int j = 0; j < objectlist->count; j = j+2) {
                    retbool2 = retbool2 && hasmore(subjectlist[0].list[i], objectlist->list[j+1], atoi(objectlist->list[j]));
                }
            }
            return retbool2;
            break;

        case LESS:
            for(int i = 0; i < subjectlist->count; i++) {
                bool retbool3 = true;
                for(int j = 0; j < objectlist->count; j = j+2) {
                    retbool3 = retbool3 && hasless(subjectlist[0].list[i], objectlist->list[j+1], atoi(objectlist->list[j]));
                }
                return retbool3;
                break;
            }
    }
}

bool isInArray(int num, int array[], int size) {
    for (int i = 0; i < size; i++) {
        if (array[i] == num) {
            return true;
        }
    }
    return false; 
}
/*
 * Checks if an action code at a given index in an array of Action structures
 * matches one of the predefined action codes.
 * Parameters:
 * - arr: An array of pointers to Action structures.
 * - index: The index of the Action structure to check.
 * Returns:
 * - true if the action code matches one of the predefined action codes, otherwise false.
 */

bool isAction(struct Action *arr[],int index){
    int actionCodes[] = {BUY,SELL,GO,BUY_FROM,SELL_TO,WHERE,TOTAL,WHO};
    return isInArray(arr[index]->code,actionCodes,8);
}
/*
 * Checks if an action code at a given index in an array of Action structures
 * matches one of the predefined if statement codes.
 * Parameters:
 * - arr: An array of pointers to Action structures.
 * - index: The index of the Action structure to check.
 * Returns:
 * - true if the action code matches one of the predefined if statement codes, otherwise false.
 */

bool isIf(struct Action *arr[],int index){
    int ifCodes[] ={LESS,MORE,HAS,AT};
    return isInArray(arr[index]->code,ifCodes,4);
}
/*
 * Traverses an array of Action structures and identifies sequences of actions and associated if statements.
 * Parameters:
 * - arr: An array of pointers to Action structures.
 * - size: The size of the array.
 * - totalnum: A pointer to an integer variable to store the total number of identified sequences.
 * Returns:
 * - A dynamically allocated 2D array where each row represents a sequence of actions and associated if statements.
 *   Each row contains three integers: the index where the action sequence starts, the index where the if statement(s) start,
 *   and the index where the if statement(s) end.
 *   Memory must be freed by the caller after use.
 */

int** traverseActionArr(struct Action *arr[], int size, int *totalnum) {
    int** result = (int**)malloc(size * sizeof(int*));
    int resultIndex = 0;
    int i = 0;    
    while (i < size) {
        int actionstart = i;
        while (arr[i] != NULL && isAction(arr,i))
            i++;
        int ifStart = i;
        while (arr[i] != NULL && isIf(arr,i))
            i++;
        int ifEnd = i - 1;
        
        result[resultIndex] = (int*)malloc(3 * sizeof(int));
        result[resultIndex][0] = actionstart;
        result[resultIndex][1] = ifStart;
        if(ifStart > ifEnd) ifEnd = ifStart;
        result[resultIndex][2] = ifEnd;
        
        resultIndex++;
    }
    
    *totalnum = resultIndex;
    return result;
}

int main() {
    people = malloc(personSize * sizeof(struct Person));
    locations = malloc(locationSize * sizeof(struct Location));
    while(true){
        char input[1024];
        printf(">> ");
        fflush(stdout);
        fgets(input,sizeof(input),stdin);
        action_count = 0;
        int len = strlen(input);
        // Do not get the newline.
        input[len - 1] = '\0';
        int word_count = 0;
        char **words = tokenize(input,&word_count);
        if(word_count == 0){
            printf("INVALID\n");
            continue;
        }
        if(strcmp(words[0],"exit") == 0 && word_count == 1)
            break;
        bool result;
        if(strcmp(words[word_count - 1],"?") == 0){
            result = parseQuestion(words,word_count);
        }else{
            result = parse(words,word_count);
        } 
        if(result){
            if(strcmp(words[word_count - 1],"?") != 0)printf("OK\n");
            int actionCodes[] = {BUY,SELL,GO,BUY_FROM,SELL_TO,WHERE,TOTAL,WHO};
            int ifCodes[] ={LESS,MORE,HAS,AT};
            int sentenceNum;
            int **sentenceIndexes = traverseActionArr(actionArr,action_count,&sentenceNum);
            for(int i = 0; i < sentenceNum; i++){
                bool ifExist = false;
                if(sentenceIndexes[i][1] < action_count) { ifExist = isInArray(actionArr[sentenceIndexes[i][1]]->code,ifCodes,4);}
                if(ifExist){
                    bool doAct = true;
                    for(int j = sentenceIndexes[i][1]; j <= sentenceIndexes[i][2]; j++){
                        doAct = doAct && ifCheck(actionArr[j]->code,actionArr[j]->subjectList,actionArr[j]->objectList,actionArr[j]->othersList);
                    }
                    if(doAct){
                        for(int k = sentenceIndexes[i][0]; k < sentenceIndexes[i][1]; k++ ){
                            action(actionArr[k]->code,actionArr[k]->subjectList,actionArr[k]->objectList,actionArr[k]->othersList);

                        }

                    }
                }else{
                        for(int k = sentenceIndexes[i][0]; k < sentenceIndexes[i][1]; k++ ){
                            action(actionArr[k]->code,actionArr[k]->subjectList,actionArr[k]->objectList,actionArr[k]->othersList);

                        }
                }
            }
        }else{
            printf("INVALID\n");
        }
        free(words);
        for(int i = 0;i < action_count;i++){
            actionArr[i] = NULL;
        }
    }
    return 0;
}