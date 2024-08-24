#include "usage.h"


class CountFilesDirTraverser: public wxDirTraverser {
    size_t count = 0;
public:
    wxDirTraverseResult OnDir(const wxString& dirname) override;
    wxDirTraverseResult OnFile(const wxString& filename) override;
    size_t getCount();

};

class RemoveOldestDirTraverser: public wxDirTraverser {
public:
    wxDirTraverseResult OnDir(const wxString& dirname) override;
    wxDirTraverseResult OnFile(const wxString& filename) override;
};

bool removeOldest(const wxString& directory);

wxString getOldest(const wxString& directory);

bool checkIfToClean(const wxString& directory);