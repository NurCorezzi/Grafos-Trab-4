/** *********************************************************
 * Módulo de pareamento Professores-Escolas
 *
 * autor NUR COREZZI - MAT. 15/0143290
 * autor CAIO MATHEUS CAMPOS DE OLIVEIRA - MAT. 14/0176713
 ***********************************************************/

#include "DirtyDeputies.h"
#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

ostream& operator<<(ostream& os, Company &rhs)
{
	os <<rhs.name;
	
	return os;
}

ostream& operator<<(ostream& os, Deputy &rhs)
{
	os << rhs.name << " do "<< rhs.political_party;
	
	return os;
}

ostream& operator<<(ostream& os, Refund &rhs)
{
	os << rhs.value << "\t" << rhs.description << " by " << *rhs.company;
	
	return os;
}

bool operator==(Company &lhs, Company &rhs)
{
	return lhs.id == rhs.id;
}

bool operator==(Refund &lhs, Refund &rhs)
{
	return *lhs.company == *rhs.company;
}

bool operator==(Deputy &lhs, Deputy &rhs)
{
	return lhs.name == rhs.name;
}

//-----------------------------------------------

void Company::AddRefund(Refund *refund)
{
	refunds.push_back(refund);
}

float Company::TotalRefundValue()
{
	float *total = new float(1.0f);
	
	for_each( refunds.begin(), refunds.end(), 
			[total](Refund *lhs) { *total += lhs->value;});
	
	float final_total = *total;
	free(total);
	return final_total;
}

//-----------------------------------------------

Refund* Deputy::GetRefund(Refund *refund)
{
	list<Refund*>::iterator it = find_if( refunds.begin(), refunds.end(), 
			[refund](Refund *lhs) {return *lhs == *refund;});
	
	return it == refunds.end() ? NULL : *it;
}

bool Deputy::HasRefund(Refund *refund)
{
	return GetRefund(refund) == NULL ? false : true;
}

void Deputy::AddRefund(Refund *refund)
{
	if(!HasRefund(refund))
		refunds.push_back(refund);
}

float Deputy::TotalRefundValue()
{
	float *total = new float(0.0f);
	
	for_each( refunds.begin(), refunds.end(), 
			[total](Refund *lhs) { *total += lhs->value;});
	
	float final_total = *total;
	free(total);
	return final_total;
}


//--------------------------------------------------------

Deputy* Graph::GetDeputy(Deputy *deputy)
{
	vector<Deputy*>::iterator it = find_if( deputies.begin(), deputies.end(), 
			[deputy](Deputy *lhs) {return *lhs == *deputy;});
	
	return it == deputies.end() ? NULL : *it;
}

bool Graph::HasDeputy(Deputy *deputy)
{
	return GetDeputy(deputy) == NULL ? false : true;
}

void Graph::AddDeputy(Deputy *deputy)
{
	if(!HasDeputy(deputy))
		deputies.push_back(deputy);
}

Company* Graph::GetCompany(Company* company)
{
	vector<Company*>::iterator it = find_if( companys.begin(), companys.end(), 
			[company](Company *lhs) {return *lhs == *company;});
			
	return it == companys.end() ? NULL : *it;
}

bool Graph::HasCompany(Company *company)
{	
	return GetCompany(company) == NULL ? false : true;
}

void Graph::AddCompany(Company *company)
{
	if(!HasCompany(company))
		companys.push_back(company);
}

//------------------------------------------

void PopulateGraph(Graph *graph, Deputy *deputy, Refund *refund, Company *company)
{	
	Deputy *real_dep = deputy;

	if(!graph->HasDeputy(deputy))
		graph->AddDeputy(deputy);
	else
		real_dep = graph->GetDeputy(deputy);
	
	Company *real_comp = company;
	
	if(!graph->HasCompany(company))
		graph->AddCompany(company);
	else
		real_comp = graph->GetCompany(company);
	
	refund->company = real_comp;
	refund->deputy = real_dep;
	
	if(real_dep->HasRefund(refund))
	{
		Refund* temp = real_dep->GetRefund(refund);
		temp->value += refund->value; 
	}
	else
	{
		real_dep->AddRefund(refund);
		real_comp->AddRefund(refund);
	}
}

void FileToGraph(Graph *graph)
{
	fstream file("DirtyDeputies.txt");
	int deputies_count = 79999;
	
	for(int j = 0; j < deputies_count; j++)
	{
		Deputy *deputy = new Deputy();
		Refund *refund = new Refund();
		Company *company = new Company();
		
		getline(file, deputy->name, ',');
		getline(file, deputy->state, ',');
		getline(file, deputy->political_party, ',');
		
		getline(file, refund->description, ',');
		
		getline(file, company->name, ',');
		getline(file, company->id, ',');
		
		getline(file, refund->date, ',');
		file >> refund->value;
		
		string flush_line;
		getline(file, flush_line);
		
		PopulateGraph(graph, deputy, refund, company);
	}
	
	
	file.close();
}

