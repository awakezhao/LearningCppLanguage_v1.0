class Context {

};

class ProStategy {
public:
    virtual double CalcPro(const Context &ctx) = 0;
    virtual ~ProStategy();
};

class VAC_Sprint : public ProStategy {
public:
    virtual double CalcPro(const Context &ctx){}
};

class VAC_Qixi : public ProStategy {
public:
    virtual double CalcPro(const Context &ctx){}
};

class VAC_Wuyi : public ProStategy {
public:
    virtual double CalcPro(const Context &ctx){}
};

class VAC_GuoQing : public ProStategy {
public:
    virtual double CalcPro(const Context &ctx){}
};

class VAC_Shengdan : public ProStategy {
public:
    virtual double CalcPro(const Context &ctx){}
};

class VAC_Qingrenjie : public ProStategy {
public:
    virtual double CalcPro(const Context &ctx){}
};


//稳定的 变化的
class Promotion {
public:
    Promotion(ProStategy *sss) : _s(sss){}
    ~Promotion(){}
    double CalcPromotion(const Context &ctx) {
        return _s->CalcPro(ctx);
    }
private:
    ProStategy *_s;
};

int main() {
    Context ctx;
    ProStategy *s = new VAC_GuoQing();
    Promotion *p = new Promotion(s);
    p->CalcPromotion(ctx);
    return 0;
}