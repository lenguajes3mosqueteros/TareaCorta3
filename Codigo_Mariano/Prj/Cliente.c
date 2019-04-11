#include <stdio.h>
#include <dirent.h>
#include <windows.h>

#define CMD_01 1 //Envio de arquivo
#define CMD_01_01 1 //Servidor informa que está aguardando o arquivo do cliente
#define CMD_01_02 2 //Servidor informa que recebeu o arquivo completo

#define CMD_02 2 //Envio de mensagem de texto

#define CMD_03 3 //Envio um beep ao cliente, com opção de frequencia e tempo

#define CMD_04 4 //Le os nomes dos arquivos em determinada pasta e envia ao servidor
#define CMD_04_01 1 //Envia ao servidor o nome da pasta em que os arquivos serão listados
#define CMD_04_02 2 //Envia o tamanho e nome do arquivo


//MENU
#define MENU_1_CONECTAR 1
#define MENU_1_LIMPAR_TELA 2

#define MENU_2_LIMPAR_TELA 1
#define MENU_2_ENVIAR_ARQUIVO 2
#define MENU_2_ENVIAR_MENSAGEM 3
#define MENU_2_BEEP 4
#define MENU_2_LISTAR_ARQUIVOS 5
#define MENU_2_LISTAR_PROCESSOS 6
#define MENU_2_EXECUTAR_ARQUIVO 7
#define MENU_2_CONTROLAR_MOUSE 8
#define MENU_2_ENVIAR_TECLAS 9


char menu1[][20] = {"CONECTAR\0","LIMPAR TELA\0", "SAIR\0"};

char menu2[][20] =
{
    "LIMPAR TELA","ENVIAR ARQUIVO","ENVIAR MENSAGEM",
     "ENVIAR BEEP", "LISTAR ARQUIVOS", "LISTAR PROCESSOS", "EXECUTAR ARQUIVO",
     "CONTROLAR MOUSE", "ENVIAR TECLAS", "SAIR"
};

struct MENU{
    int idMenu;
    char *nomeMenu;
};

struct COMANDO
{
    int idCmd;
    int idCmdAux;
}comando;

struct DADOS
{
    struct COMANDO cmd;
    char parametros[1040];
};

struct ENVIAR_ARQUIVO  //Estrutura para enviar as informacoes básicas do arquivo que será enviado
{
	long tamanhoArquivo; //tamnho do arquivo em bytes;
	char nomeDoArquivo[255];
};

struct S_BEEP  //Estrutura para enviar as mensagens de texto
{
    int frequencia;
	int tempo;
};

struct S_MENSAGEM_DE_TEXTO  //Estrutura para enviar as mensagens de texto
{
    int tamanhoDaMensagem;
	char mensagemDeTexto[255];
};

struct S_LISTAR_ARQUIVOS  //Estrutura para enviar as mensagens de texto
{
	long tamanhoDoArquivo;
    char nomeDoArquivo[1024];
};

struct INFO_ARQUIVO
{
    char caminho[1024];
    char nome[512];
    long tamanhoDoArquivo;
    FILE *arquivo;
};

int Menu(void);
int EnviarArquivo(int sock);
void EnviarMensagem(int sock);
void EnviarBeep(int sock);
int ObterInformacoesDoArquivo(struct INFO_ARQUIVO *infoArquivo);
int EnviarAoServidor(int sock, struct DADOS *dados);
void ListaArquivos(int sock);
void LimparTela(void);
int Conectar(const char *ip, int porta);
void PrepararDadosParaEnvio(void *dados, int tamanhoDosDados, struct DADOS *estrutura, int idCmd, int idComAux);

boolean CONECTADO = 0;

