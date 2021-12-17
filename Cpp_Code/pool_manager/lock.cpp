
void mutex_lock()
{
    pthread_mutex_t jobs_mtx;   //定义
    pthread_mutex_t blank_mutex = PTHREAD_MUTEX_INITIALIZER;
    memcpy(&jobs_mtx, &blank_mutex, sizeof(blank_mutex)); // 初始化

    pthread_mutex_lock(&jobs_mtx);  // 加锁

    pthread_mutex_unlock(&jobs_mtx); // 释放锁
}

pthread_cond_t jobs_cond;

void cond_lock(){
    
    pthread_cond_t blank_cond = PTHREAD_COND_INITIALIZER;
    memcpy(&jobs_cond, &blank_cond, sizeof(blank_cond));

    pthread_mutex_t jobs_mtx;   //定义
    pthread_mutex_t blank_mutex = PTHREAD_MUTEX_INITIALIZER;
    memcpy(&jobs_mtx, &blank_mutex, sizeof(blank_mutex)); // 初始化

    pthread_mutex_lock(&jobs_mtx);

    pthread_cond_wait(&jobs_cond, &jobs_mtx);

    pthread_mutex_unlock(&jobs_mtx);
}

void cond_notify_one()
{
    pthread_cond_signal(&jobs_cond);
}

void cond_notify_all()
{
    pthread_cond_broadcast(&jobs_cond);
}

void std_mutex()
{
    std::mutex m_mutex;
    std::lock_guard<std::mutex> lock(m_mutex);
}

void std_cond()
{
    std::mutex m_mutex;
    std::condition_variable m_cond_var;

    std::unique_lock<std::mutex> lock(m_mutex);
    m_cond_var.wait(lock)
}

void std_cond_notify_one()
{
    m_cond_var.notify_one();
}

void std_cond_notify_all()
{
    m_cond_var.notify_all();
}