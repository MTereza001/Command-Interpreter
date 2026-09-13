#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <string>
#include <vector>

using namespace std;

vector<string> historico;

vector<string> separar_comando(string linha) {
    vector<string> argumentos;
    string palavra = "";

    for (int i = 0; i < linha.length(); i++) {
        if (linha[i] == ' ') {
            if (palavra != "") {
                argumentos.push_back(palavra);
                palavra = "";
            }
        } else {
            palavra += linha[i];
        }
    }

    if (palavra != "") {
        argumentos.push_back(palavra);
    }

    return argumentos;
}

string procurar_comando(string comando) {
    if (comando.find('/') != string::npos) {
        if (access(comando.c_str(), X_OK) == 0) {
            return comando;
        }
        return "";
    }

    char *path = getenv("PATH");

    if (path == NULL) {
        return "";
    }

    string caminho = "";
    string diretorio = "";

    for (int i = 0; ; i++) {
        char caractere = path[i];

        if (caractere == ':' || caractere == '\0') {
            caminho = diretorio + "/" + comando;

            if (access(caminho.c_str(), X_OK) == 0) {
                return caminho;
            }

            diretorio = "";

            if (caractere == '\0') {
                break;
            }
        } else {
            diretorio += caractere;
        }
    }

    return "";
}

void comando_pwd() {
    char *diretorio = getcwd(NULL, 0);

    if (diretorio != NULL) {
        cout << diretorio << endl;
        free(diretorio);
    } else {
        cout << "Erro ao obter diretorio." << endl;
    }
}

void comando_cd(vector<string> argumentos) {
    string diretorio;

    if (argumentos.size() == 1) {
        char *home = getenv("HOME");

        if (home == NULL) {
            cout << "cd: HOME nao definida" << endl;
            return;
        }

        diretorio = home;
    } else {
        diretorio = argumentos[1];
    }

    if (chdir(diretorio.c_str()) != 0) {
        cout << "cd: diretorio nao encontrado" << endl;
    }
}

void mostrar_historico() {
    int quantidade = historico.size();

    if (quantidade > 10) {
        quantidade = 10;
    }

    for (int i = 0; i < quantidade; i++) {
        int indice = historico.size() - 1 - i;
        cout << i << " " << historico[indice] << endl;
    }
}

void limpar_historico() {
    historico.clear();
}

void process_command(string command);

void executar_historico(int offset) {
    if (offset < 0 || offset >= 10 || offset >= historico.size()) {
        cout << "history: offset invalido" << endl;
        return;
    }

    int indice = historico.size() - 1 - offset;
    string comando = historico[indice];

    cout << comando << endl;
    process_command(comando);
}

void process_command(string command) {
    vector<string> argumentos = separar_comando(command);

    if (argumentos.size() == 0) {
        return;
    }

    string nome_comando = argumentos[0];

    // Comandos internos
    if (nome_comando == "exit") {
        int codigo = 0;

        if (argumentos.size() > 1) {
            codigo = atoi(argumentos[1].c_str());
        }

        exit(codigo);
    }

    if (nome_comando == "pwd") {
        comando_pwd();
        return;
    }

    if (nome_comando == "cd") {
        comando_cd(argumentos);
        return;
    }

    if (nome_comando == "history") {
        if (argumentos.size() > 1 && argumentos[1] == "-c") {
            limpar_historico();
            return;
        }

        if (argumentos.size() > 1) {
            int offset;

            try {
                offset = stoi(argumentos[1]);
            } catch (...) {
                cout << "history: offset invalido" << endl;
                return;
            }

            executar_historico(offset);
            return;
        }

        mostrar_historico();
        return;
    }

    // Comando externo
    string absolute_path = procurar_comando(nome_comando);

    if (absolute_path == "") {
        cout << "Command not found: " << nome_comando << endl;
        return;
    }

    pid_t pid = fork();

    if (pid < 0) {
        cout << "Erro de execucao!" << endl;
        return;
    } else if (pid == 0) {
        // Processo filho
        vector<char*> argv;

        for (int i = 0; i < argumentos.size(); i++) {
            argv.push_back(const_cast<char*>(argumentos[i].c_str()));
        }

        argv.push_back(NULL);

        execv(absolute_path.c_str(), argv.data());

        cout << "Erro ao executar comando." << endl;
        exit(1);
    } else {
        // Processo pai
        waitpid(pid, NULL, 0);
    }
}

int main() {
    while (true) {
        cout << "$";
        cout.flush();

        string command;

        if (!getline(cin, command)) {
            break;
        }

        if (command == "") {
            continue;
        }

        vector<string> argumentos = separar_comando(command);

        if (argumentos.size() == 0) {
            continue;
        }

        if (argumentos[0] == "history" &&
            argumentos.size() > 1 &&
            argumentos[1] != "-c") {

            try {
                int offset = stoi(argumentos[1]);

                if (offset < 0 ||
                    offset >= historico.size() ||
                    offset >= 10) {

                    cout << "history: offset invalido" << endl;
                } else {
                    historico.push_back(command);
                    executar_historico(offset);
                }
            } catch (...) {
                cout << "history: offset invalido" << endl;
            }

            continue;
        }

        historico.push_back(command);
        process_command(command);
    }

    return 0;
}
