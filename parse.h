#include "keywords.h"
#include <ctype.h>
#define MAX_LINE_LENGTH 1024
struct ArrayList
{
   char (*list)[MAX_LINE_LENGTH];
   int count;
};

struct Action
{
   int code;
   struct ArrayList *subjectList;
   struct ArrayList *objectList;
   struct ArrayList *othersList;
};

struct Action *actionArr[MAX_LINE_LENGTH];
int action_count = 0;

bool parse(char **words, int word_count);
bool parseIf(char **words, int word_count);

void addAction(int code, struct ArrayList *subjectList, struct ArrayList *objectList, struct ArrayList *othersList)
{
   struct Action *res = (struct Action *)malloc(sizeof(struct Action));
   res->code = code;
   res->subjectList = subjectList;
   res->objectList = objectList;
   res->othersList = othersList;
   actionArr[action_count++] = res;
}

bool isSubject(char *str)
{
   if (getKeyWord(str) != -1)
      return false;
   while (*str != '\0')
   {
      if (!isalpha(*str) && *str != '_')
         return false;
      str++;
   }
   return true;
}

bool isSubjectSentence(char *str)
{
   int word_count = 0;
   char words[MAX_LINE_LENGTH][MAX_LINE_LENGTH];
   for (char *p = strtok(str, " "); p != NULL; p = strtok(NULL, " "))
   {
      strcpy(words[word_count++], p);
   }
   for (int i = 0; i < word_count; i = i + 2)
   {
      if (!isSubject(words[i]))
         return false;
      // Not connected with 'and'
      if (i + 1 < word_count && getKeyWord(words[i + 1]) != AND)
         return false;
   }
   return true;
}

bool isItem(char *count, char *name)
{
   if (getKeyWord(count) != -1)
      return false;
   if (getKeyWord(name) != -1)
      return false;
   // Is count digit?
   while (*count != '\0')
   {
      if (!isdigit(*count))
         return false;
      count++;
   }
   // Does name contain digits?
   while (*name != '\0')
   {
      if (!isalpha(*name) && *name != '_')
         return false;
      name++;
   }
   return true;
}

bool isItemSentence(char *str)
{
   int word_count = 0;
   char words[MAX_LINE_LENGTH][MAX_LINE_LENGTH];
   for (char *p = strtok(str, " "); p != NULL; p = strtok(NULL, " "))
   {
      strcpy(words[word_count++], p);
   }
   for (int i = 0; i < word_count; i = i + 3)
   {
      if (i % 3 == 2 && getKeyWord(words[i]) != AND)
      {
         // and mistake
         return false;
      }
      if (!isItem(words[i], words[i + 1]))
      {
         return false;
      }
   }
   return true;
}

bool isLocation(char *str)
{
   int word_count = 0;
   for (char *p = strtok(str, " "); p != NULL; p = strtok(NULL, " "))
   {
      word_count++;
   }
   // Location can onle be a single word.
   if (word_count > 1)
      return false;
   // It has same rules as subject.
   return isSubject(str);
}

void getSubjects(char **words, int start, int end, struct ArrayList *item_list)
{
   // Fills ArrayList with subjects
   // Ex : list = ["efe","hasan"], count = 2
   item_list->count = (end - start + 1) / 2;
   item_list->list = malloc(item_list->count * sizeof(char[MAX_LINE_LENGTH]));
   for (int i = start; i < end; i = i + 2)
   {
      strcpy(item_list->list[(i - start) / 2], words[i]);
   }
}

void getItems(char **words, int start, int end, struct ArrayList *item_list)
{
   // Fills ArrayList with items
   // Ex : list = ["12","bread","2","sword"], count = 4
   item_list->count = 2 * (end - start + 1) / 3;
   item_list->list = malloc(item_list->count * sizeof(char[MAX_LINE_LENGTH]));
   int index = 0;
   for (int i = start; i < end; i = i + 3)
   {
      strcpy(item_list->list[index++], words[i]);
      strcpy(item_list->list[index++], words[i + 1]);
   }
}

bool uniq_subjects(struct ArrayList *item_list)
{
   int size = item_list->count;
   for (int i = 0; i + 1 < size; i++)
   {
      for (int j = i + 1; j < size; j++)
      {
         if (strcmp(item_list->list[i], item_list->list[j]) == 0)
            return false;
      }
   }
   return true;
}