int main()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,0), &wsaData);
    int resultado, porta = 1500;
    char ip[] = "127.0.0.1";

    int sock;

    LimparTela();
    resultado = Menu();

    while(resultado > 0)
    {
        if (CONECTADO)
        {

            switch (resultado)
            {

                case MENU_2_LIMPAR_TELA:
                    LimparTela();
                    break;

                case MENU_2_ENVIAR_ARQUIVO:
                    EnviarArquivo(sock);
                    break;

                case MENU_2_ENVIAR_MENSAGEM:
                    EnviarMensagem(sock);
                    break;

                case MENU_2_BEEP:
                    EnviarBeep(sock);
                    break;

                case MENU_2_LISTAR_ARQUIVOS:
                    ListaArquivos(sock);
                    break;

                case MENU_2_ENVIAR_TECLAS:
                    printf("\nOpcao %d nao implementado -_-\n", resultado);
                    break;

                case MENU_2_LISTAR_PROCESSOS:
                    printf("\nOpcao %d nao implementado -_-\n", resultado);

                    break;

                case MENU_2_EXECUTAR_ARQUIVO:
                    printf("\nOpcao %d nao implementado -_-\n", resultado);

                    break;

                case MENU_2_CONTROLAR_MOUSE:
                    printf("\nOpcao %d nao implementado -_-\n", resultado);

                    break;

                default: goto fim; break;
            }

        }
        else
        {
            switch(resultado)
            {

                case MENU_1_CONECTAR:
                    sock = Conectar(ip, porta);
                    if (sock == -1) printf("Servidor não encontrado...\n");
                    break;

                case MENU_1_LIMPAR_TELA:
                    LimparTela();
                    break;

                default: goto fim; break;
            }
        }

        resultado = Menu();
    }

fim:
	closesocket(sock);
	WSACleanup();

    printf("Fim\n");

    return 0;
}

int Menu(void)
{
    int tecla;
    int cont = 0;

    printf("\n\n[===... MENU ...===]\n");
    if (!CONECTADO)
    {
        printf("[!DESCONECTADO!]\n");
        for(cont = 0; cont < (sizeof(menu1)/20); cont++)
        {
            printf("[ %d ] - %s\n", cont+1, menu1[cont]);
        }
    }
    else
    {
        printf("[* * * * C O N E C T A D O * * * *]\n");
        for(cont = 0; cont < (sizeof(menu2)/20); cont++)
        {
            printf("[ %d ] - %s\n", cont+1, menu2[cont]);
        }
    }

    printf("DIGITE A OPCAO: ");
    scanf("%d", &tecla);
    fflush(stdin);

    return tecla;
}

void LimparTela(void)
{
    system("cls");
}

int Conectar(const char *ip, int porta)
{
    SOCKADDR_IN dadosParaConexao;
    int sock;

    dadosParaConexao.sin_addr.s_addr = inet_addr(ip);
    dadosParaConexao.sin_family = AF_INET;
    dadosParaConexao.sin_port = ntohs(porta);
    memset(&dadosParaConexao.sin_zero, 0, 8);

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (sock == SOCKET_ERROR) printf("Erro ao criar socket\n");

    int status = connect(sock, (const struct sockaddr*)&dadosParaConexao, sizeof(SOCKADDR_IN));
    if (status == SOCKET_ERROR)
    {
        printf("Erro ao conectar...\n");
    }
    else CONECTADO = 1;

    return sock;
}

