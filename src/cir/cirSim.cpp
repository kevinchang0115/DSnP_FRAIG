/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include <string>
#include <cmath>

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

bool
myStr2Int(const string& str, int& num, int order)
{
   num = 0;
   size_t i = 0;
   int sign = 1;
   if (str[0] == '-') { sign = -1; i = 1; }
   bool valid = false;
   for (; i < str.size(); ++i) {
      if (isdigit(str[i])) {
         num *= order;
         num += int(str[i] - '0');
         valid = true;
      }
      else return false;
   }
   num *= sign;
   return valid;
}

class SimKey
{
public:
   SimKey() {}
   SimKey(const CirGateV& g) { _simValue = g.gate()->getSimValue(); }
   ~SimKey() {}
   size_t operator() () const { return _simValue*(~_simValue); }
   bool operator == (const SimKey& k) const { return (_simValue == k._simValue) || (_simValue == ~k._simValue); }
   bool isInv(SimKey& k) const { return _simValue == ~k._simValue; }

private:
   size_t _simValue;
};

size_t
decideLimit(vector< vector<CirGateV> >& groups, size_t base)
{
   size_t p = log10(base);
   for (size_t i=0, n=groups.size(); i<n; ++i)
      p += log(groups[i].size());
   return p;
}
/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
      // construct AIG list
   vector<CirAigGate*> dfsAigList;   
   for (size_t i = 0, n = dfsList.size(); i<n; ++i)
      if (dfsList[i]->getTypeEnum() == AIG_GATE)
         dfsAigList.push_back(static_cast<CirAigGate*>(dfsList[i]));
   if (fecGroups.size() == 0) {
         // construct the first fec group (contains all aig gates and const0)
      vector<CirGateV> fecGroup;
      fecGroup.push_back(allList[0]);
      for (size_t i=0, n=dfsAigList.size(); i<n; ++i)
         fecGroup.push_back(dfsAigList[i]);
      fecGroups.push_back(fecGroup);
   }
      // start hashing
   size_t failLimit = 40;    // this should be detemined by the complexity of the circuit
   size_t failNum = 0;
   size_t patNum = 0;
   while (failNum < failLimit) {
      vector< vector<CirGateV> > fecTmpGroups;
      bool flag = false;
         // set PI simulation
      for (size_t i=0, n=piList.size(); i<n; ++i) {
         size_t ranSim = size_t(rnGen(INT_MAX)) + (size_t(rnGen(INT_MAX))<<32);
         piList[i]->setSimValue(ranSim);
         }
         // set AIG simulation
      for (size_t i=0, n=dfsAigList.size(); i<n; ++i)
         dfsAigList[i]->setSimValue();   
         // set PO simulation
      for (size_t i=0, n=poList.size(); i<n; ++i)
         poList[i]->setSimValue();                  
         // divide into HashMap
      for (size_t i=0, n=fecGroups.size(); i<n; ++i) {
         size_t k=fecGroups[i].size();
         HashMap<SimKey, CirGateV> simHash(getHashSize(k));
         for (size_t j=0; j<k; ++j)
            simHash.insert(fecGroups[i][j]);
         if (!simHash.checkAll(fecTmpGroups))    // find fec groups
            flag = true;    // no new groups found
      }
      if (flag)
         ++failNum;
         // update fecGroups
      fecGroups.clear();
      for (size_t i=0, n=fecTmpGroups.size(); i<n; ++i)
         fecGroups.push_back(fecTmpGroups[i]);
      failLimit = decideLimit(fecTmpGroups, dfsAigList.size());
      patNum += 32;
      updateFecGroups();
   }
/*
   for (size_t i=0, n=piList.size(); i<n; ++i) {
      cout << piList[i]->getSimValue() << endl;
      cout << simToBinary(piList[i]->getSimValue()) << endl;
   }
*/
/*
   for (size_t i=0, n=dfsAigList.size(); i<n; ++i) {
      cout << dfsAigList[i]->getSimValue() << endl;
      cout << simToBinary(dfsAigList[i]->getSimValue()) << endl;
   }
*/
   cout << "MAX_FAILS = " << failLimit << endl;
   cout << patNum << " patterns simulated." << endl; 