bool uniq_items(struct ArrayList *item_list)
{
   int size = item_list->count;
   for (int i = 1; i + 2 < size; i = i + 2)
   {
      for (int j = i + 2; j < size; j = j + 2)
      {
         if (strcmp(item_list->list[i], item_list->list[j]) == 0)
            return false;
      }
   }
   return true;
}

int look_for(int *index, char **words, int word_count, int mode, struct ArrayList *item_list)
{
   // Searches for SUBJECT, ITEM or LOCATION and stops when it sees a keyword other than 'and'
   int start = *index;
   switch (mode)
   {
   case SUBJECT:
      char subjectSentence[MAX_LINE_LENGTH] = "";
      while (*index < word_count)
      {
         // Add everyhing that is not a keyword. Excluding and.
         if (getKeyWord(words[*index]) != -1 && getKeyWord(words[*index]) != AND)
         {
            if (!isSubjectSentence(subjectSentence))
               return -1;
            getSubjects(words, start, *index, item_list);
            if (!uniq_subjects(item_list))
               return -1;
            return getKeyWord(words[*index]);
         }
         strcat(subjectSentence, " ");
         strcat(subjectSentence, words[(*index)++]);
      }
      if (!isSubjectSentence(subjectSentence))
         return -1;
      getSubjects(words, start, *index, item_list);
      if (!uniq_subjects(item_list))
         return -1;
      return END_OF_STRING;
   case ITEM:
      char itemSentence[MAX_LINE_LENGTH] = "";
      while (*index < word_count)
      {
         // Invalid Keyword
         if (getKeyWord(words[*index]) != -1 && getKeyWord(words[*index]) != AND)
         {
            getItems(words, start, *index, item_list);
            if (!uniq_items(item_list))
               return -1;
            return getKeyWord(words[*index]);
         }
         // Take 'and's everytime you can
         else if (*index + 1 < word_count && getKeyWord(words[*index]) == AND)
         {
            strcat(itemSentence, " ");
            strcat(itemSentence, words[*index]);
            *index += 1;
         }
         else if (*index + 1 < word_count && isItem(words[*index], words[*index + 1]))
         {
            strcat(itemSentence, " ");
            strcat(itemSentence, words[*index]);
            strcat(itemSentence, " ");
            strcat(itemSentence, words[*index + 1]);
            *index += 2;
         }
         else
         {
            // Sentence ending with 'and'
            if (*index - start == 0)
               return -1;
            *index -= 1;
            getItems(words, start, *index, item_list);
            if (!uniq_items(item_list))
               return -1;
            return NEXT_SEQUENCE;
         }
      }
      getItems(words, start, *index, item_list);
      if (!uniq_items(item_list))
         return -1;
      return END_OF_STRING;
   case FROM:
      struct ArrayList *dummySubjectList = malloc(sizeof(struct ArrayList));
      int dummy_index = *index;
      int end_code = look_for(&dummy_index, words, word_count, SUBJECT, dummySubjectList);
      // No subject to buy from
      if (dummySubjectList->count == 0)
         return -1;
      // Multiple sellers
      if (dummySubjectList->count > 1 && end_code == END_OF_STRING)
         return -1;
      free(dummySubjectList);
      *index += 1;
      getSubjects(words, start, *index, item_list);
      if (!uniq_subjects(item_list))
         return -1;
      if (*index == word_count)
         return END_OF_STRING;
      if (getKeyWord(words[*index]) == IF)
         return IF;
      return NEXT_SEQUENCE;
   case LOCATION:
      if (!isLocation(words[*index]))
         return -1;
      // Location list should only hold 1 object.
      item_list->list = malloc(strlen(words[*index]));
      item_list->count = 1;
      strcpy(item_list->list[0], words[*index]);
      if (*index == word_count - 1)
         return END_OF_STRING;
      if (getKeyWord(words[*index + 1]) == AND)
      {
         *index += 1;
         return NEXT_SEQUENCE;
      }
   }
}

