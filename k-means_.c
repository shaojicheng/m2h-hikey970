#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int K, N, D;  //聚类的数目，数据量，数据的维数
K = 10;
N = 8000;
D = 1;
int LEVEL_WIDTH = 1000;//分级别，每一级的宽度
int COLD_DATA_THRESHOLD = 500000;//超过这个阈值就视为冷数据

unsigned int* data;  //存放数据
int* in_cluster;  //标记每个点属于哪个聚类
unsigned int* cluster_center;  //存放每个聚类的中心点

static void get_centroid();
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
    if ((fopen_s(&fp, "data.txt", "r")) != 0)    fprintf(stderr, "cannot open data.txt!\n");
    //if (fscanf_s(fp, "K=%d\n", k) != 1)        fprintf(stderr, "load error!\n");
    data = array1(N);  //生成数据数组
    for (i = 0; i < N; i++)
        fscanf_s(fp, "%u", &data[i]);  //读取数据点
    printf("Data sets:\n");
    for (i = 0; i < N; i++)
        printf("%u \n", data[i]);

    printf("-----------------------------\n");
    printf("\n-----------k=%d----------\n", K);
    cluster_center = array1(K);  //聚类的中心点
    for (i = 0; i < K; i++)
        cluster_center[i] = 0;
    get_centroid();
    for (i = 0; i < K; i++)
        printf("%u \n", cluster_center[i]);
    in_cluster = (int*)malloc(N * sizeof(int));  //每个数据点所属聚类的标志数组
    k_means();
    printf("The new center of cluster is:\n");
    for (i = 0; i < K; i++)
        printf("%u \n", cluster_center[i]);
}


///现在设k= 10
static void get_centroid() {
    /*1、遍历一遍，求得最大值，并按照1万为单位，把level切割好
     *2、再次遍历，统计每个level中的样本数
     *3、遍历10次，求出top 10，先输出来看看
     */
     //3000万能够保证不超
    int level_cnt[3000] = { 0 };

    int i = 0, level_nr = 0, level_seq = 0, centroid_cnt = 0;
    unsigned int max = 0;
    for (i = 0; i < N; i++) {
        if (data[i] < COLD_DATA_THRESHOLD && data[i] > max)
            max = data[i];
    }
    //每个level都是前闭后开
    level_nr = max / LEVEL_WIDTH;//QWJ计算出level的个数
    //printf("max = %u; level_nr = %d\n", max, level_nr);
    for (i = 0; i <N; i++) {
        if (data[i] < COLD_DATA_THRESHOLD) {
            level_seq = data[i] / LEVEL_WIDTH;
            level_cnt[level_seq]++;//QWJ统计每个level有多少个值
        }
    }

    while (centroid_cnt < K) {//QWJ 质心数CENTROID_NR=10
        int tmp_max = 0, tmp_index = 0;
        int be_cent = 1;
        for (i = 0; i < level_nr; i++) {//QWJ找出值最多的level
            if (level_cnt[i] > tmp_max) {
                tmp_max = level_cnt[i];
                tmp_index = i;
            }
        }
        //选出了一个待定质心后，看他能不能作为质心
        for (i = 0; i < centroid_cnt; i++) {
            if (cluster_center[i] / LEVEL_WIDTH == tmp_index + 1 || cluster_center[i] / LEVEL_WIDTH + 1 == tmp_index) {
                be_cent = 0;
                break;
            }
        }
        if (tmp_max == 0)
            break;
        if (be_cent) {
            //printf("centroid %d is %u, has %d points in section\n", centroid_cnt, tmp_index * LEVEL_WIDTH, tmp_max);
            cluster_center[centroid_cnt++] = tmp_index * LEVEL_WIDTH;
            printf("%d \n", centroid_cnt);
        }
        level_cnt[tmp_index] = 0;
    }
    K = centroid_cnt;
}

void  k_means()
{
    int i, j, count = 0;
    unsigned int temp1, temp2;

    cluster();  //用随机k个中心点进行聚类
    temp1 = getDifference();  //第一次中心点和所属数据点的距离之和
    count++;
    printf("The first difference between data and center is: %u\n\n", temp1);


    getCenter(in_cluster);
    cluster();  //用新的k个中心点进行第二次聚类
    temp2 = getDifference();
    count++;
    printf("The second difference between data and center is: %u\n\n", temp2);

    while (fabs(temp2 - temp1) !=0) {   //比较前后两次迭代，若不相等继续迭代
        temp1 = temp2;
        getCenter(in_cluster);
        cluster();
        temp2 = getDifference();
        count++;
        printf("The %dth difference between data and center is: %u\n\n", count, temp2);
    }

    printf("\nThe total number of cluster is: %d\n", count);  //统计迭代次数
    //system("pause");  //gcc编译需删除
}


//动态创建一维数组
unsigned int* array1(int n)
{
    unsigned int* p;
    p = (unsigned int*)malloc(n * sizeof(unsigned int));
    return p;
}

//动态创建二维数组
unsigned int** array(int m, int n)
{
    int i;
    unsigned int** p;
    p = (unsigned int**)malloc(m * sizeof(unsigned int*));
    p[0] = (unsigned int*)malloc(m * n * sizeof(unsigned int));
    for (i = 1; i < m; i++)    p[i] = p[i - 1] + n;
    return p;
}


//释放二维数组所占用的内存
void freearray(unsigned int** p)
{
    free(*p);
    free(p);
}


//计算欧几里得距离
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

//把N个数据点聚类，标出每个点属于哪个聚类
void cluster()
{
    int i, j;
    unsigned int min;
    unsigned int** distance = array(N, K);  //存放每个数据点到每个中心点的距离
    //float distance[N][K];  //也可使用C99变长数组
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

//计算所有聚类的中心点与其数据点的距离之和
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

//计算每个聚类的中心点
void getCenter(int in_cluster[])
{
    unsigned int* sum = array1(K);  //存放每个聚类中心点
    //float sum[K][D];  //也可使用C99变长数组
    int i, j, q, count;
    for (i = 0; i < K; i++)
        sum[i] = 0;
    for (i = 0; i < K; i++) {
        count = 0;  //统计属于某个聚类内的所有数据点
        for (j = 0; j < N; j++) {
            if (i == in_cluster[j]) {
                sum[i] += data[j];  //计算所属聚类的所有数据点的相应维数之和
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
