#include <string>
#include <iostream>
using namespace std;
//实现导出数据的接口，导出数据的格式包含xml，json，文本格式txt，后面可能扩展excel格式csv

class IExport {
public:
    virtual bool Export(const std::string &data) = 0;
    virtual ~IExport() {}
};

class ExportXml : public IExport {
public:
    virtual bool Export(const std::string &data) {
        cout << data << endl;
        return true;
    }    
};

class ExportJson : public IExport {
public:
    virtual bool Export(const std::string &data) {
        cout << data << endl;
        return true;
    }    
};

class ExportTxt : public IExport {
public:
    virtual bool Export(const std::string &data) {
        cout << data << endl;
        return true;
    }    
};

class IExportFactory {
public:
    virtual IExport *NewExport() = 0;
};

class ExportXmlFactory : public IExportFactory {
public:
    IExport *NewExport() {
        // 可能有其他操作，或者许多参数
        IExport *temp = new ExportXml;
        // 可能之后有什么操作
        return temp;
    }
};

class ExportJsonFactory : public IExportFactory {
public:
    IExport *NewExport() {
        // 可能有其他操作，或者许多参数
        IExport *temp = new ExportJson;
        // 可能之后有什么操作
        return temp;
    }
};

class ExportTxtFactory : public IExportFactory {
public:
    IExport *NewExport() {
        // 可能有其他操作，或者许多参数
        IExport *temp = new ExportTxt;
        // 可能之后有什么操作
        return temp;
    }
};

class ExportData {
public:
    ExportData(IExportFactory *factory) : _factory(factory) {}
    ~ExportData() {
        if (_factory != nullptr) {
            delete _factory;
            _factory = nullptr;
        }
    }
    bool Export(const std::string &data) {
        IExport *e = _factory->NewExport();
        e->Export(data);
        return true;
    }
private:
    IExportFactory *_factory;
};

int main() {
    ExportData ed(new ExportTxtFactory);
    ed.Export("hello world");
    return 0;
}