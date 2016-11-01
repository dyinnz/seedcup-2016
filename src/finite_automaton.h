/*******************************************************************************
 * Author: Dyinnz.HUST.UniqueStudio
 * Email:  ml_143@sina.com
 * Github: https://github.com/dyinnz
 * Date:   2016-08-25
 ******************************************************************************/

/**
 * This is the core part of regular expression engine. NFA, DFA and etc. is
 * implemented in this header and corresponding sources file.
 *
 * Theses finit automaton has benn optimized for tokenizer, could solve some
 * conficts when converting NFA to DFA if setting the priority correctly.
 */

#pragma once

#include <climits>
#include <cassert>

#include <algorithm>
#include <memory>
#include <string>
#include <bitset>
#include <vector>
#include <list>
#include <set>
#include <unordered_set>
#include <unordered_map>

#include "utility.h"
#include "mem_manager.h"

namespace regular_expression {

/**
 * class pre-declaration
 */

class NFAEdge;

class Node;

class NFANode;

class DFANode;

class NFAComponent;

class NFAManager;

class NFA;

class NumberSet;

class DFA;

/**
 * type definition, some memory manager.
 */
typedef SmallObjPool<NFAEdge> NFAEdgeManager;
typedef SmallObjPool<NFANode> NFANodeManager;
typedef SmallObjPool<NFAComponent> NFACompManager;

/**
 * get the string representation
 */

std::string to_string(const NFAEdge &edge);

std::string to_string(const Node &node);

std::string to_string(const NumberSet &num_set);

/**
 * for debugging
 */

void PrintNFA(const NFA *nfa);

void PrintDFA(const DFA *dfa);

/**
 * @param normal    the DFA to be minimized
 * @return          the minimum DFA, may be the same one if the parameter has
 *                  already minimum DFA
 */
std::shared_ptr<DFA> MinimizeDFA(const std::shared_ptr<DFA> normal);

/**
 * @param nfa   the NFA to be converted
 * @return      the DFA convert from NFA
 */
std::shared_ptr<DFA> ConvertNFAToDFA(const NFA *nfa);


/*----------------------------------------------------------------------------*/

/**
 * @brief   unidirectional edge connected with NFA nodes
 */
class NFAEdge {
 public:
  typedef std::bitset<CHAR_MAX + 1> CharMasks;

  NFAEdge() = default;

  NFAEdge(char c) {
    set(c);
  }

  NFAEdge(char beg, char end) {
    SetRange(beg, end);
  }

  NFAEdge(const std::string &s);

  NFANode *next_node() {
    return next_node_;
  }

  void set_next_node(NFANode *v) {
    next_node_ = v;
  }

  bool IsEpsilon() const {
    return char_masks_.none();
  }

  bool test(char c) const {
    return char_masks_.test(c);
  }

  const CharMasks &char_masks() const {
    return char_masks_;
  }

  void set() {
    char_masks_.set();
  }

  void set(char c) {
    char_masks_.set(c);
  }

  void flip() {
    char_masks_.flip();
  }

  void SetRange(char beg, char end) {
    for (char c = beg; c < end; ++c) {
      set(c);
    }
  }

 private:
  CharMasks char_masks_;
  NFANode *next_node_{nullptr};
};


/*----------------------------------------------------------------------------*/

/**
 * @brief   common abstract super class of NFANode and DFANode
 */
class Node {
 public:
  enum State {
    kStart, kEnd, kStartEnd, kNormal
  };

  constexpr static int kUnsetInt{-1};

 protected:
  Node(State state) : state_(state) {}

 public:
  int priority() const {
    return priority_;
  }

  void set_priority(int priority) {
    priority_ = priority;
  }

  bool IsStart() const {
    return kStart == state_ || kStartEnd == state_;
  }

  bool IsEnd() const {
    return kEnd == state_ || kStartEnd == state_;
  }

  bool IsNormal() const {
    return kNormal == state_;
  }

  State state() const {
    return state_;
  }

  void AttachState(State state);

  int number() const {
    return number_;
  }

  void set_number(int number) {
    number_ = number;
  }

