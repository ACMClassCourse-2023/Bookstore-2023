## 2023-Bookstore-总体需求文档

### 1. 用户交互设计

请概述各模块的功能和功能结构图；
用户可以通过命令行完成交互。
输入`HELP`查看当前用户组可以进行的操作及操作格式
详见**数据库设计**部分

### 2. 数据库设计
请设计所有的命令格式和对应的输出；
采用命令行交互


### 3. 类、结构体设计 
#### 3.1 数据存储方式

#### 3.2 各类数据存储方式
请指出你需要存储的数据以及存储方式；


某个操作通用流程：？

身份认证（Identity）：用户名和密码（或安全密钥文件）

访问控制（Access Management）
分别是账号（Account）、认证（Authentication）和授权（Authorization）
审计（Audit）


### 用户交互设计

#### 模块功能概述及功能结构图

**Guest模块：**
- Account -> Register：允许游客注册账户
- Account -> Login：允许游客登录账户

**User模块：**
- Bookinfo -> Info：查询图书信息（ISBN号、书名、作者、关键字、库存量和单价等）
- Bookinfo -> Query：根据ISBN号、作者名字、关键字、书名进行图书查询
- Selling -> Purchase：通过输入ISBN号和购买数量来购买图书

**Advanced User模块：**
- Selling -> Stocking：进货操作，指定ISBN号、数量和进货价格
- Bookinfo -> Addition：添加图书信息
- Bookinfo -> Modification：修改图书信息
- Account -> Register：注册新用户
- Document -> Record：记录工作内容以生成员工工作报告
- Account -> Login：登录客户账户并创建用户操作（订单）

**Administer模块：**
- Selling -> Query(Purchase_Information)：查询某段时间的采购信息，按照采购顺序输出图书的ISBN号、数量和单价等
- Selling -> Query(Sales Situation)：查询某段时间的销售情况，按照销售顺序输出图书的ISBN号、数量和单价等
- Selling -> Query(Profit Information)：查询某段时间的盈利信息，包括收入、支出和利润
- Document -> Employee Work Report：查看各员工的工作情况报告，按操作顺序输出每次操作类型及其对象
- Document -> System Work Log：查看系统的整体工作日志

### 数据库设计

命令格式和对应的输出如下：

**系统功能：**
- 输入`HELP`查看当前权限可以进行的操作
- 如果对应操作权限不足，返回`ERROR:Insufficient permissions`
- 如果格式不符合，返回`ERROR:Invalid input format`

**权限要求0级（游客）：**
-  Account -> Register：`REGISTER <用户名> <密码>` ，以及如果用户名可用，则 `<重复密码>`
- Account -> Login：`LOGIN <用户名> <密码>`
- Account -> Logout：`LOGOUT`

**权限要求1级（顾客）：**
- Bookinfo -> Query：`QUERY_BOOK <查询条件>`，确认查询的图书后调用`Bookinfo -> Info`
- Bookinfo -> Info：`QUERY_INFO <ISBN>`
- Order -> Purchase：`CREATE_ORDER <用户名> <订单内容>`

**权限要求2级（销售人员）：**
- Order -> Stock：`STOCK <ISBN> <数量> <进货价格>`
- Bookinfo -> Add：`ADD_BOOK <ISBN> <书名> <作者> <关键字> <库存量> <单价>`
- Bookinfo -> Modify：`MODIFY_BOOK <ISBN> <需要修改的字段> <新值>`
- Bookinfo -> Delete：`DELETE_BOOK <ISBN>`
- Document -> Record：`CREATE_RECORD <备注>`
- Account -> Login：`LOGIN <用户名>`

**权限要求3级（店长）：**
- Order -> Query(Stock Information)：`QUERY_STOCK`
- Order -> Query(Sales Situation)：`QUERY_SALE`
- Order -> Query(Profit Information)：`QUERY_PROFIT`
- Document -> View(Employee_Work_Report)：`VIEW_WORK_REPORT`
- Document -> View(System_Work_Log)：`VIEW_SYSTEM_LOG`

### 3.类、结构体设计

#### 3.1 数据存储方式
采用二进制存储，调`Fileio.h`中的接口实现文件的输入输出。

#### 3.2 类
- Bookinfo类
  - 在`./database/Bookinfo.txt`内保存数据（块状链表）
```cpp
//Bookinfo.h
    
    //新增图书并录入信息
    void Bookinfo_Add();
    
    //修改图书信息 
    void Booinfo_Modify();
    
    //删除图书信息
    void Bookinfo_Delete();
    
    //查询（根据ISBN）
    string Book_Info();
    
    //查询
    string Book_Query();
```
- User类：存储用户的属性，如用户名、密码等。
  - 维护一个`user_stack`，存储用户栈。初始化有一个`Guest`账户（对`logout`无效）
  - `login`操作时，如果本账户权限更高或已经在登录栈中，可以不输入密码。（如果登录栈内非顶层的账户，把该账户`move`至栈顶避免重复）
  - `logout`操作时，弹出栈顶的用户
- Order类：存储订单信息，包括订单内容和相应的用户。
- PurchaseInfo类：存储采购信息，包括ISBN号、数量和单价等。
