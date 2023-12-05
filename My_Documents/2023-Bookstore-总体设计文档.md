## 2023-Bookstore-总体需求文档

### 用户交互设计和数据库设计

#### 功能实现
**Guest模块：**
- Account -> Register：允许游客注册账户
- Account -> Login：允许游客登录账户

**User模块：**
- Bookinfo -> Info：查询图书信息（ISBN号、书名、作者、关键字、库存量和单价等）
- Bookinfo -> Query：根据ISBN号、作者名字、关键字、书名进行图书查询
- Stock -> Purchase：通过输入ISBN号和购买数量来购买图书

**Advanced User模块：**
- Stock -> Order：进货操作，指定ISBN号、数量和进货价格
- Bookinfo -> Addition：添加图书信息
- Bookinfo -> Modification：修改图书信息
- Account -> Register：注册新用户
- Document -> Record：记录工作内容以生成员工工作报告
- Account -> Login：登录客户账户并创建用户操作（订单）

**Administer模块：**
- Stock -> Query(Purchase_Information)：查询某段时间的采购信息，按照采购顺序输出图书的ISBN号、数量和单价等
- Stock -> Query(Sales Situation)：查询某段时间的销售情况，按照销售顺序输出图书的ISBN号、数量和单价等
- Stock -> Query(Profit Information)：查询某段时间的盈利信息，包括收入、支出和利润
- Document -> Employee Work Report：查看各员工的工作情况报告，按操作顺序输出每次操作类型及其对象
- Document -> System Work Log：查看系统的整体工作日志

#### 交互命令和数据存储

**系统功能：**
- 输入`HELP`查看当前权限可以进行的操作
- 如果对应操作权限不足，返回`ERROR:Insufficient permissions`
- 如果格式不符合，返回`ERROR:Invalid input format`

**权限要求0级（游客）：**
-  Account -> Register：`REGISTER <用户名> <密码>` ，以及如果用户名可用，则 `<重复密码>`
- Account -> Login：`LOGIN <用户名> (<密码>)`
- Account -> Logout：`LOGOUT`

**权限要求1级（顾客）：**
- Bookinfo -> Query：`QUERY_BOOK <查询条件>`，确认查询的图书后调用`Bookinfo -> Info`
- Bookinfo -> Info：`QUERY_INFO <ISBN>`
- Stock -> Purchase：`CREATE_ORDER <用户名> <订单内容>`

**权限要求2级（销售人员）：**
- Stock -> Order：`Order <ISBN> <数量> <进货价格>`
- Bookinfo -> Add：`ADD_BOOK <ISBN> <书名> <作者> <关键字> <库存量> <单价>`
- Bookinfo -> Modify：`MODIFY_BOOK <ISBN> <需要修改的字段> <新值>`
- Bookinfo -> Delete：`DELETE_BOOK <ISBN>`
- Document -> Record：`CREATE_RECORD <备注>`
- Account -> Login：`LOGIN <用户名>`

**权限要求3级（店长）：**
- Stock -> Query(Stock Information)：`QUERY_STOCK`
- Stock -> Query(Sales Situation)：`QUERY_SALE`
- Stock -> Query(Profit Information)：`QUERY_PROFIT`
- Document -> View(Employee_Work_Report)：`VIEW_WORK_REPORT`
- Document -> View(System_Work_Log)：`VIEW_SYSTEM_LOG`

**存储的文件**
- 在`./Database/Bookinfo.txt`内保存书籍数据
- 在`./Database/Users.txt`内保存用户数据
- 在`./Database/Order.txt`内保存采购数据
- 在`./Database/Sale.txt`内保存销售数据
- 在`./Database/Stock.txt`内保存库存数据
- 在`./Documents/Work_report.txt`内保存员工工作记录
- 在`./Documents/System_log/日期.txt`内保存系统日志

### 3.类、结构体设计

#### 3.1 数据存储方式
采用**二进制**存储，并规范各类信息的长度。

