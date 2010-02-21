#include <stdlib.h>
#include <stdio.h>

#include "ringbuffer.h"

//====================================================
// MT_ring_iterator
//====================================================
/** Default constructor.  This is probably never explicitly
    called. */
MT_ring_iterator::MT_ring_iterator()
{
    N = 0;
    value = 0;
    fullto = 0;
}

/** Constructor to set N and value. */
MT_ring_iterator::MT_ring_iterator(int iN, int set)
{
    N = iN;
    setvalue(set);
    fullto = 0;
}

/** Constructor to set N, value, and fullto.  Useful for
    assignment operators. */
MT_ring_iterator::MT_ring_iterator(int iN, int set, unsigned int ift)
{
    N = iN;
    setvalue(set);
    fullto = ift;
}

/** Constructor to initialize an operator with N, value = 0,
    and fullto. */
void MT_ring_iterator::init(unsigned int iN, unsigned int ift)
{
    N = iN;
    value = 0;
    fullto = ift;
}

/** Function to call when the length of the buffer has changed.  
    Must handle fullto and value correctly. */
void MT_ring_iterator::setlength(unsigned int iN)
{
    N = iN;

    // If we are shortening the buffer, make sure the position
    //  and fullto are wthin range
    if(fullto > N-1)
        fullto = N-1;       // Set to the new length
    if(value > N-1)
        value = 0;          // Go back to the beginning
}

/** Unary prepended increment operator (mod N). */
MT_ring_iterator& MT_ring_iterator::operator++()
{
    if(++value >= N)      // Increment the value, mod N.
        value = 0;
    return *this;
}

/** Unary appended increment operator (mod N). */
MT_ring_iterator MT_ring_iterator::operator++(int dummy)
{
    MT_ring_iterator out = *this;    // First copy the value
    ++(*this);                    // Use the prepend operator
    return out;                   //   for consistency.
}

/** Unary prepended decrement operator (mod fullto). */
MT_ring_iterator& MT_ring_iterator::operator--()
{
    // Going backwards is different than going forwards.
    //   We shouldn't wrap around into the unwritten parts
    //   of the buffer.
    if(value == 0 && fullto == 0) // This handles the
        value = 0;                  //   initialization case
    else if(value == 0)           // When we reach zero
        value = fullto-1;           //   wrap back to fullto-1
    else value--;                 // Otherwise just decrement

    return *this;
}

/** Unary appended decrement operator (mod fullto). */
MT_ring_iterator MT_ring_iterator::operator--(int dummy)
{
    MT_ring_iterator out = *this;    // First copy the value
    --(*this);                    // Use the prepend operator
    return out;                   //   for consistency.
}

/** Assignment operator (int).  Sets the value to
    input mod N. */
MT_ring_iterator& MT_ring_iterator::operator=(int set)
{
    setvalue(set);
    return *this;
}

/** Assignment operator (MT_ring_iterator).  Copies the input
    iterator. */
MT_ring_iterator MT_ring_iterator::operator=(MT_ring_iterator& set)
{
    // Use the handy constructor to copy the value.
    return MT_ring_iterator(set.N, set.value, set.fullto);
}

/** Function to set the value mod N.  Handles negatives correctly. */
void MT_ring_iterator::setvalue(int set)
{
    if(set >= 0)          // For positives, just use the mod operator
        value = set % N;
    else                  // For negatives, mod back from N
        value = ((set + N) % N);

}

/** Function to increment the fullto value.  But don't let it
    be greater than N. */
void MT_ring_iterator::incfullto()
{
    if(fullto < N)  // Don't go past N
        fullto++;
}

/** Function to get the value of the iterator.  This is 
    normally not used as the casting operator is given also. */
unsigned int MT_ring_iterator::getvalue() const
{
    return value;
}

/** Addition with iterator operator.  Uses left argument's
    parameters. */
MT_ring_iterator MT_ring_iterator::operator+(const MT_ring_iterator right)
{
    return MT_ring_iterator(N,value + right.getvalue(), fullto);
}

