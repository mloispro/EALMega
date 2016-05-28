/*
* DumbFucker.h
*
* Created: 5/27/2016 1:02:33 PM
* Author: lenovo
*/


#ifndef __DUMBFUCKER_H__
#define __DUMBFUCKER_H__


class DumbFucker {
    //variables
  public:
  protected:
  private:
    int _bb;
    //functions
  public:
    DumbFucker(int bb);
    ~DumbFucker();
    void Begin();
    void Print(int f);
  protected:
  private:
    DumbFucker(const DumbFucker& c);
    DumbFucker& operator=(const DumbFucker& c);

}; //DumbFucker

#endif //__DUMBFUCKER_H__
