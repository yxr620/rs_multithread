#include "IsaEC.hpp"
#include <iostream>
#include <chrono>



using namespace std;

chrono::time_point<std::chrono::high_resolution_clock> start, _end;

int main()
{
    int n = 10, k = 4, maxSize = 1024; // n：数据条带数量 k：校验条带数量
    size_t len = 1024 * 1024 * 128;                  // len：条带长度 2147483647 357913941
    size_t size = n * len;
    vvc_u8 in, out; // in：测试输入 out：ec输出
    cout << "------------------------ 随机初始化原数据中 ------------------------" << endl;
    for (int i = 0; i < n; i++)
    {
        in.push_back(vector<unsigned char>());
        in[i].resize(len);
        for (size_t j = 0; j < len; j++)
        {
            in[i][j] = rand() % 255;
        }
    }
    for (int i = 0; i < k; i++)
    {
        out.push_back(vector<unsigned char>());
        out[i].resize(len);
        for (size_t j = 0; j < len; j++)
        {
            out[i][j] = 255;
        }
    }
    // cout << "---------------------- 原始数据 ----------------------" << endl;
    // for (int i = 0; i < n; i++)
    // {
    //     for (int j = 0; j < len; j++)
    //     {
    //         printf("%02X ", in[i][j]);
    //     }
    //     cout << endl;
    // }
    // for (int i = 0; i < k; i++)
    // {
    //     for (int j = 0; j < len; j++)
    //     {
    //         printf("%02X ", out[i][j]);
    //     }
    //     cout << endl;
    // }
    cout << "------------------------ 开始计算EC ------------------------" << endl;

    IsaEC ec(n, k, maxSize);
    // EC校验的计算
    start = chrono::high_resolution_clock::now();
    ec.encode(in, out, size);
    _end = chrono::high_resolution_clock::now();
    
    chrono::duration<double> _duration = _end - start;
    printf("%f\n", _duration.count());
    printf("total data: %ld MB, speed %lf MB/s \n", (n + k) * len / 1024 / 1024, (n + k) * len / 1024 / 1024 / _duration.count());
    // cout << "------------------------ EC校验块 ------------------------" << endl;
    // for (int i = 0; i < k; i++)
    // {
    //     for (int j = 0; j < len; j++)
    //     {
    //         printf("%02X ", out[i][j]);
    //     }
    //     cout << endl;
    // }
    vvc_u8 matrix;
    matrix.insert(matrix.end(), in.begin(), in.end());
    matrix.insert(matrix.end(), out.begin(), out.end());
    // cout << "---------------------- 恢复样例(正确) ----------------------" << endl;
    // for (int i = 0; i < n + k; i++)
    // {
    //     for (int j = 0; j < len; j++)
    //     {
    //         printf("%02X ", matrix[i][j]);
    //     }
    //     cout << endl;
    // }
    int err_num = 2;
    unsigned char err_list[err_num];
    err_list[0] = 0;
    err_list[1] = 4;
    for (auto i : err_list)
    {
        for (size_t j = 0; j < len; j++)
        {
            matrix[(int)i][j] = 255;
        }
    }
    // cout << "---------------------- 恢复样例(错误) ----------------------" << endl;
    // for (int i = 0; i < n + k; i++)
    // {
    //     for (int j = 0; j < len; j++)
    //     {
    //         printf("%02X ", matrix[i][j]);
    //     }
    //     cout << endl;
    // }

    start = chrono::high_resolution_clock::now();
    ec.decode(matrix, err_num, err_list, size);
    _end = chrono::high_resolution_clock::now();

    _duration = _end - start;
    printf("decode time: %fs \n", _duration.count());
    printf("total data: %ld MB, speed %lf MB/s \n", (n + k) * len / 1024 / 1024, (n + k) * len / 1024 / 1024 / _duration.count());

    // cout << "---------------------- 恢复样例(恢复) ----------------------" << endl;
    // for (int i = 0; i < n + k; i++)
    // {
    //     for (int j = 0; j < len; j++)
    //     {
    //         printf("%02X ", matrix[i][j]);
    //     }
    //     cout << endl;
    // }
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

    // 其他函数测试
    // int minSize = ec.getMinSize();
    // cout << "minSize:" << minSize << endl;

    return 0;
}