 private:
  State state_;
  int priority_{kUnsetInt};
  int number_{kUnsetInt};
};


/*----------------------------------------------------------------------------*/

/**
 *  @brief  only for NFA diagram
 */
class NFANode : public Node {
 public:
  NFANode(State state) : Node(state) {}

  const std::list<NFAEdge *> &edges() const {
    return edges_;
  }

  void AddEdge(NFAEdge *edge, NFANode *node) {
    edge->set_next_node(node);
    edges_.push_back(edge);
  }

  void FetchEdges(NFANode *other) {
    edges_.splice(edges_.end(), other->edges_);
  }

 private:
  std::list<NFAEdge *> edges_;
};


/*----------------------------------------------------------------------------*/

/**
 * @brief   only for DFA diagram
 */
class DFANode : public Node {
 public:
  DFANode(State state) : Node(state) {}

  const std::unordered_map<char, DFANode *> &edges() const {
    return edges_;
  };

  void AddEdge(char c, DFANode *node) {
    edges_[c] = node;
  }

  const DFANode *GetNextNode(char c) const {
    auto iter = edges_.find(c);
    if (edges_.end() == iter) {
      return nullptr;
    } else {
      return iter->second;
    }
  }

 private:
  std::unordered_map<char, DFANode *> edges_;
};


/*----------------------------------------------------------------------------*/

/**
 * @brief   non-deterministic finite automaton component, only contains nodes
 */
class NFAComponent {
 public:
  NFAComponent(NFANode *start, NFAEdge *e, NFANode *end)
      : start_(start), end_(end) {
    assert(start->IsStart());
    assert(end->IsEnd());
    start_->AddEdge(e, end_);
  }

  NFANode *start() {
    return start_;
  }

  NFANode *end() {
    return end_;
  }

  NFANode *SetNewStart(NFANode *start);

  NFANode *SetNewEnd(NFANode *end);

  NFANode *RemoveStart();

  NFANode *RemoveEnd();

 private:
  NFANode *start_{nullptr};
  NFANode *end_{nullptr};
};

class NFAManager : SmallObjPool<NFA> {
 public:
  NFAEdgeManager &edge_manager() {
    return edge_manager_;
  }

  NFANodeManager &node_manager() {
    return node_manager_;
  }

  NFACompManager &comp_manager() {
    return comp_manager_;
  }

  template<class... A>
  NFAEdge *CreateEdge(A &&... args) {
    return edge_manager_.Create(std::forward<A>(args)...);
  }

  template<class... A>
  NFANode *CreateNode(A &&... args) {
    return node_manager_.Create(std::forward<A>(args)...);
  }

  template<class... A>
  NFAComponent *CreateComponent(A &&... args) {
    return comp_manager_.Create(std::forward<A>(args)...);
  }

  NFAComponent *CreateCompFromEdge(NFAEdge *e) {
    return CreateComponent(CreateNode(Node::kStart), e, CreateNode(Node::kEnd));
  }

  NFAComponent *CreateCompFromChar(char c) {
    return CreateCompFromEdge(CreateEdge(c));
  }

  NFAComponent *CreateCompFromRange(char beg, char end) {
    return CreateCompFromEdge(CreateEdge(beg, end));
  }

  NFAComponent *CreateCompFromString(const std::string &s) {
    return CreateCompFromEdge(CreateEdge(s));
  }

  NFAComponent *Concatenate(NFAComponent *lhs, NFAComponent *rhs);

  template<class ...A>
  NFAComponent *Concatenate(NFAComponent *first, A... rest);

  NFAComponent *Union(NFAComponent *lhs, NFAComponent *rhs);

  template<class ...A>
  NFAComponent *Union(NFAComponent *first, A... rest);

  NFAComponent *KleenStar(NFAComponent *nfa);

  NFAComponent *Optional(NFAComponent *nfa);

  NFAComponent *LeastOne(NFAComponent *nfa);

  NFAComponent *UnionWithMultiEnd(NFAComponent *lhs, NFAComponent *rhs);

  template<class ...A>
  NFAComponent *UnionWithMultiEnd(NFAComponent *first, A... rest);

  NFA *BuildNFA(NFAComponent *comp);

