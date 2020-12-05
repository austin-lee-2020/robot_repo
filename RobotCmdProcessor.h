/*******************************************************************
* a Template is used to dispatch robot command. It can replace complicated
* switch case or if else (if) logic blocks.
********************************************************************/
#ifndef _ROBOTCMDPROCESSOR_H_
#define _ROBOTCMDPROCESSOR_H_

#include <vector>
using namespace std;

template <typename CProcObj>
class CRobotCmdProcessor
{
private:
    typedef void (CProcObj::*ProcFunWithNoPara)();
    typedef void (CProcObj::*ProcFunWith1Para)(void *p1);
    typedef void (CProcObj::*ProcFunWith2Paras)(void *p1, void *p2);
    enum ProcFunType
    {
        ProcFunType_UnKnown,
        ProcFunType_NoPara,
        ProcFunType_With1Para,
        ProcFunType_With2Para,
    };

    union ProcFunEntry
    {
        ProcFunWith2Paras pFunWith2Paras;
        ProcFunWith1Para pFunWith1Para;
        ProcFunWithNoPara pFunWithNoPara;
    };

    struct RobotCmdEntry
    {
        int iCtrlCmd;
        ProcFunType funType;
        ProcFunEntry funEntry;
    };

public:
    CRobotCmdProcessor(CProcObj *pObj) : m_pObj(pObj){};
    virtual ~CRobotCmdProcessor(){};

public:
#ifdef _WIN32
    template <typename Fun>
    void RegisterCmdProcFunc(const int &iCtrlCmd, Fun funEntry)
    {
            typedef int ERROR_NoProcFun[-1];
    }
#else
    template <typename Fun>
    void RegisterCmdProcFunc(const int &iCtrlCmd, Fun funEntry);
#endif

    template <>
    void RegisterCmdProcFunc<ProcFunWithNoPara>(const int &iCtrlCmd, ProcFunWithNoPara funEntry)
    {
        RobotCmdEntry rCmdEntry;
        rCmdEntry.iCtrlCmd = iCtrlCmd;
        if (typeid(ProcFunWithNoPara) == typeid(funEntry))
        {
            rCmdEntry.funEntry.pFunWithNoPara = funEntry;
            rCmdEntry.funType = ProcFunType_NoPara;
        }
        aryCmdEntries.push_back(rCmdEntry);
    }

    template <>
    void RegisterCmdProcFunc<ProcFunWith1Para>(const int &iCtrlCmd, ProcFunWith1Para funEntry)
    {
        RobotCmdEntry rCmdEntry;
        rCmdEntry.iCtrlCmd = iCtrlCmd;
        if (typeid(ProcFunWith1Para) == typeid(funEntry))
        {
            rCmdEntry.funEntry.pFunWith1Para = funEntry;
            rCmdEntry.funType = ProcFunType_With1Para;
        }
        aryCmdEntries.push_back(rCmdEntry);
    }

    template <>
    void RegisterCmdProcFunc<ProcFunWith2Paras>(const int &iCtrlCmd, ProcFunWith2Paras funEntry)
    {
        RobotCmdEntry rCmdEntry;
        rCmdEntry.iCtrlCmd = iCtrlCmd;
        if (typeid(ProcFunWith2Paras) == typeid(funEntry))
        {
            rCmdEntry.funEntry.pFunWith2Paras = funEntry;
            rCmdEntry.funType = ProcFunType_With2Para;
        }
        aryCmdEntries.push_back(rCmdEntry);
    }

    bool dispatch(int ctrlid, ...)
    {
        if (!m_pObj)
            return false;
        for (size_t i = 0; i < aryCmdEntries.size(); i++)
        {
            if(aryCmdEntries[i].iCtrlCmd == ctrlid)
            {
                if (aryCmdEntries[i].funType == ProcFunType_NoParam)
                {
                    (m_pObj->*(aryCmdEntries[i].funEntry.pFunWithNoPar))();
                    return true;
                }
                else if (aryCmdEntries[i].funType == ProcFunType_With1Para)
                {
                    va_list argptr;
                    va_start(argptr, action);
                    void *p1 = va_arg(argptr, void *);
                    va_end(argptr);
                    (m_pObj->*(aryCmdEntries[i].funEntry.pFunWith1Para))(p1);
                    return true;
                }
                else if (aryCmdEntries[i].funType == ProcFunType_With2Para)
                {
                    va_list argptr;
                    va_start(argptr, action);
                    void *p1 = va_arg(argptr, void *);
                    void *p2 = va_arg(argptr, void *);
                    va_end(argptr);
                    (m_pObj->*(aryCmdEntries[i].funEntry.pFunWith2Paras))(p1, p2);
                    return true;
                }
                else
                    return false;
                break;
            }
        }
        return false;
    };

protected:
    vector<RobotCmdEntry> aryCmdEntries;
    CProcObj *m_pObj;
};
#endif