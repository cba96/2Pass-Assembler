#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <conio.h>
#define _CRT_SECURE_NO_WARNINGS

struct reg {
	char reg_name[3];
	char reg_num[4];
}Reg[20];

struct ins {
	char instruct[6];
	char dest[2];
	char sour[2];
	char word_type[2];
	char ins_code[3];
	char ins_len[2];
	char mod_reg[9];
}Instr[100], modInstr[100]; 
int MaxI, InstrP = 0;

struct symbol_tbl {
	char symbol[10];
	char word_type[2];
	int lc;
	char data[10];
}Symbol[30];

int MaxS = 0;

struct sentence 
{
	char label[10];
	char _operator[10];
	char operand[3][10];
}Sen; 
int LC;

FILE *ObjSave; 
FILE *SymbolSave;

			  

void Initialize() 
{
	int i = 0, j = 1;
	FILE *regi, *inst;

	regi = fopen("reg.txt", "r");
	inst = fopen("inst.txt", "r");
	while (!feof(regi)) 
	{
		fscanf(regi, "%s %s \n", Reg[i].reg_name, Reg[i].reg_num);
		i++;
	}
	while (!feof(inst)) 
	{
		fscanf(inst, "%6s %2s %2s %4s %3s %2s %9s \n", Instr[j].instruct, Instr[j].dest,
			Instr[j].sour, Instr[j].word_type, Instr[j].ins_code, Instr[j].ins_len,
			Instr[j].mod_reg);
		j++;
	}
	MaxI = j - 1;
	fclose(regi);
	fclose(inst);
}


int Analyze(char *operand) 
{

	int i = 0;
	char regist[][4] = { "AX","BX","CX","DX","AL","BL","CL","DL","AH","BH","CH","DH","" };

	if (isdigit(operand[0])) return 0;

	else
	{

		while (i != 13) 
		{
			if (!strcmp(operand, regist[i])) 
			{

				if (i < 4) return 1;
				else return 2;
			}
			else i++;
		}
	}
	return 3;
}

#define MAX_INS 1
int Add_Chk(char *sen) 
{

	register k = MaxI;
	int i = 0, j = 0, l = 0, wp = 0;
	char op[6][10], opcode[][10] = { "mov","" };
	while (sen[wp] != '\n') {
		while (sen[wp] == ' ' || sen[wp] == '\t' || sen[wp] == ',')wp++;
		while (sen[wp] != ' '&&sen[wp] != '\n'&&sen[wp] != ',') {
			*(op[j] + i) = sen[wp];
			i++;
			wp++;
		}
		*(op[j] + i) = '\0';
		i = 0;
		j++;
	}
	i = 0;

	while (strcmp(opcode[i], "")) 
	{
		if (_stricmp(opcode[i], op[0])) i++;
		else 
		{
			strcpy(Sen._operator, op[0]);
			for (l = 1; l < j; l++)
				strcpy(Sen.operand[l - 1], op[l]);
			break;
		}
	}

	if (i == MAX_INS) 
	{ 

		strcpy(Sen.label, op[0]);
		strcpy(Sen._operator, op[1]);
		for (l = 2; l < j; l++)
			strcpy(Sen.operand[l - 2], op[l]);
	} 
	strcpy(Instr[0].instruct, op[0]);


	switch (Analyze(op[1])) 
	{

	case 0: strcpy(Instr[0].dest, "i");
		break;
	case 1: strcpy(Instr[0].dest, "r"); 
		strcpy(Instr[0].word_type, "w");
		break;
	case 2: strcpy(Instr[0].dest, "r");
		strcpy(Instr[0].word_type, "b");
		break;
	case 3: strcpy(Instr[0].dest, "m");
		break;
	}



	switch (Analyze(op[2])) 
	{

	case 0: strcpy(Instr[0].sour, "i");
		break;
	case 1: strcpy(Instr[0].sour, "r");
		strcpy(Instr[0].word_type, "w");
		break;
	case 2: strcpy(Instr[0].sour, "r");
		strcpy(Instr[0].word_type, "b");
		break;
	case 3: strcpy(Instr[0].sour, "m");
		break;
	}
	while (_stricmp(Instr[k].instruct, Instr[0].instruct) || strcmp(Instr[k].dest, Instr[0].dest) || strcmp(Instr[k].sour, Instr[0].sour) || strcmp(Instr[k].word_type, Instr[0].word_type)) {
	
	}
	return k; 
}

void PassI(char *buf) 
{
	int i;
	static int j = 0;

	i = Add_Chk(buf);
	if (i) 
	{ 
		printf("%04X: %s", LC, buf);
		LC += atoi(Instr[i].ins_len);
	}
	else 
	{
		if (!_stricmp(Sen._operator, "dw"))
			strcpy(Symbol[j].word_type, "w");
		else if (!_stricmp(Sen._operator, "db"))
			strcpy(Symbol[j].word_type, "b");
		strcpy(Symbol[j].symbol, Sen.label);
		strcpy(Symbol[j].data, Sen.operand[0]);
		Symbol[j].lc = LC;
		printf("%04X: %s", LC, buf);
		if (*Symbol[j].word_type == 'w') LC += 2;
		else if (*Symbol[j].word_type == 'b') LC += 1;
		j++;
		MaxS++;
	}
}

