
const int MAX_LEN = 15;
const char lineEnding = '\n'; // whatever marks the end of your input.
char inputSentence [MAX_LEN + 1];
int inputIndex;
bool newInput;

const byte MAX_TOKENS = 2;
const char* delimiters = ", "; // whatever characters delimit your input string
char* tokens [MAX_TOKENS + 1];
enum indexName {command, argument};
#define PRINT_ITEM(x) printItem (x, #x)

void setup ()
{
  Serial.begin (9600);
}

void loop ()
{
 if (newInput && strlen (inputSentence))
 {
   int tokenIndex = 0;
   Serial.println (inputSentence); // tell 'em what you've got
   tokens [tokenIndex] = strtok (inputSentence, delimiters);
   while ((tokenIndex < MAX_TOKENS - 1) && tokens [tokenIndex])
   {
     tokenIndex++;
     tokens [tokenIndex] = strtok (NULL, delimiters);
   }
   PRINT_ITEM (command);
   PRINT_ITEM (argument);

   // reset things for the next lot.
   newInput = false;
   inputIndex = 0;
   inputSentence [0] = '\0';
 }
}

void serialEvent ()  // build the input string.
{
 while (Serial.available() )
 {
   char readChar = Serial.read ();
   if (readChar == lineEnding)
   {
     newInput = true;
   }
   else
   {
     if (inputIndex < MAX_LEN)
     {
       inputSentence [inputIndex++] = readChar;
       inputSentence [inputIndex] = '\0';
     }
   }
 }
}

void printItem (int index, char* name)
{
 Serial.print (name);
 Serial.print (F(" "));
 Serial.println (tokens [index]);  
}