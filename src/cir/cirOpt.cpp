/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
void
CirMgr::sweep()
{
   while (deleteNotUsed())
      updateNotUsed();
   updateFloating();
}

void
CirMgr::optimize()
{
   bool flag = true;
   while (flag) {
      flag = false;
      vector<CirAigGate*> dfsAigList;
      for (size_t i = 0, n = dfsList.size(); i<n; ++i)
         if (dfsList[i]->getTypeEnum() == AIG_GATE)
            dfsAigList.push_back(static_cast<CirAigGate*>(dfsList[i]));
      for (size_t i = 0, n = dfsAigList.size(); i<n; ++i) {
         if (optConst(dfsAigList[i])) {
            flag = true; dfsScan(); updateNotUsed();
            break;   
         }
         if (optSameInvFanin(dfsAigList[i])) {
            flag = true; dfsScan(); updateNotUsed();
            break;
         }
      }
   }
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
bool
CirMgr::deleteNotUsed()
{
   bool flag = false;
   for (size_t i = 0, n = notUsedList.size(); i<n; ++i) {
      int id = notUsedList[i];
      if (allList[id]->getTypeEnum() == PI_GATE)
         continue;
      flag = true;
      CirAigGate* aig = static_cast<CirAigGate*>(allList[id]);
      aig->_faninList[0].gate()->deleteFanout(allList[id]);
      aig->_faninList[1].gate()->deleteFanout(allList[id]);
      if (aig->isUndef1())
         if (static_cast<CirUndefGate*>(aig->_faninList[0].gate())->isNotUsed())
            cout << "Sweeping: UNDEF(" << aig->getUndefId1() << ") removed..." <<  endl;
      if (aig->isUndef2())
         if (static_cast<CirUndefGate*>(aig->_faninList[1].gate())->isNotUsed())
            cout << "Sweeping: UNDEF(" << aig->getUndefId2() << ") removed..." <<  endl;
      cout << "Sweeping: AIG(" << id << ") removed..." <<  endl;
      delete aig;
      allList[id] = 0;
   }
   return flag;
}

bool
CirMgr::optConst(CirAigGate* g)
{
   bool flag = false;
   int fanin = -1, sign = -1;   // const0 or const1
   if (g->_faninList[0].gate()->getId() == 0) {
      sign = (g->_faninList[0].isInv() ? 1 : 0);
      fanin = 1; flag = true;
   }
   else if (g->_faninList[1].gate()->getId() == 0) {
      sign = (g->_faninList[1].isInv() ? 1 : 0);
      fanin = 0; flag = true;
   }
   if (sign==1) {               // const1
      CirGate* merFanin = g->_faninList[fanin].gate();
      bool s = g->_faninList[fanin].isInv();
      int id = g->getId();
      deleteAig(g);
      merFanin->addFanoutList(g->_fanoutList, s);
      g->changeMyself(merFanin, s);
      cout << "Simplifing: " << merFanin->getId() << " merging " << (s?"!":"") << id << "..." << endl;
      delete g;
      allList[id] = 0;
   }
   else if (sign==0) {          // const0
      int id = g->getId();
      deleteAig(g);
      g->changeMyself(allList[0], false);
      allList[0]->addFanoutList(g->_fanoutList, false);
      cout << "Simplifing: 0 merging " << id << "..." << endl;
      delete g;
      allList[id] = 0;
   }
   return flag;
}

bool
CirMgr::optSameInvFanin(CirAigGate* g)
{
   bool flag = false, inv = false;
   CirGate *in1 = g->_faninList[0].gate(), *in2 = g->_faninList[1].gate();
   bool s1 = g->_faninList[0].isInv(), s2 = g->_faninList[1].isInv();
   int id = g->getId();
   if (in1 == in2) {
      flag = true;
      inv = (s1 == s2);
   }
   if (flag) {
      if (inv) {   // same fanin
         deleteAig(g);
         in1->addFanoutList(g->_fanoutList, s1);
         g->changeMyself(in1, s1);
         cout << "Simplifing: " << in1->getId() << " merging " << (s1?"!":"") << id << "..." << endl;
         delete g;
         allList[id] = 0;
      }
      else {       // inv fanin 
         deleteAig(g);
         g->changeMyself(allList[0], false);
         allList[0]->addFanoutList(g->_fanoutList, false);         
         cout << "Simplifing: 0 merging " << id << "..." << endl;
         delete g;
         allList[id] = 0;
      }
   }
   return flag;
}

void
CirMgr::deleteAig(CirAigGate* g)
{
   g->_faninList[0].gate()->deleteFanout(g);
   g->_faninList[1].gate()->deleteFanout(g);
}

