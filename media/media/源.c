# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <math.h>
#pragma warning(disable:4996)
struct SNode {
	long int freq; // Ƶ��
	int depth; // ��ȣ����볤��
	unsigned char gray;
	struct SNode * pPar; 
	struct SNode * pLeft;
	struct SNode * pRight; 
	char * code; // �ֽڱ���

};
struct SNode nodes[256]; // ���ֽڱ���
struct SNode tree[520];
struct SNode* head = 0; // ������ʱ���У�ָ���һλ��ָ��
struct SNode* temp = 0; // ��ʱָ�룬���ڽ���ʱ��ָ��ǰ���ߵ��Ľ��
int m_iCodeNum; // �Ѿ�����ĸ���
int m_iRead;
long int imcodenum = 0;
double efficiency;
double avglen;
int totallens;
//////////////��ʼ������///////////////////
void init(struct SNode*p, int n) {
	int i;
	for (i = 0; i < n; i++) {
		p->code = "";
		p->freq = 0;
		p->pLeft = NULL;
		p->pPar = NULL;
		p->pRight = NULL;
		p->depth = 0;
		p->gray = i;
		p = p + 1;
	}
}
///////////ͳ��ÿ���Ҷ�ֵ��Ƶ��////////////////////
void freqadd(unsigned char n)
{
	nodes[n].freq++;
}
//////////////����ƽ�����볤�ȼ�����Ч��///////////
void geteffavg(){
	int sumfreq=0;
	avglen = 0;
	double xinyuanshang = 0;
	for(int i = 0; i<256;i++){
		sumfreq = sumfreq + nodes[i].freq;
	}
	for(int i = 0; i < 256;i++){
		avglen += (nodes[i].depth-1) * (double)nodes[i].freq / (double)sumfreq;
			}
	efficiency = avglen/8;
	printf("�����ƽ���볤Ϊ%lf\n",avglen);
	printf("����Ч��Ϊ%lf\n",efficiency);
	
}
/////////�Ҷ�ֵ���򴴽���ʱ����//////////////
void addtolist(struct SNode* pNode)
{
	if (head == NULL)  				// ����Ϊ�գ�		

	{
		head = pNode;// ֱ�Ӽ���ͷ��
	}
	else
	{
		if (pNode->freq < head->freq)//���¼���=head����Ƶ��ʱ�����½��ŵ�����
		{
			pNode->pPar = head;
			head = pNode;
		}
		else
		{
			struct	SNode* p = head;
			while ((p->pPar != 0) && (p->pPar->freq < pNode->freq))
			{
				p = p->pPar;
			}
			pNode->pPar = p->pPar;
			p->pPar = pNode;
		}
	}
}
//////////////////���ɱ���//////////////////////////////
void createcode(struct SNode * snode) {
	struct	SNode* p = snode;
	int post = 0;
	if (snode->depth == 1) {//��ʱ������ֻ��һ��Ԫ��
		snode->code = (char *)malloc(sizeof(char));
		snode->code[0] = '0';
	}
	else {
		snode->code = (char *)malloc(sizeof(char)*snode->depth);//Ϊ����������洢�ռ�
		post = snode->depth - 1;
		snode->code[post] = '\0';
		post--;
		while (p->pPar != NULL) {
			if (p == p->pPar->pLeft) // �ý��Ϊ�丸�ڵ������
			{
				snode->code[post] = '0';
			}
			else
			{
				snode->code[post] = '1';//�ý��Ϊ�丸�ڵ���Һ���
			}
			post--;
			p = p->pPar;
		}
	}
}
/////////// ���ӽ����ӽڵ�ı��볤��//////////////////////////
void AddChildLen(struct SNode* pNode)
{
	pNode->depth++;
	if (pNode->pLeft != NULL)
	{
		AddChildLen(pNode->pLeft);
	}
	if (pNode->pRight != NULL)
	{
		AddChildLen(pNode->pRight);
	}
}
/////////////////��������huffman��/////////////////
void createcodetree(struct SNode *ptree, int len) {
	int offset, i = 0;

	while (head != NULL && head->pPar != NULL)
	{
		// ȡ��ǰ����Ƶ����С���
		struct SNode *p1 = head;
		struct SNode *p2 = head->pPar;
		head = p2->pPar;
		offset = len + i;
		ptree[offset].freq = p1->freq + p2->freq; // Ƶ��Ϊ����֮��
		ptree[offset].pLeft = p1;
		ptree[offset].pRight = p2;
		p1->pPar = &ptree[offset];
		p2->pPar = &ptree[offset];
		AddChildLen(&ptree[offset]);//��������ÿ���������
		addtolist(&ptree[offset]);//���²����Ľ�㣬�ŵ���ʱ������
		i++;
	}
	// ��������ɣ����ɱ���
	for (i = 0; i < 256; i++)
	{
		if (nodes[i].freq > 0)
		{
			createcode(&nodes[i]);
		}
	}


}
/////////////////////////////////���ر�����������Ԫ�ĳ��Ⱥ�////////////////////////////
int getallcodelen() {
	int alllen = 0, i = 0;
	for (; i < 256; i++) {
		alllen = alllen + nodes[i].depth - 1;
	}
	return alllen;
}
////////////////////�����еĽ��ı���ŵ�codebuf������/////////////////////////
char * getcode(int len) {
	char *codebuf;
	int i, offset = 0;
	codebuf = (char *)malloc(sizeof(char)*(len + 1));
	codebuf[len] = '\0';
	for (i = 0; i < 256; i++) {
		
			strcpy(codebuf + offset, nodes[i].code);
			offset += (nodes[i].depth - 1);
		
	}

	return codebuf;
}
////////////////////////////����//////////////////////////////////
void huffmancode() {
	struct SNode *p;//�м�ָ��
	unsigned char b[512];//��������
	char * buf, *test;
	int n = 0, len = 0, j = 0;
	int i = 0;
	int codelens = 0;//���б�����ܳ��ȡ�
	int count = 0;
	FILE * fpin, *fpout;
	init(nodes, 256);//��ÿһ������ʼ��
	init(tree, 511);
	fpin = fopen("lena.bmp", "rb");
	fseek(fpin, 1078, 0);//����ǰ54�ֽ�
	len = fread(b, sizeof(char), 512, fpin);//ѭ������ͼ������ݲ���
	while (len > 0)
	{
		for (i = 0; i < len; i++)
		{
			freqadd(b[i]);//ͳ�ƸûҶ�ֵ��Ƶ��
		}
		len = fread(b, sizeof(char), 512, fpin);
		count++;
	}
	printf("��ȡ����Ϊ%d\n", count);
	for (i = 0, len = 0; i < 256; i++) {//������������ʱ���У�����С��������
		if (nodes[i].freq > 0) {
			addtolist(&nodes[i]);
			nodes[i].depth++;
			len++;
			tree[i] = nodes[i];
		}

	}
	createcodetree(tree, len);//����huffman���������б���
	codelens = getallcodelen();//�õ�����Ԫ�ر�����ܵı��볤��
	buf = getcode(codelens);//�����еı����ݴ浽buf������
	printf("ÿ���Ҷ�ֵ����Ӧ�ı���������ʾ\n");
	for (n = 0; n < 256; n++) {
		printf("%d:%s\n", n, nodes[n].code);
	}
	printf("\n\n");

	fpout = fopen("huffman.txt", "wb+");
	count = 0;
	fseek(fpin, 1078, 0);
	len = fread(b, sizeof(char), 512, fpin);//ѭ������ͼ������ݲ���
	while (len > 0) {
		for (i = 0; i < len; i++) {
			n = b[i];
			for (j = 0; j < (nodes[n].depth - 1); j++) {

				fwrite(&nodes[n].code[j], sizeof(char), 1, fpout);
			}
		}
		len = fread(b, sizeof(char), 512, fpin);

	}
	fclose(fpin);//�ر��ļ�ָ��
	fclose(fpout);

}
////////////////////����////////////////////////
void decode() {
	FILE * hufin, *hufout, *imgin;
	struct SNode *p;
	int i = 0, n = 0, num = 0;
	char b;
	char debuf[1968288];

	imgin = fopen("lena.bmp", "rb");
	hufout = fopen("decode.bmp", "wb+");
	hufin = fopen("huffman.txt", "rt+");
	fread(debuf, sizeof(char), 1078, imgin);//��Դ�ļ���ȡ�ļ�ͷ
	fwrite(debuf, sizeof(char), 1078, hufout);//д���ļ�ͷ
	hufin = fopen("huffman.txt", "rt+");
	fgets(debuf, 1968288, hufin);  //�������еı�������
	p = head;                
	for (i = 0; debuf[i] != '\0'; i++) {//��ʼ����
		switch (debuf[i]) {
		case '0': {
			if (p->pLeft != NULL) {//����Ϊ0�������Ӳ�Ϊ�գ�ָ�����ӡ�
				p = p->pLeft;
				break;
			}
			else {              //����Ϊ�գ�ΪҶ�ӽ�㣬ȡ����Ҷ�ֵ
				b = p->gray;
				fwrite(&b, sizeof(char), 1, hufout);

				p = head;
				i = i - 1;//��Ϊ��i++
				break;
			}

		}
		case '1': {
			if (p->pRight != NULL) {//����Ϊ1�����Һ��Ӳ�Ϊ�գ�ָ���Һ���
				p = p->pRight;
				break;
			}

			else {//�Һ���Ϊ�գ�ΪҶ�ӽ�㣬ȡ����Ҷ�ֵ
				b = p->gray;
				fwrite(&b, sizeof(char), 1, hufout);
				p = head;
				i = i - 1;
				break;
			}
		}
		default: break;
		}
	}
	b = p->gray;//������һ��������
	fwrite(&b, sizeof(char), 1, hufout);
}


void main() {
	char ans = 0;
	huffmancode();
	geteffavg();
	printf("����d��ʼ����");
	scanf("%c", &ans);
	switch (ans) {
	case 'd':
		decode();
		printf("�������");
		break;
	default:break;
	}


}














