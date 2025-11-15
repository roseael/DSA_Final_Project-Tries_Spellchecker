#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
using namespace std;

const int Alphabet_Size = 26;

struct TrieNode {
    TrieNode* children[Alphabet_Size];

    bool isEndOfWord;

    TrieNode() : isEndOfWord(false) {
        for(int i=0; i<Alphabet_Size; ++i) {
            children[i] = nullptr;
        }
    }
    ~TrieNode(){
        for(int i=0; i<Alphabet_Size; ++i){
            delete children[i];
        }
    }
};

void insert(TrieNode* root, const string& word) {
    TrieNode* current = root;
    for (char c : word) {
        int index = charToIndex(c);
        if (current->children[index] == nullptr) {
            current->children[index] = createNode();
        }
        current = current->children[index];
    }
    current->isEndOfWord = true;
}
