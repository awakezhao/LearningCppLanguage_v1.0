


#ifndef __ZMQ_YPIPE_HPP_INCLUDED__
#define __ZMQ_YPIPE_HPP_INCLUDED__

#include "atomic_ptr.cpp"
#include "yqueue.cpp"


template <typename T, int N>
class ypipe_t {
public:

    inline ypipe_t() {

        queue.push(); //yqueue_t的尾指针+1，开始back_chunk为空，现在back_chunk指向第一个chunk_t块的第一个位置

        r = w = f = &queue.back(); //就是让r, w, f, c四个指针都指向这个end迭代器
        c.set(&queue.back());
    }

    inline virtual ~ypipe_t()
    {}

#ifdef ZMQ_HAVE_OPENVMS
#pragma message save
#pragma message disable(UNINIT)
#endif
    inline void write(const T &value_, bool incomplete_) {

        queue.back() = value_;
        queue.push();

        if (!incomplete_)
            f = &queue.back();  // 记录要刷新的位置
    }

#ifdef ZMQ_HAVE_OPENVMS
#pragma message restore
#endif
    inline bool unwrite(T *value_) {

        if (f == &queue.back())
            return false;

        queue.unpush();

        *value_ = queue.back();
        return true;
    }

    inline bool flush() {

        if (w == f)  //不需要刷新，即是还没有新元素加入
            return true;

        if (c.cas(w, f) != w) { // 尝试将c设置为f，即是准备更新w的位置

            //  Compare-and-swap was unseccessful because 'c' is NULL.
            //  This means that the reader is asleep. Therefore we don't
            //  care about thread-safeness and update c in non-atomic
            //  manner. We'll return false to let the caller know
            //  that reader is sleeping.
            c.set(f);
            w = f;
            return false; // 线程看到flush返回false之后会发送一个消息给读线程，这个是需要写业务去做处理
        }

        w = f;
        return true;
    }

    //  Check whether item is available for reading.
    // 这里面有两个点，一个是检查是否有数据可读，一个是预取
    inline bool check_read() {

        //  Was the value prefetched already? If so, return.
        if (&queue.front() != r && r) //判断是否在前几次调用read函数时已经预取数据了return true;
            return true;

        //  There's no prefetched value, so let us prefetch more values.
        //  Prefetching is to simply retrieve the
        //  pointer from c in atomic fashion. If there are no
        //  items to prefetch, set c to NULL (using compare-and-swap).
        r = c.cas(&queue.front(), NULL); //尝试预读

        //  If there are no elements prefetched, exit.
        //  During pipe's lifetime r should never be NULL, however,
        //  it can happen during pipe shutdown when items
        //  are being deallocated.
        if (&queue.front() == r || !r)
            return false;

        // There was at least one value prefetched.
        return true;
    }

    inline bool read(T *value_) {

        if (!check_read())
            return false;

        *value_ = queue.front();
        queue.pop();

        return true;
    }

    //  Applies the function fn to the first elemenent in the pipe
    //  and returns the value returned by the fn.
    //  The pipe mustn't be empty or the function crashes.
    inline bool probe(bool (*fn)(T &)) {

        bool rc = check_read();

        return (*fn)(queue.front());
    }

protected:

    yqueue_t<T, N> queue;

    T *w; // 指向第一个未刷新的元素，只被写线程使用

    T *r; // 指向第一个还没预提取的元素，只被写线程使用

    T *f; // 指向下一轮要被刷新的一批元素中的第一个

    atomic_ptr_t<T> c; //读写线程共享指针，指向每一轮刷新的起点。当C为空时，表示读线程睡眠（只会在读线程中被设置为空）

    ypipe_t(const ypipe_t &);
    const ypipe_t &operator=(const ypipe_t &);
};


#endif