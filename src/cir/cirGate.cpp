/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;
extern size_t _globalRef;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirPiGate::reportFanin(int level) const
{
   ++_globalRef;
   dfsFanin(level);
}

void
CirPiGate::reportFanout(int level) const
{
   ++_globalRef;
   dfsFanout(level);
}

void
CirConstGate::reportFanin(int level) const
{
   ++_globalRef;
   dfsFanin(level);
}

void
CirConstGate::reportFanout(int level) const
{
   ++_globalRef;
   dfsFanout(level);
}

void
CirPoGate::reportFanin(int level) const
{
   ++_globalRef;
   dfsFanin(level);
}

void
CirPoGate::reportFanout(int level) const
{
   ++_globalRef;
   dfsFanout(level);
}

void
CirAigGate::reportFanin(int level) const
{
   ++_globalRef;
   dfsFanin(level);
}

void
CirAigGate::reportFanout(int level) const
{
   ++_globalRef;
   dfsFanout(level);
}

void
CirUndefGate::reportFanin(int level) const
{
   ++_globalRef;
   dfsFanin(level);
}

void
CirUndefGate::reportFanout(int level) const
{
   ++_globalRef;
   dfsFanout(level);
}

void
CirPiGate::dfsFanin(int level, int spaceNo, bool s) const
{
   assert(level>=0);
   cout << string(spaceNo, ' ');
   cout << (s ? "!" : "") << "PI " << getId() << endl;
}

void
CirPiGate::dfsFanout(int level, int spaceNo, bool s) const
{
   assert(level>=0);
   cout << string(spaceNo, ' ');
   cout << (s ? "!" : "") << "PI " << getId() << endl;
   if (level > 0)
      for (size_t i = 0; i<_fanoutList.size(); ++i)
         _fanoutList[i].gate()->dfsFanout(level-1, spaceNo+2, _fanoutList[i].isInv());
}

void
CirConstGate::dfsFanin(int level, int spaceNo, bool s) const
{
   assert(level>=0);
   cout << string(spaceNo, ' ');
   cout << (s ? "!" : "") << "CONST " << getId() << endl;
}

void
CirConstGate::dfsFanout(int level, int spaceNo, bool s) const
{
   assert(level>=0);
   cout << string(spaceNo, ' ');
   cout << (s ? "!" : "") << "CONST " << getId() << endl;
   if (level > 0)
      for (size_t i = 0; i<_fanoutList.size(); ++i)
         _fanoutList[i].gate()->dfsFanout(level-1, spaceNo+2, _fanoutList[i].isInv());
}

void
CirUndefGate::dfsFanin(int level, int spaceNo, bool s) const
{
   assert(level>=0);
   cout << string(spaceNo, ' ');
   cout << (s ? "!" : "") << "UNDEF " << getId() << endl;
}

void
CirUndefGate::dfsFanout(int level, int spaceNo, bool s) const
{
   assert(level>=0);
   cout << string(spaceNo, ' ');
   cout << (s ? "!" : "") << "UNDEF " << getId() << endl;
   if (level > 0)
      for (size_t i = 0; i<_fanoutList.size(); ++i)
         _fanoutList[i].gate()->dfsFanout(level-1, spaceNo+2, _fanoutList[i].isInv());
}

void
CirPoGate::dfsFanin(int level, int spaceNo, bool s) const
{
   assert(level>=0);
   cout << string(spaceNo, ' ');
   cout << (s ? "!" : "") << "PO " << getId() << endl;
   if (level > 0)
      if (!_undef)
         _faninList[0].gate()->dfsFanin(level-1, spaceNo+2, _faninList[0].isInv());
      else
         cout << string(spaceNo+2, ' ') << "UNDEF " << _undefId << endl;
   else {}
}

void
CirPoGate::dfsFanout(int level, int spaceNo, bool s) const
{
   assert(level>=0);
   cout << string(spaceNo, ' ');
   cout << (s ? "!" : "") << "PO " << getId() << endl;
}

void
CirAigGate::dfsFanin(int level, int spaceNo, bool s) const
{
   assert(level>=0);
   cout << string(spaceNo, ' ');
   cout << (s ? "!" : "") << "AIG " << getId();
   if (_scanned == _globalRef && level > 0) {
      cout << " (*)" << endl;
      return;
   }
   cout << endl;
   if (level > 0) {
      _ref = _globalRef;
      if (_faninList.size() != 0)
         _scanned = _globalRef;
      if (!_undef1)
         _faninList[0].gate()->dfsFanin(level-1, spaceNo+2, _faninList[0].isInv());
      else
         cout << string(spaceNo+2, ' ') << "UNDEF " << _undefId1 << endl;
      if (!_undef2)
         _faninList[1].gate()->dfsFanin(level-1, spaceNo+2, _faninList[1].isInv());
      else
         cout << string(spaceNo+2, ' ') << "UNDEF " << _undefId2 << endl;
   }
}

