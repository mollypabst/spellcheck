// libraries
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// definitions
#define LENGTH 45
#define HASH_SIZE 2000

// node structure
typedef struct node {
    char word[LENGTH+1];
    struct node* next;
} node;

// global variables
node* hashtable[HASH_SIZE];
int i;
int menuchoice;

// prototypes
int hash_function(const char* word);
bool check(const char* word);
bool load(const char* word);
unsigned int size(void);
bool unload(void);
int menu(void);
void replace(char *str, const char *oldWord, const char *newWord, int count);

// prints menu and returns user selection
int menu(void){
    printf("\nPlease Select a Menu Option:\n");
    printf("1: Spellcheck\t\t\t\t2: Search & Replace");
    printf("\n3: Save Modified File to Output File\t4: Exit Program\n");
    scanf("%d", &menuchoice);
    return menuchoice;
}

// places word in hash table based on remainder
int hash_function(const char* word){
    int sum = 0;
    int word_length = 0;
    word_length = strlen(word);
    for (i = 0; i < word_length; i++){
        sum += word[i];
    }
    
    int bucket = sum % HASH_SIZE;
    return bucket;
}

// returns true if word is in dictionary
bool check(const char* word) {
    int word_length = 0;
    word_length = strlen(word);
    char lower_word[LENGTH+1];
    
    // convert word to lowercase
    for (i = 0; i < word_length; i++){
        if(isupper(word[i])){
            lower_word[i] = tolower(word[i]);
        }
        else{
            lower_word[i] = word[i];
        }
    }
    
    lower_word[word_length] = '\0';
    // uses hash function to find correct "bucket" to place word in
    int bucket = hash_function(lower_word);
    // set cursor to first node
    node* cursor = hashtable[bucket];
    
    // compares each word in hash table
    while (cursor != NULL){
        if(strcmp(lower_word, cursor->word) == 0){
            return true;
        }
        cursor = cursor->next;
    }
    return false;
}

// loads dictionary into memory
bool load(const char* dictionary){
    // set hash table to NULL
    for(i = 0; i < HASH_SIZE; i++){
        hashtable[i] = NULL;
    }
    
    FILE* dict;
    dict = fopen(dictionary, "r");
    
    if(dict == NULL){
        printf("Failed to load dictionary");
        return false;
    }
    
    char buffer[LENGTH+1];
    
    // loop through each word in dictionary and save into buffer
    while((fscanf(dict, "%s", buffer) > 0)){
        // allocate memory for node
        node* new_node = malloc(sizeof(node));
        // set node to whats stored in buffer
        new_node->next = NULL;
        strcpy(new_node->word, buffer);
        int bucket = hash_function(new_node->word);
        if(hashtable[bucket] == NULL){
            hashtable[bucket] = new_node;
        }
        else{
            new_node->next = hashtable[bucket];
            hashtable[bucket] = new_node;
        }
    }
    fclose(dict);
    return true;
}

// unloads dictionary
bool unload(void){
    for(i = 0; i < HASH_SIZE; i++){
        node* cursor = hashtable[i];
        while(cursor != NULL){
            node* temp = cursor;
            cursor = cursor->next;
            free(temp);
        }
    }
    return true;
}

// Replaces all occurences of a given word
void replace(char *str, const char *oldWord, const char *newWord, int count){
    char *linepos, temp[1000];
    int index = 0;
    int length;
    
    length = strlen(oldWord);
    
    // replace
    while(((linepos = strstr(str, oldWord)) != NULL)){
        strcpy(temp, str);
        index = linepos - str;
        printf("%s found on line %d at position %d\n", oldWord, count,  index);
        str[index] = '\0';
        strcat(str, newWord);
        strcat(str, temp+index+length);
    }
}

int main(int argc, char*argv[]){
    char* dictionary = argv[3];
    FILE* fp4 = fopen(dictionary, "a");
    char* text = argv[1];
    FILE* fp = fopen(text, "r");
    char* output = argv[2];
    FILE* fp2 = fopen(output, "w");
    FILE* fp3;
    int count = 0;
    
    while(count == 0){
        menuchoice = menu();
        
        // spell check
        if(menuchoice == 1){
        
            load(dictionary);
    
            if (fp == NULL){
                printf("Could not open %s.\n", text);
                exit(1);
            }

            int index = 0, misspellings = 0, words = 0;
            char word[LENGTH+1];
            int c;
            char input;
            int cnt = 0;
        
            for(c = fgetc(fp); c!=EOF; c = fgetc(fp)){
                // is a character
                if (isalpha(c) || (c == '\''&& index > 0)){
                    word[index] = c;
                    index++;
            
                    if (index > LENGTH){
                        while ((c = fgetc(fp)) != EOF && isalpha(c));
                        index = 0;
                    }
                }
                // not a digit
                else if (isdigit(c)){
                    while ((c = fgetc(fp)) != EOF && isalnum(c));
                    index = 0;
                }
                
                // spellcheck word
                else if (index > 0){
                    word[index] = '\0';
                    bool misspelled = !check(word);
                    if (misspelled){
                        printf("%s is not found in the dictionary. ", word);
                        misspellings++;
                        printf("Would you like to replace the word? (Y/N): ");
                        scanf(" %c", &input);
                        if(input == 'Y'){
                            printf("Please enter a word you would like to replace with: ");
                            scanf(" %s", word);
                        }
                        if(input == 'N'){
                            if(cnt == 0){
                                char personal_dictionary[LENGTH+1];
                                printf("Please enter a name (single word) for a personal dictionary: ");
                                scanf(" %s.txt", personal_dictionary);
                                fp3 = fopen(personal_dictionary, "w");
                                cnt++;
                            }
                            fprintf(fp3, "%s\n", word);
                            fprintf(fp4, "%s\r\n", word);
                            load(dictionary);
                        }
                    }
                    fprintf(fp2, "%s", word);
                    index = 0;
                }
                if(c == ' '){
                    fprintf(fp2, " ");
                }
                if( c == '\n'){
                    fprintf(fp2, "\n");
                }
                if(c== '\''){
                    fprintf(fp2, "\'");
                }
                if(c == ','){
                    fprintf(fp2,",");
                }
                if(c == '!'){
                    fprintf(fp2, "!");
                }
                if(c == ':'){
                    fprintf(fp2, ":");
                }
                if(c == '?'){
                    fprintf(fp2, "?");
                }
        
            }
        
        }
    
        // search and replace
        if(menuchoice == 2){
            char buffer[1000];
            char oldWord[100], newWord[100];
            int count = 1;
        
            if(fp == NULL){
                printf("Unable to open file.");
                exit(1);
            }
        
            printf("Enter a word you would like to search for: ");
            scanf("%s", oldWord);
            printf("\nWhat would you like to replace '%s' with?: ", oldWord);
            scanf("%s", newWord);
        
            // gets word and replaces
            while ((fgets(buffer, 1000, fp))!=NULL){
                replace(buffer, oldWord, newWord, count);
                fputs(buffer, fp2);
                count++;
            }
        
        }
        // save file
        if(menuchoice == 3){
            printf("Modified file saved to output file.\n");
        }
        // exit program
        if(menuchoice == 4){
            exit(1);
        }
    }
    // close and unload memory
    fclose(fp);
    unload();
    fclose(fp2);
    fclose(fp3);
    fclose(fp4);
    return 0;
    }

