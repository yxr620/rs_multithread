#include "IsaEC.hpp"
#include <iostream>
#include <chrono>


using namespace std;

chrono::time_point<std::chrono::high_resolution_clock> start, _end;

int main()
{
    int n = 10, k = 4, maxSize = 1024 * 128; // n：数据条带数量 k：校验条带数量
    size_t len = 1024 * 1024 * 128;                  // len：条带长度 2147483647 357913941
    size_t size = n * len;
    vvc_u8 in, out; // in：测试输入 out：ec输出
    cout << "------------------------ 随机初始化原数据中 ------------------------" << endl;
    for (int i = 0; i < n; i++)
    {
        in.push_back(vector<unsigned char>());
        in[i].resize(len);
        // for (size_t j = 0; j < len; j++)
        // {
        //     in[i][j] = rand() % 255;
        // }
    }
    for (int i = 0; i < k; i++)
    {
        out.push_back(vector<unsigned char>());
        out[i].resize(len);
        // for (size_t j = 0; j < len; j++)
        // {
        //     out[i][j] = 255;
        // }
    }

    cout << "------------------------ 开始计算EC ------------------------" << endl;


    // EC校验的计算, maxSize = len / 4
    IsaEC ec(n, k, maxSize, 3);
    // EC校验的计算
    start = chrono::high_resolution_clock::now();
    ec.encode(in, out, size);
    _end = chrono::high_resolution_clock::now();
    
    chrono::duration<double> _duration = _end - start;
    printf("time: %fs \n", _duration.count());
    printf("total data: %ld MB, speed %lf MB/s \n", (n + k) * len / 1024 / 1024, (n + k) * len / 1024 / 1024 / _duration.count());

    vvc_u8 matrix;
    matrix.insert(matrix.end(), in.begin(), in.end());
    matrix.insert(matrix.end(), out.begin(), out.end());

    int err_num = 4;
    unsigned char err_list[err_num];
    err_list[0] = 0;
    err_list[1] = 4;
    err_list[2] = 7;
    err_list[3] = 10;
    for (auto i : err_list)
    {
        for (size_t j = 0; j < len; j++)
        {
            matrix[(int)i][j] = 255;
        }
    }

    start = chrono::high_resolution_clock::now();
    ec.decode(matrix, err_num, err_list, size);
    _end = chrono::high_resolution_clock::now();

    _duration = _end - start;
    printf("decode time: %fs \n", _duration.count());
    printf("total data: %ld MB, speed %lf MB/s \n", (n + k) * len / 1024 / 1024, (n + k) * len / 1024 / 1024 / _duration.count());

    if (matrix[0] == in[0])
    {
        cout << "校验对比错误条带{ 0 }正确无误" << endl;
        return 0;
    }
    else
    {
        cout << "err: not equal" << endl;
        return -1;
    }

    return 0;
}
