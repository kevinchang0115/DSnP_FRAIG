/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGateV
{
public:
   #define NEG 0x1
   CirGateV(CirGate* g = 0, size_t phase = 0)
      : _gateV(size_t(g) + phase) {}
   CirGate* gate() const { return (CirGate*)(_gateV & ~size_t(NEG)); }
   bool isInv() const { return (_gateV & NEG); }
   bool operator == (const CirGateV& g) const { return _gateV ==  g._gateV; }
   size_t getGateV() const { return _gateV; }
   CirGateV toInv() const { return CirGateV(gate(), 1); }
   
private:
   size_t _gateV;
};

class CirGate
{
public:
   CirGate() {}
   virtual ~CirGate() {}
   friend class cirMgr;

   // Basic access methods
   string getTypeStr() const { return ""; }
   unsigned getLineNo() const { return 0; }
   virtual GateType getTypeEnum() const = 0;

   // Printing functions
   virtual void printGate() const = 0;
   virtual void reportGate() const = 0;
   virtual void reportFanin(int level) const= 0;
   virtual void reportFanout(int level) const = 0;
   virtual void dfsFanin(int level, int spaceNo, bool s) const = 0;
   virtual void dfsFanout(int level, int spaceNo, bool s) const = 0;
   virtual void addFanout(CirGateV g) = 0;
   virtual void addFanoutList(vector<CirGateV>& l, bool s) = 0;
   virtual void deleteFanout(CirGate* g) = 0;
   virtual int  getId() const = 0;
   virtual const size_t& getSimValue() const = 0;
   virtual void addFecGroup(CirGateV g) = 0;
   virtual void deleteFecGroup() = 0;
};

class CirPiGate : public CirGate
{
public:
   CirPiGate(int id = -1, int line = 0)
      : _id(id), _lineNo(line), _ref(0) {}
   ~CirPiGate() {}
   friend class CirMgr;

   GateType getTypeEnum() const { return PI_GATE; }
   void printGate() const;
   void reportGate() const;
   void addFanout(CirGateV g) { _fanoutList.push_back(g); }
   void addFanoutList(vector<CirGateV>& l, bool s);
   void deleteFanout(CirGate* g);
   int  getId() const { return _id; }
   bool isNotUsed() const { return _fanoutList.empty(); }
   void reportFanin(int level) const;
   void reportFanout(int level) const;
   void dfsFanin(int level, int spaceNo=0, bool s=0) const;
   void dfsFanout(int level, int spaceNo=0, bool s=0) const;
   void setName(string n) { _sName = n; }
   string getName() const { return _sName; }
   void setSimValue(size_t sim) { _simValue = sim; }
   const size_t& getSimValue() const { return _simValue; }
   void addFecGroup(CirGateV g) {}
   void deleteFecGroup() {}   

private:
   int                 _id;
   int                 _lineNo;
   vector<CirGateV>    _fanoutList;
   mutable size_t      _ref;
   string              _sName;
   size_t              _simValue;
};

class CirConstGate : public CirGate
{
public:
   CirConstGate(int id = 0, int line = 0)
      : _id(id), _lineNo(line), _ref(0) {}
   ~CirConstGate() {}
   friend class CirMgr;

   GateType getTypeEnum() const { return CONST_GATE; }
   void printGate() const { cout << "CONST0" << endl; }
   void reportGate() const;
   void addFanout(CirGateV g) { _fanoutList.push_back(g); }
   void addFanoutList(vector<CirGateV>& l, bool s);
   void deleteFanout(CirGate* g);
   int  getId() const { return _id; }
   void reportFanin(int level) const;
   void reportFanout(int level) const;
   void dfsFanin(int level, int spaceNo=0, bool s=0) const;
   void dfsFanout(int level, int spaceNo=0, bool s=0) const;
   const size_t& getSimValue() const { return _simValue; }
   void addFecGroup(CirGateV g) { _fecGroup.push_back(g); }
   void deleteFecGroup() { _fecGroup.clear(); }

private:
   int                 _id;
   int                 _lineNo;
   vector<CirGateV>    _fanoutList;
   mutable size_t      _ref;
   size_t              _simValue;
   vector<CirGateV>    _fecGroup;
};

class CirAigGate : public CirGate
{
public:
   CirAigGate(int id = -1, int line = 0)
      : _id(id), _lineNo(line), _ref(0), _undef1(false), _undef2(false), _undefId1(-1), _undefId2(-1) {}
   ~CirAigGate() {}
   friend class CirMgr;
   friend class FaninKey;

