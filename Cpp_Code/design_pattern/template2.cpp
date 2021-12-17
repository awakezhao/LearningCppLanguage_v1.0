#include <iostream>
using namespace std;

class ZooShow {
public:
    // 固定流程封装到这里
    void show() {
        Show0();
        Show1();
        Show2();
        Show3();
    }

protected:
    // 子流程 使用protected保护起来 不被客户调用 但允许子类扩展
    virtual void Show0() {
        cout << "show0" << endl;
    }

    virtual void Show1() {
        cout << "show1" << endl;
    }

    virtual void Show2() {
        cout << "show2" << endl;
    }

    virtual void Show3() {
        cout << "show3" << endl;
    }
};

class ZooShowEx : public ZooShow {
public:
    virtual void Show1() {
        cout << "show1 show1" << endl;
    }
    virtual void Show3() {
        cout << "show3 show3" << endl;
    }
};

class ZooShowEx1 : public ZooShow {
public:
    virtual void Show0() {
        cout << "show0 show0" << endl;
    }

    virtual void Show2() {
        cout << "show2 show2" << endl;
    }
};



int main() {
    ZooShow *zs = new ZooShow;
    zs->show();
    zs = new ZooShowEx1;
    zs->show();
    zs = new ZooShowEx;
    zs->show();
    return 0;
}