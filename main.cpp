#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstring>

using namespace std;

struct Tranzitie {
    string nod;
    string simbol;
    string nod_viitor;
};

class Lambda_NFA {
    string nod_initial;
    vector<string> noduri_finale;
    vector<Tranzitie> tranzitii;
    
public:
    Lambda_NFA() {}

    void load_NFA_from_file(string filename) {
        ifstream file(filename);

        file >> nod_initial;

        int nr_noduri_finale;
        file >> nr_noduri_finale;

        for (int i = 0; i < nr_noduri_finale; i++) {
            string nod;
            file >> nod;
            noduri_finale.push_back(nod);
        }

        int nr_tranzii;
        file >> nr_tranzii;

        for (int i = 0; i < nr_tranzii; i++) {
            string from, to, symbol;
            file >> from >> symbol >> to;
            tranzitii.push_back({ from, symbol, to });
        }

        file.close();
    }

    string get_nod_initial() const {
        return this->nod_initial;
    }

    vector<string> get_noduri() const {
        vector<string> noduri;
        for (auto tranzitie : tranzitii) {
            bool exists1 = false, exists2 = false;
            for (string nod : noduri) {
                if (nod == tranzitie.nod)
                    exists1 = true;
                if (nod == tranzitie.nod_viitor)
                    exists2 = true;
                if (exists1 and exists2)
                    break;
            }
            if (tranzitie.nod == tranzitie.nod_viitor && !exists1) {
                noduri.push_back(tranzitie.nod);
            }
            else {
                if (!exists1) {
                    noduri.push_back(tranzitie.nod);
                }
                if (!exists2) {
                    noduri.push_back(tranzitie.nod_viitor);
                }
            }
        }
        return noduri;
    }

    vector<string> get_simboluri() const {
        vector<string> simboluri;
        for (auto tranzitie : tranzitii) {
            bool exists = false;
            for (string simbol : simboluri) {
                if (simbol == tranzitie.simbol) {
                    exists = true; break;
                }
            }
            if (!exists) {
                simboluri.push_back(tranzitie.simbol);
            }
        }
        return simboluri;
    }

    vector<Tranzitie> get_tranzitii() const {
        return this->tranzitii;
    }

    vector<string> get_noduri_finale() const {
        return this->noduri_finale;
    }

    void print_nfa() const {
        cout << "Stare initiala: " << nod_initial << endl << endl;
        cout << "Stari finale: ";
        for (string nod : noduri_finale)
            cout << nod << ", ";
        cout << endl << endl << "Tranzitii: " << endl;
        for (Tranzitie tranz : tranzitii)
            cout << tranz.nod << " " << tranz.simbol << " " << tranz.nod_viitor << endl;
    }
};

class DFA {
    string nod_initial;
    vector<string> noduri_finale;
    vector<Tranzitie> tranzitii;

public:
    DFA() {}

    void set_nod_initial(const string nod) {
        this->nod_initial = nod;
    }

    string get_nod_initial() const{
        return this->nod_initial;
    }

    void set_nod_final(const string nod) {
        this->noduri_finale.push_back(nod);
    }

    vector<string> get_noduri_finale() const{
        return this->noduri_finale;
    }

    bool check_nod_final(const string nod_) const {
        for (string nod : noduri_finale) {
            if (nod == nod_) {
                return true;
            }
        }
        return false;
    }

    void add_tranzitie(const Tranzitie tranzitie) {
        this->tranzitii.push_back(tranzitie);
    }    

    void print_dfa() const {
        cout << "Stare initiala: " << nod_initial << endl << endl;
        cout << "Stari finale: ";
        for (string nod : noduri_finale)
            cout << nod << ", ";
        cout << endl << endl << "Tranzitii: " << endl;
        for (Tranzitie tranz : tranzitii)
            cout << tranz.nod << " " << tranz.simbol << " " << tranz.nod_viitor << endl;
    }