void
CirAigGate::dfsFanout(int level, int spaceNo, bool s) const
{
   assert(level>=0);
   cout << string(spaceNo, ' ');
   cout << (s ? "!" : "") << "AIG " << getId();
   if (_scanned == _globalRef && level > 0) {
      cout << " (*)" << endl;
      return;
   }
   cout << endl;
   if (level > 0) {
      _ref = _globalRef;
      if (_fanoutList.size() != 0)
         _scanned = _globalRef;
      for (size_t i = 0; i<_fanoutList.size(); ++i)
         _fanoutList[i].gate()->dfsFanout(level-1, spaceNo+2, _fanoutList[i].isInv());
   }
}

void
CirPiGate::reportGate() const
{
   string tmp1, tmp2, tmp3;
   stringstream s1, s2, s3;
   s1 << "= PI(" << _id << ")";
   if (_sName.size()!=0) { s1 << "\"" << getName() <<"\""; }
   s1 << ", line " << _lineNo;
   tmp1 = s1.str();   tmp1.append(49-tmp1.size(), ' ');
   s2 << "= FECs: ";
   tmp2 = s2.str();   tmp2.append(49-tmp2.size(), ' ');
   s3 << "= Value: ";
   s3 << simToBinary(_simValue);   
   tmp3 = s3.str();   tmp3.append(49-tmp3.size(), ' ');
   cout << "==================================================" << endl;
   cout << tmp1 << "=" << endl;
   cout << tmp2 << "=" << endl;
   cout << tmp3 << "=" << endl;                     
   cout << "==================================================" << endl;
}

void
CirConstGate::reportGate() const
{
   string tmp1, tmp2, tmp3;
   stringstream s1, s2, s3;
   s1 << "= CONST(" << _id << "), line " << _lineNo;
   tmp1 = s1.str();   tmp1.append(49-tmp1.size(), ' ');
   s2 << "= FECs: ";
   for (size_t i=0, n=_fecGroup.size(); i<n; ++i) {
      s2 << (_fecGroup[i].isInv()?"!":"") << _fecGroup[i].gate()->getId() << " ";  
   }
   tmp2 = s2.str();
   if (tmp2.size() <= 49)
      tmp2.append(49-tmp2.size(), ' ');
   s3 << "= Value: ";
   s3 << simToBinary(_simValue);   
   tmp3 = s3.str();   tmp3.append(49-tmp3.size(), ' ');
   cout << "==================================================" << endl;
   cout << tmp1 << "=" << endl;
   cout << tmp2 << "=" << endl;
   cout << tmp3 << "=" << endl;                     
   cout << "==================================================" << endl;
}

void
CirUndefGate::reportGate() const
{
   string tmp1, tmp2, tmp3;
   stringstream s1, s2, s3;
   s1 << "= UNDEF(" << _id << "), line " << _lineNo;
   tmp1 = s1.str();   tmp1.append(49-tmp1.size(), ' ');
   s2 << "= FECs:";
   tmp2 = s2.str();   tmp2.append(49-tmp2.size(), ' ');
   s3 << "= Value: ";
   s3 << simToBinary(_simValue);   
   tmp3 = s3.str();   tmp3.append(49-tmp3.size(), ' ');
   cout << "==================================================" << endl;
   cout << tmp1 << "=" << endl;
   cout << tmp2 << "=" << endl;
   cout << tmp3 << "=" << endl;                     
   cout << "==================================================" << endl;
}

void
CirAigGate::reportGate() const
{
   string tmp1, tmp2, tmp3;
   stringstream s1, s2, s3;
   s1 << "= AIG(" << _id << "), line " << _lineNo;
   tmp1 = s1.str();   tmp1.append(49-tmp1.size(), ' ');
   s2 << "= FECs: ";
   for (size_t i=0, n=_fecGroup.size(); i<n; ++i) {
      s2 << (_fecGroup[i].isInv()?"!":"") << _fecGroup[i].gate()->getId() << " ";  
   }
   tmp2 = s2.str();
   if (tmp2.size() <= 49)
      tmp2.append(49-tmp2.size(), ' ');
   s3 << "= Value: ";
   s3 << simToBinary(_simValue);   
   tmp3 = s3.str();   tmp3.append(49-tmp3.size(), ' ');
   cout << "==================================================" << endl;
   cout << tmp1 << "=" << endl;
   cout << tmp2 << "=" << endl;
   cout << tmp3 << "=" << endl;                     
   cout << "==================================================" << endl;
}

void
CirPoGate::reportGate() const
{
   string tmp1, tmp2, tmp3;
   stringstream s1, s2, s3;
   s1 << "= PO(" << _id << ")";
   if (_sName.size()!=0) { s1 << "\"" << getName() <<"\""; }
   s1 << ", line " << _lineNo;
   tmp1 = s1.str();   tmp1.append(49-tmp1.size(), ' ');
   s2 << "= FECs:";
   tmp2 = s2.str();   tmp2.append(49-tmp2.size(), ' ');
   s3 << "= Value: ";
   s3 << simToBinary(_simValue);   
   tmp3 = s3.str();   tmp3.append(49-tmp3.size(), ' ');
   cout << "==================================================" << endl;
   cout << tmp1 << "=" << endl;
   cout << tmp2 << "=" << endl;
   cout << tmp3 << "=" << endl;                     
   cout << "==================================================" << endl;
}

