# Interpretador de Comandos 
O projeto uma implementação de um shell utilizando a linguagem C++. O programa shell.cpp executa comandos do sistema Linux e possui comandos internos, como cd, pwd, history e exit.

O projeto utiliza chamadas do sistema Linux por meio do fork(), execv(), waitpid(), chdir(), getcwd() e access().

## Como compilar?
Utilize o seguinte comando no terminal:
g++ shell.cpp -o shell

Depois de compilar, execute-o com: 
./shell

Em seguida, o programa irá apresentar o simbolo "$", indicando a espera por um comando.

Exemplo de comando:
$ls
$pwd
/home/usuario
$

## Funcionamento
O programa shell executa comandos do sistema quando o usuário digita um comando. Sempre que um comando é digitado, ele é lido e executado, e depois o programa volta a mostrar o `$` para que outro comando possa ser informado. O shell possui implementados comandos internos cd, pwd, history e exit, e externos, nativos do sistema Linux. O programa continua funcionando até que o usuário digite `exit`, que encerra o shell.

## Comandos internos
Existem quatro comandos principais, são eles:
- exit:
Encerra o shell.
- pwd:
Mostra o diretório atual, por meio da utilização da função getcwd().
- cd:
Permite mudar o diretório atual, essa mudança é feita através da função chdir().
- history:
O comando history armazena comandos digitados durante a execução, são mostrados os últimos comandos feitos. Para limpar o histórico digite: $history -c. É possível executar usando seu offset, o número informado recupera o comando e executa novamente.

## Execução de Comandos Externos
O shell procura por meio do PATH, quando o comando não é interno. Em seguida, utiliza o fork() para criar um novo processo, o processo filho utiliza execv() para substituir o processo pelo programa solicitado. Já o processo pai, utiliza waitpid(), aguardando o processo filho terminar. Desse modo, o shell volta a aceitar outro comando depois que o anterior termina.

## Histórico
Os comandos são armazenados em um vetor e, sempre que um comando é executado, esse comando é adicionado ao histórico. O programa permite rever os dez últimos comandos. Ao encerrar o shell, todo o histórico é perdido, já que o mesmo não é salvo em arquivo.

## Variáveis de Ambiente
O programa utiliza variáveis através de getenv(), sendo elas:
PATH: encontra comando externos.
HOME: utilizada pelo comando cd quando nenhum diretório é informado.
