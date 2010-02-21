#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <vector>

/* An automatic iterator class for the MT_ringbuffer that
   acts as an int.  Can be used to keep track of the
   buffer's fullness and automatically takes care of
   wraparounds.*/
class MT_ring_iterator
{
private:
    unsigned int value;   //!< Actual value of iterator
    unsigned int N;       //!< Total number of elements
                          //!<   in the corresponding buffer
    unsigned int fullto;  //!< How full the buffer is

public:
    // Constructors
    MT_ring_iterator();
    MT_ring_iterator(int iN, int set);
    MT_ring_iterator(int iN, int set, unsigned int ift);

    // Operators
    MT_ring_iterator& operator++();
    MT_ring_iterator operator++(int dummy);
    MT_ring_iterator& operator--();
    MT_ring_iterator operator--(int dummy);
    MT_ring_iterator& operator=(int set);
    MT_ring_iterator operator=(MT_ring_iterator& set);
    operator int() const { return value; };
    MT_ring_iterator operator +(const MT_ring_iterator right);
    MT_ring_iterator operator +(const int righ);
    MT_ring_iterator operator -(const MT_ring_iterator right);
    MT_ring_iterator operator -(const int righ);

    // Normal member functions
    void init(unsigned int iN, unsigned int ift);
    unsigned int getvalue() const;
    void setvalue(int set);
    void incfullto();
    void setlength(unsigned int iN);

};

/* A circular buffer template that fills up to a predetermined
   maximum size.  Using a MT_ring_iterator as much of the
   background work is automated as possible. */
template <class T>
class MT_ringbuffer
{
private:
    unsigned int N;         //!< Total number of elements.
    MT_ring_iterator write_at; //!< Automatic iterator.
    std::vector<T> data;    //!< Vector containing data.

protected:

public:
    unsigned int fullto;    //!< How many elements are currently full.
    // Constructors
    MT_ringbuffer(unsigned int N);

    // Operators
    /** Subscript operator - returns value at position relative 
        to current. */
    T operator[](int index){ return atrel(index); }; 
    
    // Normal member functions
    void push(T newdata);
    void append(T newdata);
    T getlast();
    void reset();
    T atrel(int loc);

    // Functions to handle function pointers
    void ForEach(void (*f)(T x));
    void ForEach(void (*f)(int n, T x));

};

#endif