void FindAnomalies(Graph * graph)
{
	sort(graph->companys.begin(), graph->companys.end(),
			[](Company *lhs, Company *rhs) { return lhs->name < rhs->name;} );
	
	//for_each(graph->deputies.begin(), graph->deputies.end(), 
			//[](Deputy *deputy) { cout << *deputy << endl;} );
	
	float *community_total_refunds = new float(0.0f);
	
	for_each(graph->deputies.begin(), graph->deputies.end(),
		[community_total_refunds](Deputy* dep) 
		{			
			for_each(dep->refunds.begin(), dep->refunds.end(), 
				[dep](Refund *refund) { dep->total_refunds_value += refund->value;}	);
			
			*community_total_refunds += dep->total_refunds_value;
		});
	
	cout << "Reembolso medio por deputado: " <<*community_total_refunds / (float)graph->deputies.size() << endl;
	
	sort(graph->deputies.begin(), graph->deputies.end(),
		[](Deputy* lhs, Deputy* rhs) { return lhs->total_refunds_value > rhs->total_refunds_value; });
	
	cout << "\tPossiveis gastos fora do esperado:" << endl;
	
	for_each(graph->deputies.begin(), ++++graph->deputies.begin(),
		[](Deputy *dep)
		{
			cout << "\t" << *dep << endl;
			cout << "\t\t" << "Gasto Total: " << dep->total_refunds_value << endl;
			dep->refunds.sort( [](Refund* lhs, Refund* rhs) {return lhs->value > rhs->value;} );
			cout << "\t\t" << "Maior parcela de gastos: " << *dep->refunds.front() << endl;
		});
	
}

//-------------------------------------------------------------

float GetWeight(Deputy *dep, Company* comp)
{
	list<Refund*>::iterator it = find_if(dep->refunds.begin(), dep->refunds.end(),
		[comp](Refund* ref) {return *(ref->company) == *comp;} );
	
	return (it != dep->refunds.end() ? (*it)->value : 0.0f);
}

float ModularityByWeight(Graph *community, float total_weight)
{
	float *modularity = new float(0.0f);
	
	for_each(community->deputies.begin(), community->deputies.end(),
		[community, modularity, total_weight](Deputy *dep)
		{
			for_each(community->companys.begin(), community->companys.end(),
				[dep, modularity, total_weight](Company* comp)
				{
					*modularity += GetWeight(dep, comp);
					*modularity -= (float)(dep->TotalRefundValue() * comp->TotalRefundValue()) / (float)total_weight;
				});
		});
	
	*modularity *= 1.0f/(float)total_weight;
	
	float mod_final = *modularity;
	free(modularity);
	
	return mod_final;
}

void BuildCommunities(Graph *deputies_net)
{
	list<Graph*> *communities = new list<Graph*>();
	Graph *cur_community;
	string last_party = "none";
	
	//Separacao de comunidades por partidos politicos
	for(vector<Deputy*>::iterator dep = deputies_net->deputies.begin(); dep != deputies_net->deputies.end(); dep++)
	{
		if((*dep)->political_party != last_party)
		{			
			cur_community = new Graph();
			communities->push_back(cur_community);
			last_party = (*dep)->political_party; 
		}
		
		cur_community->deputies.push_back((*dep));
		for_each((*dep)->refunds.begin(), (*dep)->refunds.end(),
			[cur_community](Refund* ref) 
			{
				cur_community->companys.push_back(ref->company);
			});		
	}
	
	//================================================
	
	float *total_weight = new float(0.0f);
	
	for_each(deputies_net->deputies.begin(), deputies_net->deputies.end(),
		[total_weight](Deputy *dep)
		{
			for_each(dep->refunds.begin(), dep->refunds.end(),
				[total_weight](Refund* ref) 
				{
					*total_weight += ref->value;
				});
		});
	
	cout  << endl << "Valor de modularidade via separacao por partido politico: " << endl;
	cout << "|------------------------------------------------------------------|" << endl;
	
	for_each( communities->begin(), communities->end(),
		[total_weight](Graph *community)
		{
			cout << (*community->deputies.begin())->political_party <<  ": ";
			cout << ModularityByWeight(community, *total_weight) << endl;
		});
	
	cout << "|------------------------------------------------------------------|" << endl;
	
	//=========================================================
	
	cout  << endl << "Anomalias identificadas em cada comunidade: " << endl;
	cout << "|------------------------------------------------------------------|" << endl;
	
	for_each( communities->begin(), communities->end(),
		[total_weight](Graph *community)
		{
			cout << "|=============|" << endl;
			cout << (*community->deputies.begin())->political_party <<  ": " << endl;
			FindAnomalies(community);
			
			cout << "|=============|" << endl << endl;
		});
	
	cout << "|------------------------------------------------------------------|" << endl;
		
}

int main()
{
	Graph *deputies_net = new Graph();
	
	cout << "Processamento pode demorar alguns segundos..." << endl;
	
	FileToGraph(deputies_net);
	BuildCommunities(deputies_net);
	
	return 0;
}