# Mini Search Engine - MACS Society Project

## 📌 Project Overview

A high-performance, C++ based search engine capable of indexing text documents and ranking them using TF-IDF scoring. It supports full-text search and real-time query autocomplete.

## 🛠️ Approach & Design

The engine operates in two phases:

1.  **Offline Indexing**:
    - Reads the "20 Newsgroups" dataset recursively.
    - Tokenizes text (removes punctuation, converts to lowercase).
    - Filters out common "stopwords" (e.g., "the", "is") to save memory.
    - Builds an **Inverted Index** mapping words to document IDs.
    - Simultaneously inserts words into a **Trie** for autocomplete.
2.  **Online Searching**:
    - Calculates **TF-IDF** scores for relevant documents.
    - Ranks documents by score (descending).
    - Provides autocomplete suggestions for partial queries.

## 🧬 Data Structures Used

- **Inverted Index (`std::unordered_map<string, vector<DocInfo>>`)**:
  - Chosen for $O(1)$ average time complexity during lookups.
  - Maps every unique word to a list of documents containing it.
- **Trie (Prefix Tree)**:
  - Used for **Autocomplete**.
  - Allows finding predictive text in $O(L)$ time (where $L$ is the prefix length), independent of the total dataset size.
- **Min-Heap / Sorting**:
  - Used `std::sort` to rank the final results based on relevance score.

## ⏳ Time & Space Complexity

- **Indexing Time**: $O(D \times W)$, where $D$ is the number of documents and $W$ is the average words per document.
- **Search Time**: $O(Q \times K)$, where $Q$ is query terms and $K$ is the number of documents containing the term.
- **Space Complexity**: $O(V)$, where $V$ is the vocabulary size (total unique words).

## 🚀 How to Run

1.  Ensure you have a C++ compiler supporting C++17.
2.  Place the `dataset` folder and `stopwords.txt` in the same directory as `main.cpp`.
3.  Compile:
    ```bash
    g++ main.cpp -o search_engine -std=c++17 -O3
    ```
4.  Run:
    ```bash
    ./search_engine
    ```
5.  Type a search query (e.g., "nasa") or an autocomplete command (e.g., "auto:spa").

## 🧪 Sample Queries & Outputs

### Query 1: "space shuttle"

**Input:** `space shuttle`
**Output:**

```
Found 1083 results in 0.0010s
Rank 1 | Score: 0.1337 | File: dataset/20_newsgroups/sci.space/60859
Rank 2 | Score: 0.1052 | File: dataset/20_newsgroups/sci.space/61151
Rank 3 | Score: 0.1052 | File: dataset/20_newsgroups/sci.space/61430
```

### Query 2: "honda"

**Input:** `space shuttle`
**Output:**

```

Found 155 results in 0.0002s
Rank 1 | Score: 0.1204 | File: dataset/20_newsgroups/rec.autos/103212
Rank 2 | Score: 0.0903 | File: dataset/20_newsgroups/rec.autos/103302

```

### Query 3: Autocomplete

**Input**: auto:spa
**Output:**

```
Suggestions: space, spacecraft, spatial, spam, spain
