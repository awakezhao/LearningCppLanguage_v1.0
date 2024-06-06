## 可变参数

va_list, va_start, va_arg, va_end是C语言中用于处理可变参数的宏。定义在<stdarg.h>头文件中。
* va_list: 这是一个用于访问可变参数列表的类型。
* va_start: 用于初始化va_list变量。需要两个参数：一个va_list变量和一个可变参数列表的前一个参数。
* va_arg: 获取可变参数列表的下一个参数。需要两个参数：一个va_list变量和一个参数的类型。
* va_end: 用于清理va_list变量。在使用完va_list变量之后，应该调用va_end.