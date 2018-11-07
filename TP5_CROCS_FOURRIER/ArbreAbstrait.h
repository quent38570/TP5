#ifndef ARBREABSTRAIT_H
#define ARBREABSTRAIT_H

// Contient toutes les déclarations de classes nécessaires
//  pour représenter l'arbre abstrait

#include <vector>
#include <iostream>
#include <iomanip>
using namespace std;

#include "Symbole.h"
#include "Exceptions.h"

////////////////////////////////////////////////////////////////////////////////
class Noeud {
// Classe abstraite dont dériveront toutes les classes servant à représenter l'arbre abstrait
// Remarque : la classe ne contient aucun constructeur
  public:
    virtual int  executer() =0 ; // Méthode pure (non implémentée) qui rend la classe abstraite
    virtual void ajoute(Noeud* instruction) { throw OperationInterditeException(); }
    virtual ~Noeud(){} // Présence d'un destructeur virtuel conseillée dans les classes abstraites
    virtual void traduitCPP(ostream & cout, unsigned int indentation) const {};
};

////////////////////////////////////////////////////////////////////////////////
class NoeudSeqInst : public Noeud {
// Classe pour représenter un noeud "sequence d'instruction"
//  qui a autant de fils que d'instructions dans la séquence
  public:
     NoeudSeqInst();   // Construit une séquence d'instruction vide
    ~NoeudSeqInst() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();    // Exécute chaque instruction de la séquence
    void ajoute(Noeud* instruction);  // Ajoute une instruction à la séquence
    void traduitCPP(ostream & cout, unsigned int indentation) const;

  private:
    vector<Noeud *> m_instructions; // pour stocker les instructions de la séquence
};

////////////////////////////////////////////////////////////////////////////////
class NoeudAffectation : public Noeud {
// Classe pour représenter un noeud "affectation"
//  composé de 2 fils : la variable et l'expression qu'on lui affecte
  public:
     NoeudAffectation(Noeud* variable, Noeud* expression); // construit une affectation
    ~NoeudAffectation() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();        // Exécute (évalue) l'expression et affecte sa valeur à la variable
    void traduitCPP(ostream & cout, unsigned int indentation) const;
  private:
    Noeud* m_variable;
    Noeud* m_expression;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudOperateurBinaire : public Noeud {
// Classe pour représenter un noeud "opération binaire" composé d'un opérateur
//  et de 2 fils : l'opérande gauche et l'opérande droit
  public:
    NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit);
    // Construit une opération binaire : operandeGauche operateur OperandeDroit
   ~NoeudOperateurBinaire() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();            // Exécute (évalue) l'opération binaire)
    void traduitCPP(ostream & cout, unsigned int indentation) const;
    
  private:
    Symbole m_operateur;
    Noeud*  m_operandeGauche;
    Noeud*  m_operandeDroit;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstSi : public Noeud {
// Classe pour représenter un noeud "instruction si"
//  et ses 2 fils (minimum): la condition du si et la séquence d'instruction associée
  public:
    NoeudInstSi(vector<Noeud*> condition, vector<Noeud*> sequence, Noeud* defaut);
     // Construit une "instruction si" avec sa condition et sa séquence d'instruction
   ~NoeudInstSi() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();  // Exécute l'instruction si : si condition vraie on exécute la séquence
    void traduitCPP(ostream & cout, unsigned int indentation) const; // Traduit un noeud si en c++

  private:
    vector<Noeud*>  m_condition;
    vector<Noeud*>  m_sequence;
    Noeud* m_defaut;
};



////////////////////////////////////////////////////////////////////////////////
class NoeudInstTantQue : public Noeud{
    //classe pour représenter un noeud "tant que"
    //et ses 2 fils : condition du tant que et sa séquence
public :
    NoeudInstTantQue(Noeud* condition,Noeud* sequence);
    ~NoeudInstTantQue(){}
    int executer();
    void traduitCPP(ostream & cout, unsigned int indentation) const;
    
private :
    Noeud* m_conditionTantQue;
    Noeud* m_sequenceTantQue;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstRepeter : public Noeud{
    //classe pour représenter un noeud "repeter jusqua"
    //et ses 2 fils : condition du tant que et sa séquence
public :
    NoeudInstRepeter(Noeud* condition,Noeud* sequence);
    ~NoeudInstRepeter(){}
    int executer();
    void traduitCPP(ostream & cout, unsigned int indentation) const;
    
private :
    Noeud* m_conditionRepeter;
    Noeud* m_sequenceRepeter;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstPour : public Noeud{
    //classe pour représenter un noeud "pour finpour"
    //et ses 2 fils : condition du tant que et sa séquence
public :
    NoeudInstPour(Noeud* affectation1, Noeud* condition, Noeud* affectation2,Noeud* sequence);
    ~NoeudInstPour(){}
    int executer();
    void traduitCPP(ostream & cout, unsigned int indentation) const;
    
private :
    Noeud* m_conditionPour;
    Noeud* m_sequencePour;
    Noeud* m_affectation1Pour;
    Noeud* m_affectation2Pour;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstEcrire : public Noeud{
    //classe pour représenter un noeud "Ecrire"
    //et ses 2 fils minimum : chaine et/ou expression
public :
    NoeudInstEcrire(vector<Noeud*> noeuds);
    ~NoeudInstEcrire(){}
    int executer();
    void traduitCPP(ostream & cout, unsigned int indentation) const;
    
private :
    vector<Noeud*> m_noeuds;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstLire : public Noeud{
    //classe pour représenter un noeud "Ecrire"
    //et ses 2 fils minimum : chaine et/ou expression
public :
    NoeudInstLire(vector<Noeud*> variables);
    ~NoeudInstLire(){}
    int executer();
    void traduitCPP(ostream & cout, unsigned int indentation) const;
    
private :
    vector<Noeud*> m_variables_Lire;
};

#endif /* ARBREABSTRAIT_H */
