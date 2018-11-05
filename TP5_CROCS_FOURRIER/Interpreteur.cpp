#include "Interpreteur.h"
#include <stdlib.h>
#include <iostream>
 
using namespace std;

Interpreteur::Interpreteur(ifstream & fichier) :
m_lecteur(fichier), m_table(), m_arbre(nullptr) {
}

void Interpreteur::analyse() {
  m_arbre = programme(); // on lance l'analyse de la première règle
}

void Interpreteur::tester(const string & symboleAttendu) const throw (SyntaxeException) {
  // Teste si le symbole courant est égal au symboleAttendu... Si non, lève une exception
  static char messageWhat[256];
  if (m_lecteur.getSymbole() != symboleAttendu) {
    sprintf(messageWhat,
            "Ligne %d, Colonne %d - Erreur de syntaxe - Symbole attendu : %s - Symbole trouvé : %s",
            m_lecteur.getLigne(), m_lecteur.getColonne(),
            symboleAttendu.c_str(), m_lecteur.getSymbole().getChaine().c_str());
    throw SyntaxeException(messageWhat);
  }
}

void Interpreteur::testerEtAvancer(const string & symboleAttendu) throw (SyntaxeException) {
  // Teste si le symbole courant est égal au symboleAttendu... Si oui, avance, Sinon, lève une exception
  tester(symboleAttendu);
  m_lecteur.avancer();
}

void Interpreteur::erreur(const string & message) const throw (SyntaxeException) {
  // Lève une exception contenant le message et le symbole courant trouvé
  // Utilisé lorsqu'il y a plusieurs symboles attendus possibles...
  static char messageWhat[256];
  sprintf(messageWhat,
          "Ligne %d, Colonne %d - Erreur de syntaxe - %s - Symbole trouvé : %s",
          m_lecteur.getLigne(), m_lecteur.getColonne(), message.c_str(), m_lecteur.getSymbole().getChaine().c_str());
  throw SyntaxeException(messageWhat);
}

Noeud* Interpreteur::programme() {
  // <programme> ::= procedure principale() <seqInst> finproc FIN_FICHIER
  testerEtAvancer("procedure");
  testerEtAvancer("principale");
  testerEtAvancer("(");
  testerEtAvancer(")");
  Noeud* sequence = seqInst();
  testerEtAvancer("finproc");
  tester("<FINDEFICHIER>");
  return sequence;
}

Noeud* Interpreteur::seqInst() {
  // <seqInst> ::= <inst> { <inst> }
  NoeudSeqInst* sequence = new NoeudSeqInst();
  do {
    sequence->ajoute(inst());
  } while (m_lecteur.getSymbole() == "<VARIABLE>" || 
          m_lecteur.getSymbole() == "si" ||
          m_lecteur.getSymbole() == "tantque" ||
          m_lecteur.getSymbole() == "repeter" ||
          m_lecteur.getSymbole() == "pour"    ||
          m_lecteur.getSymbole() == "ecrire"  ||
          m_lecteur.getSymbole() == "lire"
          );
  // Tant que le symbole courant est un début possible d'instruction...
  // Il faut compléter cette condition chaque fois qu'on rajoute une nouvelle instruction
  return sequence;
}

