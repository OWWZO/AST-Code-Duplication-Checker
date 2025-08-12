// 演示代码集合 - 用于代码查重系统答辩
// 包含10个不同复杂度的C++代码示例

#include <iostream>
#include <vector>
#include <string>

// 演示代码1: 简单变量声明和赋值
string demo1 = R"(
int main() {
    int a = 10;
    int b = 20;
    int c = a + b;
    return 0;
}
)";

// 演示代码2: 类似代码1，但变量名不同
string demo2 = R"(
int main() {
    int x = 10;
    int y = 20;
    int z = x + y;
    return 0;
}
)";

// 演示代码3: if语句结构
string demo3 = R"(
int main() {
    int score = 85;
    if (score >= 90) {
        printf("优秀");
    } else if (score >= 80) {
        printf("良好");
    } else {
        printf("及格");
    }
    return 0;
}
)";

// 演示代码4: 类似代码3，但条件不同
string demo4 = R"(
int main() {
    int grade = 75;
    if (grade >= 95) {
        printf("优秀");
    } else if (grade >= 70) {
        printf("良好");
    } else {
        printf("及格");
    }
    return 0;
}
)";

// 演示代码5: for循环结构
string demo5 = R"(
int main() {
    int sum = 0;
    for (int i = 1; i <= 10; i++) {
        sum = sum + i;
    }
    printf("总和: %d", sum);
    return 0;
}
)";

// 演示代码6: 类似代码5，但循环条件不同
string demo6 = R"(
int main() {
    int total = 0;
    for (int j = 0; j < 5; j++) {
        total = total + j;
    }
    printf("总和: %d", total);
    return 0;
}
)";

// 演示代码7: while循环结构
string demo7 = R"(
int main() {
    int count = 0;
    while (count < 5) {
        printf("计数: %d", count);
        count++;
    }
    return 0;
}
)";

// 演示代码8: 嵌套循环结构
string demo8 = R"(
int main() {
    for (int i = 1; i <= 3; i++) {
        for (int j = 1; j <= 3; j++) {
            printf("%d*%d=%d ", i, j, i*j);
        }
        printf("\n");
    }
    return 0;
}
)";

// 演示代码9: switch语句结构
string demo9 = R"(
int main() {
    int choice = 2;
    switch (choice) {
        case 1:
            printf("选项1");
            break;
        case 2:
            printf("选项2");
            break;
        default:
            printf("默认选项");
    }
    return 0;
}
)";

// 演示代码10: 复杂混合结构
string demo10 = R"(
int main() {
    int n = 5;
    int result = 1;
    
    if (n > 0) {
        for (int i = 1; i <= n; i++) {
            result = result * i;
        }
        printf("阶乘: %d", result);
    } else {
        printf("输入错误");
    }
    
    return 0;
}
)";

// 获取演示代码的函数
string getDemoCode(int index) {
    switch (index) {
        case 1: return demo1;
        case 2: return demo2;
        case 3: return demo3;
        case 4: return demo4;
        case 5: return demo5;
        case 6: return demo6;
        case 7: return demo7;
        case 8: return demo8;
        case 9: return demo9;
        case 10: return demo10;
        default: return demo1;
    }
}

// 获取演示代码描述
string getDemoDescription(int index) {
    switch (index) {
        case 1: return "简单变量声明和赋值";
        case 2: return "类似代码1，但变量名不同";
        case 3: return "if语句结构";
        case 4: return "类似代码3，但条件不同";
        case 5: return "for循环结构";
        case 6: return "类似代码5，但循环条件不同";
        case 7: return "while循环结构";
        case 8: return "嵌套循环结构";
        case 9: return "switch语句结构";
        case 10: return "复杂混合结构";
        default: return "未知代码";
    }
} 