   GateType getTypeEnum() const { return AIG_GATE; }
   void printGate() const { cout << "AIG " << _id; printFanin(); cout << endl; }
   void reportGate() const;
   void addFanout(CirGateV g) { _fanoutList.push_back(g); } 
   void addFanoutList(vector<CirGateV>& l, bool s);
   void deleteFanout(CirGate* g);
   void addFanin(CirGateV g) { _faninList.push_back(g); }
   void printFanin() const;
   int  getId() const { return _id; }
   void setUndef1() { _undef1 = true; }
   void setUndef2() { _undef2 = true; }
   bool isUndef1() const { return _undef1; } 
   bool isUndef2() const { return _undef2; }
   void setUndefId1(int id) { _undefId1 = id; }
   void setUndefId2(int id) { _undefId2 = id; }
   int  getUndefId1() const { return _undefId1; }
   int  getUndefId2() const { return _undefId2; }
   bool floatFanin() const { return _undef1 || _undef2; }
   bool isNotUsed() const { return _fanoutList.empty(); }
   void reportFanin(int level) const;
   void reportFanout(int level) const;
   void dfsFanin(int level, int spaceNo=0, bool s=0) const;
   void dfsFanout(int level, int spaceNo=0, bool s=0) const;
   void changeMyself(CirGate* in, bool s);
   void setSimValue();
   const size_t& getSimValue() const { return _simValue; }
   void addFecGroup(CirGateV g) { _fecGroup.push_back(g); }
   void deleteFecGroup() { _fecGroup.clear(); }

private:
   int                 _id;
   int                 _lineNo;
   vector<CirGateV>    _faninList;
   vector<CirGateV>    _fanoutList;
   mutable size_t      _ref;
   mutable size_t      _scanned;
   bool                _undef1;
   bool                _undef2;
   int                 _undefId1;
   int                 _undefId2;
   size_t              _simValue;
   vector<CirGateV>    _fecGroup;
};

class CirPoGate : public CirGate
{
public:
   CirPoGate(int id = -1, int line = 0)
      : _id(id), _lineNo(line), _undef(false), _undefId(-1) {}
   ~CirPoGate() {}
   friend class CirMgr;
   friend class CirAigGate;

   GateType getTypeEnum() const { return PO_GATE; }
   void printGate() const;
   void reportGate() const;
   void addFanin(CirGateV g) { _faninList.push_back(g); }
   void addFanout(CirGateV g) {}
   void addFanoutList(vector<CirGateV>& l, bool s) {}
   void deleteFanout(CirGate* g) {}
   void printFanin() const;
   int  getId() const { return _id; }
   void setUndef() { _undef = true; }
   void setUndefId(int id) { _undefId = id; }
   int  getUndefId() const { return _undefId; }
   bool isUndef() const { return _undef; }
   bool floatFanin() const { return _undef;}
   void reportFanin(int level) const;
   void reportFanout(int level) const;
   void dfsFanin(int level, int spaceNo=0, bool s=0) const;
   void dfsFanout(int level, int spaceNo=0, bool s=0) const;
   void setName(string n) { _sName = n; }
   string getName() const { return _sName; }
   void setSimValue();
   const size_t& getSimValue() const { return _simValue; }
   void addFecGroup(CirGateV g) {}
   void deleteFecGroup() {}      

private:
   int                 _id;
   int                 _lineNo;
   vector<CirGateV>    _faninList;
   bool                _undef;
   int                 _undefId;
   string              _sName;
   size_t              _simValue;
};

class CirUndefGate : public CirGate
{
public:
   CirUndefGate(int id = -1, int line = 0)
      : _id(id), _lineNo(line), _ref(0) {}
   ~CirUndefGate() {}
   friend class CirMgr;

   GateType getTypeEnum() const { return UNDEF_GATE; }
   void printGate() const { cout << "UNDEF" << endl; }
   void reportGate() const;
   void addFanout(CirGateV g) { _fanoutList.push_back(g); }
   void addFanoutList(vector<CirGateV>& l, bool s);
   void deleteFanout(CirGate* g);
   int  getId() const { return _id; }
   bool isNotUsed() const { return _fanoutList.empty(); }
   void reportFanin(int level) const;
   void reportFanout(int level) const;
   void dfsFanin(int level, int spaceNo=0, bool s=0) const;
   void dfsFanout(int level, int spaceNo=0, bool s=0) const;
   const size_t& getSimValue() const { return _simValue; }
   void addFecGroup(CirGateV g) {}
   void deleteFecGroup() {}      

private:
   int                 _id;
   int                 _lineNo;
   vector<CirGateV>    _fanoutList;
   mutable size_t      _ref;
   size_t              _simValue;
};
#endif // CIR_GATE_H