Noeud* Interpreteur::inst() {
  // <inst> ::= <affectation>  ; | <instSi>
  if (m_lecteur.getSymbole() == "<VARIABLE>") {
      try {
    Noeud *affect = affectation();
    testerEtAvancer(";");
    return affect;
      }
      catch (InterpreteurException & e) {
          cout << e.what() << endl;
          m_lecteur.avancer();
      }
  }
  // Compléter les alternatives chaque fois qu'on rajoute une nouvelle instruction
  else if (m_lecteur.getSymbole() == "si"){//alternative pour si/siRiche
      try{
    return instSi();
      }
      catch (InterpreteurException & e){
          cout<<e.what()<<endl;
          m_lecteur.avancer();
      }
  }
  else if (m_lecteur.getSymbole() == "tantque"){//alternative pour tant que
      try{
      return insTantQue();
      }
      catch(InterpreteurException & e){
          cout<<e.what()<<endl;
          m_lecteur.avancer();
      }
  }
  else if (m_lecteur.getSymbole() == "repeter"){
      try{
      return instRepeter();
      }
      catch(InterpreteurException & e){
          cout<<e.what()<<endl;
          m_lecteur.avancer();
      }
  }
  else if (m_lecteur.getSymbole() == "pour"){
      try{
          return instPour();
      }
      catch(InterpreteurException & e){
          cout<<e.what()<<endl;
          m_lecteur.avancer();
      }
  }
  else if (m_lecteur.getSymbole() == "ecrire"){
      try{
      return instEcrire();
      }
      catch(InterpreteurException & e){
          cout<<e.what()<<endl;
          m_lecteur.avancer();
      }
  }
  else if (m_lecteur.getSymbole() == "lire"){
      try{
      return instLire();}
      catch(InterpreteurException & e){
          cout<<e.what()<<endl;
          m_lecteur.avancer();
      }
  }
  else erreur("Instruction incorrecte");
}

Noeud* Interpreteur::affectation() {
  // <affectation> ::= <variable> = <expression> 
  tester("<VARIABLE>");
  Noeud* var = m_table.chercheAjoute(m_lecteur.getSymbole()); // La variable est ajoutée à la table et on la mémorise
  m_lecteur.avancer();
  testerEtAvancer("=");
  Noeud* exp = expression();             // On mémorise l'expression trouvée
  return new NoeudAffectation(var, exp); // On renvoie un noeud affectation
}

Noeud* Interpreteur::expression() {
  // <expression> ::= <facteur> { <opBinaire> <facteur> }
  //  <opBinaire> ::= + | - | *  | / | < | > | <= | >= | == | != | et | ou
  Noeud* fact = facteur();
  while ( m_lecteur.getSymbole() == "+"  || m_lecteur.getSymbole() == "-"  ||
          m_lecteur.getSymbole() == "*"  || m_lecteur.getSymbole() == "/"  ||
          m_lecteur.getSymbole() == "<"  || m_lecteur.getSymbole() == "<=" ||
          m_lecteur.getSymbole() == ">"  || m_lecteur.getSymbole() == ">=" ||
          m_lecteur.getSymbole() == "==" || m_lecteur.getSymbole() == "!=" ||
          m_lecteur.getSymbole() == "et" || m_lecteur.getSymbole() == "ou"   ) {
    Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
    m_lecteur.avancer();
    Noeud* factDroit = facteur(); // On mémorise l'opérande droit
    fact = new NoeudOperateurBinaire(operateur, fact, factDroit); // Et on construuit un noeud opérateur binaire
  }
  return fact; // On renvoie fact qui pointe sur la racine de l'expression
}

Noeud* Interpreteur::facteur() {
  // <facteur> ::= <entier> | <variable> | - <facteur> | non <facteur> | ( <expression> )
  Noeud* fact = nullptr;
  if (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "<ENTIER>") {
    fact = m_table.chercheAjoute(m_lecteur.getSymbole()); // on ajoute la variable ou l'entier à la table
    m_lecteur.avancer();
  } else if (m_lecteur.getSymbole() == "-") { // - <facteur>
    m_lecteur.avancer();
    // on représente le moins unaire (- facteur) par une soustraction binaire (0 - facteur)
    fact = new NoeudOperateurBinaire(Symbole("-"), m_table.chercheAjoute(Symbole("0")), facteur());
  } else if (m_lecteur.getSymbole() == "non") { // non <facteur>
    m_lecteur.avancer();
    // on représente le moins unaire (- facteur) par une soustractin binaire (0 - facteur)
    fact = new NoeudOperateurBinaire(Symbole("non"), facteur(), nullptr);
  } else if (m_lecteur.getSymbole() == "(") { // expression parenthésée
    m_lecteur.avancer();
    fact = expression();
    testerEtAvancer(")");
  } else
    erreur("Facteur incorrect");
  return fact;
}

