#include "node.h"
#include <iostream>

using namespace std;
bool link_cost_changed = false;
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
  printRT(nd);

  // part b
  for(auto &e: nd)
  {
    if(e->getName()=="B") e->modify_Rtable("10.0.1.3",16);
    if(e->getName()=="C") e->modify_Rtable("10.0.1.23",16);
  }
  cout<<"tables after increasing B-C link cost"<<'\n';
  link_cost_changed = true;
  printRT(nd);
  cout<<"starting updates"<<'\n';
  for(int i=0;i<no_nodes;i++)
  {
    nd[i]->sendMsg();
  }

}


void RoutingNode::recvMsg(RouteMsg *msg) 
{

  string from_ip = msg->from;
  string to_ip = msg->recvip;
  cout<<"processing msg from "<<from_ip<<" to "<<to_ip<<'\n';
  vector<RoutingEntry> received_table = msg->mytbl->tbl;
  bool modified = false;
  int rcvd_size = received_table.size();
  int cost_to_add = 0;
  int link_cost = 1;
  if((from_ip=="10.0.1.23" && to_ip=="10.0.1.3" && link_cost_changed)||(to_ip=="10.0.1.23" && from_ip=="10.0.1.3" && link_cost_changed))
    {
      link_cost = 16;
    }
  for(int i=0;i<rcvd_size;i++)
  {
    string dest_ip = received_table[i].dstip;
    string my_interface = received_table[i].ip_interface;
    bool is_self_loop = dest_ip == my_interface;
    
    int cost = received_table[i].cost+link_cost;
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
            if(e.cost<16) modified = true;
            e.cost = min(16,cost);
            e.nexthop = from_ip;
            e.ip_interface = to_ip;
            
            
            
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
            e.cost = link_cost;
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
    cout<<"table modified"<<'\n';
    printTable();
    sendMsg();
  }
  else
  {
    cout<<"table not modified"<<'\n';
  }
  
  //your code here
}




