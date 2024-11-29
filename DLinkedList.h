/*
 * File:   DLinkedList.h
 */

#ifndef DLINKEDLIST_H
#define DLINKEDLIST_H

#include <iostream>
#include <sstream>
#include <type_traits>

#include "list/IList.h"
using namespace std;

template <class T>
class DLinkedList : public IList<T> {
 public:
  class Node;         // Forward declaration
  class Iterator;     // Forward declaration
  class BWDIterator;  // Forward declaration

 protected:
  Node *head;  // this node does not contain user's data
  Node *tail;  // this node does not contain user's data
  int count;
  bool (*itemEqual)(T &lhs, T &rhs);  // function pointer: test if two items
                                      // (type: T&) are equal or not
  void (*deleteUserData)(
      DLinkedList<T> *);  // function pointer: be called to remove items (if
                          // they are pointer type)

 public:
  DLinkedList(void (*deleteUserData)(DLinkedList<T> *) = 0,
              bool (*itemEqual)(T &, T &) = 0);
  DLinkedList(const DLinkedList<T> &list);
  DLinkedList<T> &operator=(const DLinkedList<T> &list);
  ~DLinkedList();

  // Inherit from IList: BEGIN
  void add(T e);
  void add(int index, T e);
  T removeAt(int index);
  void remove(Node* node);
  bool removeItem(T item, void (*removeItemData)(T) = 0);
  bool empty();
  int size();
  void clear();
  T &get(int index);
  int indexOf(T item);
  bool contains(T item);
  string toString(string (*item2str)(T &) = 0);
  bool checkIndex(int index);
  // Inherit from IList: END

  void println(string (*item2str)(T &) = 0) {
    cout << toString(item2str) << endl;
  }
  void setDeleteUserDataPtr(void (*deleteUserData)(DLinkedList<T> *) = 0) {
    this->deleteUserData = deleteUserData;
  }

  bool contains(T array[], int size) {
    int idx = 0;
    for (DLinkedList<T>::Iterator it = begin(); it != end(); it++) {
      if (!equals(*it, array[idx++], this->itemEqual)) return false;
    }
    return true;
  }

  /*
   * free(DLinkedList<T> *list):
   *  + to remove user's data (type T, must be a pointer type, e.g.: int*,
   * Point*)
   *  + if users want a DLinkedList removing their data,
   *      he/she must pass "free" to constructor of DLinkedList
   *      Example:
   *      DLinkedList<T> list(&DLinkedList<T>::free);
   */
  static void free(DLinkedList<T> *list) {
    typename DLinkedList<T>::Iterator it = list->begin();
    while (it != list->end()) {
      delete *it;
      it++;
    }
  }

  /* begin, end and Iterator helps user to traverse a list forwardly
   * Example: assume "list" is object of DLinkedList

   DLinkedList<char>::Iterator it;
   for(it = list.begin(); it != list.end(); it++){
          char item = *it;
          std::cout << item; //print the item
   }
   */
  Iterator begin() { return Iterator(this, true); }
  Iterator end() { return Iterator(this, false); }

  /* last, beforeFirst and BWDIterator helps user to traverse a list backwardly
   * Example: assume "list" is object of DLinkedList

   DLinkedList<char>::BWDIterator it;
   for(it = list.last(); it != list.beforeFirst(); it--){
          char item = *it;
          std::cout << item; //print the item
   }
   */
  BWDIterator bbegin() { return BWDIterator(this, true); }
  BWDIterator bend() { return BWDIterator(this, false); }

 protected:
  static bool equals(T &lhs, T &rhs, bool (*itemEqual)(T &, T &)) {
    if (itemEqual == 0)
      return lhs == rhs;
    else
      return itemEqual(lhs, rhs);
  }
  void copyFrom(const DLinkedList<T> &list);
  void removeInternalData();
  Node *getPreviousNodeOf(int index);

  //////////////////////////////////////////////////////////////////////
  ////////////////////////  INNER CLASSES DEFNITION ////////////////////
  //////////////////////////////////////////////////////////////////////
 public:
  class Node {
   public:
    T data;
    Node *next;
    Node *prev;
    friend class DLinkedList<T>;

   public:
    Node(Node *next = 0, Node *prev = 0) {
      this->next = next;
      this->prev = prev;
    }
    Node(T data, Node *next = 0, Node *prev = 0) {
      this->data = data;
      this->next = next;
      this->prev = prev;
    }
  };