    void transform_lambdaNFA_to_DFA(const Lambda_NFA nfa) {
        vector<vector<string>> lambda_inchideri;
        vector<vector<string>> inchideri_simboluri;
        vector<string> simboluri_nfa;
        vector<string> noduri_nfa;
        string nod_init_nfa;
        string temp;

        // Pentru a obtine lambda-inchiderile fiecarui nod al automatului, plec din fiecare nod si obtin drumul posibil folosind doar lambda
        noduri_nfa = nfa.get_noduri();
        sort(noduri_nfa.begin(), noduri_nfa.end());
        lambda_inchideri.resize(noduri_nfa.size());
        for (int i = 0; i < noduri_nfa.size(); i++) {
            lambda_inchideri[i].push_back(noduri_nfa[i]);
            int j = 0, inc_dim = lambda_inchideri[i].size();
            while(j < inc_dim) {
                for (Tranzitie tranzitie : nfa.get_tranzitii()) {
                    if (tranzitie.nod == lambda_inchideri[i][j] && tranzitie.simbol == "#" && tranzitie.nod_viitor != lambda_inchideri[i][j]) {
                        // Verific daca nodul este deja in vector
                        bool exists = false;
                        for (string nod : lambda_inchideri[i]) {
                            if (nod == tranzitie.nod_viitor) {
                                exists = true;
                            }
                        }
                        // Daca nu este, inseamna ca are sens sa il adaug
                        if (!exists) {
                            lambda_inchideri[i].push_back(tranzitie.nod_viitor);
                        }
                    }
                }
                j++;
                inc_dim = lambda_inchideri[i].size();
            }
        }

        /*
        for (auto linie : lambda_inchideri) {
            for (auto inchidere : linie)
                cout << inchidere << " ";
            cout << endl;
        }
        */

        // Salvam in simboluri_nfa simbolurile din lambda-NFA sortate
        simboluri_nfa = nfa.get_simboluri();
        sort(simboluri_nfa.begin(), simboluri_nfa.end());
        simboluri_nfa.erase(simboluri_nfa.begin());

        /*
        for (auto sim : simboluri_nfa) {
            cout << sim << " ";
        }*/

        // Obtinem nodul initial din lambda-inchiderea nodului initial al lambda-NFA-ului
        nod_init_nfa = nfa.get_nod_initial();
        inchideri_simboluri.resize(1);
        
        for (vector<string> inchidere : lambda_inchideri) {
            if (inchidere[0] == nod_init_nfa) {
                bool final = false;
                string nod_init_dfa;
                for (string nod : inchidere) {
                    nod_init_dfa += "-";
                    nod_init_dfa += nod;
                    for (string nod_fin : nfa.get_noduri_finale()) {
                        if (nod == nod_fin) {
                            final = true;
                        }
                    }
                }
                nod_init_dfa.erase(nod_init_dfa.begin());
                this->set_nod_initial(nod_init_dfa);
                if (final) {
                    this->set_nod_final(nod_init_dfa);
                }
                inchideri_simboluri[0].push_back(nod_init_dfa);
                break;
            }
        }

        int j = 0, inc_dim = 1;
        while (j < inc_dim) { 
            for (int i = 0; i < simboluri_nfa.size(); i++) {
                vector<string> inc_stare_part;
                vector<string> inc_stare;
                string stare;
                // Pe baza multimii de noduri posibile initial, formez multimea nodurilor posibile obtinute prin deplasarea cu simbolul curent
                for (int k = 0; k <= inchideri_simboluri[j][0].size(); k++) {
                    if(inchideri_simboluri[j][0][k] != '-' && k != inchideri_simboluri[j][0].size()){
                        stare += inchideri_simboluri[j][0][k];
                    }
                    else {
                        for (Tranzitie tranzitie : nfa.get_tranzitii()) {
                            if (tranzitie.nod == stare && simboluri_nfa[i] == tranzitie.simbol) {
                                bool exists = false;
                                for (string stare_existenta : inc_stare_part) {
                                    if (tranzitie.nod_viitor == stare_existenta) {
                                        exists = true;
                                        break;
                                    }
                                }
                                if (!exists) {
                                    inc_stare_part.push_back(tranzitie.nod_viitor);
                                }
                            }
                        }
                        stare.clear();
                    }
                }

                // Reunesc lambda-inchiderile fiecarui nod obtinul anterior, rezultand astfel nodul final
                for (string stare2 : inc_stare_part) {
                    for (vector<string> lamb_inc : lambda_inchideri) {
                        if (lamb_inc[0] == stare2) {
                            for (string lambda_stare : lamb_inc) {
                                bool exists = false;
                                for (string inchidere_stare : inc_stare) {
                                    if (inchidere_stare == lambda_stare) {
                                        exists = true;
                                        break;
                                    }
                                }
                                if (!exists) {
                                    inc_stare.push_back(lambda_stare);
                                }
                            }
                        }
                    }
                }
                sort(inc_stare.begin(), inc_stare.end());
                
                // Introduc starea finala pe pozitia aferenta
                bool final = false;
                temp.clear();
                for (string x : inc_stare) {
                    temp += "-";
                    temp += x;
                    // In timp ce creez starea finala sub forma de string, verific daca este si o stare finala 
                    for (string nod_fin : nfa.get_noduri_finale()) {
                        if (x == nod_fin) {
                            final = true;
                        }
                    }
                }
                temp.erase(temp.begin());
                inchideri_simboluri[j].push_back(temp);

                // Verific daca starea finala pe care am adaugat-o are deja tranzitiile create. Daca nu, o adaug pentru a ii fi create.
                bool exists = false;
                for (vector<string> inchideri : inchideri_simboluri) {
                    if (inchideri[0] == temp) {
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    inchideri_simboluri.resize(inchideri_simboluri.size()+1);
                    inchideri_simboluri[inchideri_simboluri.size() - 1].push_back(temp);
                    inc_dim++;
                }

                // Adaug fiecare tranzitie nou creata in DFA-ul nou construit
                if (inchideri_simboluri[j][0] != "" && inchideri_simboluri[j][i + 1] != "") {
                    this->add_tranzitie({ inchideri_simboluri[j][0], simboluri_nfa[i], inchideri_simboluri[j][i + 1] });
                }
                // Daca noile noduri create sunt si finale, atunci le marchez ca atare in noul DFA
                if (final) {
                    bool exists = false;
                    for (string nod_fin : this->get_noduri_finale()) {
                        if (temp == nod_fin) {
                            exists = true;
                            break;
                        }
                    }
                    if (!exists) {
                        this->set_nod_final(temp);
                    }
                }
            }
            j++;
        }
    }
};

int main()
{
    Lambda_NFA lambda_nfa;
    string filename;
    cin >> filename;
    lambda_nfa.load_NFA_from_file(filename);
    lambda_nfa.print_nfa();
    
    cout << endl;
    for (int i = 0; i < 70; i++) {
        cout << "*";
    }
    cout << endl << endl;
    
    DFA dfa_trans;
    dfa_trans.transform_lambdaNFA_to_DFA(lambda_nfa);
    dfa_trans.print_dfa();
}

