#pragma once
#include "usage.h"

class UniversalDataObject: public wxDataObjectSimple {
private:
    size_t m_datasize;
    char *m_data;
public:
    UniversalDataObject(const wxDataFormat& dataFormat): wxDataObjectSimple(dataFormat) {}

    size_t GetDataSize() const override;
    bool GetDataHere(void *buf) const override;
    bool SetData(size_t size, const void *data) override;

    virtual ~UniversalDataObject() noexcept {
        delete[] m_data;
    }
};