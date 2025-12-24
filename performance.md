# 📊 Performance Report

**System Specifications:**
* **Language**: C++ (Standard 17)
* **Dataset**: 20 Newsgroups (19,997 documents)

## 1️⃣ Search Time Analysis
We measured the time taken to retrieve and rank results for various queries using `std::chrono::high_resolution_clock`.

| Query Type | Query Term | Time Taken (Seconds) | Results Found |
| :--- | :--- | :--- | :--- |
| **Common Word** | "computer" | **0.00163 s** | 2598 docs |
| **Multi-Word** | "space shuttle" | **0.00100 s** | 1083 docs |
| **Rare Word** | "honda" | **0.00020 s** | 155 docs |

**Observation:**
Search time remains consistently low (sub-millisecond to 1ms range) because the **Inverted Index (Hash Map)** allows direct access to the document list without scanning the whole dataset. Even for common terms appearing in over 2,500 documents, retrieval and ranking are near-instantaneous.

## 2️⃣ Memory Usage Observations
* **Total Documents Indexed**: **19,997**
* **Approximate Index Size**: **44 MB**

**Optimization Strategy:**
* We used `std::unordered_map` instead of `std::map` to reduce overhead ($O(1)$ vs $O(\log n)$).
* Stopword removal significantly reduced the index size by ignoring high-frequency, low-value words.
* The Trie structure allowed for memory-efficient storage of prefixes for the autocomplete feature.