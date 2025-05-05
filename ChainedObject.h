#ifndef CHAINEDOBJECT_H
#define CHAINEDOBJECT_H

#include <iostream>
#include <vector>
#include <memory>

/* A PoolItem is a slot of the Pool, containing chain information and T object data */
template<class T>
struct PoolItem {
    T object;
    int index;
    bool isAvailable = true;
    int nextFree = -1;
    int nextObject = -1;
    int prevObject = -1;

    T* operator->() { return &object; }
    T& operator*() { return object; }
};

// Define Vector as std::vector<PoolItem<T>> to make the code cleaner
template<class T>
using Vector = std::vector<PoolItem<T>>;

/* The pool itself containing all the PoolItems */
template<class T>
class Pool {
public:
    Pool(uint32_t size = 10000);

    template<class... Args>
    uint32_t createObject(Args&&... args);

    T& operator[](uint32_t i);
    T* getItemAt(uint32_t i);
    T* getFirstItem();
    T* getNext(T*& item);

    uint32_t size() const { return m_size; }

    void remove(int i);
    void resize(uint32_t size);

private:
    uint32_t m_size = 0;
    int m_firstFree = -1;
    int m_firstObject = -1;
    std::vector<PoolItem<T>> m_data;
};

// Constructor to initialize the pool with a given size
template<class T>
Pool<T>::Pool(uint32_t size) {
    resize(size);
}

// Resize the pool, initializing all free slots and their chain links
template<class T>
void Pool<T>::resize(uint32_t size) {
    m_data.resize(size);
    for (uint32_t i = 0; i < size; ++i) {
        m_data[i].index = i;
        m_data[i].nextFree = (i + 1) < size ? i + 1 : -1;
    }
    m_firstFree = 0;
}

// Create a new object in the pool
template<class T>
template<class... Args>
uint32_t Pool<T>::createObject(Args&&... args) {
    if (m_firstFree == -1) {
        std::cout << "Capacity overflow" << std::endl;
        m_data.push_back(PoolItem<T>());
        m_data.back().index = static_cast<int>(m_data.size()) - 1;
    }

    uint32_t index = m_firstFree;
    PoolItem<T>& newItem = m_data[index];
    m_firstFree = newItem.nextFree;

    new(&newItem.object) T(std::forward<Args>(args)...);
    newItem.isAvailable = false;
    newItem.nextObject = m_firstObject;
    newItem.prevObject = -1;

    if (m_firstObject != -1) m_data[m_firstObject].prevObject = index;
    m_firstObject = index;
    ++m_size;

    return index;
}

// Access an object in the pool
template<class T>
T& Pool<T>::operator[](uint32_t i) {
    return m_data[i].object;
}

template<class T>
T* Pool<T>::getItemAt(uint32_t i) {
    return &m_data[i].object;
}

// Remove an object from the pool
template<class T>
void Pool<T>::remove(int i) {
    --m_size;
    PoolItem<T>& item = m_data[i];
    item.nextFree = m_firstFree;
    item.isAvailable = true;
    m_firstFree = i;

    if (item.prevObject != -1) {
        m_data[item.prevObject].nextObject = item.nextObject;
    }
    else {
        m_firstObject = item.nextObject;
    }

    if (item.nextObject != -1) {
        m_data[item.nextObject].prevObject = item.prevObject;
    }
}

// Get the first used object in the pool
template<class T>
T* Pool<T>::getFirstItem() {
    return m_firstObject != -1 ? &m_data[m_firstObject].object : nullptr;
}

// Get the next object in the chain of used items
template<class T>
T* Pool<T>::getNext(T*& item) {
    if (item) {
        int nextIndex = m_data[item->index].nextObject;
        item = nextIndex != -1 ? &m_data[nextIndex].object : nullptr;
    }
    else {
        item = getFirstItem();
    }
    return item;
}

#endif // CHAINEDOBJECT_H
