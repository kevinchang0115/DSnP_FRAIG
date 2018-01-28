/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;
size_t _globalRef = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine const (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
CirGate* 
CirMgr::getGate(unsigned gid) const
{
   if (int(gid) <= nmax+no)
      return allList[gid];
   else 
      return 0;
}
/*
USING:
size_t 
myStrGetTok(const string& str, string& tok, size_t pos = 0,
            const char del = ' ')
bool
myStr2Int(const string& str, int& num)
*/
bool
CirMgr::readCircuit(const string& fileName)
{
   // open file
   _cirfile.open(fileName.c_str());
   if (!_cirfile.is_open()) {
      _cirfile.close();
      cerr << "Cannot open design \"" << fileName << "\"!!" << endl;
      return false;
   }

   // parse the circuit
   string dummy;
   _cirfile >> dummy >> nmax >> ni >> nl >> no >> ng;
   int iid;
   int* gid = new int[ng];
   int* gin1 = new int[ng];
   int* gin2 = new int[ng];
   int* oin = new int[no];
   size_t s1, s2;
      // construct all gate list
   allList = new CirGate*[nmax+no+1];
   for (int i= 0; i<nmax+no+1; ++i)
      allList[i] = 0;
      // construct const gate
   allList[0] = new CirConstGate();
      // construct pi gates
   for (int i=0; i<ni; ++i) {
      _cirfile >> iid;
      CirPiGate* newPiGate = new CirPiGate(iid/2, i+2);
      allList[iid/2] = newPiGate;
      piList.push_back(newPiGate);
   }
      // remember po's fanin
   for (int i=0; i<no; ++i)
      _cirfile >> oin[i];
      // construct aig gates
   for (int i=0; i<ng; ++i) {
      _cirfile >> gid[i] >> gin1[i] >>gin2[i];
      CirAigGate* newAigGate = new CirAigGate(gid[i]/2, i+2+ni+no);
      allList[gid[i]/2] = newAigGate;
   }
      // construct po gates
   for (int i=0; i<no; ++i) {
      CirPoGate* newPoGate = new CirPoGate(nmax+i+1, i+2+ni);
      allList[nmax+i+1] = newPoGate;
      poList.push_back(newPoGate);
   }
      // handle the symbol part
   _cirfile.getline(buf, 1024);
   bool check = true;
   while (_cirfile.getline(buf, 1024) && check) {
      string s = buf;
      string tok;
      int n;
      size_t pos = myStrGetTok(s, tok);
      myStr2Int(s.substr(1,pos), n);
      switch (s[0]) {
         case 'i':
            piList[n]->setName(s.substr(pos+1));
            break;
         case 'o':
            poList[n]->setName(s.substr(pos+1));
            break;
         case 'c':
            check = false;
            break;
      }
   }
      // read complete
      // connect aig gates
   for (int i=0; i<ng; ++i) {
      CirAigGate* aig = static_cast<CirAigGate*>(allList[gid[i]/2]);
      CirGate* in1 = allList[gin1[i]/2];
      CirGate* in2 = allList[gin2[i]/2];
      s1 = gin1[i] % 2;   s2 = gin2[i] % 2;
         // take care undef problem
      if (!in1 || !in2)
         floatingList.push_back(gid[i]/2);         
      if (!in1 || in1->getTypeEnum() == UNDEF_GATE) {
         if (!in1) {
            CirUndefGate* newUndefGate = new CirUndefGate(gin1[i]/2);
            allList[gin1[i]/2] = newUndefGate;
         }
         aig->setUndef1();    
         aig->setUndefId1(gin1[i]/2);
      }
      in1 = allList[gin1[i]/2];
      in1->addFanout(CirGateV(aig, s1));
      if (!in2 || in2->getTypeEnum() == UNDEF_GATE) {
         if (!in2) {
            CirUndefGate* newUndefGate = new CirUndefGate(gin2[i]/2);
            allList[gin2[i]/2] = newUndefGate;
         }
         aig->setUndef2();
         aig->setUndefId2(gin2[i]/2);
      }
      in2 = allList[gin2[i]/2];
      in2->addFanout(CirGateV(aig, s2));
      aig->addFanin(CirGateV(in1, s1));
      aig->addFanin(CirGateV(in2, s2));
   }
      // connect po gates
   for (int i=0; i<no; ++i) {
      CirPoGate* po = static_cast<CirPoGate*>(allList[i+nmax+1]);
      CirGate* in = allList[oin[i]/2];
      s1 = oin[i] % 2;
         // take care undef problem
      if (!in || in->getTypeEnum() == UNDEF_GATE) {
         if (!in) {
            CirUndefGate* newUndefGate = new CirUndefGate(oin[i]/2);
            allList[oin[i]/2] = newUndefGate;            
         }
         po->setUndef();
         po->setUndefId(oin[i]/2);
         floatingList.push_back(i+nmax+1);
      }
      in = allList[oin[i]/2];
      in->addFanout(CirGateV(po, s1));
      po->addFanin(CirGateV(in, s1));
   }
/*
   for (size_t i=0, n=nmax+no; i<=n; ++i)
      cerr<<allList[i]<<endl;
*/   
      // update notUsedList
   updateNotUsed();
      // construct dfsList
   dfsScan();
      // delete dynamic array
   delete gid;
   delete gin1;
   delete gin2;
   delete oin;
   return true;
}

void
CirMgr::updateNotUsed()
{
   ng = 0;   // update aig's number (ng is the only one that will change)
   notUsedList.clear();
   for (int i=1; i<=nmax; ++i)
      if (allList[i] !=0 && allList[i]->getTypeEnum() == PI_GATE) {
         CirPiGate* pi = static_cast<CirPiGate*>(allList[i]);
         if(pi->isNotUsed())
            notUsedList.push_back(pi->getId());
      }
      else if (allList[i] !=0 && allList[i]->getTypeEnum() == AIG_GATE) {
         ++ng;
         CirAigGate* aig = static_cast<CirAigGate*>(allList[i]);
         if (aig->isNotUsed())
            notUsedList.push_back(aig->getId());
      }
}

void
CirMgr::updateFloating()
{
   floatingList.clear();
   for (int i=1; i<=nmax+no; ++i)
      if (allList[i] != 0 && allList[i]->getTypeEnum() == AIG_GATE) {
         CirAigGate* aig = static_cast<CirAigGate*>(allList[i]);
         if (aig->floatFanin())
            floatingList.push_back(aig->getId());
      }
      else if (allList[i] !=0 && allList[i]->getTypeEnum() == PO_GATE) {
         CirPoGate* po = static_cast<CirPoGate*>(allList[i]);
         if(po->floatFanin())
            floatingList.push_back(po->getId());
      }
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
   cout << endl;
   cout << "Circuit Statistics" << endl;
   cout << "==================" << endl;
   cout << "  PI   " << right << setw(9) << ni << endl;
   cout << "  PO   " << right << setw(9) << no << endl;
   cout << "  AIG  " << right << setw(9) << ng << endl;
   cout << "------------------" << endl;
   cout << "  Total" << right << setw(9) << ni+no+ng << endl;
}

void
CirMgr::printNetlist() const
{
   cout << endl;
   for (size_t i = 0; i<dfsList.size(); ++i) {
      cout << "[" << i << "] ";
      dfsList[i]->printGate();
   }
}

void
CirMgr::dfsScan() const
{
   ++_globalRef;
   dfsList.clear();
   for (int i=0; i<no; ++i) {
      dfsSearch(poList[i]);
   }
}

void 
CirMgr::dfsSearch(CirGate* g) const
{
//   cerr << "!" << endl; 
   switch(g->getTypeEnum()) {
      case PO_GATE:
      {
         CirPoGate* po = static_cast<CirPoGate*>(g);
         if (!po->isUndef())
            dfsSearch(po->_faninList[0].gate());
         dfsList.push_back(g);
         break;
      }
      case AIG_GATE:
      {
         CirAigGate* aig = static_cast<CirAigGate*>(g);
         if (aig->_ref == _globalRef)
            break;
         aig->_ref = _globalRef;
         if (!aig->isUndef1())
            dfsSearch(aig->_faninList[0].gate());
         if (!aig->isUndef2())
            dfsSearch(aig->_faninList[1].gate());
         dfsList.push_back(g);
         break;
      }
      case PI_GATE:
      {
         CirPiGate* pi = static_cast<CirPiGate*>(g);
         if (pi->_ref != _globalRef) {
            pi->_ref = _globalRef;
            dfsList.push_back(g);
         }
         break;
      }
      case CONST_GATE:
      {
         CirConstGate* con = static_cast<CirConstGate*>(g);
         if (con->_ref != _globalRef) {
            con->_ref = _globalRef;
            dfsList.push_back(g);
         }
         break;
      }
      default:
         cerr << "Error: wrong dfs search!!";
         break;
   }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for (int i=0; i<ni; ++i)
      cout << " " << piList[i]->getId();
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for (int i=0; i<no; ++i)
      cout << " " << poList[i]->getId();
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   size_t floatNum = floatingList.size();
   size_t notUsedNum = notUsedList.size();
   if (floatNum) {
      cout << "Gates with floating fanin(s):";
      for (size_t i = 0; i<floatNum; ++i)
         cout << " " << floatingList[i];
      cout << endl;
   }
   if (notUsedNum) {
      cout << "Gates defined but not used  :";
      for (size_t i = 0; i<notUsedNum; ++i)
         cout << " " << notUsedList[i];
      cout << endl;
   }
}

void
CirMgr::printFECPairs() const
{
   for (size_t i=0, n=fecGroups.size(); i<n; ++i) {
      cout << "[" << i << "] ";
      for (size_t j=0, m=fecGroups[i].size(); j<m; j++)
         cout << (fecGroups[i][j].isInv()?"!":"") << fecGroups[i][j].gate()->getId() << " ";
      cout << endl;
   }
}

void
CirMgr::writeAag(ostream& outfile) const
{
   outfile << "aag " << nmax << " " << ni << " " << nl << " " << no << " " << ng << "\n";
   for (int i = 0; i<ni; ++i)
      outfile << piList[i]->getId()*2 << "\n";
   for (int i = 0; i<no; ++i)
      outfile << poList[i]->_faninList[0].gate()->getId()*2 + poList[i]->_faninList[0].isInv() << "\n";
   for (size_t i = 0, n = dfsList.size(); i < n; ++i)
      if (dfsList[i]->getTypeEnum() == AIG_GATE) {
         CirAigGate* aig = static_cast<CirAigGate*>(dfsList[i]);
         outfile << aig->getId()*2 << " "
                 << aig->_faninList[0].gate()->getId()*2 + aig->_faninList[0].isInv() << " "
                 << aig->_faninList[1].gate()->getId()*2 + aig->_faninList[1].isInv() << endl;
      }
}

void
CirMgr::clear() const
{
   if (nmax == 0)
      return;
   for (int i = 0; i<nmax+no+1; ++i)
      delete allList[i];
}

