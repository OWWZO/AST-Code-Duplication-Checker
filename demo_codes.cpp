// ��ʾ���뼯�� - ���ڴ������ϵͳ���
// ����10����ͬ���Ӷȵ�C++����ʾ��

#include <iostream>
#include <vector>
#include <string>

// ��ʾ����1: �򵥱��������͸�ֵ
string demo1 = R"(
int main() {
    int a = 10;
    int b = 20;
    int c = a + b;
    return 0;
}
)";

// ��ʾ����2: ���ƴ���1������������ͬ
string demo2 = R"(
int main() {
    int x = 10;
    int y = 20;
    int z = x + y;
    return 0;
}
)";

// ��ʾ����3: if���ṹ
string demo3 = R"(
int main() {
    int score = 85;
    if (score >= 90) {
        printf("����");
    } else if (score >= 80) {
        printf("����");
    } else {
        printf("����");
    }
    return 0;
}
)";

// ��ʾ����4: ���ƴ���3����������ͬ
string demo4 = R"(
int main() {
    int grade = 75;
    if (grade >= 95) {
        printf("����");
    } else if (grade >= 70) {
        printf("����");
    } else {
        printf("����");
    }
    return 0;
}
)";

// ��ʾ����5: forѭ���ṹ
string demo5 = R"(
int main() {
    int sum = 0;
    for (int i = 1; i <= 10; i++) {
        sum = sum + i;
    }
    printf("�ܺ�: %d", sum);
    return 0;
}
)";

// ��ʾ����6: ���ƴ���5����ѭ��������ͬ
string demo6 = R"(
int main() {
    int total = 0;
    for (int j = 0; j < 5; j++) {
        total = total + j;
    }
    printf("�ܺ�: %d", total);
    return 0;
}
)";

// ��ʾ����7: whileѭ���ṹ
string demo7 = R"(
int main() {
    int count = 0;
    while (count < 5) {
        printf("����: %d", count);
        count++;
    }
    return 0;
}
)";

// ��ʾ����8: Ƕ��ѭ���ṹ
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

// ��ʾ����9: switch���ṹ
string demo9 = R"(
int main() {
    int choice = 2;
    switch (choice) {
        case 1:
            printf("ѡ��1");
            break;
        case 2:
            printf("ѡ��2");
            break;
        default:
            printf("Ĭ��ѡ��");
    }
    return 0;
}
)";

// ��ʾ����10: ���ӻ�Ͻṹ
string demo10 = R"(
int main() {
    int n = 5;
    int result = 1;
    
    if (n > 0) {
        for (int i = 1; i <= n; i++) {
            result = result * i;
        }
        printf("�׳�: %d", result);
    } else {
        printf("�������");
    }
    
    return 0;
}
)";

// ��ȡ��ʾ����ĺ���
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

// ��ȡ��ʾ��������
string getDemoDescription(int index) {
    switch (index) {
        case 1: return "�򵥱��������͸�ֵ";
        case 2: return "���ƴ���1������������ͬ";
        case 3: return "if���ṹ";
        case 4: return "���ƴ���3����������ͬ";
        case 5: return "forѭ���ṹ";
        case 6: return "���ƴ���5����ѭ��������ͬ";
        case 7: return "whileѭ���ṹ";
        case 8: return "Ƕ��ѭ���ṹ";
        case 9: return "switch���ṹ";
        case 10: return "���ӻ�Ͻṹ";
        default: return "δ֪����";
    }
} 