  //////////////////////////////////////////////////////////////////////
  class Iterator {
   private:
    DLinkedList<T> *pList;
    Node *pNode;

   public:
    Iterator(DLinkedList<T> *pList = 0, bool begin = true) {
      if (begin) {
        if (pList != 0)
          this->pNode = pList->head->next;
        else
          pNode = 0;
      } else {
        if (pList != 0)
          this->pNode = pList->tail;
        else
          pNode = 0;
      }
      this->pList = pList;
    }

    Iterator &operator=(const Iterator &iterator) {
      this->pNode = iterator.pNode;
      this->pList = iterator.pList;
      return *this;
    }
    void remove(void (*removeItemData)(T) = 0){
      pNode->prev->next = pNode->next;
      pNode->next->prev = pNode->prev;
      Node *pNext = pNode->prev;  // MUST prev, so iterator++ will go to end
      if (removeItemData != 0) removeItemData(pNode->data);
      delete pNode;
      pNode = pNext;
      pList->count -= 1;
    }

    T &operator*() { return pNode->data; }
    bool operator!=(const Iterator &iterator) {
      return pNode != iterator.pNode;
    }
    // Prefix ++ overload
    Iterator &operator++() {
      pNode = pNode->next;
      return *this;
    }
    Iterator &operator--() {
      pNode = pNode->prev;
      return *this;
    }
    // Postfix ++ overload
    Iterator operator++(int) {
      Iterator iterator = *this;
      ++*this;
      return iterator;
    }
    Iterator operator--(int) {
      Iterator iterator = *this;
      --*this;
      return iterator;
    }

    DLinkedList::Node* getpNode(){ return this->pNode; }
    DLinkedList<T> getpList(){ return this->pList; }
  };

  class BWDIterator {
    private:
    DLinkedList<T> *pList;
    Node *pNode;

   public:
    BWDIterator(DLinkedList<T> *pList = 0, bool begin = true) {
      if (begin) {
        if (pList != 0)
          this->pNode = pList->tail->prev;
        else
          pNode = 0;
      } 
      else {
        if (pList != 0)
          this->pNode = pList->head;
        else
          pNode = 0;
      }
      this->pList = pList;
    }

    bool operator!=(const BWDIterator &bwditerator) {
      return pNode != bwditerator.pNode;
    }

    T &operator*() { return pNode->data; }

    void remove(void (*removeItemData)(T) = 0) {
      pNode->prev->next = pNode->next;
      pNode->next->prev = pNode->prev;
      Node *pNext = pNode->next;  // MUST prev, so iterator++ will go to end
      if (removeItemData != 0) removeItemData(pNode->data);
      delete pNode;
      pNode = pNext;
      pList->count -= 1;
    }
    // Prefix ++ overload
    BWDIterator &operator++() {
      if (pNode!=nullptr) pNode = pNode->prev;
      return *this;
    }

    // Postfix ++ overload
    BWDIterator operator++(int) {
      BWDIterator bwditerator = *this;
      ++*this;
      return bwditerator;
    }

    BWDIterator &operator--() {
      pNode = pNode->next;
      return *this;
    } 

     // Postfix -- overload
    BWDIterator operator--(int) {
      BWDIterator bwditerator = *this;
      --*this;
      return bwditerator;
    }

    DLinkedList::Node* getpNode(){ return this->pNode; }
    DLinkedList<T> getpList(){ return this->pList; }
  };
};
//////////////////////////////////////////////////////////////////////
// Define a shorter name for DLinkedList:

template <class T>
using List = DLinkedList<T>;

//////////////////////////////////////////////////////////////////////
////////////////////////     METHOD DEFNITION      ///////////////////
//////////////////////////////////////////////////////////////////////
template <class T>
DLinkedList<T>::DLinkedList(
    void (*deleteUserData)(DLinkedList<T> *),
    bool (*itemEqual)(T &, T &))
{
    // TODO
    this->deleteUserData = deleteUserData;
    this->itemEqual = itemEqual;

    this->head = new Node(); 
    this->tail = new Node(); 

    this->head->next = this->tail;
    this->tail->prev = this->head;

    this->count = 0;
}

