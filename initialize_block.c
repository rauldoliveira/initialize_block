#include "postgres.h"

#include <string.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>

#include "storage/bufpage.h"

FILE *fp = NULL;
int segmentNumber;

void
PageInit(char *page, Size pageSize, Size specialSize)
{
	PageHeader	p = (PageHeader) page;

	specialSize = MAXALIGN(specialSize);

	Assert(pageSize == BLCKSZ);
	Assert(pageSize > specialSize + SizeOfPageHeaderData);

	/* Make sure all fields of page are zero, as well as unused space */
	MemSet(p, 0, pageSize);

	p->pd_flags = 0;
	p->pd_lower = SizeOfPageHeaderData;
	p->pd_upper = pageSize - specialSize;
	p->pd_special = pageSize - specialSize;
	PageSetPageSizeAndVersion(page, pageSize, PG_PAGE_LAYOUT_VERSION);
	/* p->pd_prune_xid = InvalidTransactionId;		done by above MemSet */
}

static unsigned int
GetSegmentNumberFromFileName(const char *fileName)
{
	int			segnumOffset = strlen(fileName) - 1;

	if (segnumOffset < 0)
		return 0;

	while (isdigit(fileName[segnumOffset]))
	{
		segnumOffset--;
		if (segnumOffset < 0)
			return 0;
	}

	if (fileName[segnumOffset] != '.')
		return 0;

	return atoi(&fileName[segnumOffset + 1]);
}


int main(int argc, char *argv[]){

    char arquivo[500]; 
	char *block;
    int bloco;
	int pageOffset;
	unsigned int position;
    
    bloco = atoi(argv[2]); ;
    strcpy(arquivo, argv[1]);

    printf("Inicializando bloco = <%d> do arquivo = <%s>\n", bloco, arquivo);
    
    fp = fopen(arquivo, "r+b");
	if (!fp){
		printf("Nao foi possivel abrir <%s>.\n", arquivo);
		exit(1);
	}

	segmentNumber = GetSegmentNumberFromFileName(arquivo);

	pageOffset = bloco % ((1024 * 1024 * 1024) / BLCKSZ);

	block = (char *)malloc(BLCKSZ);
	if (!block)
	{
		printf("\nErro: Nao foi possivel criar buffer do tamanho <%d>.\n",
			   BLCKSZ);
	}

	position = BLCKSZ * pageOffset;

	if (fseek(fp, position, SEEK_SET) != 0)
	{
		printf("Error: Seek encontrou um erro antes do start block <%d>.\n", position);
		exit(2);
	}

	if (fread(block, 1, BLCKSZ, fp) != BLCKSZ)
	{
		printf("Erro ao ler data file <%s>.\n", arquivo);
		exit(2);
	}


	PageInit(block,BLCKSZ, 0);

	if (fseek(fp, position, SEEK_SET) != 0)
	{
		printf("Error: Seek encontrou um erro antes do start block <%d>.\n", position);
		exit(2);
	}

	if (fwrite(block, 1, BLCKSZ, fp) != BLCKSZ)
	{
		printf("Erro ao escrever data file <%s>.\n", arquivo);
		exit(3);
	}
	else
	{
		printf("Sucesso ao escrever data file <%s>.\n", arquivo);	
	}

	fclose(fp);

	return 0;
}