char **tokenize(char *str, int *word_count)
{
   // Split sentence into words.
   char **words = malloc(MAX_LINE_LENGTH * sizeof(char *));
   for (char *p = strtok(str, " "); p != NULL; p = strtok(NULL, " "))
   {
      words[*word_count] = malloc(strlen(p));
      strcpy(words[(*word_count)++], p);
   }
   return words;
}
bool parseIf(char **words, int word_count)
{
   // Function to parse if conditions.
   int index = 0;
   struct ArrayList *subjectList = malloc(sizeof(struct ArrayList));
   int subject_code = look_for(&index, words, word_count, SUBJECT, subjectList);
   // Sentence without subject. INVALID
   if (subjectList->count == 0)
      return false;
   index++;
   switch (subject_code)
   {
   case HAS:
      // Ending with has. INVALID.
      if (index == word_count)
         return false;
      // Subject HAS [(MORE | LESS) THAN] ITEM(s)
      if (getKeyWord(words[index]) == MORE || getKeyWord(words[index]) == LESS)
      {
         int mode = getKeyWord(words[index]);
         // Sentence ending with more or less. INVALID.
         if (index == word_count - 1)
            return false;
         if (getKeyWord(words[index + 1]) == THAN)
         {
            struct ArrayList *ArrayList = malloc(sizeof(struct ArrayList));
            index += 2;
            int compare_code = look_for(&index, words, word_count, ITEM, ArrayList);
            // Error while checking
            if (compare_code == -1)
               return false;
            if (compare_code == END_OF_STRING)
            {
               if (mode == MORE)
                  addAction(MORE, subjectList, ArrayList, NULL);
               else
                  addAction(LESS, subjectList, ArrayList, NULL);
               return true;
            }
            if (compare_code == NEXT_SEQUENCE)
            {
               if (mode == MORE)
                  addAction(MORE, subjectList, ArrayList, NULL);
               else
                  addAction(LESS, subjectList, ArrayList, NULL);
               index++;
               struct ArrayList *dummyList = malloc(sizeof(struct ArrayList));
               int dummy_index = index;
               int search_code = look_for(&dummy_index, words, word_count, SUBJECT, dummyList);
               free(dummyList);
               // If conditions ahead.
               if (search_code == AT || search_code == HAS)
                  return parseIf(&words[index], word_count - index);
               // Regular parsing.
               return parse(&words[index], word_count - index);
            }
         }
      }
      else
      {
         // SUBJECT has ITEM(s)
         struct ArrayList *ArrayList = malloc(sizeof(struct ArrayList));
         int compare_code = look_for(&index, words, word_count, ITEM, ArrayList);
         // Invalid compare code
         if (compare_code == -1)
            return false;
         if (compare_code == END_OF_STRING)
         {
            addAction(HAS, subjectList, ArrayList, NULL);
            return true;
         }
         if (compare_code == NEXT_SEQUENCE)
         {
            addAction(HAS, subjectList, ArrayList, NULL);
            index++;
            struct ArrayList *dummyList = malloc(sizeof(struct ArrayList));
            int dummy_index = index;
            int search_code = look_for(&dummy_index, words, word_count, SUBJECT, dummyList);
            free(dummyList);
            if (search_code == AT || search_code == HAS)
               return parseIf(&words[index], word_count - index);
            return parse(&words[index], word_count - index);
         }
         return false;
      }
   case AT:
      // SUBJECT AT LOCATION
      // Sentence Ending with 'At'
      if (index == word_count)
         return false;
      struct ArrayList *locationList = malloc(sizeof(struct ArrayList));
      int location_code = look_for(&index, words, word_count, LOCATION, locationList);
      if (location_code == END_OF_STRING)
      {
         addAction(AT, subjectList, locationList, NULL);
         return true;
      }
      if (location_code == NEXT_SEQUENCE)
      {
         addAction(AT, subjectList, locationList, NULL);
         index++;
         struct ArrayList *dummyList = malloc(sizeof(struct ArrayList));
         int dummy_index = index;
         int search_code = look_for(&dummy_index, words, word_count, SUBJECT, dummyList);
         free(dummyList);
         if (search_code == AT || search_code == HAS)
            return parseIf(&words[index], word_count - index);
         return parse(&words[index], word_count - index);
      }
   default:
      // Unwanted keyword. INVALID.
      return false;
   }
}
bool parse(char **words, int word_count)
{
   int index = 0;
   struct ArrayList *subjectList = malloc(sizeof(struct ArrayList));
   int subject_code = look_for(&index, words, word_count, SUBJECT, subjectList);
   // Sentence without a subject. INVALID.
   if (subjectList->count == 0)
   {
      return false;
   }
   index++;
   switch (subject_code)
   {
   case END_OF_STRING:
      // Sentence consisting only by subject. INVALID.
      return false;
   case BUY:
      struct ArrayList *buyItemsList = malloc(sizeof(struct ArrayList));
      int buy_code = look_for(&index, words, word_count, ITEM, buyItemsList);
      // Nothing to buy
      if (buyItemsList->count == 0)
         return false;
      if (buy_code == FROM)
      {
         // SUBJECT(s) BUY ITEM(s) FROM SUBJECT
         index++;
         struct ArrayList *otherSubjectList = malloc(sizeof(struct ArrayList));
         int from_code = look_for(&index, words, word_count, FROM, otherSubjectList);
         if (from_code == -1)
            return false;
         if (from_code == NEXT_SEQUENCE)
         {
            addAction(BUY_FROM, subjectList, buyItemsList, otherSubjectList);
            index++;
            return parse(&words[index], word_count - index);
         }
         if (from_code == END_OF_STRING)
         {
            addAction(BUY_FROM, subjectList, buyItemsList, otherSubjectList);
            return true;
         }
         if (from_code == IF)
         {
            // SUBJECT(s) BUY ITEM(s) FROM SUBJECT IF CONDITIONS(s)
            index++;
            addAction(BUY_FROM, subjectList, buyItemsList, otherSubjectList);
            return parseIf(&words[index], word_count - index);
         }
      }
      // SUBJECT(s) BUY ITEM(s)
      else if (buy_code == NEXT_SEQUENCE)
      {
         addAction(BUY, subjectList, buyItemsList, NULL);
         index++;
         if (!parse(&words[index], word_count - index))
            return false;
         return true;
      }
      else if (buy_code == END_OF_STRING)
      {
         addAction(BUY, subjectList, buyItemsList, NULL);
         return true;
      }
      // SUBJECT(s) BUY ITEM(s) IF CONDITIONS(s)
      else if (buy_code == IF)
      {
         index++;
         addAction(BUY, subjectList, buyItemsList, NULL);
         return parseIf(&words[index], word_count - index);
      }
      else
         return false;
      break;
   case SELL:
      struct ArrayList *sellItemsList = malloc(sizeof(struct ArrayList));
      int sell_code = look_for(&index, words, word_count, ITEM, sellItemsList);
      // Nothing to sell
      if(sellItemsList->count == 0)
         return false;
      // SUBJECT(s) SELL ITEM(s) TO SUBJECT
      if (sell_code == TO)
      {
         index++;
         struct ArrayList *otherSubjectList = malloc(sizeof(struct ArrayList));
         // FROM and TO work very similarly so I call the same function.
         int to_code = look_for(&index, words, word_count, FROM, otherSubjectList);
         // INVALID return code
         if (to_code == -1)
            return false;
         index++;
         if (to_code == NEXT_SEQUENCE)
         {
            addAction(SELL_TO, subjectList, sellItemsList, otherSubjectList);
            return parse(&words[index], word_count - index);
         }
         if (to_code == END_OF_STRING)
         {
            addAction(SELL_TO, subjectList, sellItemsList, otherSubjectList);
            return true;
         }
         if (to_code == IF)
         {
            // SUBJECT(s) SELL ITEM(s) TO SUBJECT IF CONDITION(s)
            addAction(SELL_TO, subjectList, sellItemsList, otherSubjectList);
            return parseIf(&words[index], word_count - index);
         }
      }
      // SUBJECT(s) SELL ITEM(s) IF CONDITION(s)
      if (sell_code == IF)
      {
         addAction(SELL, subjectList, sellItemsList, NULL);
         index++;
         return parseIf(&words[index], word_count - index);
      }
      // SUBJECT(s) SELL ITEM(s)
      if (sell_code == NEXT_SEQUENCE)
      {
         addAction(SELL, subjectList, sellItemsList, NULL);
         index++;
         return parse(&words[index], word_count - index);
      }
      if (sell_code == END_OF_STRING)
      {
         // Sentence ending with sell
         if(sellItemsList->count == 0)
            return false; 
         addAction(SELL, subjectList, sellItemsList, NULL);
         return true;
      }
      return false;
      break;
   case GO:
      // GO without to or sentence ending with GO or GO TO
      if (index == word_count - 1 || index == word_count || strcmp(words[index], "to") != 0)
         return false;
      index++;
      struct ArrayList *locationList = malloc(sizeof(struct ArrayList));
      int go_code = look_for(&index, words, word_count, LOCATION, locationList);
      // INVALID return type
      if (go_code == -1)
         return false;
      // SUBJECT GO TO LOCATION
      if (go_code == END_OF_STRING)
      {
         addAction(GO, subjectList, locationList, NULL);
         return true;
      }
      if (go_code == NEXT_SEQUENCE)
      {
         addAction(GO, subjectList, locationList, NULL);
         index++;
         return parse(&words[index], word_count - index);
      }
      // SUBJECT GO TO LOCATION IF CONDITIONS(s)
      if (go_code == IF)
      {
         addAction(GO, subjectList, locationList, NULL);
         index += 2;
         return parseIf(&words[index], word_count - index);
      }
      else
         return false;
   // Unexpected keyword. INVALID
   default:
      return false;
   }
}

