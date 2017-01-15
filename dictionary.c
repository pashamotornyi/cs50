/**
 * Implements a dictionary's functionality.
 */

#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <malloc.h>

#include "dictionary.h"

// maximum alphabet size ( a-z + ' )
#define ALPHABET_SIZE 27

// trie structure definition
typedef struct node
{
    bool is_word;
    struct node *children[ALPHABET_SIZE];
}
node;

// pointer to a trie beginning
node *root = NULL;

// dictionary words counter
unsigned int dictionary_word_count = 0;

/**
 * Returns alphabetical index of a char
 */
int char_index(char c)
{
    int index;
    // index of alphabetical character
    if (isalpha(c))
    {
        index = c - 'a';
    }
    // index of ' is 26
    else
    {
        index = 26;
    }
    
    return index;
}

/**
 * Returns new trie node initialized to NULLs
 */
node *node_init(void)
{
    // declaring new node and allocating memory
    node *new_node = NULL;
    new_node = malloc(sizeof(node));
    
    // setting is_word to false and every children to NULL
    if (new_node)
    {
        new_node->is_word = false;
        
        for (int i = 0; i < 27; i++)
        {
            new_node->children[i] = NULL;
        }
    }
    
    // return the node created
    return new_node;
}

/**
 * Puts a new word into a trie
 */
void trie_insert(node *root, const char *word)
{
    node *trie = root;
    int index;
    
    for (int i = 0, length = strlen(word); i < length; i++)
    {
        // getting alphabetical index of the char
        index = char_index(word[i]);
        
        // if there is no node with such index, create one
        if (!trie->children[index])
        {
            trie->children[index] = node_init();
        }
        
        // going to the next node
        trie = trie->children[index];
    }
    
    // setting is_word to true at the end of the word
    trie->is_word = true;
}

/**
 * Returns true if word is in dictionary else false.
 */
bool check(const char *word)
{
    node *trie = root;
    int index;
    
    for (int i = 0, length = strlen(word); i < length; i++)
    {
        // setting char to lower case and getting its alphabetical index
        index = char_index(tolower(word[i]));
        
        // if there is no node with such index, return false
        if (!trie->children[index])
        {
            return false;
        }
        
        // going to the next node
        trie = trie->children[index];
    }
    
    // if current node is not NULL and is_word return true, else false
    return (trie != NULL && trie->is_word);
}

/**
 * Loads dictionary into memory. Returns true if successful else false.
 */
bool load(const char *dictionary)
{
    // opening dictionary file 
    FILE *fp = fopen(dictionary, "r");
    if (fp == NULL)
    {
        printf("Could not open %s.\n", dictionary);
        unload();
        return false;
    }
    
    // if there is still no trie, initializing one
    if (!root)
    {
        root = node_init();
    }
    
    // array for reading the words from dictionary
    char word[LENGTH];
    
    // reading words one by one
    while (fscanf(fp, "%s", word) == 1) 
    {
        // insert word in a trie
        trie_insert(root, word);
        
        // increase words counter
        dictionary_word_count++;
    }
    
    return true;
}

/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */
unsigned int size(void)
{
    return dictionary_word_count;
}

/**
 * Recursive function for freing up trie node's memory
 */
bool free_node(node *root)
{
    node *trie = root;
    
    // loop for check every node's children
    for (int i = 0; i < ALPHABET_SIZE; i++)
    {
        // if child is not NULL, recursively call free_node function
        if (trie->children[i] != NULL)
        {
            return free_node(trie->children[i]);
        }
    }
    
    // free memory
    free(trie);
    
    return true;
}

/**
 * Unloads dictionary from memory. Returns true if successful else false.
 */
bool unload(void)
{
    return free_node(root);
}