int EnviarArquivo(int sock)
{
    #define BUFFER_MAX_LEITURA 1024*100 // 100 KByte
    struct DADOS dados;
    int cont, contBytesEnviados = 0;
    int qtdLeituras = 0, qtdBytesRestantes = 0, bytesEnviados, bytesLidos, retorno;
    int tamanhoDoBufferLeitura = BUFFER_MAX_LEITURA;
    char buffer[BUFFER_MAX_LEITURA];
    struct ENVIAR_ARQUIVO cmdEnviarArquivo;
    struct INFO_ARQUIVO infoArquivo;

    retorno = ObterInformacoesDoArquivo(&infoArquivo);
    if(retorno == -1) return -1;
    
    strcpy(cmdEnviarArquivo.nomeDoArquivo, infoArquivo.nome);
    cmdEnviarArquivo.tamanhoArquivo = infoArquivo.tamanhoDoArquivo;

    dados.cmd.idCmd = CMD_01;
    memcpy(&dados.parametros, &cmdEnviarArquivo, sizeof(cmdEnviarArquivo));
    EnviarAoServidor(sock, &dados);

    recv(sock, (char*)&comando, sizeof(struct COMANDO), 0);
     //memcpy(&comando, buffer, sizeof(int)*2);
    if(comando.idCmd == CMD_01 && comando.idCmdAux == CMD_01_01)
    {
        printf("Servidor aguardando o arquivo...\n");
    }

    if (infoArquivo.tamanhoDoArquivo <= tamanhoDoBufferLeitura){
        bytesLidos = fread(buffer, 1, infoArquivo.tamanhoDoArquivo, infoArquivo.arquivo);
        if(bytesLidos != infoArquivo.tamanhoDoArquivo) printf("Erro ao ler dados na funcao read(). 1");

        bytesEnviados = send(sock, buffer, infoArquivo.tamanhoDoArquivo, 0 );
        if(bytesEnviados != infoArquivo.tamanhoDoArquivo) printf("Envio incompleto de bytes na funcao send(). 1\n");
        printf("%.2f%% Enviado!", ((float)bytesEnviados / (float)infoArquivo.tamanhoDoArquivo)* 100);
    }
    else{
        qtdLeituras = infoArquivo.tamanhoDoArquivo / tamanhoDoBufferLeitura; //Calcula quantas vezes será necessário ler o arquivo com a função fread()
        qtdBytesRestantes = infoArquivo.tamanhoDoArquivo % tamanhoDoBufferLeitura; //Calcula quantos bytes ficaram faltando para leitura
    }

    for(cont = 1; cont <= qtdLeituras; cont++)
    {
        bytesLidos = fread(buffer, 1, tamanhoDoBufferLeitura, infoArquivo.arquivo);
        if(bytesLidos != tamanhoDoBufferLeitura) printf("Erro ao ler dados na funcao read(). 2");

        bytesEnviados = send(sock, buffer, tamanhoDoBufferLeitura, 0);
        if(bytesEnviados != tamanhoDoBufferLeitura) printf("Envio incompleto de bytes na funcao send(). 2\n");
        contBytesEnviados+= bytesEnviados;

        printf("\r%.2f%% Enviado! ", ((float)contBytesEnviados / (float)infoArquivo.tamanhoDoArquivo)* 100);
    }

    if (qtdBytesRestantes > 0)
    {
        bytesLidos = fread(buffer, 1, qtdBytesRestantes, infoArquivo.arquivo);
        if(bytesLidos != qtdBytesRestantes) printf("Erro ao ler dados na funcao read(). 3");

        bytesEnviados = send(sock, buffer, qtdBytesRestantes, 0 );
        if(bytesEnviados != qtdBytesRestantes) printf("Envio incompleto de bytes na funcao send(). 3\n");
        contBytesEnviados+= bytesEnviados;
        printf("\r%.2f%% Enviado! ", (float)(contBytesEnviados / (float)infoArquivo.tamanhoDoArquivo)* 100);
    }

    printf(" --- Arquivo enviado --- !\n\n");

    recv(sock, (char*)&comando, sizeof(struct COMANDO), 0);
    if(comando.idCmd == CMD_01 && comando.idCmdAux == CMD_01_02)
    {
        printf(" === Servidor informa que recebeu o arquivo por completo! ===\n\n");
    }
}

void EnviarMensagem(int sock)
{
    struct S_MENSAGEM_DE_TEXTO s_Mensagem;
    struct DADOS dados;

    printf("Digite a mensagem: ");
    scanf("%[^\n]s", s_Mensagem.mensagemDeTexto);
    fflush(stdin);

    s_Mensagem.tamanhoDaMensagem = strlen(s_Mensagem.mensagemDeTexto);

    PrepararDadosParaEnvio(&s_Mensagem, sizeof(s_Mensagem), &dados, CMD_02, 0);
    EnviarAoServidor(sock, &dados);
}

void EnviarBeep(int sock)
{
    struct S_BEEP s_Beep;
    struct DADOS dados;
    int frequencia, tempo;

    printf("Digite a frequencia: ");
    scanf("%d", &frequencia);
    fflush(stdin);
    printf("Digite o tempo em segundos: ");
    scanf("%d", &tempo);
    fflush(stdin);

    s_Beep.frequencia = frequencia;
    s_Beep.tempo = tempo;

    PrepararDadosParaEnvio(&s_Beep, sizeof(s_Beep), &dados, CMD_03, 0);
    EnviarAoServidor(sock, &dados);
}

