
#ifndef __ZMQ_YQUEUE_HPP_INCLUDE__
#define __ZMQ_YQUEUE_HPP_INCLUDE__

#include <stdlib.h>
#include <stddef.h>

#include "atomic_ptr.cpp"

template <typename T, int N>
class yqueue_t
{
public:

    inline yqueue_t() {

        begin_chunk = (chunk_t *)malloc(sizeof(chunk_t));
        alloc_assert(begin_chunk);
        begin_pos = 0;
        back_chunk = NULL; //总是指向队列中最后一个元素所在的chunk，现在还没有元素
        back_pos = 0;
        end_chunk = begin_chunk;
        end_pos = 0;

    }

    inline ~yqueue_t() {

        while(true) {
            if (begin_chunk == end_chunk) {
                free(begin_chunk);
                break;
            }
            chunk_t *o = begin_chunk;
            begin_chunk = begin_chunk->next;
            free(o);
        }

        chunk_t *sc = spare_chunk.xchg(NULL);
        free(sc);
    }

    inline T &front() {
        return begin_chunk->values[begin_pos];
    }

    inline T &back() {
        return back_chunk->values[back_pos];
    }

    inline void push() {

        back_chunk = end_chunk;
        back_pos = end_pos;

        if (++end_pos != N)
            return ;
        
        chunk_t *sc = spare_chunk.xchg(NULL);
        if (sc) {
            end_chunk->next = sc;
            sc->prev = end_chunk;
        } else {

            static int s_count = 0;
            end_chunk->next = (chunk_t *)malloc(sizeof(chunk_t));
            alloc_assert(end_chunk->next);
            end_chunk->next->prev = end_chunk;
        }
        end_chunk = end_chunk->next;
        end_pos = 0;
    }

    inline void unpush() {

        if (back_pos)
            --back_pos;
        else {
            back_pos = N - 1;
            back_chunk = back_chunk->prev;
        }

        if (end_pos)
            --end_pos;
        else {

            end_pos = N - 1;
            end_chunk = end_chunk->prev;
            free(end_chunk->next);
            end_chunk->next = NULL;
        }
    }

    inline void pop() {

        if (++begin_pos == N) { //删除满一个chunk才回收chunk

            chunk_t *o = begin_chunk;
            begin_chunk = begin_chunk->next;
            begin_chunk->prev = NULL;
            begin_pos = 0;

            chunk_t *cs = spare_chunk.xchg(o);
            free(cs);
        }
    }

private:
    struct chunk_t{
        T values[N];
        chunk_t *prev;
        chunk_t *next;
    };
    
    chunk_t *begin_chunk; //链表头结点
    int begin_pos;  // 起始点
    chunk_t *back_chunk;  //队列中最后一个元素所在链表节点
    int back_pos;  //尾部
    chunk_t *end_chunk;  //  哪来扩容的，总是指向链表的最后一个节点
    int end_pos; 

    atomic_ptr_t<chunk_t> spare_chunk; //空闲块

    yqueue_t(const yqueue_t &);
    const yqueue_t &operator=(const yqueue_t &);
};

#endif