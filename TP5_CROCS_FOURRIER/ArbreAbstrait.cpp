#include <stdlib.h>
#include "ArbreAbstrait.h"
#include "Symbole.h"
#include "SymboleValue.h"
#include "Exceptions.h"
#include <typeinfo>

////////////////////////////////////////////////////////////////////////////////
// NoeudSeqInst
////////////////////////////////////////////////////////////////////////////////

NoeudSeqInst::NoeudSeqInst() : m_instructions() {
}

int NoeudSeqInst::executer() {
  for (unsigned int i = 0; i < m_instructions.size(); i++)
    m_instructions[i]->executer(); // on exécute chaque instruction de la séquence
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudSeqInst::ajoute(Noeud* instruction) {
  if (instruction!=nullptr) m_instructions.push_back(instruction);
}

void NoeudSeqInst::traduitCPP(ostream& cout, unsigned int indentation) const {
    for(unsigned int i =0;i<m_instructions.size();i++){
    cout<< setw(indentation) <<""<<m_instructions[i]<<"\n";
    }
    cout<<endl;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudAffectation
////////////////////////////////////////////////////////////////////////////////

NoeudAffectation::NoeudAffectation(Noeud* variable, Noeud* expression)
: m_variable(variable), m_expression(expression) {
}

int NoeudAffectation::executer() {
  int valeur = m_expression->executer(); // On exécute (évalue) l'expression
  ((SymboleValue*) m_variable)->setValeur(valeur); // On affecte la variable
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudAffectation::traduitCPP(ostream& cout, unsigned int indentation) const {
    cout<<setw(indentation)<<""<<m_variable<<m_expression<<endl;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudOperateurBinaire
////////////////////////////////////////////////////////////////////////////////

NoeudOperateurBinaire::NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit)
: m_operateur(operateur), m_operandeGauche(operandeGauche), m_operandeDroit(operandeDroit) {
}

int NoeudOperateurBinaire::executer() {
  int og, od, valeur;
  if (m_operandeGauche != nullptr) og = m_operandeGauche->executer(); // On évalue l'opérande gauche
  if (m_operandeDroit != nullptr) od = m_operandeDroit->executer(); // On évalue l'opérande droit
  // Et on combine les deux opérandes en fonctions de l'opérateur
  if (this->m_operateur == "+") valeur = (og + od);
  else if (this->m_operateur == "-") valeur = (og - od);
  else if (this->m_operateur == "*") valeur = (og * od);
  else if (this->m_operateur == "==") valeur = (og == od);
  else if (this->m_operateur == "!=") valeur = (og != od);
  else if (this->m_operateur == "<") valeur = (og < od);
  else if (this->m_operateur == ">") valeur = (og > od);
  else if (this->m_operateur == "<=") valeur = (og <= od);
  else if (this->m_operateur == ">=") valeur = (og >= od);
  else if (this->m_operateur == "et") valeur = (og && od);
  else if (this->m_operateur == "ou") valeur = (og || od);
  else if (this->m_operateur == "non") valeur = (!og);
  else if (this->m_operateur == "/") {
    if (od == 0) throw DivParZeroException();
    valeur = og / od;
  }
  return valeur; // On retourne la valeur calculée
}

void NoeudOperateurBinaire::traduitCPP(ostream& cout, unsigned int indentation) const {
    cout<<setw(indentation)<<""<<m_operandeGauche<<" "<<m_operateur<<" "<<m_operandeDroit<<endl;
}


////////////////////////////////////////////////////////////////////////////////
// NoeudInstSi
////////////////////////////////////////////////////////////////////////////////

NoeudInstSi::NoeudInstSi(vector<Noeud*> condition, vector<Noeud*> sequence, Noeud* defaut)
: m_condition(condition), m_sequence(sequence), m_defaut(defaut) {
}

int NoeudInstSi::executer() {
    
    
  for(int i =0; i< m_condition.size();i++)
  {
  if (m_condition.at(i)->executer()) 
      return m_sequence.at(i)->executer();// La valeur renvoyée ne représente rien !
  }
  if (m_defaut != nullptr)
      return m_defaut->executer();
}

void
void NoeudInstSi::traduitCPP(ostream & cout, unsigned int indentation) const {
    cout << setw(4*indentation)<<""<<"if (";
    m_condition[0]->traduitCPP(cout, 0);
    cout << ") {"<<endl;
    m_sequence[0]->traduitCPP(cout, indentation+1);
    cout<<setw(4*indentation)<<""<<"}"<<endl;
    
    for (int i=1; i<m_condition.size();i++)
    {
        cout << setw(4*indentation)<<""<<"elseif (";
    m_condition[i]->traduitCPP(cout, 0);
    cout << ") {"<<endl;
    m_sequence[i]->traduitCPP(cout, indentation+1);
    cout<<setw(4*indentation)<<""<<"}"<<endl;
    }
    
    if (m_defaut != nullptr) {
    cout << setw(4*indentation)<<""<<"else (";
    m_condition[0]->traduitCPP(cout, 0);
    cout << ") {"<<endl;
    m_sequence[0]->traduitCPP(cout, indentation+1);
    cout<<setw(4*indentation)<<""<<"}"<<endl;
}


////////////////////////////////////////////////////////////////////////////////
// NoeudInstTantQue
////////////////////////////////////////////////////////////////////////////////

NoeudInstTantQue::NoeudInstTantQue(Noeud* condition, Noeud* sequence): m_conditionTantQue(condition),m_sequenceTantQue(sequence) {

}

int NoeudInstTantQue::executer(){
    while(m_conditionTantQue->executer()){
        m_sequenceTantQue->executer();    
    }
    return 0;
}


////////////////////////////////////////////////////////////////////////////////
// NoeudInstRepeter
////////////////////////////////////////////////////////////////////////////////

NoeudInstRepeter::NoeudInstRepeter(Noeud* condition, Noeud* sequence): m_conditionRepeter(condition),m_sequenceRepeter(sequence) {

}

int NoeudInstRepeter::executer(){
    do
        m_sequenceRepeter->executer();
    while(m_conditionRepeter->executer());
    return 0;//pas de sens
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstPour
////////////////////////////////////////////////////////////////////////////////

NoeudInstPour::NoeudInstPour(Noeud* affectation1, Noeud* condition, Noeud* affectation2, Noeud* sequence)
:m_conditionPour(condition),m_sequencePour(sequence),m_affectation1Pour(affectation1),m_affectation2Pour(affectation2) {
}

int NoeudInstPour::executer(){
    for (m_affectation1Pour->executer(); m_conditionPour->executer(); m_affectation2Pour->executer()) {
        m_sequencePour->executer();
    }
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstEcrire
////////////////////////////////////////////////////////////////////////////////

NoeudInstEcrire::NoeudInstEcrire(vector<Noeud*> noeuds)
:m_noeuds(noeuds) {
}

int NoeudInstEcrire::executer(){
    int j = 0;
    while(j<m_noeuds.size()){
        if(typeid(*m_noeuds[j])==typeid(SymboleValue) && *((SymboleValue*)m_noeuds[j])== "<CHAINE>" ){
            string s = ((SymboleValue*)m_noeuds[j])->getChaine();
            for(int i = 1; i<s.size() -1; i++){
                cout << s[i];
            }
        }else{
            cout << m_noeuds[j]->executer();
        }
    j++;
    }
    return 0;
}

NoeudInstLire::NoeudInstLire(vector<Noeud*> variables)
:m_variables_Lire(variables){

}

int NoeudInstLire::executer(){
    int v;
    for(int i = 0;i<m_variables_Lire.size();i++){
        std::cin >> v;
        ((SymboleValue*)m_variables_Lire[i])->setValeur(v);
    }
    return 0;
}
