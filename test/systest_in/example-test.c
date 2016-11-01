#include <stdio.h>
int main() {
  // 支持printf 返回值
  int hello = printf("hello %d\n", 1024);

  // 支持十六进制和八进制
  int world = -0x10;

  // 支持悬空的{}块
  {
    printf("hello + world: %d\n", hello + world);
  }

  // 支持任意的表达式嵌套
  int a = b = c = 1;
  int comlex_expr = hello = world = 1 == 10 < -a++ * b - c, 999;

  // 支持括号 和 括号嵌套
  int parentheses = ((3 + b) * (c > 9) , 999);

  // 支持 if, while, do-while, for 的条件表达式中出现任意表达式
  if (1, 2, 3) {
    for (int i = 0; printf("dead loop\n"); i++) {
      do {
        // 支持任意的结构嵌套
        c = 5;
        while (a = b = c) {
          // 支持非赋值形式
          c--, b = 1;
        }
      } while (a == b ++ + -c, false);
      break;
    }

  }
}
