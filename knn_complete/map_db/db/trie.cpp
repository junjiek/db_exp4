#include "trie.h"

#include <cstring>

// Map 'a'..'z' to 0..25, 'A'..'Z' to 0..25, '0'..'9' to 26..35,
// others to kInvalidChar.
int MapChar(char c) {
    if (c >= 'a' && c <= 'z')
        return c - 'a';
    if (c >= 'A' && c <= 'Z')
        return c - 'A';
    if (c >= '0' && c <= '9')
        return c - '0' + 26;
    return kInvalidChar;
}

Trie::TrieNode::TrieNode() {
    for (unsigned index = 0; index < kChildNum; ++index)
        child_[index] = kInvalidNodeRef;
}

Trie::TrieNode::~TrieNode() {}

int Trie::TrieNode::GetChild(unsigned c) const {
    return child_[c];
}

bool Trie::TrieNode::AddChild(unsigned c, int child_ref) {
    if (child_[c] != kInvalidNodeRef)
        return false;
    child_[c] = child_ref;
    return true;
}


void Trie::TrieNode::AddDataRef(unsigned ref) {
    data_refs_.push_back(ref);
}

const std::vector<unsigned>* Trie::TrieNode::GetDataRefs() const {
    return &data_refs_;
}

Trie::Trie() {
    nodes_.reserve(kTrieNodesCapacity);
    nodes_.push_back(TrieNode());
}

Trie::~Trie() {}

bool Trie::Push(const std::string& str, unsigned ref) {
    unsigned node_ref = 0;
    for (int i = 0; i < (int)str.length(); i++) {
        TrieNode& node = nodes_[node_ref];
        node.AddDataRef(ref);
        int c = MapChar(str[i]);
        if (c == kInvalidChar)
            return false;
        int child_ref = node.GetChild(c);
        if (child_ref == kInvalidNodeRef) {
            child_ref = nodes_.size();
            node.AddChild(c, child_ref);
            nodes_.push_back(TrieNode());
        }
        node_ref = child_ref;
    }
    nodes_[node_ref].AddDataRef(ref);
    return true;
}

const std::vector<unsigned>* Trie::GetRefs(const std::string& str) {
    unsigned node_ref = 0;
    for (int i = 0; i < (int)str.length(); i++) {
        TrieNode& node = nodes_[node_ref];
        int c = MapChar(str[i]);
        if (c == kInvalidChar)
            return NULL;
        int child_ref = node.GetChild(c);
        if (child_ref == kInvalidNodeRef)
            return NULL;
        node_ref = child_ref;
    }
    return nodes_[node_ref].GetDataRefs();
}

unsigned Trie::Size() {
    return nodes_.size();
}

void Trie::Clear() {
    nodes_.clear();
    nodes_.push_back(TrieNode());
}
