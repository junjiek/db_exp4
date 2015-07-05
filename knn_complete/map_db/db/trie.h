#ifndef _TRIE_H_
#define _TRIE_H_

#include <vector>
#include <string>

const int kInvalidChar = -1;
const int kInvalidNodeRef = -1;
const unsigned kChildNum = 37;
const int kTrieNodesCapacity = 25000;

int MapChar(char c);

class Trie {
public:
    class TrieNode {
    public:
        TrieNode();
        ~TrieNode();

        int GetChild(unsigned c) const;
        bool AddChild(unsigned c, int child_ref);

        void AddDataRef(unsigned ref);
        const std::vector<unsigned>* GetDataRefs() const;

    private:
        int child_[kChildNum];
        std::vector<unsigned> data_refs_;
    };

    Trie();
    ~Trie();

    bool Push(const std::string& str, unsigned ref);
    const std::vector<unsigned>* GetRefs(const std::string& str);
    unsigned Size();
    void Clear();

private:
    std::vector<TrieNode> nodes_;
};

#endif  // _TRIE_H_