void
CirAigGate::printFanin() const
{
   CirGate* in1 = _faninList[0].gate();
   CirGate* in2 = _faninList[1].gate();  
   bool s1 = _faninList[0].isInv();
   bool s2 = _faninList[1].isInv();
   int id1 = (isUndef1() ? getUndefId1() : in1->getId());
   int id2 = (isUndef2() ? getUndefId2() : in2->getId());
   cout << " " << (isUndef1() ? "*" : "");     // undef
   cout << (s1 ? "!" : "") << id1;             // inv
   cout << " " << (isUndef2() ? "*" : "");     // undef
   cout << (s2 ? "!" : "") << id2;             // inv
}

void
CirPoGate::printFanin() const
{
   CirGate* in = _faninList[0].gate();
   bool s = _faninList[0].isInv();
   int id = (isUndef() ? getUndefId() : in->getId());
   cout << " " << (isUndef() ? "*" : "");      // undef
   cout << (s ? "!" : "") << id;               // inv
}

void
CirPiGate::printGate() const
{
   cout << "PI  " << _id;
   if (_sName.size()!=0)
      cout << " (" << _sName << ")";
   cout << endl; 
}

void
CirPoGate::printGate() const
{
   cout << "PO  " << _id; printFanin();
   if (_sName.size()!=0)
      cout << " (" << _sName << ")";
   cout << endl;
}

void
CirAigGate::deleteFanout(CirGate* g)
{
   for (size_t i = 0, n = _fanoutList.size(); i<n; ++i)
      if (_fanoutList[i].gate() == g)
         _fanoutList.erase(_fanoutList.begin()+i);
} 

void
CirPiGate::deleteFanout(CirGate* g)
{
   for (size_t i = 0, n = _fanoutList.size(); i<n; ++i)
      if (_fanoutList[i].gate() == g)
         _fanoutList.erase(_fanoutList.begin()+i);
} 

void
CirConstGate::deleteFanout(CirGate* g)
{
   for (size_t i = 0, n = _fanoutList.size(); i<n; ++i)
      if (_fanoutList[i].gate() == g)
         _fanoutList.erase(_fanoutList.begin()+i);
} 

void
CirUndefGate::deleteFanout(CirGate* g)
{
   for (size_t i = 0, n = _fanoutList.size(); i<n; ++i)
      if (_fanoutList[i].gate() == g)
         _fanoutList.erase(_fanoutList.begin()+i);
}

void
CirAigGate::addFanoutList(vector<CirGateV>& l, bool s)
{
   for (size_t i=0, n=l.size(); i<n; ++i)
      _fanoutList.push_back(CirGateV(l[i].gate(), (l[i].isInv()!=s)));
}

void
CirPiGate::addFanoutList(vector<CirGateV>& l, bool s)
{
   for (size_t i=0, n=l.size(); i<n; ++i)
      _fanoutList.push_back(CirGateV(l[i].gate(), (l[i].isInv()!=s)));
}

void
CirConstGate::addFanoutList(vector<CirGateV>& l, bool s)
{
   for (size_t i=0, n=l.size(); i<n; ++i)
      _fanoutList.push_back(CirGateV(l[i].gate(), (l[i].isInv()!=s)));
}

void
CirUndefGate::addFanoutList(vector<CirGateV>& l, bool s)
{
   for (size_t i=0, n=l.size(); i<n; ++i)
      _fanoutList.push_back(CirGateV(l[i].gate(), (l[i].isInv()!=s)));
}

void
CirAigGate::changeMyself(CirGate* in, bool s)
{
   CirGate* newFanin = in;
   for (size_t i=0, n=_fanoutList.size(); i<n; ++i) {
      if (_fanoutList[i].gate()->getTypeEnum() == PO_GATE) {
         CirPoGate* po = static_cast<CirPoGate*>(_fanoutList[i].gate());
         po->_faninList[0] = CirGateV(newFanin, s!=po->_faninList[0].isInv());
      }
      else {       // must be AIG_GATE
         CirAigGate* aig = static_cast<CirAigGate*>(_fanoutList[i].gate());    
         if (aig->_faninList[0].gate() == this)
            aig->_faninList[0] = CirGateV(newFanin, s!=aig->_faninList[0].isInv());
         else   // must be _faninList[1]
            aig->_faninList[1] = CirGateV(newFanin, s!=aig->_faninList[1].isInv());
      }
   }
}

void
CirAigGate::setSimValue()
{
   size_t sim1 = _faninList[0].gate()->getSimValue();
   size_t sim2 = _faninList[1].gate()->getSimValue();
   if (_faninList[0].isInv())
      sim1 = ~sim1;
   if (_faninList[1].isInv())
      sim2 = ~sim2;
   _simValue = sim1 & sim2;
}

void
CirPoGate::setSimValue()
{
   if (_faninList[0].isInv())
      _simValue = ~_faninList[0].gate()->getSimValue();
   else
      _simValue = ~_faninList[0].gate()->getSimValue();
}
 
 
