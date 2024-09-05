#include "dataobjects.h"

size_t DIFDataObject::GetDataSize() const {
    return m_datasize;
}

bool DIFDataObject::GetDataHere(void *buf) const {
    memcpy(buf, reinterpret_cast<void *>(m_data), m_datasize);
    return true;
}

bool DIFDataObject::SetData(size_t size, const void *data) {
    m_datasize = size;
    m_data = new char[size];
    memcpy(reinterpret_cast<void *>(m_data), data, size);
    return true;
}
