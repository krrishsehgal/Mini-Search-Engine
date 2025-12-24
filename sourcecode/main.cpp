#include <bits/stdc++.h>

using namespace std;

namespace fs = filesystem;

class DocInfo {
public:
    int doc_id;
    int count;
};

const int ALPHABET_SIZE = 26;

class TrieNode {
public:
    TrieNode *children[ALPHABET_SIZE];
    bool isEndOfWord;

    TrieNode() {
        isEndOfWord = false;
        for (int i = 0; i < ALPHABET_SIZE; i++)
            children[i] = nullptr;
    }
};

TrieNode *root = new TrieNode();

unordered_set<string> load_stopwords(const string &filepath) {
    unordered_set<string> stopwords;
    ifstream file(filepath);
    string word;
    if (file.is_open()) {
        while (file >> word) {
            stopwords.insert(word);
        }
        file.close();
    }
    return stopwords;
}

string preprocess_word(string word) {
    string clean_word = "";
    for (char c : word) {
        if (isalnum(c)) {
            clean_word += tolower(c);
        }
    }
    return clean_word;
}

vector<string> tokenize(const string &text, const unordered_set<string> &stopwords) {
    vector<string> tokens;
    stringstream ss(text);
    string word;
    while (ss >> word) {
        string clean = preprocess_word(word);
        if (!clean.empty() && stopwords.find(clean) == stopwords.end()) {
            tokens.push_back(clean);
        }
    }
    return tokens;
}

void insert_trie(TrieNode *root, const string &key) {
    TrieNode *pCrawl = root;
    for (char c : key) {
        if (c < 'a' || c > 'z')
            continue;
        int index = c - 'a';
        if (!pCrawl->children[index])
            pCrawl->children[index] = new TrieNode();
        pCrawl = pCrawl->children[index];
    }
    pCrawl->isEndOfWord = true;
}

void collect_suggestions(TrieNode *node, string currentPrefix, vector<string> &results) {
    if (results.size() >= 5)
        return;
    if (node->isEndOfWord) {
        results.push_back(currentPrefix);
    }
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i]) {
            char nextChar = 'a' + i;
            collect_suggestions(node->children[i], currentPrefix + nextChar, results);
        }
    }
}

vector<string> get_autocomplete(TrieNode *root, const string &query) {
    TrieNode *pCrawl = root;
    vector<string> results;
    for (char c : query) {
        if (c < 'a' || c > 'z')
            return results;
        int index = c - 'a';
        if (!pCrawl->children[index])
            return results;
        pCrawl = pCrawl->children[index];
    }
    collect_suggestions(pCrawl, query, results);
    return results;
}

void add_document_to_index(int doc_id, const string &text,
                           unordered_map<string, vector<DocInfo>> &index,
                           unordered_map<int, int> &doc_lengths,
                           const unordered_set<string> &stopwords) {
    vector<string> tokens = tokenize(text, stopwords);
    doc_lengths[doc_id] = tokens.size();
    unordered_map<string, int> word_counts;
    for (const string &token : tokens) {
        word_counts[token]++;
        insert_trie(root, token);
    }
    for (auto &entry : word_counts) {
        index[entry.first].push_back({doc_id, entry.second});
    }
}

double calculate_idf(int total_docs, int docs_with_term) {
    return log10(static_cast<double>(total_docs) / (1 + docs_with_term));
}

vector<pair<int, double>> search_query(
    const string &query,
    unordered_map<string, vector<DocInfo>> &index,
    unordered_map<int, int> &doc_lengths,
    int total_docs,
    const unordered_set<string> &stopwords) {
    unordered_map<int, double> doc_scores;
    vector<string> query_tokens = tokenize(query, stopwords);
    for (const string &term : query_tokens) {
        if (index.find(term) != index.end()) {
            vector<DocInfo> &doc_list = index[term];
            double idf = calculate_idf(total_docs, doc_list.size());
            for (const auto &info : doc_list) {
                double tf = static_cast<double>(info.count) / doc_lengths[info.doc_id];
                doc_scores[info.doc_id] += tf * idf;
            }
        }
    }
    vector<pair<int, double>> ranked_results;
    for (auto const& [id, score] : doc_scores) {
        ranked_results.push_back({id, score});
    }
    sort(ranked_results.begin(), ranked_results.end(),
         [](const pair<int, double> &a, const pair<int, double> &b) {
             return a.second > b.second;
         });
    return ranked_results;
}

string read_file(const string &filepath) {
    ifstream file(filepath);
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

long long estimate_memory(const unordered_map<string, vector<DocInfo>> &index) {
    long long size = 0;
    for (const auto &entry : index) {
        size += entry.first.capacity();
        size += sizeof(vector<DocInfo>);
        size += entry.second.capacity() * sizeof(DocInfo);
    }
    return size;
}

int main() {
    unordered_set<string> stopwords = load_stopwords("stopwords.txt");
    unordered_map<string, vector<DocInfo>> inverted_index;
    unordered_map<int, int> doc_lengths;
    vector<string> doc_paths;
    string dataset_path = "dataset";

    cout << "Loading and Indexing..." << endl;
    auto start_load = chrono::high_resolution_clock::now();
    int doc_id = 0;

    try {
        for (const auto &entry : fs::recursive_directory_iterator(dataset_path)) {
            if (entry.is_regular_file()) {
                string content = read_file(entry.path().string());
                add_document_to_index(doc_id, content, inverted_index, doc_lengths, stopwords);
                doc_paths.push_back(entry.path().string());
                doc_id++;
                if (doc_id % 100 == 0)
                    cout << "Indexed " << doc_id << " files...\r";
            }
        }
    } catch (const fs::filesystem_error &e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    auto end_load = chrono::high_resolution_clock::now();
    chrono::duration<double> load_duration = end_load - start_load;

    cout << "\nIndexing complete. Total Docs: " << doc_id << "\n";
    cout << "Time taken to index: " << fixed << setprecision(4) << load_duration.count() << " seconds" << endl;

    long long mem_usage = estimate_memory(inverted_index);
    cout << "Approximate Index Memory Usage: " << mem_usage / (1024 * 1024) << " MB\n\n";

    while (true) {
        string input_line;
        cout << "Enter query (or type 'auto:pref' for suggestions): ";
        getline(cin, input_line);

        if (input_line == "exit")
            break;

        if (input_line.substr(0, 5) == "auto:") {
            string prefix = input_line.substr(5);
            vector<string> suggestions = get_autocomplete(root, prefix);
            cout << "Suggestions: ";
            for (const auto &s : suggestions)
                cout << s << ", ";
            cout << "\n--------------------------------\n";
            continue;
        }

        auto start_search = chrono::high_resolution_clock::now();
        vector<pair<int, double>> results = search_query(
            input_line, inverted_index, doc_lengths, doc_id, stopwords);
        auto end_search = chrono::high_resolution_clock::now();
        chrono::duration<double> search_time = end_search - start_search;

        cout << "Found " << results.size() << " results in " << search_time.count() << "s\n";

        int limit = min((int)results.size(), 5);
        for (int i = 0; i < limit; ++i) {
            cout << "Rank " << i + 1 << " | Score: " << fixed << setprecision(4) << results[i].second
                 << " | File: " << doc_paths[results[i].first] << "\n";
        }
        cout << "--------------------------------\n";
    }

    return 0;
}