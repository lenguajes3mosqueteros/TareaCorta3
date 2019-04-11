#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>
#include <windows.h>

#define CMD_01 1 //Envio de arquivo
#define CMD_01_01 1 //Envio de arquivo
#define CMD_01_02 2//Envio de arquivo

#define CMD_02 2 //Envio de mensagem de texto
#define CMD_02_01 1
#define CMD_02_02 2

#define CMD_03 3 //Envio de mensagem de texto

#define CMD_04 4 //Envio de mensagem de texto
#define CMD_04_01 1
#define CMD_04_02 2

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

struct S_BEEP
{
	int frequencia;
	int tempo;	
};

struct S_RECEBER_ARQUIVO
{
	long tamanhoArquivo; //tamnho do arquivo em bytes;
	char nomeDoArquivo[255];
};

struct S_MENSAGEM_DE_TEXTO
{
	int tamanhoDaMensagem;
	char mensagemDeTexto[255];
};

struct S_LISTAR_ARQUIVOS  //Estrutura para enviar as mensagens de texto
{
	long tamanhoDoArquivo;
    char nomeDoArquivo[1024];
};


/* run this program using the console pauser or add your own getch, system("pause") or input loop */
void ReceberArquivo(int sock, struct S_RECEBER_ARQUIVO *cmdReceberArquivo);
void Codificar(int cmd, int cmdAux);

int main(int argc, char *argv[]) {
	struct DADOS dados;
	struct S_RECEBER_ARQUIVO cmdReceberArquivo;
	struct S_MENSAGEM_DE_TEXTO s_Mensagem_de_Texto;
	struct S_BEEP s_Beep;
	struct S_LISTAR_ARQUIVOS s_Listar_Arquivos;
	SOCKADDR_IN data;
	SOCKADDR_IN dataServer;
	int status, qtdByteRecebidos, porta = 1500;
	int sockServer;
	char buffer[sizeof(struct DADOS)];
	char caminhoDoArquivo[1024];
	
	WSADATA wsaData;
	if(WSAStartup(MAKEWORD(2,2), &wsaData) == -1)
	{
		printf("Erro ao inicializar ");
	}else printf("Socket Windows - OK: %s\n", wsaData.szDescription);
	
	data.sin_addr.s_addr = (ULONG)INADDR_ANY;
	data.sin_family = AF_INET;
	
	data.sin_port = htons(porta);
	memset(&data.sin_zero, 0, 8);
	
	memset(&dataServer, 0, sizeof(SOCKADDR_IN));
	
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if(sock == SOCKET_ERROR) printf("Erro ao criar socket.\n");

	status = bind(sock, (const struct sockaddr *) &data, sizeof(struct sockaddr));
	if (status == -1) printf("Erro ao executar a funcao bind().\n");
	
	status = listen(sock, 1);
	printf("Servidor ativo - Porta: %d\n- A.g.u.a.r.d.a.n.d.o Conexao -\n", porta);
	
conectar:
	sockServer = accept(sock, NULL, NULL);
	if (sockServer == SOCKET_ERROR)
	{
		printf("!!!Erro aceitar conexao...");
	};
	
	printf("\n\n[ CONEXAO RECEBIDA! ]\n...Aguardando comando...\n");
	
	while(1)
	{
		//printf("\n\nEntrou no while...\n");
		
		qtdByteRecebidos = recv(sockServer, buffer, sizeof(struct DADOS), 0);
		if (qtdByteRecebidos < 1) break;
		
		//printf("Qtd bytes recebidos: %d\n", qtdByteRecebidos);
		memcpy(&dados, buffer, sizeof(dados));
		//printf("ID Comando: %d\n", dados.cmd.idCmd);
		
		switch(dados.cmd.idCmd)
		{
			case CMD_01:
				{
					memcpy(&cmdReceberArquivo, dados.parametros, sizeof(dados.parametros));
					printf("[ .. RECEBER ARQUIVO .. ]\nNome do Arquivo: %s\nTamanho do Arquivo: %d\n\n", cmdReceberArquivo.nomeDoArquivo, cmdReceberArquivo.tamanhoArquivo);
					memcpy(buffer, (const char *) &dados , sizeof(int));
					Codificar(CMD_01, CMD_01_01);
					send(sockServer, (const char *)&comando, sizeof(struct COMANDO), 0);
					ReceberArquivo(sockServer, &cmdReceberArquivo);
					break;
				}
			case CMD_02:
				memcpy(&s_Mensagem_de_Texto, dados.parametros, sizeof(struct S_MENSAGEM_DE_TEXTO));
				printf("[ MENSAGEM DO CLIENTE ]: %s\n\n", s_Mensagem_de_Texto.mensagemDeTexto);
				break;
				
			case CMD_03:
				memcpy(&s_Beep, dados.parametros, sizeof(struct S_BEEP));
				printf("[ BEEP ]:\nFrequencia: %d\nTempo: %d\n\n", s_Beep.frequencia, s_Beep.tempo);
				Beep(s_Beep.frequencia,s_Beep.tempo);
				break;
				
			case CMD_04:
				
				if(dados.cmd.idCmdAux == CMD_04_01)
				{
					memcpy(caminhoDoArquivo, dados.parametros, sizeof(dados.parametros));
					printf("[ LISTAGEM DE ARQUIVOS ]:\nCaminho: %s\n", caminhoDoArquivo);
				}
				else
				{
					memcpy(&s_Listar_Arquivos, dados.parametros, sizeof(s_Listar_Arquivos));
					if(s_Listar_Arquivos.tamanhoDoArquivo < 0) printf("\n[PASTA] %s", s_Listar_Arquivos.nomeDoArquivo);
					else if(s_Listar_Arquivos.tamanhoDoArquivo < 1024) printf("\n[%d B] %s", s_Listar_Arquivos.tamanhoDoArquivo, s_Listar_Arquivos.nomeDoArquivo);
					else if(s_Listar_Arquivos.tamanhoDoArquivo >= 1024) printf("\n[%.2f KB] %s", (float)(s_Listar_Arquivos.tamanhoDoArquivo / 1000), s_Listar_Arquivos.nomeDoArquivo);
				}
 				break;
				
			default:
				break;
				
		}

	}
	
	closesocket(sockServer);
	
	goto conectar;
	
	WSACleanup();
	
	return 0;
}

