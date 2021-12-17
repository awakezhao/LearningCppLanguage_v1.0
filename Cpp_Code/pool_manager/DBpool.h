#ifndef DEPOLL_H
#define DBPOLL_H

#include <iostream>
#include <list>
#include <mutex>
#include <condition_variable>
#include <map>
#include <stdint.h>

#include <mysql.h>

#define MAX_ESCAPE_STRING_LEN 10240

using namespace std;

// 返回结果 select的时候使用
class CResultSet {
public:
    CResultSet(MYSQL_RES* res);
    virtual ~CResultSet();

    bool Next();
    int GetInt(const char* key);
    char* GetString(const char* key);
private:
    int _GetIndex(const char* key);

    MYSQL_RES *m_res;
    MYSQL_ROW m_row;
    map<string, int> m_key_map;
};

// 插入数据用
class CPrepareStatement {
public:
    CPrepareStatement();
    virtual ~CPrepareStatement();

    bool Init(MYSQL* mysql, string& sql);

    void SetParam(uint32_t index, int& value);
    void SetParam(uint32_t index, uint32_t& value);
    void SetParam(uint32_t index, string& value);
    void SetParam(uint32_t index, const string& value);

    bool ExecuteUpdate();
    uint32_t GetInsertId();
private:
    MYSQL_STMT* m_stmt;
    MYSQL_BIND* m_param_bind;
    uint32_t m_param_cnt;
};

class CDBPool;

class CDBConn {
public:
    CDBConn(CDBPool* pDBPool);
    virtual ~CDBConn();
    int Init();

    // 创建表
    bool ExecuteCreate(const char* sql_query);
    // 删除表
    bool ExecuteDrop(const char* sql_query);
    // 查询
    CResultSet* ExecuteQuery(const char* sql_query);

    /*
        执行DB更新，修改
        sql_query: sql
        care_affected_rows: 是否在意影响的行数，false：不在意；true:在意
    */
    bool ExecuteUpdate(const char* sql_query, bool care_affected_rows = true);
    uint32_t GetInsertId();

    //开启事务
    bool StartTransaction();
    // 提交事务
    bool Commit();
    // 回滚事务
    bool Rollback();
    // 获取连接池名
    const char* GetPoolName();
    MYSQL* GetMysql() {return m_mysql;}

private:
    CDBPool* m_pDBPool; // to get MySql server information;
    MYSQL* m_mysql; // 对应一个连接
    char m_escape_string[MAX_ESCAPE_STRING_LEN + 1];
};

class CDBPool {
public:
    CDBPool(){}
    CDBPool(const char* pool_name, const char* db_server_ip, uint16_t db_server_port,
            const char* username, const char* password, const char* db_name, int max_conn_cnt);
    virtual ~CDBPool();

    int Init(); //连接数据库，创建连接
    CDBConn* GetDBConn(const int timeout_ms = -1); // 获取连接资源
    void RelDBConn(CDBConn* pConn); //归还连接资源

    const char* GetPoolName() { return m_pool_name.c_str(); }
    const char* GetDBServerIP() { return m_db_server_ip.c_str(); }
    uint16_t GetDBServerPort() { return m_db_server_port; }
    const char* GetUsername() { return m_username.c_str(); }
    const char* GetPassword() { return m_password.c_str(); }
    const char* GetDBName() { return m_db_name.c_str(); }
private:
    string m_pool_name;
    string m_db_server_ip;
    uint16_t m_db_server_port;
    string m_username;
    string m_password;
    string m_db_name;
    int m_db_cur_conn_cnt; //当前启用的连接数量
    int d_db_max_conn_cnt; //最大连接数
    list<CDBConn*> m_free_list;
    list<CDBConn*> m_used_list;
    std::mutex m_mutex;
    std::condition_variable m_cond_var;
    bool m_abort_request = false;
};

#endif