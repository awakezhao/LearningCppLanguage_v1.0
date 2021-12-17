#include <string>
//实现导出数据的接口，导出数据的格式包含xml，json，txt 后面可能扩展csv等
class IExport {
public:
    virtual bool Export(const std::string &data) = 0;
    virtual ~IExport(){}
};

class ExportXml : public IExport {
public:
    virtual bool Export(const std::string &data) {
        return true;
    }    
};

class ExportJson : public IExport {
public:
    virtual bool Export(const std::string &data) {
        return true;
    }    
};

class ExportTxt : public IExport {
public:
    virtual bool Export(const std::string &data) {
        return true;
    }    
};

int main() {
    std::string choose;
    std::string ctx("hello world");
    if(choose == "txt") {
        IExport *e = new ExportTxt;
        e->Export(ctx);
    } else if(choose == "json") {
        IExport *e = new ExportJson;
        e->Export(ctx);
    } else if(choose == "xml") {
        IExport *e = new ExportXml;
        e->Export(ctx);
    }
}