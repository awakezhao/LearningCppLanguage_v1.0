#include <string>

class Context {
public:
    std::string name;
    int day;
};

class IHandle {
public:
    virtual ~IHandle() {}
    void SetNextHandler(IHandle *next = nullptr){
        next = next;
    }
    bool Handle(Context &ctx) {
        if (CanHandle(ctx)) {
            return HandleRequest(ctx);
        } else if (GetNextHandler()) {
            return GetNextHandler()->HandleRequest(ctx);
        } else {

        }
    }
protected:
    virtual bool CanHandle(const Context &ctx) = 0;
    virtual bool HandleRequest(const Context &ctx) = 0;
    IHandle *GetNextHandler() {
        return next;
    }
private:
    IHandle *next;
};

class HandleByMainProgram : public IHandle {
protected:
    virtual bool HandleRequest(const Context &ctx) {

    }
    virtual bool CanHandle(const Context &ctx) {

    }
};

class HandleByProMgr : public IHandle {
protected:
    virtual bool HandleRequest(const Context &ctx) {

    }
    virtual bool CanHandle(const Context &ctx) {
        
    }
};

class HandleByBoss : public IHandle {
protected:
    virtual bool HandleRequest(const Context &ctx) {

    }
    virtual bool CanHandle(const Context &ctx) {
        
    }
};

int main() {
    IHandle *h1 = new HandleByMainProgram();
    IHandle *h2 = new HandleByProMgr();
    IHandle *h3 = new HandleByBoss();
    h1->SetNextHandler(h2);
    h2->SetNextHandler(h3);

    Context ctx;
    h1->Handle(ctx);
    return 0;
}