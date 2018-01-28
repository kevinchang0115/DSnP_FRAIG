/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
class FaninKey
{
public:
   FaninKey() {}
   FaninKey(CirAigGate* g) { _fanin1 = g->_faninList[0]; _fanin2 = g->_faninList[1]; }
   ~FaninKey() {}
   size_t operator() () const
   { 
      return _fanin1.getGateV() * _fanin2.getGateV();  // this should be improved
   }
   bool operator == (const FaninKey& k) const 
   {
      return ((_fanin1 == k._fanin1) && (_fanin2 == k._fanin2))
          || ((_fanin1 == k._fanin2) && (_fanin2 == k._fanin1));
   }

private:
   CirGateV _fanin1;
   CirGateV _fanin2;
};
/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
void
CirMgr::strash()
{
   bool flag = true;
   while (flag) {
     vector<CirAigGate*> dfsAigList;   
     flag = false;
     for (size_t i = 0, n = dfsList.size(); i<n; ++i)
         if (dfsList[i]->getTypeEnum() == AIG_GATE)
            dfsAigList.push_back(static_cast<CirAigGate*>(dfsList[i]));
      size_t aigSize = dfsAigList.size();
      HashMap<FaninKey, CirAigGate*> strHash(getHashSize(aigSize));
      for (size_t i=0; i<aigSize; ++i) {
         strHash.insert(dfsAigList[i]);
      } 
//      strHash.printDis();   // used for checking
      CirAigGate *aig1, *aig2;  
      if (strHash.checkAll(aig1, aig2)) {
//         cout << aig1 << aig2 << endl;
         emergeAig(aig1, aig2);
         flag = true;
         dfsScan();
         updateNotUsed();   // update aig gate's number
      }
   }
}
void
CirMgr::fraig()
{
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
void
CirMgr::emergeAig(CirAigGate* aig1, CirAigGate* aig2)
{
   int id1 = aig1->getId(), id2 = aig2->getId();
   deleteAig(aig2);
   aig2->changeMyself(aig1, false);
   aig1->addFanoutList(aig2->_fanoutList, false);
   cout << "Strashing: " << id1 << " merging " << id2 << "..." << endl;
   allList[id2] = 0;
   delete aig2;
}


