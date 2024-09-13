#include "dataobjects.h"

size_t UniversalDataObject::GetDataSize() const {
    return m_datasize;
}

bool UniversalDataObject::GetDataHere(void *buf) const {
    general_usage::debug(wxString{"[UniversalDataObject] "} << "GetDataHere called");       //debug
    memcpy(buf, reinterpret_cast<void *>(m_data), m_datasize);
    return true;
}

bool UniversalDataObject::SetData(size_t size, const void *data) {
    m_datasize = size;
    m_data = new char[size];
    memcpy(reinterpret_cast<void *>(m_data), data, size);
    return true;
}
