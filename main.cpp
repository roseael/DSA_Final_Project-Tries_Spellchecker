#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
using namespace std;

const int Alphabet_Size = 26;
const int MAX_EDIT_DISTANCE = 2;
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

int charToIndex(char c) {
    return tolower(c) - 'a';
}

TrieNode* createNode() {
    return new TrieNode();
}

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

bool search(TrieNode* root, const string& word) {
    TrieNode* current = root;
    for (char c : word) {
        int index = charToIndex(c);
        if (index < 0 || index >= Alphabet_Size) return false;

        if (current->children[index] == nullptr) {
            return false;
        }
        current = current->children[index];
    }
    return current != nullptr && current->isEndOfWord;
}

bool delete_word(TrieNode* root, const string& word) {
    TrieNode* current = root;
    for (char c : word) {
        int index = charToIndex(c);
        if (index < 0 || index >= Alphabet_Size || current->children[index] == nullptr) {
            return false;
        }
        current = current->children[index];
    }
    if (current != nullptr && current->isEndOfWord) {
        current->isEndOfWord = false;
        return true;
    }
    return false;
}

// --- Levenshtein Distance & Suggestion Functions ---

/**
 * @brief Calculates the Levenshtein (Edit) Distance between two words.
 * This is the number of single-character edits required to change s1 into s2.
 */
int calculateLevenshteinDistance(const string& s1, const string& s2) {
    int m = s1.length();
    int n = s2.length();
    
    // Create a 2D vector (matrix) to store distances.
    // Dimensions are (m+1) x (n+1)
    vector<vector<int>> dp(m + 1, vector<int>(n + 1));

    // Initialize the matrix borders (distance from empty string)
    for (int i = 0; i <= m; ++i) dp[i][0] = i; 
    for (int j = 0; j <= n; ++j) dp[0][j] = j; 

    // Fill the rest of the matrix using dynamic programming
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            // Cost of substitution: 0 if characters match, 1 otherwise
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;

            // Choose the minimum of the three options:
            // 1. Deletion (dp[i - 1][j] + 1)
            // 2. Insertion (dp[i][j - 1] + 1)
            // 3. Substitution (dp[i - 1][j - 1] + cost)
            dp[i][j] = min({
                dp[i - 1][j] + 1,
                dp[i][j - 1] + 1,
                dp[i - 1][j - 1] + cost
            });
        }
    }
    // The result is in the bottom-right corner 
    return dp[m][n];
}

/**
 * @brief Recursive helper to find all words stored in the Trie.
 */
void getAllWords(TrieNode* node, string current_word, vector<string>& all_words) {
    if (!node) return;

    if (node->isEndOfWord) {
        all_words.push_back(current_word);
    }

    for (int i = 0; i < Alphabet_Size; ++i) {
        if (node->children[i]) {
            char nextChar = 'a' + i;
            getAllWords(node->children[i], current_word + nextChar, all_words);
        }
    }
}

/**
 * @brief Finds spelling suggestions using Levenshtein Distance.
 * @param root The root of the Trie.
 * @param misspelled_word The word the user typed.
 * @return A vector of pairs: {suggestion_word, distance_score}
 */
vector<pair<string, int>> findSuggestions(TrieNode* root, const string& misspelled_word) {
    vector<string> all_dict_words;
    // Step 1: Extract all words from the dictionary (Trie)
    getAllWords(root, "", all_dict_words); 

    vector<pair<string, int>> suggestions;
    
    // Step 2: Check the distance for every word in the dictionary
    for (const string& dict_word : all_dict_words) {
        
        // Optimization: Don't bother calculating distance if lengths are too far apart.
        if (abs((int)dict_word.length() - (int)misspelled_word.length()) > MAX_EDIT_DISTANCE) {
            continue;
        }

        int distance = calculateLevenshteinDistance(misspelled_word, dict_word);

        // Step 3: Filter results based on the allowed threshold
        if (distance <= MAX_EDIT_DISTANCE) {
            suggestions.push_back({dict_word, distance});
        }
    }
    
    // Step 4: Sort suggestions by distance (lowest distance is the best suggestion)
    sort(suggestions.begin(), suggestions.end(), [](const auto& a, const auto& b) {
        return a.second < b.second;
    });

    return suggestions;
}


int main() {
    
    TrieNode* root_ptr = createNode();

    // Dictionary words for demonstration
    vector<string> dictionary_words = {
        "read", "ready", "reader", "raid", "run", "ran", "ring"
    };
    
    cout << "--- Trie Spell Checker Demonstration ---\n\n";

    cout << "1. Inserting dictionary words:\n";
    for(const string& word : dictionary_words) {
        insert(root_ptr, word);
        cout << "   + " << word << "\n";
    }
    
    cout << "\n------------------------------------------------------\n";

    // --- Search & Suggestion Demo ---
    string misspelled_word = "raed"; // Should suggest 'read' and 'raid'
    cout << "2. Checking Word: '" << misspelled_word << "'\n";
    
    if (search(root_ptr, misspelled_word)) {
        cout << "   STATUS: Word found (Correctly spelled).\n";
    } else {
        cout << "   STATUS: Word not found (Misspelled).\n";
        
        vector<pair<string, int>> suggestions = findSuggestions(root_ptr, misspelled_word);

        if (!suggestions.empty()) {
            cout << "   SUGGESTIONS (Edit Distance <= " << MAX_EDIT_DISTANCE << "): \n";
            for (const auto& suggestion : suggestions) {
                cout << "     -> " << suggestion.first << " (Distance: " << suggestion.second << ")\n";
            }
        } else {
            cout << "   No close suggestions found.\n";
        }
    }

    cout << "\n------------------------------------------------------\n";
    
    // Existing Delete Demo
    cout << "3. Deleting 'read' \n";
    delete_word(root_ptr, "read");
    
    cout << "4. Checking state after deletion:\n";
    cout << " - Is 'read' present now? " << (search(root_ptr, "read") ? "YES" : "NO") << "\n"; 
    cout << " - Is 'ready' present? " << (search(root_ptr, "ready") ? "YES" : "NO") << "\n"; 
    
    // Cleanup
    delete root_ptr; 
    
    return 0;
}


