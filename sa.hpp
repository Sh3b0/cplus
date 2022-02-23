#ifndef SA_H
#define SA_H

using namespace std;

namespace cplus {
    class sa {
    public:
        sa();

        bool checkRecordAccess(string id1, string id2);
        bool checkArrayAccess(string id, int idx);
        bool checkVariableAccess(string id);
        bool checkRoutineCall(string id, int nargs);
    };
}

#endif // SA_H