调用`Fileio.h`中的接口实现文件的输入输出。

#### 3.2 类
- Bookinfo类
  - 在`./Database/Bookinfo.txt`内保存书籍数据（块状链表）
```cpp
//Bookinfo.h
    
    //新增图书并录入信息
    void Bookinfo_Add();
    
    //修改图书信息 
    void Booinfo_Modify();
    
    //删除图书信息
    void Bookinfo_Delete();
    
    //查询（根据ISBN）
    <指向Bookinfo.txt中某图书位置的指针> Book_Info();
    
    //查询
    <指向Bookinfo.txt中某图书位置的指针> Book_Query();
```
- User类：存储用户的属性，如用户名、密码等。
  - 在`./Database/Users.txt`内保存用户数据（块状链表）。其中每个用户存储`用户名 - 用户等级 - 用户密码的加密值`。
  - 维护一个`user_stack`，存储用户栈。初始化有一个`Guest`账户（对`logout`无效）
  - `login`操作时，如果本账户权限更高或已经在登录栈中，可以不输入密码。（如果登录栈内非顶层的账户，把该账户`move`至栈顶避免重复）
  - `logout`操作时，弹出栈顶的用户
  - 用户名禁止修改
```cpp
//User.h
    //用户栈    
    std::vector<User> user_stack;

    //登入，返回栈中登录后账户指针
    <指针> Login();
    
    //登出，返回栈中登出后账户指针
    <指针> Logout();
    
    //注册（默认可以注册顾客，如果传入user_access=2要求则店长操作）
    void Register(std::string user_name, std::string password, int user_access = 1);
    
    //修改账户信息？（如果有需求）
    void Account_Modify();
```
- Stock类：处理和存储库存、订单等信息 
```cpp
//Stock.h
    //查询采购信息（按采购顺序输出采购图书的 ISBN 号、数量、单价等）
    //数据在./Database/Order.txt
    void Query_Order(<时间段>);

    //查询销售情况（按照销售顺序输出售出图书的 ISBN 号、数量、单价等）
    //数据在./Database/Sale.txt
    void Query_Sale(<时间段>)
    
    //查询盈利信息（输出该段时间的收入，支出以及利润）
    void Query_Profit(<时间段>);
    
    //进货，修改./Database/Stock.txt的库存数据，调用Document.h中的Record记录员工工作
    //采购信息记录在./Database/Order.txt中
    void Stock(<时间>, <ISBN>, <数量>, <进货价格>, <操作人员>);
    
    //Purchase时先判断库存是否足够，不够的话返回错误信息    
    //如果足够，修改./Database/Stock.txt的库存数据，并在./Database/Sale.txt记录
    //Purchase操作人员分为两种情况，分别为用户自己操作和员工代替用户操作
    //如果是后者，要额外调用Document.h中的Record记录员工工作
    void Purchase(<顾客ID>, <订单内容>, <是否由员工代为操作>)
```
- Document类：生成并查看文档和日志
```cpp
//Document.h
    //记录员工工作内容，写入./Documents/Work_report.txt
    void Record(std::string <员工ID>, <具体内容>);

    //查看员工工作报告
    void View_Work_Report(std::string <员工ID>, <时间范围>);

    //生成并查看系统工作日志，保存在./Documents/System_log/日期.txt
    void View_System_Log();
    
```
- Fileio类：统一文件读写
```cpp
//Fileio.h（待完善）
    //修改
    void File_Modify(<文件路径>, <修改信息>);

    //新增
    void File_Add(<文件路径>, <新增信息>);
    
    //删除
    void File_Del(<文件路径>, <删除信息>);
    
    //查询
    <指针> File_Search(<文件路径>, <查询信息>);
```
- 主程序
```cpp
//main.cpp
    //如果首次运行，系统初始化，创建相应文档
    
    //如果非首次运行但是新启动，初始化用户栈
    
    //根据输入的命令行调用相应子系统
    
    //系统退出时清空缓存数据
```