void ReceberArquivo(int sock, struct S_RECEBER_ARQUIVO *cmdReceberArquivo)
{

	FILE *arquivo;
	char buffer[300 * 1024]; //cria um buffer de 300KB para salvar o arquivo

	int bytesRecebidos, contadorBytes = 0;
	
	arquivo = fopen(cmdReceberArquivo->nomeDoArquivo, "wb");
	if (!arquivo) printf("Erro ao criar arquivo para salvar\n");
	
	while(1)
	{
		bytesRecebidos = recv(sock, buffer, sizeof(buffer), 0);

		fwrite(buffer, 1, bytesRecebidos, arquivo);
		contadorBytes = contadorBytes + bytesRecebidos;
	
		printf("\r%.2f Recebido! ", ((float)contadorBytes /  (float)cmdReceberArquivo->tamanhoArquivo) * 100);
		
		if(contadorBytes == cmdReceberArquivo->tamanhoArquivo)
		{
			printf("\n== Arquivo baixado com sucesso! ==\n");
			Codificar(CMD_01, CMD_01_02);
			send(sock, (const char *)&comando, sizeof(struct COMANDO), 0);
			fflush(arquivo);
			break;
		}
	}
	
	fclose(arquivo);
	printf("Final do recebimento\n\n");
	
}


void Codificar(int cmd, int cmdAux)
{
	comando.idCmd = cmd;
	comando.idCmdAux = cmdAux;
	
}