template <class T>
DLinkedList<T>::DLinkedList(const DLinkedList<T> &list)
{
    // TODO
    if(list.head==nullptr){
      this->head = nullptr;
      this->tail = nullptr;
      this->count = 0;
    } else{ 
      this->head = new Node(); 
      this->tail = new Node();
      this->head->next = this->tail;
      this->tail->prev = this->head;
      this->count = 0;

      this->itemEqual = list.itemEqual;
      this->deleteUserData = list.deleteUserData;

      Node* current = list.head->next;
      for(int i=0; i<list.count; i++){
        this->add(current->data); 
        current = current->next;
      }
    }
}

template <class T>
DLinkedList<T> &DLinkedList<T>::operator=(const DLinkedList<T> &list)
{
    // TODO
  if(this==&list) return *this;
  this->removeInternalData();
  this->copyFrom(list);
  return *this;
}

template <class T>
DLinkedList<T>::~DLinkedList() {
  // TODO
  //cout<<"call ~"<<endl;
  this->removeInternalData();
}


template<class T>
bool DLinkedList<T>::checkIndex(int index){
    if(index<0 || index>this->count) return false;
    return true;
}

template <class T>
void DLinkedList<T>::add(T e)
{
    // TODO
  Node* newNode = new Node(e, nullptr, nullptr);
  Iterator it = this->end();
  Node* currNode = it.getpNode()->prev;

  newNode->next = it.getpNode();
  it.getpNode()->prev = newNode;
  newNode->prev = currNode;
  currNode->next = newNode;
  this->count++;
}
template <class T>
void DLinkedList<T>::add(int index, T e)
{
    // TODO
    if(!this->checkIndex(index)) throw out_of_range("Index is out of range!");
    else if(index==this->count) add(e);
    else{
      Node* newNode = new Node(e, nullptr, nullptr);
      Iterator it =this->begin();
        if(index==0){
          it.getpNode()->prev = newNode;
          newNode->next = it.getpNode();
          head->next = newNode;
          newNode->prev = head;
        } else{
            for(int i=0; i<index; i++) ++it;
            Node* currNode = it.getpNode();
            Node* prevNode = currNode->prev;
            prevNode->next = newNode;
            newNode->prev = prevNode;
            newNode->next = currNode;
            currNode->prev = newNode;
        }
      this->count++;
    }
}

template <class T>
typename DLinkedList<T>::Node *DLinkedList<T>::getPreviousNodeOf(int index)
{
    /**
     * Returns the node preceding the specified index in the doubly linked list.
     * If the index is in the first half of the list, it traverses from the head; otherwise, it traverses from the tail.
     * Efficiently navigates to the node by choosing the shorter path based on the index's position.
     */
    // TODO
    if(!checkIndex(index)) throw out_of_range("Index is out of range!");
    else{
      if(index<=(this->count/2)){
        Iterator it = this->begin();
        for(int i=0; i<index; i++) ++it;
        return it.getpNode();
      } else{
        BWDIterator it = this->end();
        for(int i=this->count-1; i>=index; i--) --it; 
        return it.getpNode()->prev;
      }
    }
}

template <class T>
T DLinkedList<T>::removeAt(int index) {
  // TODO implement
  if(index<0 || index>=this->count) throw out_of_range("Index is out of range!");
  else{
    Iterator it = this->begin();
    for(int i=0; i<index; i++) ++it;
    it.getpNode()->prev->next = it.getpNode()->next;
    it.getpNode()->next->prev = it.getpNode()->prev;

    it.getpNode()->next = nullptr;
    it.getpNode()->prev = nullptr;

    this->count--;
    T returnNode = it.getpNode()->data;
    delete it.getpNode();
    return returnNode;
  }
}

template <class T>
bool DLinkedList<T>::empty()
{
    // TODO
    return this->count==0;
}

template <class T>
int DLinkedList<T>::size()
{
    // TODO
    return this->count;
}

template <class T>
void DLinkedList<T>::clear()
{
    // TODO
    if(this->count!=0){
      if(deleteUserData) deleteUserData(this);
      Node* temp = head->next;
      for(int i=0; i<this->count; i++){
        Node* deleteNode = temp;
        temp = temp->next;
        delete deleteNode;
      }
    }
    head->next = tail;
    tail->prev = head;
    this->count = 0; 
    //cout<<"clear"<<endl;
}

template <class T>
T &DLinkedList<T>::get(int index)
{
    // TODO
    if(index<0 || index>=this->count) throw out_of_range("Index is out of range!");
    else{
      if(index<=(this->count/2)){
        Node* temp = head;
        for(int i=0; i<=index; i++) temp = temp->next;
        return temp->data;
      } else{
        Node* temp = tail;
        for(int i=this->count-1; i>=index; i--) temp = temp->prev;
        return temp->data;
      }
    }
}