Noeud* Interpreteur::instSi() { 
  // <instSi> ::= si ( <expression> ) <seqInst> finsi
  vector<Noeud*> condition ;
  vector<Noeud*> sequence ;
  Noeud* sequenceDefault = nullptr;
  
  testerEtAvancer("si");
  testerEtAvancer("(");
  condition.push_back(expression()); // On mémorise la condition
  testerEtAvancer(")");
  sequence.push_back(seqInst());     // On mémorise la séquence d'instruction
  
  while (m_lecteur.getSymbole() == "sinonsi") {
        testerEtAvancer("sinonsi");
        testerEtAvancer("(");
        condition.push_back(expression()); // On mémorise la 2ème condition
        testerEtAvancer(")");
        sequence.push_back(seqInst());     // On mémorise la 2ème séquence d'instruction
  }
  
  if (m_lecteur.getSymbole() == "sinon") {
    testerEtAvancer("sinon");
    sequenceDefault = seqInst();     // On mémorise la séquence d'instruction par défaut
  }

  testerEtAvancer("finsi");
  return new NoeudInstSi(condition, sequence, sequenceDefault); // Et on renvoie un noeud Instruction Si
}

Noeud* Interpreteur::insTantQue() {
    //<instTantQue> ::=tantque(<expression> )<seqInst> fintantque
    testerEtAvancer("tantque");
    testerEtAvancer("(");
    Noeud* condition = expression();
    testerEtAvancer(")");
    Noeud * sequence = seqInst();
    testerEtAvancer("fintantque");
    return new NoeudInstTantQue(condition,sequence);
}

Noeud* Interpreteur::instRepeter() {
    //   <instRepeter> ::=repeter<seqInst> jusqua(<expression> )
    testerEtAvancer("repeter");
    Noeud* sequence = seqInst();
    testerEtAvancer("jusqua");
    testerEtAvancer("(");
    Noeud* condition = expression();
    testerEtAvancer(")");
    return new NoeudInstRepeter(condition,sequence);
}

Noeud* Interpreteur::instPour() {
    // <instPour> ::=pour( [ <affectation> ];<expression> ;[ <affectation> ])<seqInst> finpour
    testerEtAvancer("pour");
    testerEtAvancer("(");
    Noeud* affectation1 = affectation();
    testerEtAvancer(";");
    Noeud* condition = expression();
    testerEtAvancer(";");
    Noeud* affectation2 = affectation();
    testerEtAvancer(")");
    Noeud* sequence = seqInst();
    testerEtAvancer("finpour");
    return new NoeudInstPour(affectation1,condition,affectation2,sequence);
}

Noeud* Interpreteur::instEcrire() {
    //   <instEcrire>  ::=ecrire(<expression> |<chaine> {,<expression> | <chaine> })
    
    vector<Noeud*> noeuds ;
    
    testerEtAvancer("ecrire");
    testerEtAvancer("(");
    
    do{
        if(m_lecteur.getSymbole() == ","){
            m_lecteur.avancer();
        }
        if(m_lecteur.getSymbole()=="<CHAINE>"){
            noeuds.push_back(m_table.chercheAjoute(m_lecteur.getSymbole()));
            m_lecteur.avancer();
        }else{
            noeuds.push_back(expression());
        }
    }while(m_lecteur.getSymbole() == ",");

    testerEtAvancer(")");
    testerEtAvancer(";");
    return new NoeudInstEcrire(noeuds);
}

Noeud* Interpreteur::instLire() {
    //<instLire>    ::=lire( <variable> {,<variable> })
    
    vector<Noeud*> variablesLire;
    testerEtAvancer("lire");
    testerEtAvancer("(");
    
    while (m_lecteur.getSymbole() != ")") {
        if(m_lecteur.getSymbole()!= ","){
            variablesLire.push_back(m_table.chercheAjoute(m_lecteur.getSymbole()));
        }
            m_lecteur.avancer();
    }

    testerEtAvancer(")");
    testerEtAvancer(";");
    return new NoeudInstLire(variablesLire);
}

void Interpreteur::traduitCPP(ostream& cout, unsigned int indentation) const {
    cout << setw(4*indentation) << "" << "int main() {"endl;
    getArbre()->traduitCPP(cout,indentation+1);
    cout
}

