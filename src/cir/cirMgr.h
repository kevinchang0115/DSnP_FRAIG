/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"

extern CirMgr *cirMgr;
class CirGateV;
class CirPiGate;
class CirPoGate;
class CirAigGate;

class CirMgr
{
public:
   CirMgr() : nmax(0), ni(0), nl(0), no(0), ng(0) {}
   ~CirMgr() { clear(); }

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const;

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit optimization
   void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs() const;
   void writeAag(ostream&) const;

private:
   ofstream*                  _simLog;
   ifstream                   _cirfile;
   CirGate**                  allList;
   vector<CirPiGate*>         piList;
//   vector<CirAigGate*>      aigList;
   vector<CirPoGate*>         poList;
   vector<int>                floatingList;
   vector<int>                notUsedList;
   mutable vector<CirGate*>   dfsList;
   int                        nmax, ni, nl, no;
   mutable int                ng;
   vector< vector<CirGateV> > fecGroups;
   // private member function
   void dfsScan() const;
   void dfsSearch(CirGate*) const;
   void clear() const;
   bool deleteNotUsed();
   void updateNotUsed();
   void updateFloating();
   bool optConst(CirAigGate*);
   bool optSameInvFanin(CirAigGate*);
   void deleteAig(CirAigGate*);
   void emergeAig(CirAigGate*, CirAigGate*);
   void patternSim(string*);
   bool patternCheck(string p);
   void updateFecGroups();
};

#endif // CIR_MGR_H
