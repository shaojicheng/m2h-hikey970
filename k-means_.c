#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int K, N, D;  //�������Ŀ�������������ݵ�ά��
K = 4;
N = 10;
D = 1;

unsigned int* data;  //�������
int* in_cluster;  //���ÿ���������ĸ�����
unsigned int* cluster_center;  //���ÿ����������ĵ�

unsigned int** array(int m, int n);
unsigned int* array1(int n);
void freearray(unsigned int** p);
unsigned int getDistance(unsigned int a, unsigned int b);
void cluster();
unsigned int getDifference();
void getCenter(int in_cluster[]);
void  k_means();

int main()
{
    int i, j;
    FILE* fp;
    if ((fopen_s(&fp, "data3.txt", "r")) != 0)    fprintf(stderr, "cannot open data.txt!\n");
    //if (fscanf_s(fp, "K=%d\n", k) != 1)        fprintf(stderr, "load error!\n");
    data = array1(N);  //������������
    for (i = 0; i < N; i++)
        fscanf_s(fp, "%u", &data[i]);  //��ȡ���ݵ�
    printf("Data sets:\n");
    for (i = 0; i < N; i++)
        printf("%u \n", data[i]);

    printf("-----------------------------\n");
    printf("\n-----------k=%d----------\n", K);
    cluster_center = array1(K);  //��������ĵ�
    in_cluster = (int*)malloc(N * sizeof(int));  //ÿ�����ݵ���������ı�־����
    k_means();
    printf("The new center of cluster is:\n");
    for (i = 0; i < K; i++)
        printf("%u \n", cluster_center[i]);
}


void  k_means()
{
    int i, j, count = 0;
    unsigned int temp1, temp2;


    srand((unsigned int)(time(NULL)));  //�����ʼ��k�����ĵ�
    for (i = 0; i < K; i++)
        cluster_center[i] = data[(int)((double)N * rand() / (RAND_MAX + 1.0))];
    printf("The new center of cluster is:\n");
    for (i = 0; i < K; i++)
        printf("%u \n", cluster_center[i]);

    cluster();  //�����k�����ĵ���о���
    temp1 = getDifference();  //��һ�����ĵ���������ݵ�ľ���֮��
    count++;
    printf("The first difference between data and center is: %u\n\n", temp1);


    getCenter(in_cluster);
    cluster();  //���µ�k�����ĵ���еڶ��ξ���
    temp2 = getDifference();
    count++;
    printf("The second difference between data and center is: %u\n\n", temp2);

    while (fabs(temp2 - temp1) !=0) {   //�Ƚ�ǰ�����ε�����������ȼ�������
        temp1 = temp2;
        getCenter(in_cluster);
        cluster();
        temp2 = getDifference();
        count++;
        printf("The %dth difference between data and center is: %u\n\n", count, temp2);
    }

    printf("\nThe total number of cluster is: %d\n", count);  //ͳ�Ƶ�������
    //system("pause");  //gcc������ɾ��
}


//��̬����һά����
unsigned int* array1(int n)
{
    unsigned int* p;
    p = (unsigned int*)malloc(n * sizeof(unsigned int));
    return p;
}

//��̬������ά����
unsigned int** array(int m, int n)
{
    int i;
    unsigned int** p;
    p = (unsigned int**)malloc(m * sizeof(unsigned int*));
    p[0] = (unsigned int*)malloc(m * n * sizeof(unsigned int));
    for (i = 1; i < m; i++)    p[i] = p[i - 1] + n;
    return p;
}


//�ͷŶ�ά������ռ�õ��ڴ�
void freearray(unsigned int** p)
{
    free(*p);
    free(p);
}


//����ŷ����þ���
unsigned int getDistance(unsigned int a, unsigned int b)
{
    unsigned int sum = 0;
    if (a > b) {
        sum = pow(a - b, 2);
    }
    else
    {
        sum = pow(b - a, 2);
    }
    return sqrt(sum);
}

//��N�����ݵ���࣬���ÿ���������ĸ�����
void cluster()
{
    int i, j;
    unsigned int min;
    unsigned int** distance = array(N, K);  //���ÿ�����ݵ㵽ÿ�����ĵ�ľ���
    //float distance[N][K];  //Ҳ��ʹ��C99�䳤����
    for (i = 0; i < N; ++i) {
        unsigned int zero;
        zero = 0;
        min = zero - 1;
        for (j = 0; j < K; ++j) {
            distance[i][j] = getDistance(data[i], cluster_center[j]);
            //printf("%f\n", distance[i][j]);
            if (distance[i][j] < min) {
                min = distance[i][j];
                in_cluster[i] = j;
            }
        }
        printf("data[%d] in cluster-%d\n", i, in_cluster[i] + 1);
    }
    printf("-----------------------------\n");
    free(distance);
}

//�������о�������ĵ��������ݵ�ľ���֮��
unsigned int getDifference()
{
    int i, j;
    unsigned int sum = 0;
    for (i = 0; i < K; ++i) {
        for (j = 0; j < N; ++j) {
            if (i == in_cluster[j])
                sum += getDistance(data[j], cluster_center[i]);
        }
    }
    return sum;
}

//����ÿ����������ĵ�
void getCenter(int in_cluster[])
{
    unsigned int* sum = array1(K);  //���ÿ���������ĵ�
    //float sum[K][D];  //Ҳ��ʹ��C99�䳤����
    int i, j, q, count;
    for (i = 0; i < K; i++)
        sum[i] = 0;
    for (i = 0; i < K; i++) {
        count = 0;  //ͳ������ĳ�������ڵ��������ݵ�
        for (j = 0; j < N; j++) {
            if (i == in_cluster[j]) {
                sum[i] += data[j];  //��������������������ݵ����Ӧά��֮��
                count++;
            }
        }
        if (count == 0) {
            srand((unsigned int)(time(NULL)));
            cluster_center[i] = data[(int)((double)N * rand() / (RAND_MAX + 1.0))];
        }
        else {
            cluster_center[i] = sum[i] / count;
        }
    }
    printf("The new center of cluster is:\n");
    for (i = 0; i < K; i++)
        printf("%u \n", cluster_center[i]);
    free(sum);
}