int ObterInformacoesDoArquivo(struct INFO_ARQUIVO *infoArquivo)
{
    char caminho[1024];
    int cont = 0;

    memset(infoArquivo, 0, sizeof(struct INFO_ARQUIVO));
    memset(caminho, 0, 1024);

    printf("\nFavor informar o caminho do arquivo:\n");

    scanf("%[^\n]s", caminho);
    fflush(stdin);

    infoArquivo->arquivo = (FILE *)fopen(caminho, "rb"); //abre o arquivo no modo leitura binária
    if ((long)infoArquivo->arquivo < 1){
    	
        printf("Nao foi possivel abrir o arquivo. Caso o caminho ou nome contenha caracteres acentuados, por favor, remova-os e tente novamente.\n");
        return -1;
    }

    for(cont = strlen(caminho); cont > 0; cont--)
    {
        if(caminho[cont] == '\\'){
            strcpy(infoArquivo->nome, &caminho[cont+1]);
            break;
        }
    }

    strcpy(infoArquivo->caminho, caminho);

    int ret = fseek(infoArquivo->arquivo, 0, SEEK_END);
    if (ret == -1){
        printf("!!!Erro - nao foi possivel obter o tamanho do arquivo...\n");
        return -1;
    }

    infoArquivo->tamanhoDoArquivo = ftell(infoArquivo->arquivo);
    rewind(infoArquivo->arquivo);

    return 0;
}

void ListaArquivos(int sock)
{
    DIR *diretorio;
    FILE *arquivoParaConsulta;
    struct dirent *arquivo;
    struct DADOS dados;
    struct S_LISTAR_ARQUIVOS s_Listar_Arquivos;
    long tamanhoDoArquivo;
    char caminho[1024];
    char caminhoCompleto[2048];

    printf("Digite o caminho:\n");
    fflush(stdin);
    scanf("%[^\n]s", caminho);
    fflush(stdin);

    diretorio = opendir(caminho);

    PrepararDadosParaEnvio(caminho, sizeof(caminho), &dados, CMD_04, CMD_04_01);
    EnviarAoServidor(sock, &dados);

    while(1)
    {
        tamanhoDoArquivo = -1;
        memset(caminhoCompleto, 0, 2048);
        arquivo = readdir(diretorio);
        if(arquivo == NULL) break;

        printf("%s\n",arquivo->d_name);

        strcpy(caminhoCompleto, caminho);
        if( caminho[strlen(caminho)-1] != '\\' )
        {
            strcat(caminhoCompleto,"\\");
        }
        strcat(caminhoCompleto, arquivo->d_name);
        arquivoParaConsulta = (FILE *)fopen(caminhoCompleto, "r");

        if ((int)arquivoParaConsulta > 1)
        {
            fseek(arquivoParaConsulta, 0, SEEK_END);
            tamanhoDoArquivo = ftell(arquivoParaConsulta);
            fclose(arquivoParaConsulta);
        }

        s_Listar_Arquivos.tamanhoDoArquivo = tamanhoDoArquivo;
        strcpy(s_Listar_Arquivos.nomeDoArquivo, arquivo->d_name);
        PrepararDadosParaEnvio(&s_Listar_Arquivos, sizeof(s_Listar_Arquivos), &dados, CMD_04, CMD_04_02);
        EnviarAoServidor(sock, &dados);

    }

}

void PrepararDadosParaEnvio(void *dados, int tamanhoDosDados, struct DADOS *estrutura, int idCmd, int idComAux)
{
    memset(estrutura, 0, sizeof(struct DADOS));
    estrutura->cmd.idCmd = idCmd;
    estrutura->cmd.idCmdAux = idComAux;
    memcpy(estrutura->parametros, dados, tamanhoDosDados);
}

int EnviarAoServidor(int sock, struct DADOS *dados)
{
    int bytesEnviados;

    bytesEnviados = send(sock, (const char *)dados, sizeof(struct DADOS), 0);

    if (bytesEnviados != sizeof(struct DADOS) || bytesEnviados == 0) //verifica se a estrutura foi enviada por completo
    {
        printf("Dados enviados incompletos!");
        return 1;
    }

    return 0;
}



