#include <string.h>
enum
{
   SUBJECT,
   ITEM,
   LOCATION,
   BUY,
   SELL,
   GO,
   END_OF_STRING,
   NEXT_SEQUENCE,
   FROM,
   TO,
   BUY_FROM,
   SELL_TO,
   AT,
   HAS,
   IF,
   LESS,
   MORE,
   THAN,
   EXIT,
   WHERE,
   TOTAL,
   WHO,
   NOBODY,
   NOTHING,
   NOWHERE,
   AND,
   INVENTORY,
};

int getKeyWord(char *str)
{
   if (strcmp(str, "sell") == 0)
      return SELL;
   if (strcmp(str, "buy") == 0)
      return BUY;
   if (strcmp(str, "go") == 0)
      return GO;
   if (strcmp(str, "to") == 0)
      return TO;
   if (strcmp(str, "from") == 0)
      return FROM;
   if (strcmp(str, "and") == 0)
      return AND;
   if (strcmp(str, "at") == 0)
      return AT;
   if (strcmp(str, "has") == 0)
      return HAS;
   if (strcmp(str, "if") == 0)
      return IF;
   if (strcmp(str, "less") == 0)
      return LESS;
   if (strcmp(str, "more") == 0)
      return MORE;
   if (strcmp(str, "than") == 0)
      return THAN;
   if (strcmp(str, "exit") == 0)
      return EXIT;
   if (strcmp(str, "where") == 0)
      return WHERE;
   if (strcmp(str, "total") == 0)
      return TOTAL;
   if (strcmp(str, "who") == 0)
      return WHO;
   if (strcmp(str, "NOBODY") == 0)
      return NOBODY;
   if (strcmp(str, "NOTHING") == 0)
      return NOTHING;
   if (strcmp(str, "NOWHERE") == 0)
      return NOWHERE;
   // Invalid keyword
   return -1;
}