template <class T>
int DLinkedList<T>::indexOf(T item)
{
    // TODO
    int index = -1;
    Node* temp = head;
    for(int i=0; i<this->count; i++){
      temp = temp->next; 
      if(equals(temp->data, item, this->itemEqual)){
          index = i;
          break;
      } else continue;
    }
    if(index!=-1) return index;
    return -1;
}
template<class T>
void DLinkedList<T>::remove(Node* node){
  if(node==nullptr) return;
  else{
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->next =nullptr;
    node->prev = nullptr;
    delete node;
    //if(deleteUserData) deleteUserData(this);
    this->count--;
  }
}
/*
template <class T>
bool DLinkedList<T>::removeItem(T item, void (*removeItemData)(T)) {
  // TODO implement
  if(this->count==0) return false;
  int index = -1;
  Iterator it = this->begin();
  for(int i=0; i<this->count; i++){
    ++it;
    if(equals(it.getpNode()->data, item, this->itemEqual)){
      index = i;
      break;
    }
  }
  if(index==-1) return false;
  else{
    it.getpNode()->next->prev = it.getpNode()->prev;
    it.getpNode()->prev->next = it.getpNode()->next;

    if(removeItemData!=nullptr) removeItemData(it.getpNode()->data);
    remove(it.getpNode());

    this->count--;
  }
  return true;
}
*/


template <class T>
bool DLinkedList<T>::removeItem(T item, void (*removeItemData)(T)) {
  // TODO implement
  if(this->count==0) return false;
  int index = -1;
  Node* deleteNode = head;
  for(int i=0; i<this->count; i++){
    deleteNode = deleteNode->next;
    if(equals(deleteNode->data, item, this->itemEqual)){
      index = i;
      break;
    }
  }
  if(index==-1) return false;
  else{
    //deleteNode->next->prev = deleteNode->prev;
    //deleteNode->prev->next = deleteNode->next;
    if(removeItemData!=nullptr) removeItemData(deleteNode->data);
    remove(deleteNode);
  }
  return true;
}

template <class T>
bool DLinkedList<T>::contains(T item)
{
    // TODO
    if(this->count==0) return false;
    else{
      Node* temp = head;
      for(int i=0; i<this->count; i++){
        temp = temp->next;
        if(equals(item, temp->data, this->itemEqual)) return true;
    }
    return false;
    }
}

template <class T>
string DLinkedList<T>::toString(string (*item2str)(T &)) {
  // TODO implement
  stringstream ss;
  ss << "[";
  if(this->count==0) ss << "]";
  else{
    Node* temp = head;
    for(int i=0; i<this->count; i++){
      temp = temp->next;
      if(item2str!=nullptr) ss << item2str(temp->data);
      else ss << temp->data;

      if(i<this->count-1) ss << ", ";
      else ss << "]";
    }
  }
  return ss.str();
}

template <class T>
void DLinkedList<T>::copyFrom(const DLinkedList<T> &list)
{
    /**
     * Copies the contents of another doubly linked list into this list.
     * Initializes the current list to an empty state and then duplicates all data and pointers from the source list.
     * Iterates through the source list and adds each element, preserving the order of the nodes.
     */
    // TODO
    head = tail = new Node;
    head->next = tail;
    tail->prev = head;
    this->count = 0;
    this->deleteUserData = list.deleteUserData;
    this->itemEqual = itemEqual;
    if(list.count==0) return;
    else{
      Node* currNode = list.head;
      while(currNode!=list.tail){
        currNode = currNode->next;
        this->add(currNode->data);
      }
    }
}

template <class T>
void DLinkedList<T>::removeInternalData()
{
    /**
     * Clears the internal data of the list by deleting all nodes and user-defined data.
     * If a custom deletion function is provided, it is used to free the user's data stored in the nodes.
     * Traverses and deletes each node between the head and tail to release memory.
     */
    // TODO
    if(deleteUserData) deleteUserData(this);
    Node* temp = head;
    while (temp!=nullptr) {
        Node* deleteNode = temp;
        temp = temp->next;
        delete deleteNode;
        this->count--;
    }
    this->count=0;
    head = nullptr;
    tail = nullptr;
    //cout<<"call removeInternal"<<endl;
}

#endif /* DLINKEDLIST_H */
