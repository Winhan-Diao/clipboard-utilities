#pragma once
#include "usage.h"

class DIFDataObject: public wxDataObjectSimple {
private:
    size_t m_datasize;
    char *m_data;
public:
    DIFDataObject(): wxDataObjectSimple(wxDF_DIF) {}
    
    size_t GetDataSize() const override;
    bool GetDataHere(void *buf) const override;
    bool SetData(size_t size, const void *data) override;

    virtual ~DIFDataObject() noexcept {
        delete[] m_data;
    }
};