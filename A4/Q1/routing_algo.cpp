#include "node.h"
#include <iostream>

using namespace std;

void printRT(vector<RoutingNode*> nd)
{
/*Print routing table entries*/
	for (int i = 0; i < nd.size(); i++) 
  {
	  nd[i]->printTable();
	}
}

void routingAlgo(vector<RoutingNode*> nd)
{
  //Your code here
   printRT(nd);
  int no_nodes = nd.size();
  for(int i=0;i<no_nodes;i++)
  {
    nd[i]->sendMsg();
  }
  /*Print routing table entries after routing algo converges */
  cout<<"tables after converging"<<'\n';
  printRT(nd);

 

}


void RoutingNode::recvMsg(RouteMsg *msg) 
{

  string from_ip = msg->from;
  string to_ip = msg->recvip;
  // cout<<"processing msg from "<<from_ip<<" to "<<to_ip<<'\n';
  vector<RoutingEntry> received_table = msg->mytbl->tbl;
  bool modified = false;
  int rcvd_size = received_table.size();
  int cost_to_add = 0;
  

  for(int i=0;i<rcvd_size;i++)
  {
    string dest_ip = received_table[i].dstip;
    string my_interface = received_table[i].ip_interface;
    bool is_self_loop = dest_ip == my_interface;
    int cost = received_table[i].cost+1;
    bool found = false;

    for( auto &e: this->mytbl.tbl)
    {
      if(is_self_loop) continue;
      if(e.dstip==dest_ip)
      {
        found = true;
        if(e.cost>cost)
        {
          e.cost = cost;
          e.nexthop = from_ip;
          e.ip_interface = to_ip;
          modified = true;
        }
        else if(e.nexthop==from_ip)
        {
          if(e.cost!=cost)
          {
            e.cost = min(16,cost);
            e.nexthop = from_ip;
            e.ip_interface = to_ip;
            modified = true;
          }
        }
        break;
      }
    }
    if(!found)
    {
      if(is_self_loop)
      {
        bool exists = false;
        for(auto &e : this->mytbl.tbl)
        {
          if(e.dstip == dest_ip)
          {
            exists = true;
            e.cost = 1;
            e.ip_interface = to_ip;
            e.nexthop = from_ip;
            break;
          }
        }
        if(!exists) 
        {
          modified = true;
          this->addTblEntry(dest_ip,from_ip,to_ip,cost);
        }
      } 
      else 
      {
        this->addTblEntry(dest_ip,from_ip,to_ip,cost);
        modified = true;
      }
    }
  }
  if(modified)
  {
    // cout<<"table modified"<<'\n';
    // printTable();
    sendMsg();
  }
  else
  {
    // cout<<"table not modified"<<'\n';
  }
  
  //your code here
}