 private:
  NFAEdgeManager edge_manager_;
  NFANodeManager node_manager_;
  NFACompManager comp_manager_;
};


/*----------------------------------------------------------------------------*/

/**
 * @brief   non-deterministic finite automaton, could match or search a string.
 */
class NFA {
 public:
  NFA(NFANode *start);

  bool Match(const char *beg, const char *end) const;

  const char *Search(const char *begin, const char *end) const;

  const NFANode *start() const {
    return start_;
  }

  size_t size() const {
    return nodes_.size();
  }

  const NFANode *GetNode(size_t number) const {
    return nodes_[number];
  }

 private:
  void CollectNodes(NFANode *start, std::unordered_set<NFANode *> &visits);

  const char *MatchDFS(NFANode *curr, const char *beg, const char *end) const;

  const char *SearchDFS(NFANode *curr, const char *beg, const char *end) const;

 private:
  NFANode *start_{nullptr};
  std::vector<NFANode *> nodes_;
};


/*----------------------------------------------------------------------------*/

/**
 * @brief   a set contains number, support hashing and sharing with others
 */
class NumberSet {
 public:
  struct Hasher {
    size_t operator()(const NumberSet &num_set) const;
  };

 public:
  NumberSet() : pset_(std::make_shared<std::set<int>>()) {}

  const std::set<int> &set() const {
    return *pset_;
  }

  std::set<int>::const_iterator begin() const {
    return pset_->begin();
  }

  std::set<int>::const_iterator end() const {
    return pset_->end();
  }

  bool empty() const {
    return pset_->empty();
  }

  bool contains(int num) const {
    return pset_->end() != pset_->find(num);
  }

  bool insert(int num) {
    return pset_->insert(num).second;
  }

  void insert(const NumberSet &num_set) {
    pset_->insert(num_set.begin(), num_set.end());
  }

 private:
  std::shared_ptr<std::set<int>> pset_;
};

inline bool operator==(const NumberSet &lhs, const NumberSet &rhs) {
  return lhs.set() == rhs.set();
}

inline bool operator!=(const NumberSet &lhs, const NumberSet &rhs) {
  return !(lhs == rhs);
}


/*----------------------------------------------------------------------------*/

/**
 * @brief   deterministric finite automaton
 */
class DFA {
 public:
  DFA(DFANode *start,
      std::vector<DFANode *> &&ends,
      std::vector<DFANode *> &&nodes)
      : start_(start), ends_(std::move(ends)), nodes_(std::move(nodes)) {
    NumberNode();
  }

  ~DFA() {
    for (DFANode *node : nodes_) {
      delete node;
    }
  }

  size_t size() const {
    return nodes_.size();
  }

  const DFANode *start() const {
    return start_;
  }

  const DFANode *GetNode(int number) const {
    return nodes_[number];
  }

  bool Match(const char *beg, const char *end) const;

  bool Match(const std::string &s) const;

  const char *Search(const char *begin, const char *end) const;

  size_t Search(const std::string &s) const;

 private:
  void NumberNode();

 private:
  DFANode *start_{nullptr};
  std::vector<DFANode *> ends_;
  std::vector<DFANode *> nodes_;
};

/*----------------------------------------------------------------------------*/

template<class ...A>
NFAComponent *NFAManager::Concatenate(NFAComponent *first, A... rest) {
  if (0 == sizeof...(rest)) {
    return first;
  } else {
    NFAComponent *rest_result = Concatenate(rest...);
    return Concatenate(first, rest_result);
  }
}

template<class ...A>
NFAComponent *NFAManager::Union(NFAComponent *first, A... rest) {
  if (0 == sizeof...(rest)) {
    return first;
  } else {
    NFAComponent *rest_result = Union(rest...);
    return Union(first, rest_result);
  }
}

template<class ...A>
NFAComponent *NFAManager::UnionWithMultiEnd(NFAComponent *first, A... rest) {
  if (0 == sizeof...(rest)) {
    return first;
  } else {
    NFAComponent *rest_result = UnionWithMultiEnd(rest...);
    return UnionWithMultiEnd(first, rest_result);
  }
}

} // end of namespace transition_map