//   cout << fecGroups.size() << " fec groups produced." <<endl;
}
/*
myStrGetTok(const string& str, string& tok, size_t pos = 0,
            const char del = ' ')
*/
void
CirMgr::fileSim(ifstream& patternFile)
{
   // construct AIG list
   vector<CirAigGate*> dfsAigList;   
   for (size_t i = 0, n = dfsList.size(); i<n; ++i)
      if (dfsList[i]->getTypeEnum() == AIG_GATE)
         dfsAigList.push_back(static_cast<CirAigGate*>(dfsList[i]));
   // start simulation from file
   string line, pattern[32];
   size_t num = 0, patNum = 0;
   bool flag = true;
   while (getline(patternFile, line) && flag) {
      if (line == "")
         continue;
      string tok;
      for (size_t i=0, pos=0, n=num; pos != string::npos; ++i) {
         pos = myStrGetTok(line, tok, pos);
         if (!patternCheck(tok)) {
            flag = false; break;
         }
         pattern[n+i] = tok;  ++num;
      }
      if (num%32 == 0 && flag) {
         // set PI simulaton
         patternSim(pattern);
         if (fecGroups.size() == 0) {
            // construct the first fec group (contains all aig gates and const0)
            vector<CirGateV> fecGroup;
            fecGroup.push_back(allList[0]);
            for (size_t i=0, n=dfsAigList.size(); i<n; ++i)
               fecGroup.push_back(dfsAigList[i]);
            fecGroups.push_back(fecGroup);
         }
         vector< vector<CirGateV> > fecTmpGroups;
            // set AIG simulation
         for (size_t i=0, n=dfsAigList.size(); i<n; ++i)
            dfsAigList[i]->setSimValue();
            // set PO simulation
         for (size_t i=0, n=poList.size(); i<n; ++i)
            poList[i]->setSimValue();
         if (_simLog) {
            for (size_t i=0; i<32; ++i) {
               *_simLog << pattern[i] << " ";             
               for (size_t j=0, n=poList.size(); j<n; ++j)
                  *_simLog << (poList[j]->getSimValue()>>(31-i))%2;
               *_simLog << endl;
            }
         }
            // divide into hashMap
         for (size_t i=0, n=fecGroups.size(); i<n; ++i) {
            size_t k=fecGroups[i].size();
            HashMap<SimKey, CirGateV> simHash(getHashSize(k));
            for (size_t j=0; j<k; ++j)
               simHash.insert(fecGroups[i][j]);
            // find fec groups
            simHash.checkAll(fecTmpGroups);
         }
         fecGroups.clear();
         for (size_t i=0, n=fecTmpGroups.size(); i<n; ++i)
            fecGroups.push_back(fecTmpGroups[i]);
         patNum += 32;
         num = 0;
         updateFecGroups();
      }
   }
   cout << patNum << " patterns simulated." << endl;    
//   cout << fecGroups.size() << " fec groups produced." <<endl;
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
void
CirMgr::patternSim(string* p)
{
   string simValue[ni];
   for (size_t i=0; i<32; ++i)
      for (int j=0; j<ni; ++j)
         simValue[j] += p[i][j];
/*
   for (int i=0; i<ni; ++i)
      cout << simValue[i] << endl;
*/
   for (int i=0; i<ni; ++i) {
      int sim;
      myStr2Int(simValue[i], sim, 2);
      piList[i]->setSimValue(sim);
   }
}

bool
CirMgr::patternCheck(string p)
{
   if (int(p.size()) != ni) {
      cout << "Error: Pattern(" << p << ") length(" << p.size()
           << ") does not match the number of inputs(" << ni
           << ") in a circuit!!" << endl;
      return false;
   }
   for (size_t j=0, n=p.size(); j<n; ++j)
      if (p[j] != '0' && p[j] != '1') {
         cout << "Error: Pattern(" << p << ") contains a non-0/1 character('"
              << p[j] << "')." << endl;
         return false;
      }
   return true;
}

void
CirMgr::updateFecGroups()
{
   for (size_t i=0, n=fecGroups.size(); i<n; ++i) {
      for (size_t j=0, m=fecGroups[i].size(); j<m; ++j) {
         fecGroups[i][j].gate()->deleteFecGroup();
         for (size_t k=0; k<m; ++k)
            if (k!=j)
               fecGroups[i][j].gate()->addFecGroup(fecGroups[i][k]);
      }
   }
}
