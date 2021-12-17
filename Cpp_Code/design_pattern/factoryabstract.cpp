#include <string>

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

class ExportTxt : public IExport {
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

class IImport {
public:
    virtual bool Import(const std::string &data) = 0;
    virtual ~IImport(){}
};

class ImportXml : public IImport {
public:
    virtual bool Import(const std::string &data) {
        return true;
    }
};

class ImportJson : public IImport {
public:
    virtual bool Import(const std::string &data) {
        return true;
    }
};

class ImportTxt : public IImport {
public:
    virtual bool Import(const std::string &data) {
        return true;
    }
};

class IDataApiFactory {
public:
    IDataApiFactory() {
        _export = nullptr;
        _import = nullptr;
    }
    virtual ~IDataApiFactory() {
        if(_export) {
            delete _export;
            _export = nullptr;
        }
        if(_import) {
            delete _import;
            _import = nullptr;
        }
    }

    bool Export(const std::string &data) {
        if(_export == nullptr) {
            _export = NewExport();
        }
        return _export->Export(data);
    }

    bool Import(const std::string &data) {
        if(_import == nullptr) {
            _import = NewImport();
        }
        return _import->Import(data);
    }

protected:
    virtual IExport *NewExport() = 0;
    virtual IImport *NewImport() = 0;
private:
    IExport *_export;
    IImport *_import;
};

class XmlApiFactory : public IDataApiFactory {
protected:
    virtual IExport *NewExport() {
        IExport *temp = new ExportXml;
        return temp;
    }

    virtual IImport *NewImport() {
        IImport *temp = new ImportXml;
        return temp;
    }
};

class JsonApiFactory : public IDataApiFactory {
protected:
    virtual IExport *NewExport() {
        IExport *temp = new ExportJson;
        return temp;
    }

    virtual IImport *NewImport() {
        IImport *temp = new ImportJson;
        return temp;
    }
};

class TxtApiFactory : public IDataApiFactory {
protected:
    virtual IExport *NewExport() {
        IExport *temp = new ExportTxt;
        return temp;
    }

    virtual IImport *NewImport() {
        IImport *temp = new ImportTxt;
        return temp;
    }
};

int main() {
    IDataApiFactory *factory = new XmlApiFactory();
    factory->Export("Hello world");
    factory->Import("hello world");
    return 0;
}