/** Addition with int operator.  Keeps argument's parameters. */
MT_ring_iterator MT_ring_iterator::operator+(const int right)
{
    return MT_ring_iterator(N,value + right, fullto);
}

/** Subtraction with iterator operator.  Uses left argument's
    parameters. */
MT_ring_iterator MT_ring_iterator::operator-(const MT_ring_iterator right)
{
    return MT_ring_iterator(N,value - right.getvalue());
}

/** Subtraction with int operator.  Keep's argument's parameters. */
MT_ring_iterator MT_ring_iterator::operator-(const int right)
{
    return MT_ring_iterator(N,value - right);
}


//====================================================
// MT_ringbuffer
//====================================================

/** Constructor to specify size.  This is really the only one
    we need. */
template <class T> 
MT_ringbuffer<T>::MT_ringbuffer(unsigned int size)
{
    // we shouldn't specify size = 0, but just in case...
    if(size == 0)
        size = 1;
    N = size;
    fullto = 0;
    write_at.init(N,fullto);
    data.resize(N,0.0);
}

/** Function to push data to the next logical spot.  Fills
    the buffer then wraps back around to the beginning. */
template <class T> 
void MT_ringbuffer<T>::push(T newdata)
{
    data.at(write_at++) = newdata;
    if(fullto < N){          // Update the fullto counter
        fullto++;              //   in the class and in the
        write_at.incfullto();  //   write iterator.
    }
}

/** Function to increase the length of the vector and add
    a value at the new final position. */
template <class T>
void MT_ringbuffer<T>::append(T newdata)
{
    data.push_back(newdata);
    N++;
    fullto = N-1;
    write_at.setlength(N);
    write_at.incfullto();
    write_at = N-1;
}

/** Function to get the last added value.  Returns the value
    in the vector that was just written. */
template <class T> 
T MT_ringbuffer<T>::getlast()
{
    return data.at(write_at - 1);
}

/** Function to execute f(T x) for each value in the buffer.
    Starting from the last written value and either stopping
    at zero (if the buffer is not full) or wrapping back around
    until it reaches the next value to be written. */
template <class T>
void MT_ringbuffer<T>::ForEach(void (*f)(T x))
{
    MT_ring_iterator read_at = write_at; // Use an iterator
    int stop = --read_at;             // starting at the last written
    do
    {
        (*f)(data.at(read_at));            // execute f
    } while(--read_at != stop);       // until we get back to here
}

/** Function to execute f(int n, T x) for each value in the buffer.
    Starting from the last written value and either stopping
    at zero (if the buffer is not full) or wrapping back around
    until it reaches the next value to be written. */
template <class T>
void MT_ringbuffer<T>::ForEach(void (*f)(int n,T x))
{
    MT_ring_iterator read_at = write_at; // Use an iterator
    int stop = --read_at;             // starting at the last written
    do
    {
        (*f)((int) read_at, data.at(read_at));  // execute f
    } while(--read_at != stop);       // until we get back to here

}

/** Function to reset the buffer to all zeros and reset counters. */
template <class T>
void MT_ringbuffer<T>::reset()
{
    for(unsigned int i = 0; i < N; i++)
        data.at(i) = 0.0;

    fullto = 0;
    write_at.init(N,0);
}

/** Function to access values relative (counting backwards) to
    the last write position. */
template <class T>
T MT_ringbuffer<T>::atrel(int loc)
{
    MT_ring_iterator j(N, write_at - 1 - loc, fullto);
    return data.at(j);
}

// Let the compiler know what kinds of types we expect to use the
//   template on.  Theoretically anything that can be cast to a
//   scalar value should work.
//template class MT_ringbuffer<double>;
template class MT_ringbuffer<float>;
template class MT_ringbuffer<double>;
/*  -- Right now we're only using double.  
    This reduces program size by ~70 kb!.
    template class ringbuffer<float>;
    template class ringbuffer<long>;
    template class ringbuffer<int>;
    template class ringbuffer<unsigned long>;
    template class ringbuffer<unsigned int>;
    template class ringbuffer<char>;
    template class ringbuffer<unsigned char>;
*/
