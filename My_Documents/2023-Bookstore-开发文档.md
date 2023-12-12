# Bookstore开发文档
作者：2023级，skywong

## 程序功能概述

这是一个书店管理系统，可以通过命令行交互实现各类操作。


## 主体逻辑说明
### 系统的初始化，开启，和退出

- 如果为**初次运行系统**，调用初始化函数，**创建**店主账户，创建相关存储文件。
- 如果已初始化，系统开启时将**游客**加入用户栈，等待命令。
- 退出系统时，清空用户栈

### 处理指令的一般流程（main函数）

由主程序负责调度，在**运行过程**中，对于每一条指令：

- 如果指令不符合格式，返回提示信息

- 对于格式正确的指令，首先判断当前用户权限是否足够，如不足，返回提示信息

- 拆分指令，提取参数，并调用系统模块执行指令。

  

### 具体指令

#### 基础指令

- `quit`或`exit`：退出系统 

#### 账户系统指令
此部分调用`User.h`模块
**User.txt的维护**：调用`Fileio.h`的函数来维护，`index`为用户名，`value`为`用户权限，密码加密值`。文件存储在`./Database/Users.txt`。

**登录栈（User_Stack）的维护**：使用`vector<pair<string, string> >`模拟栈，其中`pair<string, string>`前一个`string`存`UserID`，后一个存储`select`操作选的数的`ISBN`，初始为空。

- `su [UserID] ([Password])?`：如果当前用户权限大于要登陆的用户，可忽略密码，否则判断密码时候正确。操作合法则执行`User_Stack.push_back(...)`。
- `logout`：`User_Stack.pop_back()`，如果弹出后登陆栈为空，`push_back`一个游客账户。
- `register`：判断用户名是否冲突，为冲突则在`./Database/Users.txt`中添加新用户信息
- `passwd`：判断操作是否合法，然后再`./Database/Users.txt`中更新密码
- `useradd`：判断当前账户权限和用户名冲突以及是否权限足够，随后更新`./Database/Users.txt`
- `delete`：合法则调用`Fileio.h`中的`del`函数即可。

#### 图书系统指令
此部分调用`Bookinfo.h`和`Stock.h`
- `show`：检索图书，如果关键字为`ISBN`则直接调用`Bookinfo.h`的`seek`，否则在`./Database/Search/xxx.txt`中查询关键词，把所有符合条件的图书加入`vector`返回后输出。（*可考虑实现*：额外创建一个文件，用于存储各关键字所对应的图书，一样采用块状链表维护）
- `select`：检查`ISBN`对应书是否存在，存在则把登陆栈中对应该用户的`.second`修改，不存在则创建。
- `modify`：检验操作是否合法，随后调用Bookinfo的修改函数。
- `buy`：检验操作是否合法，随后更新`./Database/Order.txt`和`./Database/Stock.txt`。
- `import`：检验操作是否合法，随后根据数量和价格更新`./Database/Order.txt`和`./Database/Stock.txt`。

#### 日志系统指令
此部分调用`Documents.h`模块
- `show finance ([Count])?`：在`./Database/Order.txt`找到最后的`Count`条记录。
- `report finance`：生成财务报表，导出在`./Documents/Finance/生成时间.csv`中。
- `report employee`：在`./Documents/Work_report/生成时间.csv`内保存员工工作记录
- `log`：在`/Documents/System_log/日期.csv`内保存生成的系统日志。系统日志是一段时间内各类数据的总结。

## 代码文件结构
Bookstore程序的代码文件结构如下：
- `main.cpp`：主函数，程序的入口点。
- `Fileio.h` 和 `Fileio.cpp`
- `Bookinfo.h` 和 `Bookinfo.cpp`。
- `User.h` 和 `User.cpp`
- `Stack.h` 和 `Stack.cpp`
- `Documents.h` 和 `Documents.cpp`
## 类及成员说明
### 功能说明
#### Fileio类
- 块状链表：实例化类模板，将`index`和`value`分开存储，支持根据`index`实现查找、新增、修改、删除。
- 普通文件：根据具体操作实现对应函数
#### Bookinfo类成员变量
- `index`: `[ISBN]`
- `value`：`[ISBN]-[BookName]-[Auther]-[Keyword]`，后面三个值统一长度后存储。
#### User类成员变量
- `index`: `username`
- `value`：`[username]-[权限]-[密码加密值]`，密码加密值统一为`20`位字符串。
#### Stock类
该类管理`Order.txt`（采购和销售数据）以及`Stock.txt`（库存数据）。
#### Documents类
该类负责生成和保存文档。

### 接口说明
* 待完善

## 文件存储说明
所有文件操作都调用`Fileio.h`，其中**用户**和**书籍**（包括书籍对应的关键词索引）因为修改操作较多，采用块状链表。而其他文件采用**顺序存储**。

**存储的文件**
- 在`./Database/Bookinfo.txt`内保存书籍数据 
- 在`./Database/Users.txt`内保存用户数据
- 在`./Database/Order.txt`内保存采购和销售数据
- 在`./Database/Stock.txt`内保存库存数据
- 在`./Database/Search/Author.txt, Keyword.txt, Name.txt`内保存关键字和拥有该关键字的所有图书的ISBN

其中`Bookinfo.txt`，`Users.txt`和`./Database/Search/xxx.txt`因使用块状链表，实际上各自会有`_index.txt`和`_value.txt`两个文件。

除此之外，`./Documents`目录下会有由指令生成的文档。

除了生成的文档外，所有数据采用二进制保存。


## 其他补充说明
**程序的使用方法**:将该系统存储在某个位置，运行`main.exe`启动系统，输入`quit/exit`指令退出系统