int btoi(char *dig) 
{
	register i = 0, ret = 0;

	while (*(dig + i) != '\0') 
	{
		if (*(dig + i) == '1') ret += pow(2, strlen(dig + i) - 1);
		i++;
	}
	return ret;
}

void PassII(char *buf) 
{
	int i, j = 0, k = 0;
	ObjSave = fopen("objCode.txt", "a+");
	i = Add_Chk(buf);
	if (i) 
	{
		modInstr[InstrP] = Instr[i];
		printf("%04x: %3s", LC, Instr[i].ins_code);
		fprintf(ObjSave, "%04x: %3s", LC, Instr[i].ins_code);
		if (!strcmp(Instr[i].dest, "r")) 
		{
			while (_stricmp(Reg[j].reg_name, Sen.operand[0]))
				j++;
			strncpy(strchr(modInstr[InstrP].mod_reg, '?'), Reg[j].reg_num, 3);
		}

		j = 0;

		if (!strcmp(Instr[i].sour, "r")) 
		{
			while (_stricmp(Reg[j].reg_name, Sen.operand[1]))
				j++;
			strncpy(strchr(modInstr[InstrP].mod_reg, '?'), Reg[j].reg_num, 3);
		}

		if (strcmp(Instr[i].dest, "m") && strcmp(Instr[i].sour, "m")) 
		{
			printf("%02X\t\t%s", btoi(modInstr[InstrP].mod_reg), buf);
			fprintf(ObjSave, "%02X\t\t%s", btoi(modInstr[InstrP].mod_reg), buf);
		}
		else 
		{

			if (!strcmp(Instr[i].dest, "m"))
				while (strcmp(Symbol[k].symbol, Sen.operand[0])) k++;
			if (!strcmp(Instr[i].sour, "m"))
				while (strcmp(Symbol[k].symbol, Sen.operand[1])) k++;
			if (Symbol[k].lc == (atoi(Symbol[k].data))) 
			{
				printf("  %02X\t%04X\t%s", btoi(modInstr[InstrP].mod_reg), Symbol[k].lc, buf);
				fprintf(ObjSave, "  %02X\t%04X\t%s", btoi(modInstr[InstrP].mod_reg), Symbol[k].lc, buf);
			}
			else 
			{
				printf("  %02X\t%04X R\t%s", btoi(modInstr[InstrP].mod_reg), Symbol[k].lc, buf);
				fprintf(ObjSave, "  %02X\t%04X R\t%s", btoi(modInstr[InstrP].mod_reg), Symbol[k].lc, buf);
			}
		}
		LC += atoi(Instr[i].ins_len);
	}
	else 
	{
		k = 0;
		while (strcmp(Symbol[k].symbol, Sen.label)) k++;
		if (!strcmp(Symbol[k].word_type, "w")) 
		{

			printf("%04X:  %02X\t\t%s", LC, atoi(Symbol[k].data), buf);
			fprintf(ObjSave, "%04X:  %02X\t\t%s", LC, atoi(Symbol[k].data), buf);
		}
		if (!strcmp(Symbol[k].word_type, "b")) 
		{

			printf("%04X:  %02X\t\t\t%s", LC, atoi(Symbol[k].data), buf);
			fprintf(ObjSave, "%04X:  %02X\t\t\t%s", LC, atoi(Symbol[k].data), buf);
		}
		if (*Symbol[k].word_type == 'w') LC += 2;
		else if (*Symbol[k].word_type == 'b') LC += 1;
	}
	InstrP++;
	fclose(ObjSave);
}

void Symbol_Print() 
{
	int i = 0;

	SymbolSave = fopen("symbol.txt", "w+");
	printf("\n* Symbol table *\n");
	printf("SYMBOL\tDATA(ADDRESS)\tRELOCATION\n");
	for (i = 0; i < MaxS; i++) 
	{
		if (!strcmp(Symbol[i].word_type, "w")) 
		{
			printf("%s\t%X\t\t%d\n", Symbol[i].symbol, Symbol[i].lc, (Symbol[i].lc != atoi(Symbol[i].data) ? 1 : 0));
			fprintf(SymbolSave, "%s\t%X\t\t%d\n", Symbol[i].symbol, Symbol[i].lc, (Symbol[i].lc != atoi(Symbol[i].data) ? 1 : 0));
		}
		if (!strcmp(Symbol[i].word_type, "b")) 
		{
			printf("%s\t%X\t\t%d\n", Symbol[i].symbol, Symbol[i].lc, (Symbol[i].lc != atoi(Symbol[i].data) ? 1 : 0));
			fprintf(SymbolSave, "%s\t%X\t\t%d\n", Symbol[i].symbol, Symbol[i].lc, (Symbol[i].lc != atoi(Symbol[i].data) ? 1 : 0));
		}
	}
	fclose(SymbolSave);
}

void main() 
{
	char buf[50];
	FILE *in;

	in = fopen("test1.asm", "r");
	Initialize();
	printf("\nPass1:\n");
	while (1) 
	{
		fgets(buf, 30, in);
		if (feof(in)) break;
		PassI(buf);
	}
	Symbol_Print();
	rewind(in);
	LC = 0;
	ObjSave = fopen("objCode.txt", "w+");
	printf("\nPass2:\n");
	while (1) 
	{
		fgets(buf, 30, in);
		if (feof(in)) break;
		PassII(buf);
	}
	fclose(in);
	_getch();

}