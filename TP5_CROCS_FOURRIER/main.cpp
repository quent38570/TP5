#include <iostream>
using namespace std;
#include "Interpreteur.h"
#include "Exceptions.h"

int main(int argc, char* argv[]) {
  string nomFich;
  string etat = "next";
  int i = -1;
  
  std::vector<std::string> nomFichs = {"testSi.txt","testTantQue.txt","testRepeter.txt","testPour.txt","testEcrire.txt","testLire.txt","testErreur.txt"};
  while(etat == "next"){
      i++;
      cout << "Nom du fichier test: " << nomFichs[i] << endl;
        nomFich = nomFichs[i];
      ifstream fichier(nomFich.c_str());
      try {
        Interpreteur interpreteur(fichier);
        interpreteur.analyse();
        // Si pas d'exception levée, l'analyse syntaxique a réussi
        cout << endl << "================ Syntaxe Correcte" << endl;
        // On affiche le contenu de la table des symboles avant d'exécuter le programme
        cout << endl << "================ Table des symboles avant exécution : " << interpreteur.getTable();
        cout << endl << "================ Execution de l'arbre" << endl;
        // On exécute le programme si l'arbre n'est pas vide
        if (interpreteur.getArbre()!=nullptr) interpreteur.getArbre()->executer();
        // Et on vérifie qu'il a fonctionné en regardant comment il a modifié la table des symboles
        cout << endl << "================ Table des symboles apres exécution : " << interpreteur.getTable();
        cout << endl << "================ Résultat traduction c++ : ";
        interpreteur.traduitCPP(cout,1);

      } catch (InterpreteurException & e) {
        cout << e.what() << endl;
      }
      if(i == nomFichs.size()-1){
          i--;
      }
      cout << "(next -> " << nomFichs[i+1] << ")" << endl;
      cout << "Taper 'next' pour passer au prochain test et autre chose pour quitter:";
      cin >> etat;     
  }
  return 0;
}