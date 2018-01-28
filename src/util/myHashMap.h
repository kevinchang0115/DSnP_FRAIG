/****************************************************************************
  FileName     [ myHashMap.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashMap and Cache ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2009-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_MAP_H
#define MY_HASH_MAP_H

#include <vector>

using namespace std;

// TODO: Implement your own HashMap and Cache classes.

//-----------------------
// Define HashMap classes
//-----------------------
// To use HashMap ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class HashKey
// {
// public:
//    HashKey() {}
// 
//    size_t operator() () const { return 0; }
// 
//    bool operator == (const HashKey& k) const { return true; }
// 
// private:
// };
//
template <class HashKey, class HashData>
class HashMap
{
typedef pair<HashKey, HashData> HashNode;

public:
   HashMap() : _numBuckets(0), _buckets(0) {}
   HashMap(size_t b) : _numBuckets(0), _buckets(0) { init(b); }
   ~HashMap() { reset(); }

   // [Optional] TODO: implement the HashMap<HashKey, HashData>::iterator
   // o An iterator should be able to go through all the valid HashNodes
   //   in the HashMap
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   // (_bId, _bnId) range from (0, 0) to (_numBuckets, 0)
   //
   class iterator
   {
      friend class HashMap<HashKey, HashData>;

   public:

   private:
   };

   void init(size_t b) {
      reset(); _numBuckets = b; _buckets = new vector<HashNode>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<HashNode>& operator [] (size_t i) { return _buckets[i]; }
   const vector<HashNode>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const { iterator(); }
   // Pass the end
   iterator end() const { iterator(); }
   // return true if no valid data
   bool empty() const { return true; }
   // number of valid data
   size_t size() const { size_t s = 0; return s; }
   void insert(const HashData& d)
   {
      HashKey key = d;
      _buckets[bucketNum(key)].push_back(pair<HashKey, HashData>(key, d));
   }
   void printDis() const  // used to check distribution
   {
      cout << _numBuckets << endl;
      for (size_t i = 0; i<_numBuckets; ++i)
         cout << _buckets[i].size() << endl;
         
      for (size_t i = 0; i<_numBuckets; ++i)
         for (size_t j = 0; j<_buckets[i].size(); ++j)
            cout << _buckets[i][j].first() << endl;
         
   }
   bool checkAll(HashData& d1, HashData& d2) const
   {
      for (size_t i = 0; i<_numBuckets; ++i)
         if (_buckets[i].size() > 1)
            if (checkRow(i, d1, d2))
               return true;
      return false;
   }
   
   bool checkAll(vector< vector<HashData> >& groups) const
   {
      bool flag = false;
      for (size_t i = 0; i<_numBuckets; ++i)
         if (_buckets[i].size() > 1)
            if (checkRow(i, groups))
               flag = true;
      return flag;
   }
private:
   // Do not add any extra data member
   size_t                   _numBuckets;
   vector<HashNode>*        _buckets;

   size_t bucketNum(const HashKey& k) const {
      return (k() % _numBuckets); }
      
   bool checkRow(size_t& r, HashData& d1, HashData& d2) const
   {
      for (size_t i=0, n=_buckets[r].size(); i<n; ++i)
         for (size_t j=i+1; j<n; ++j)
            if (checkEq(r,i,j,d1,d2))
               return true;
      return false;
   }
   
   bool checkRow(size_t& r, vector< vector<HashData> >& groups) const
   {
      bool flag = false;
      vector<HashKey> key;
      for (size_t i=0, n=_buckets[r].size(); i<n; ++i) {
         typename vector<HashKey>::iterator it = find(key.begin(), key.end(), _buckets[r][i].first);    
         if (!(it == key.end()))
            continue;
         if (key.size() >= 2)  // new groups found
            flag = true;
         key.push_back(_buckets[r][i].first);
         vector<HashData> tmpGroup;
         tmpGroup.push_back(_buckets[r][i].second);
         bool sameKey = false;
         for (size_t j=i+1; j<n; ++j)
            if (checkEq(r,i,j, tmpGroup))
               sameKey = true;
         if (sameKey)
            groups.push_back(tmpGroup);
      }
      return flag;
   } 

     
   bool checkEq(size_t& r, size_t& i, size_t& j, HashData& d1, HashData& d2) const
   {
//      cout << "checking..." << endl;
//      cout << r << i << j << endl;
//      cout << _buckets[r][i].first() << endl << _buckets[r][j].first() << endl;
      if (_buckets[r][i].first == _buckets[r][j].first) {
         d1 = _buckets[r][i].second;
         d2 = _buckets[r][j].second;
         return true;
      }
      return false;
   }
   bool checkEq(size_t& r, size_t& i, size_t& j, vector<HashData>& group) const
   {
      if (_buckets[r][i].first == _buckets[r][j].first) {
         if (_buckets[r][i].first.isInv(_buckets[r][j].first))  // inv
            group.push_back(_buckets[r][j].second.toInv());
         else
            group.push_back(_buckets[r][j].second);    
         return true;  
      }     
      return false;
   }
};


//---------------------
// Define Cache classes
//---------------------
// To use Cache ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class CacheKey
// {
// public:
//    CacheKey() {}
//    
//    size_t operator() () const { return 0; }
//   
//    bool operator == (const CacheKey&) const { return true; }
//       
// private:
// }; 
// 
template <class CacheKey, class CacheData>
class Cache
{
typedef pair<CacheKey, CacheData> CacheNode;

public:
   Cache() : _size(0), _cache(0) {}
   Cache(size_t s) : _size(0), _cache(0) { init(s); }
   ~Cache() { reset(); }

   // NO NEED to implement Cache::iterator class

   // TODO: implement these functions
   //
   // Initialize _cache with size s
   void init(size_t s) { }
   void reset() { }

   size_t size() const { return _size; }

   CacheNode& operator [] (size_t i) { return _cache[i]; }
   const CacheNode& operator [](size_t i) const { return _cache[i]; }

   // return false if cache miss
   bool read(const CacheKey& k, CacheData& d) const { return false; }
   // If k is already in the Cache, overwrite the CacheData
   void write(const CacheKey& k, const CacheData& d) { }

private:
   // Do not add any extra data member
   size_t         _size;
   CacheNode*     _cache;
};


#endif // MY_HASH_H
