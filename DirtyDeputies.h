/** *********************************************************
 * Módulo de pareamento Professores-Escolas
 *
 * autor NUR COREZZI - MAT. 15/0143290
 * autor CAIO MATHEUS CAMPOS DE OLIVEIRA - MAT. 14/0176713
 ***********************************************************/

#include <list>
#include <vector>
#include <string>

using namespace std;

class Company;
class Refund;
class Deputy;

ostream& operator<<(ostream& os, Company &rhs);
ostream& operator<<(ostream& os, Refund &rhs);
ostream& operator<<(ostream& os, Deputy &rhs);

class Company
{
public:
	string name;
	string id;
	float total_refunds_value = 0;
	vector<Refund*> refunds;
	
	float TotalRefundValue();
	void AddRefund(Refund* refund);
};

class Refund
{
public:
	string description;
	string date;
	float value;
	Company *company;
	Deputy *deputy;
};

class Deputy
{
public:
	string name;
	string state;
	string political_party;
	float total_refunds_value = 0;
	list<Refund*> refunds;
	
	float TotalRefundValue();
	bool HasRefund(Refund *refund);
	void AddRefund(Refund *refund);
	Refund* GetRefund(Refund *refund);
};

class Graph
{
public:
	vector<Deputy*> deputies;
	vector<Company*> companys;
	
	bool HasDeputy(Deputy *deputy);
	void AddDeputy(Deputy *deputy);
	Deputy* GetDeputy(Deputy *deputy);
	bool HasCompany(Company *company);
	void AddCompany(Company *company);
	Company* GetCompany(Company* company);
};
