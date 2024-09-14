#include "usage.h"
#include "utils.h"
#include "file_operating.h"
#include "dataobjects.h"

struct AbstractDataRecord {
private:
protected:
    wxDataObject *dataObject;
    size_t hash;

    AbstractDataRecord(wxDataObject *dataObject): dataObject{dataObject}, hash{0} {}

    virtual bool saveClipboardToDataObject() = 0;
    virtual bool saveDataObjectToBuffer() = 0;
    virtual void saveBufferToFile() = 0;

    virtual void saveHashFromBuffer() = 0;
    virtual void onNotSave() {}
public:
    void operator() () {
        if (saveClipboardToDataObject()) {
            if (saveDataObjectToBuffer()) {
                saveHashFromBuffer();
                saveBufferToFile();
            } else {
                onNotSave();
            }
        }
    }
    virtual ~AbstractDataRecord() noexcept {
        delete dataObject;
    }
};

struct AbstractBufferBaseDataRecord: public AbstractDataRecord {
protected:
    size_t size{0};
    std::unique_ptr<std::string> buffer;
    AbstractBufferBaseDataRecord(wxDataObject *dataObject): AbstractDataRecord(dataObject), buffer{nullptr} {}
};

struct UnicodeDataRecord: public AbstractBufferBaseDataRecord {
protected:
    bool saveClipboardToDataObject() override;
    bool saveDataObjectToBuffer() override;
    void saveBufferToFile() override;
    void saveHashFromBuffer() override;
public:
    UnicodeDataRecord(): AbstractBufferBaseDataRecord{new wxTextDataObject{}} {}
};


struct HTMLDataRecord: public AbstractBufferBaseDataRecord {
protected:
    bool saveClipboardToDataObject() override;
    bool saveDataObjectToBuffer() override;
    void saveBufferToFile() override;
    void saveHashFromBuffer() override;
public:
    HTMLDataRecord(): AbstractBufferBaseDataRecord{new UniversalDataObject{wxDF_HTML}} {}
};

struct ImageDataRecord: public AbstractDataRecord {
private:
    std::unique_ptr<wxBitmap> bitmap;
protected:
    bool saveClipboardToDataObject() override;
    bool saveDataObjectToBuffer() override;
    void saveBufferToFile() override;
    void saveHashFromBuffer() override;
public:
    ImageDataRecord(): AbstractDataRecord{new wxBitmapDataObject{}}, bitmap{nullptr} {}
};

struct DIFDataRecord: public AbstractBufferBaseDataRecord {
protected:
    bool saveClipboardToDataObject() override;
    bool saveDataObjectToBuffer() override;
    void saveBufferToFile() override;
    void saveHashFromBuffer() override;
public:
    DIFDataRecord(): AbstractBufferBaseDataRecord(new UniversalDataObject(wxDF_DIF)) {}
};

struct TextDataRecord: public AbstractDataRecord {
private:
    std::unique_ptr<wxString> buffer;
protected:
    bool saveClipboardToDataObject() override;
    bool saveDataObjectToBuffer() override;
    void saveBufferToFile() override;
    void saveHashFromBuffer() override;
public:
    TextDataRecord(): AbstractDataRecord(new wxTextDataObject()), buffer{nullptr} {}
};