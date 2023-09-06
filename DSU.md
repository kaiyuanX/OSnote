参考
- [软件动态更新技术-蒋炎岩](https://zhuanlan.zhihu.com/p/425845057)

# 软件的动态更新

> Dynamic Software Update, DSU

无论是修复 bug 还是添加新功能，软件都需要更新

软件更新能不中断服务就是 DSU 技术做的事情

## 实现动态更新

`DSU()` 把函数 `foo()` 指针指向的内容改变了

不再指向 `foo()` 的第一条语句，而是指向一条跳转到 `foo_new()` 的跳转指令

```c
int main() {
  foo();
  DSU(foo, foo_new);
  foo();
}
```

详见 [code](./code/dsu.c) 