bool parseQuestion(char **words, int word_count)
{
   // Function to parse queries. I look for word counts.
   if (word_count == 3)
   {
      // SUBJECT WHERE ?
      if (getKeyWord(words[1]) == WHERE)
      {
         struct ArrayList *subjectList = malloc(sizeof(struct ArrayList));
         if (!isSubject(words[0]))
            return false;
         subjectList->list = malloc(sizeof(char[MAX_LINE_LENGTH]));
         strcpy(subjectList->list[0], words[0]);
         subjectList->count = 1;
         addAction(WHERE, subjectList, NULL, NULL);
         return true;
      }
      // SUBJECT TOTAL ?
      if (getKeyWord(words[1]) == TOTAL)
      {
         struct ArrayList *subjectList = malloc(sizeof(struct ArrayList));
         if (!isSubject(words[0]))
            return false;
         subjectList->list = malloc(sizeof(char[MAX_LINE_LENGTH]));
         strcpy(subjectList->list[0], words[0]);
         subjectList->count = 1;
         addAction(TOTAL, subjectList, NULL, NULL);
         return true;
      }
      return false;
   }
   else if (word_count == 4)
   {
      // SUBJECT TOTAL ITEM ?
      if (strcmp(words[1], "total") == 0)
      {
         struct ArrayList *subjectList = malloc(sizeof(struct ArrayList));
         int index = 0;
         int subject_code = look_for(&index, words, word_count, SUBJECT, subjectList);
         if (subject_code != TOTAL)
            return false;
         index++;
         // Subject count less than expected
         if (word_count - index != 2)
            return false;
         // Is asked Item really Item ? (I am using subject function because they work the same).
         if (!isSubject(words[index]))
            return false;
         struct ArrayList *itemList = malloc(sizeof(struct ArrayList));
         itemList->list = malloc(sizeof(char[MAX_LINE_LENGTH]));
         strcpy(itemList->list[0], words[index]);
         itemList->count = 1;
         addAction(TOTAL, subjectList, itemList, NULL);
         return true;
      }
      // WHO AT LOCATION ?
      struct ArrayList *locationList = malloc(sizeof(struct ArrayList));
      if (strcmp(words[0], "who") != 0 || strcmp(words[1], "at") != 0)
         return false;
      if (!isLocation(words[2]))
         return false;
      locationList->list = malloc(sizeof(char[MAX_LINE_LENGTH]));
      strcpy(locationList->list[0], words[2]);
      locationList->count = 1;
      addAction(WHO, locationList, NULL, NULL);
      return true;
   }
   else
   {
      // SUBJECTS TOTAL ITEM ?
      struct ArrayList *subjectList = malloc(sizeof(struct ArrayList));
      int index = 0;
      int subject_code = look_for(&index, words, word_count, SUBJECT, subjectList);
      if (subject_code != TOTAL)
         return false;
      index++;
      if (word_count - index != 2)
         return false;
      // Is asked really an ITEM?
      if (!isSubject(words[index]))
         return false;
      struct ArrayList *itemList = malloc(sizeof(struct ArrayList));
      itemList->list = malloc(sizeof(char[MAX_LINE_LENGTH]));
      strcpy(itemList->list[0], words[index]);
      itemList->count = 1;
      addAction(TOTAL, subjectList, itemList, NULL);
      return true;
   }
}